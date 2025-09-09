/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2025 ORPAL Technology, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <filesystem>
namespace fs = std::filesystem;
#include <iostream>
#include <fstream>
#include "aapi_calibrator.h"
#include "utils/aapi_math_utils.h"

///////////////////////////////////////////////////////////////////////////////
// OSL definitions
///////////////////////////////////////////////////////////////////////////////

#define OSL_CORR_FILE_MAGIC     0xE0B07010
#define OSL_CORR_FILE_VERSION   0x00040002

#define HWE_CORR_FILE_MAGIC     0xE0B08010
#define HWE_CORR_FILE_VERSION   0x00040002

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPICalibrator implementation
///////////////////////////////////////////////////////////////////////////////

AAPiString AAPiCalibrator::m_dir = AAPiCalibrator::get_calibration_dir(true);

AAPiCalibrator *AAPiCalibrator::create(AAPiConfig *config, bool addRef)
{
    AAPiCalibrator *obj = create(addRef);
    if( obj ) {
        obj->m_config = config;

        AAPI_ADDREF(config);
    }
    return obj;
}

AAPiCalibrator::AAPiCalibrator()
    : m_config(nullptr)
    , m_osl_file(-1)
    , m_osl_status(AAPI_CAL_FILE_INVALID)
    , m_hwe_status(AAPI_CAL_FILE_INVALID)
{
}

AAPiCalibrator::~AAPiCalibrator()
{
    AAPI_DISPOSE(m_config);
}

int AAPiCalibrator::init()
{
    int ret;

    m_osl_file      = m_config->get_osl_selected();
    m_osl_status    = AAPI_CAL_FILE_INVALID;
    m_hwe_status    = AAPI_CAL_FILE_INVALID;

    const fs::path dirPath( (const char *)get_calibration_dir() );
    if (! fs::exists( dirPath ))
    {
        if (! fs::create_directory( dirPath ))
        {
            qDebug() << "Failed to create directory.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    // load OSL correction file 
    ret = load_osl_correction_file( m_osl_file );
    if ( AAPI_FAILED( ret ))
    {
        // Ignore error code
    }

    // load hardware error correction file
    ret = load_hwe_correction_file();
    if (AAPI_FAILED( ret ))
    {
        // Ignore error code
    }

    return AAPI_SUCCESS;
}

int AAPiCalibrator::set_osl_file(int f)
{
    if (! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( m_osl_status != f )
    {
        // Mark invalid as the file has changed 
        m_osl_status    &= ~AAPI_CAL_FILE_VALID;
        m_osl_file      = f;
    }

    return AAPI_SUCCESS;
}

int AAPiCalibrator::set_osl_z(uint8_t scanType, int index, const AAPiComplex& z)
{
    double r0;

    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if (index < 0 || index >= AAPI_CAL_NUM_ENTRIES)
    {
        return AAPI_E_INVALID_ARG;
    }

    r0 = m_config->get_base_r0();

    switch (scanType)
    {
    case AAPI_CAL_FILE_SHORT_SCANNED:
        m_osl_correction [index].g_short = gamma_from_z( z, r0 ).__rep();
        break;

    case AAPI_CAL_FILE_LOAD_SCANNED:
        m_osl_correction [index].g_load = gamma_from_z( z, r0 ).__rep();
        break;

    case AAPI_CAL_FILE_OPEN_SCANNED:
        m_osl_correction [index].g_open = gamma_from_z( z, r0 ).__rep();
        break;

    default:
        return AAPI_E_INVALID_ARG;
    }

    return AAPI_SUCCESS;
}

int AAPiCalibrator::scan_osl_begin(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Clear status flags 
    m_osl_status    &= ~AAPI_CAL_FILE_VALID;
    m_osl_status    &= ~scan_type;

    return 0;
}

int AAPiCalibrator::scan_osl_ended(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Set flags 
    m_osl_status    |= scan_type;

    return 0;
}

void AAPiCalibrator::scan_hwe_begin()
{
    // Set correction status to invalid in the beginning
    m_hwe_status = AAPI_CAL_FILE_INVALID;
}

int AAPiCalibrator::set_hwe_entry(int index, double mag0, double phas0)
{
    if ( index < 0 || index >= AAPI_CAL_NUM_ENTRIES )
    {
        return AAPI_E_INVALID_ARG;
    }

    m_hwe_correction [index].mag_0 = mag0;
    m_hwe_correction [index].phas_0 = phas0;

    return 0;
}

void AAPiCalibrator::scan_hwe_ended()
{
    m_hwe_status = AAPI_CAL_FILE_VALID;
}

bool AAPiCalibrator::osl_file_exists(int f) const
{
    if (! is_valid_file( f ))
    {
        return false;
    }

    const fs::path filePath( (const char *)get_osl_correction_file_name(f) );
    return fs::exists( filePath );
}

int AAPiCalibrator::load_osl_correction_file(int f)
{
    if (! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    std::ifstream file( get_osl_correction_file_name( f ),
                       std::ios::in | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to open OSL correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Read serialized data from the file

    // Read and check the header 
    quint32 magic, version;
    quint32 length;
    size_t  data_size = sizeof( m_osl_correction );

    // Read 4-byte magic
    file.read( (char*)&magic, sizeof(quint32) );
    if ( file.good() )
    {
        // Read 4-byte version
        file.read( (char*)&version, sizeof(quint32) );
    }

    if ( file.good()    &&
         OSL_CORR_FILE_MAGIC    == magic    &&
         OSL_CORR_FILE_VERSION  == version )
    {
        // Read 4-byte length
        file.read( (char*)&length, sizeof(quint32) );

        if ( file.good()    &&
             length == data_size )
        {
            // read OSL entries 
            file.read( (char*) m_osl_correction, data_size );

            if ( file.good() )
            {
                // Successfully loaded entries 
                m_osl_status = AAPI_CAL_FILE_VALID;
                m_osl_file  = f;

                qDebug() << "Successfully loaded OSL correction file: " << f;
                return AAPI_SUCCESS;
            }
        }
    }

    // The correction file is invalid
    m_osl_status = AAPI_CAL_FILE_INVALID;
    m_osl_file  = -1;

    qDebug() << "Invalid OSL correction file: " << f;
    return AAPI_CAL_E_FILE_INVALID;
}

bool AAPiCalibrator::is_valid_file(int f)
{
    return AAPI_CAL_FILE_IS_VALID( f );
}

int AAPiCalibrator::flush_osl_correction_file()
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( m_osl_status != AAPI_CAL_FILE_VALID )
    {
        return AAPI_CAL_E_NOT_CALCULATED;
    }

    AAPiString fileName = get_osl_correction_file_name( m_osl_file );

    std::ofstream file( fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to create OSL correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Will serialize the data into the file
    quint32 data_size = sizeof( m_osl_correction );
    quint32 magic = static_cast<quint32> ( OSL_CORR_FILE_MAGIC );
    quint32 version = static_cast<quint32> ( OSL_CORR_FILE_VERSION );
    int ret;

    // Write a header with a "magic number" and a version
    file.write( (const char *)&magic, sizeof(quint32) );

    if ( file.good() )
    {
        file.write( (const char *)&version, sizeof(quint32) );
    }

    // Write buffer length
    if ( file.good() )
    {
        file.write( (const char*)&data_size, sizeof(quint32) );
    }
    // Write OSL correction coefficients
    if ( file.good() )
    {
        file.write( (const char*) m_osl_correction, data_size );
    }

    if ( !file.good() )
    {
        fs::remove( (const char *)fileName );

        qDebug() << "Failed to save OSL correction file.";
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // Flush file buffers 
    file.flush();

    return AAPI_SUCCESS;
}

int AAPiCalibrator::load_hwe_correction_file()
{
    std::ifstream file( get_hwe_correction_file_name(),
                       std::ios::in | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to open HW error correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Read serialized data from the file

    quint32 magic, version;
    quint32 length;
    size_t  data_size = sizeof( m_hwe_correction );

    // Read 4-byte magic
    file.read( (char*)&magic, sizeof(quint32) );
    if ( file.good() )
    {
        // Read 4-byte version
        file.read( (char*)&version, sizeof(quint32) );
    }

    if ( file.good()    &&
         HWE_CORR_FILE_MAGIC   == magic &&
         HWE_CORR_FILE_VERSION == version )
    {
        // Read 4-byte length 
        file.read( (char *) &length, sizeof(quint32) );

        if ( file.good()    &&
             length == data_size )
        {
            // Read correction entries
            file.read( (char *) m_hwe_correction, data_size );

            if ( file.good() )
            {
                m_hwe_status = AAPI_CAL_FILE_VALID;

                qDebug() << "Successfully loaded HW error correction file.";
                return AAPI_SUCCESS;
            }
        }
    }

    // Error correction buffer is invalid
    m_hwe_status = AAPI_CAL_FILE_INVALID;

    qDebug() << "Invalid HW error correction file";
    return AAPI_CAL_E_FILE_INVALID;
}

int AAPiCalibrator::flush_hwe_correction_file()
{
    if( m_hwe_status != AAPI_CAL_FILE_VALID )
    {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    AAPiString fileName = get_hwe_correction_file_name( );

    std::ofstream file( fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to create HW error correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Will serialize the data into a file
    quint32 data_size = sizeof( m_hwe_correction );
    quint32 magic = static_cast<quint32> ( HWE_CORR_FILE_MAGIC );
    quint32 version = static_cast<quint32> ( HWE_CORR_FILE_VERSION );
    int ret;

    // Write a header with a "magic number" and a version
    file.write( (const char *)&magic, sizeof(quint32) );

    if ( file.good() )
    {
        file.write( (const char *)&version, sizeof(quint32) );
    }

    // Write the buffer length
    if ( file.good() )
    {
        file.write( (const char *)&data_size, sizeof(quint32) );
    }
    // Write error correction coefficients
    if ( file.good() )
    {
        file.write( (const char *) m_hwe_correction, data_size );
    }

    if ( !file.good() )
    {
        fs::remove( (const char *)fileName );

        qDebug() << "Failed to save HW error correction file.";
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // Flush file buffers 
    file.flush();

    return AAPI_SUCCESS;
}

static const AAPiComplex Z_1    = AAPiComplex( 1.f, 0.f );
static const AAPiComplex Z_N1   = AAPiComplex( -1.f, 0.f );


int AAPiCalibrator::calc_osl_entries()
{
    if( !is_valid_file( m_osl_file ) )
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if( ( m_osl_status & AAPI_CAL_FILE_FULLY_SCANNED ) != AAPI_CAL_FILE_FULLY_SCANNED )
    {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    double r_load, r_short, r_open;
    double r0;

    r0 = m_config->get_base_r0(); // System-wide base impedance, usually 50 Ohm
    r_load = m_config->get_osl_r_load();
    r_short = m_config->get_osl_r_short();
    r_open = m_config->get_osl_r_open();

    AAPiComplex g_load( ( r_load - r0 ) / ( r_load + r0 ),  0.0f);
    AAPiComplex g_short(( r_short - r0 )/ ( r_short + r0 ), 0.0f);
    AAPiComplex g_open( ( r_open - r0 ) / ( r_open + r0 ),  0.0f);

    /* Calculate calibration coefficients from measured reflection coefficients */
    for( uint i = 0; i < AAPI_CAL_NUM_ENTRIES; i++ )
    {
        OSLCorrectionEntry& ent = m_osl_correction [i];
        AAPiComplex result[3];
        AAPiComplex ent_g_short(ent.g_short);
        AAPiComplex ent_g_load(ent.g_load);
        AAPiComplex ent_g_open(ent.g_open);
        AAPiComplex a12 = g_short * ent_g_short;
        AAPiComplex a22 = g_load * ent_g_load;
        AAPiComplex a32 = g_open * ent_g_open;
        AAPiComplex a13 = Z_N1 * g_short;
        AAPiComplex a23 = Z_N1 * g_load;
        AAPiComplex a33 = Z_N1 * g_open;
        MathUtils::cramers_rule( Z_1, a12, a13, ent_g_short,
                                 Z_1, a22, a23, ent_g_load,
                                 Z_1, a32, a33, ent_g_open, result);
        ent.e_00 = result[0].__rep();
        ent.e_11 = result[1].__rep();
        ent.d_e  = result[2].__rep();
    }

    // Set valid status now 
    m_osl_status = AAPI_CAL_FILE_VALID;

    return AAPI_SUCCESS;
}

void AAPiCalibrator::correct_hwe(uint32_t freq, double *mag_ratio, double *phas_diff)
{
    int i = index_by_freq( freq );
    if( i < 0 )
    {
        return;
    }

    if( m_hwe_status != AAPI_CAL_FILE_VALID )
    {
        return;
    }

    *mag_ratio *= m_hwe_correction [i].mag_0;
    *phas_diff -= m_hwe_correction [i].phas_0;
}

int AAPiCalibrator::correct_gamma(uint32_t freq, AAPiComplex& gamma)
{
    if( ! AAPI_IS_FREQ_INBAND( freq ) )
    {
        /*We can't analyze on out-of-band frequencies */
        return AAPI_CAL_E_FREQ_OUT_OF_RANGE;
    }

    int f = get_osl_file();
    AAPiComplex res;
    int ret, i;
    OSLCorrectionEntry ent;
    OSLCorrectionEntry *entries = m_osl_correction;

    if(! is_valid_file( f ))
    {
        f = m_config->get_osl_selected();
    }
    if(! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if( !(get_osl_status() & AAPI_CAL_FILE_VALID) )
    {
        ret = load_osl_correction_file( f );
        if (AAPI_FAILED( ret ))
        {
            return ret;
        }
    }
    
    i = ( freq - AAPI_BAND_FMIN ) / AAPI_CAL_SCAN_STEP; // Nearest lower OSL entry
    if( 0 == ( freq % AAPI_CAL_SCAN_STEP ) )
    {
        // We already have exact value for the freq 
        ent = entries[ i ];
    }
    else if( i == 0 )
    {
        // Linear interpolate 2 OSL factors from 2 nearby entries 
        double prop = static_cast<double>( freq - AAPI_BAND_FMIN ) / AAPI_CAL_SCAN_STEP;
        ent.e_00 = ( entries[1].e_00 - entries[0].e_00 ) * prop + entries[0].e_00;
        ent.e_11 = ( entries[1].e_11 - entries[0].e_11 ) * prop + entries[0].e_11;
        ent.d_e  = ( entries[1].d_e - entries[0].d_e ) * prop + entries[0].d_e;
    }
    else
    {
        // We have 3 OSL points near freq, use parabolic interpolation 
        double f1, f2, f3;
        f1 = static_cast<double>( i - 1 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FMIN;
        f2 = static_cast<double>( i + 0 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FMIN;
        f3 = static_cast<double>( i + 1 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FMIN;

        ent.e_00 = MathUtils::parabolic_interpolate( AAPiComplex(entries[i-1].e_00),
                                                    AAPiComplex(entries[i].e_00),
                                                    AAPiComplex(entries[i+1].e_00),
                                                    f1, f2, f3,
                                                    static_cast<double>(freq)).__rep();

        ent.e_11 = MathUtils::parabolic_interpolate( AAPiComplex(entries[i-1].e_11),
                                                    AAPiComplex(entries[i].e_11),
                                                    AAPiComplex(entries[i+1].e_11),
                                                    f1, f2, f3,
                                                    static_cast<double>(freq)).__rep();

        ent.d_e = MathUtils::parabolic_interpolate( AAPiComplex(entries[i-1].d_e),
                                                    AAPiComplex(entries[i].d_e),
                                                    AAPiComplex(entries[i+1].d_e),
                                                    f1, f2, f3,
                                                    static_cast<double>(freq)).__rep();
    }

    res = ( gamma * AAPiComplex( ent.e_11 )) - AAPiComplex( ent.d_e );
    res = ( gamma * AAPiComplex( ent.e_00 )) / ( MathUtils::_cnonz( res ));

    //res = ( gamma.__rep() * ent.e_11 ) - ( ent.d_e );
    //res = ( gamma.__rep() - ent.e_00 ) / ( AAPIMathUtils::_cnonz( res ).__rep() );

    gamma = res;

    return 0;
}

int AAPiCalibrator::correct_z(uint32_t freq, AAPiComplex& z)
{
    double r0;
    int ret;
    AAPiComplex gamma;

    r0 = m_config->get_base_r0();
    gamma = gamma_from_z( z, r0 );

    ret = correct_gamma( freq, gamma );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    if (gamma.real() > 1.)
    {
        gamma = AAPiComplex( 1., gamma.imag() );
    }
    else if (gamma.real() < -1.)
    {
        gamma = AAPiComplex( -1., gamma.imag() );
    }

    if (gamma.imag() > 1.)
    {
        gamma = AAPiComplex( gamma.real(), 1. );
    }
    else if (gamma.imag() < -1.)
    {
        gamma = AAPiComplex( gamma.real(), -1. );
    }

    z = z_from_gamma( gamma, r0 );

    return 0;
}

uint32_t AAPiCalibrator::freq_by_index(size_t index)
{
    if( index >= AAPI_CAL_NUM_ENTRIES )
    {
        return 0;
    }

    return AAPI_BAND_FMIN + static_cast<uint32_t>( index * AAPI_CAL_SCAN_STEP );
}

int AAPiCalibrator::index_by_freq(uint32_t freq)
{
    if( ! AAPI_IS_FREQ_INBAND( freq ) )
    {
        return -1;
    }

    return std::round( static_cast<double>(freq) / AAPI_CAL_SCAN_STEP ) - ( AAPI_BAND_FMIN / AAPI_CAL_SCAN_STEP );
}

AAPiString AAPiCalibrator::get_calibration_dir(bool generate)
{
    if( !generate && m_dir.length() != 0 )
    {
        return m_dir;
    }

    fs::path dirPath = (const char *)AAPiConfig::get_config_dir();
    dirPath += "calibration";

    return dirPath.c_str();
}

AAPiString AAPiCalibrator::get_osl_correction_file_name(int osl_file)
{
    fs::path filePath = (const char *)get_calibration_dir();
    AAPiString fileName = AAPiString("osl_") + AAPiVariant( osl_file ).toString() + ".corr";
    filePath += (const char *)fileName;

    return filePath.c_str();
}

AAPiString AAPiCalibrator::get_hwe_correction_file_name()
{
    fs::path filePath = (const char *)get_calibration_dir();
    filePath += "hw_err.corr";

    return filePath.c_str();
}

AAPiComplex AAPiCalibrator::gamma_from_z(const AAPiComplex& z, double r0)
{
    AAPiComplex z0( r0, 0. );
    AAPiComplex g = ( z - z0 ) / ( z + z0 );

    if( std::isnan( g.real() ) || std::isnan( g.imag() ) )
    {
        g = AAPiComplex( 0.99999999f, 0. );
    }
    return g;
}

AAPiComplex AAPiCalibrator::z_from_gamma(const AAPiComplex& gamma, double r0)
{
    double gr2, gi2;
    double dg, re, im;

    gr2 = std::pow( gamma.real(), 2 );
    gi2 = std::pow( gamma.imag(), 2 );
    dg = MathUtils::_nonz( std::pow( 1. - gamma.real(), 2 ) + gi2 );

    re = r0 * ( 1. - gr2 - gi2 ) / dg;
    if( re < 0. )
    {
        // This may happen due to limited computational accuracy 
        re = 0.;
    }
    
    im = r0 * 2. * gamma.imag() / dg;

    return AAPiComplex( re, im );
}

} //namespace aapi
