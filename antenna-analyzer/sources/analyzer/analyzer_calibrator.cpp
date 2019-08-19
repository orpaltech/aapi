/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <memory>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include "analyzer_calibrator.h"
#include "analyzer/math_utils.h"

///////////////////////////////////////////////////////////////////////////////
// OSL definitions
///////////////////////////////////////////////////////////////////////////////

#define OSL_CORR_FILE_MAGIC         0xE0B07010
#define OSL_CORR_FILE_VERSION       4

#define HWERR_CORR_FILE_MAGIC       0xE0B07020
#define HWERR_CORR_FILE_VERSION     4

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPICalibrator implementation
///////////////////////////////////////////////////////////////////////////////

QString AAPICalibrator::m_dir = AAPICalibrator::get_calibration_dir(true);

AAPICalibrator *AAPICalibrator::create(AAPIConfig* config, bool add_ref)
{
    AAPICalibrator *obj = create(add_ref);
    if( obj ) {
        obj->m_config = config;

        AAPI_ADDREF(config);
    }
    return obj;
}

AAPICalibrator::AAPICalibrator()
    : m_config(nullptr)
    , m_osl_file(-1)
    , m_osl_status(CALIB_FILE_INVALID)
    , m_hwerror_status(CALIB_FILE_INVALID)
{
}

AAPICalibrator::~AAPICalibrator()
{
    AAPI_DISPOSE(m_config);
}

int AAPICalibrator::init()
{
    int ret;

    m_osl_file      = m_config->get_osl_selected();
    m_osl_status    = CALIB_FILE_INVALID;
    m_hwerror_status = CALIB_FILE_INVALID;

    QDir dir( get_calibration_dir() );
    if (! dir.exists() )
    {
        if (! dir.mkpath( get_calibration_dir() ))
            return AAPI_E_CREATE_DIR_FAILED;
    }

    // load OSL correction file 
    ret = load_osl_correction_file( m_osl_file );
    if ( AAPI_FAILED( ret ))
    {
        // Ignore error code
    }

    // load hardware error correction file
    ret = load_hwerror_correction_file();
    if ( AAPI_FAILED( ret ))
    {
        // Ignore error code
    }

    return AAPI_SUCCESS;
}

int AAPICalibrator::set_osl_file(int f)
{
    if (! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( m_osl_status != f )
    {
        // Mark invalid as the file has changed 
        m_osl_status    &= ~CALIB_FILE_VALID;
        m_osl_file      = f;
    }

    return AAPI_SUCCESS;
}

int AAPICalibrator::set_osl_z(uint8_t scanType, int index, const std::complex<float>& z)
{
    float r0;

    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( index < 0 || index >= CALIB_NUM_ENTRIES )
    {
        return AAPI_E_INVALID_ARG;
    }

    r0 = m_config->get_base_r0();

    switch (scanType)
    {
    case CALIB_FILE_SHORT_SCANNED:
        m_osl_correction_entries [index].g_short = gamma_from_z( z, r0 ).__rep();
        break;

    case CALIB_FILE_LOAD_SCANNED:
        m_osl_correction_entries [index].g_load = gamma_from_z( z, r0 ).__rep();
        break;

    case CALIB_FILE_OPEN_SCANNED:
        m_osl_correction_entries [index].g_open = gamma_from_z( z, r0 ).__rep();
        break;

    default:
        return AAPI_E_INVALID_ARG;
    }

    return AAPI_SUCCESS;
}

int AAPICalibrator::scan_osl_begin(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Clear status flags 
    m_osl_status    &= ~CALIB_FILE_VALID;
    m_osl_status    &= ~scan_type;

    return 0;
}

int AAPICalibrator::scan_osl_ended(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Set flags 
    m_osl_status    |= scan_type;

    return 0;
}

void AAPICalibrator::scan_hwerror_begin()
{
    // Set correction status to invalid in the beginning
    m_hwerror_status = CALIB_FILE_INVALID;
}

int AAPICalibrator::set_hwerror_entry(int index, float mag0, float phas0)
{
    if ( index < 0 || index >= CALIB_NUM_ENTRIES )
    {
        return AAPI_E_INVALID_ARG;
    }

    m_hwerror_correction_entries [index].mag_0 = mag0;
    m_hwerror_correction_entries [index].phas_0 = phas0;

    return 0;
}

void AAPICalibrator::scan_hwerror_ended()
{
    m_hwerror_status = CALIB_FILE_VALID;
}

bool AAPICalibrator::osl_file_exists(int f) const
{
    if (! is_valid_file( f ))
    {
        return false;
    }

    QFile file( get_osl_correction_file_name( f ));
    return file.exists();
}

int AAPICalibrator::load_osl_correction_file(int f)
{
    if (! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    QFile file( get_osl_correction_file_name( f ) );
    if (! file.open( QIODevice::ReadOnly ) )
    {
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Read serialized data from the file
    QDataStream in( &file );
    // Read and check the header 
    quint32 magic, version;
    quint32 len;
    int ret;
    in >> magic;
    in >> version;

    if ( OSL_CORR_FILE_MAGIC    == magic	&&
         OSL_CORR_FILE_VERSION  == version )
    {
        // Read 4-byte length
        ret = in.readRawData( (char*)&len, sizeof(quint32) );
        if ( ret == sizeof( quint32 ) &&
             len == sizeof( m_osl_correction_entries ) )
        {
            // read OSL entries 
            ret = in.readRawData( (char*) m_osl_correction_entries, sizeof( m_osl_correction_entries ) );
            if ( ret == len )
            {
                // Successfully loaded entries 
                m_osl_status = CALIB_FILE_VALID;
                m_osl_file  = f;
                return AAPI_SUCCESS;
            }
            else
            {
                // OSL entries buffer is invalid 
                m_osl_status = CALIB_FILE_INVALID;
                m_osl_file  = -1;
            }
        }
    }

    // bad file format 
    return AAPI_CAL_E_FILE_INVALID;
}

bool AAPICalibrator::is_valid_file(int f)
{
    return CALIB_IS_VALID_FILE( f );
}

int AAPICalibrator::save_osl_correction_file()
{
    if (! is_valid_file( m_osl_file ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( m_osl_status != CALIB_FILE_VALID )
    {
        return AAPI_CAL_E_NOT_CALCULATED;
    }

    QFile file( get_osl_correction_file_name( m_osl_file ) );
    if (! file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    QDataStream out(&file); // we will serialize the data into the file
    quint32 len;
    int ret;

    // Write a header with a "magic number" and a version 
    out << static_cast<quint32> ( OSL_CORR_FILE_MAGIC );
    out << static_cast<quint32> ( OSL_CORR_FILE_VERSION );

    // Write buffer length 
    len = sizeof( m_osl_correction_entries );
    ret = out.writeRawData( reinterpret_cast<const char*>(&len), sizeof(quint32) );
    if ( ret != sizeof( quint32 ) )
    {
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // Write OSL correction coefficients 
    ret = out.writeRawData( reinterpret_cast<const char*>( m_osl_correction_entries ), len );
    if ( ret != len )
    {
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // Flush file buffers 
    file.flush();

    return AAPI_SUCCESS;
}

int AAPICalibrator::load_hwerror_correction_file()
{
    QFile file( get_hwerror_correction_file_name() );
    if(! file.open( QIODevice::ReadOnly ) )
    {
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    /*Read serialized data from the file*/
    QDataStream in( &file );
    /* Read and check the header */
    quint32 magic, version, len;
    int ret;
    in >> magic;
    in >> version;

    if( HWERR_CORR_FILE_MAGIC   == magic    &&
        HWERR_CORR_FILE_VERSION == version )
    {
        // Read 4-byte length 
        ret = in.readRawData( reinterpret_cast<char*>(&len), sizeof(quint32) );
        if ( ret == sizeof( quint32 ) &&
             len == sizeof( m_hwerror_correction_entries ) )
        {
            // read correction entries 
            ret = in.readRawData( reinterpret_cast<char *>( m_hwerror_correction_entries ), len );
            if ( ret == len )
            {
                // Error corrections array loaded 
                m_hwerror_status = CALIB_FILE_VALID;
                return AAPI_SUCCESS;
            }
            else
            {
                // Error correction buffer is invalid 
                m_hwerror_status = CALIB_FILE_INVALID;
            }
        }
    }

    // bad file format
    return AAPI_CAL_E_FILE_INVALID;
}

int AAPICalibrator::save_hwerror_correction_file()
{
    if( m_hwerror_status != CALIB_FILE_VALID )
    {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    QFile file( get_hwerror_correction_file_name() );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    QDataStream out( &file ); // we will serialize the data into the file
    quint32 len;
    int ret;

    // Write a header with a "magic number" and a version
    out << static_cast<quint32> ( HWERR_CORR_FILE_MAGIC );
    out << static_cast<quint32> ( HWERR_CORR_FILE_VERSION );

    // output buffer length 
    len = sizeof( m_hwerror_correction_entries );
    ret = out.writeRawData( reinterpret_cast<const char *>(&len), sizeof(quint32) );
    if( ret != sizeof(quint32) )
    {
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // flush error correction coefficients 
    ret = out.writeRawData( reinterpret_cast<const char *>( m_hwerror_correction_entries ), len );
    if( ret != len )
    {
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    // Flush file buffers 
    file.flush();

    return 0;
}

static const std::complex<float> Z_1    = std::complex<float>( 1.f, 0.f );
static const std::complex<float> Z_N1   = std::complex<float>( -1.f, 0.f );


int AAPICalibrator::calc_osl_entries()
{
    if( !is_valid_file( m_osl_file ) )
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if( ( m_osl_status & CALIB_FILE_FULLY_SCANNED ) != CALIB_FILE_FULLY_SCANNED )
    {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    float r_load, r_short, r_open;
    float r0;

    r0 = m_config->get_base_r0(); // System-wide base impedance, usually 50 Ohm
    r_load = m_config->get_osl_r_load();
    r_short = m_config->get_osl_r_short();
    r_open = m_config->get_osl_r_open();

    std::complex<float> g_load( ( r_load - r0 ) / ( r_load + r0 ),  0.0f);
    std::complex<float> g_short(( r_short - r0 )/ ( r_short + r0 ), 0.0f);
    std::complex<float> g_open( ( r_open - r0 ) / ( r_open + r0 ),  0.0f);

    /* Calculate calibration coefficients from measured reflection coefficients */
    for( uint i = 0; i < CALIB_NUM_ENTRIES; i++ )
    {
        OSLCorrectionEntry& ent = m_osl_correction_entries [i];
        std::complex<float> result[3];
        std::complex<float> ent_g_short(ent.g_short);
        std::complex<float> ent_g_load(ent.g_load);
        std::complex<float> ent_g_open(ent.g_open);
        std::complex<float> a12 = g_short * ent_g_short;
        std::complex<float> a22 = g_load * ent_g_load;
        std::complex<float> a32 = g_open * ent_g_open;
        std::complex<float> a13 = Z_N1 * g_short;
        std::complex<float> a23 = Z_N1 * g_load;
        std::complex<float> a33 = Z_N1 * g_open;
        AAPIMathUtils::cramers_rule( Z_1, a12, a13, ent_g_short,
                                     Z_1, a22, a23, ent_g_load,
                                     Z_1, a32, a33, ent_g_open, result);
        ent.e_00 = result[0].__rep();
        ent.e_11 = result[1].__rep();
        ent.d_e  = result[2].__rep();
    }

    // Set valid status now 
    m_osl_status = CALIB_FILE_VALID;

    return AAPI_SUCCESS;
}

void AAPICalibrator::correct_hwerror(uint32_t freq, float *mag_ratio, float *phas_diff)
{
    int i = index_by_freq( freq );
    if( i < 0 )
    {
        return;
    }

    if( m_hwerror_status != CALIB_FILE_VALID )
    {
        return;
    }

    *mag_ratio *= m_hwerror_correction_entries[i].mag_0;
    *phas_diff -= m_hwerror_correction_entries[i].phas_0;
}

int AAPICalibrator::correct_gamma(uint32_t freq, std::complex<float>& gamma)
{
    if( AA_IS_FREQ_INBAND(freq) == false )
    {
        /*We can't analyze on out-of-band frequencies */
        return AAPI_CAL_E_FREQ_OUT_OF_RANGE;
    }

    int f = get_osl_file();
    std::complex<float> res;
    int ret, i;
    OSLCorrectionEntry ent;
    OSLCorrectionEntry *entries = m_osl_correction_entries;

    if(! is_valid_file( f ))
    {
        f = m_config->get_osl_selected();
    }
    if(! is_valid_file( f ))
    {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if( !(get_osl_status() & CALIB_FILE_VALID) )
    {
        ret = load_osl_correction_file( f );
        if( AAPI_FAILED( ret ) )
        {
            return ret;
        }
    }
    
    i = ( freq - AA_BAND_FMIN ) / CALIB_SCAN_STEP; // Nearest lower OSL entry
    if( 0 == ( freq % CALIB_SCAN_STEP ) )
    {
        // We already have exact value for the freq 
        ent = entries[ i ];
    }
    else if( i == 0 )
    {
        // Linear interpolate 2 OSL factors from 2 nearby entries 
        float prop = static_cast<float>( freq - AA_BAND_FMIN ) / CALIB_SCAN_STEP;
        ent.e_00 = ( entries[1].e_00 - entries[0].e_00 ) * prop + entries[0].e_00;
        ent.e_11 = ( entries[1].e_11 - entries[0].e_11 ) * prop + entries[0].e_11;
        ent.d_e  = ( entries[1].d_e - entries[0].d_e ) * prop + entries[0].d_e;
    }
    else
    {
        // We have 3 OSL points near freq, use parabolic interpolation 
        float f1, f2, f3;
        f1 = static_cast<float>( i - 1 ) * CALIB_SCAN_STEP + AA_BAND_FMIN;
        f2 = static_cast<float>( i + 0 ) * CALIB_SCAN_STEP + AA_BAND_FMIN;
        f3 = static_cast<float>( i + 1 ) * CALIB_SCAN_STEP + AA_BAND_FMIN;

        ent.e_00 = AAPIMathUtils::parabolic_interpolate( entries[i-1].e_00,
                                                        entries[i].e_00,
                                                        entries[i+1].e_00,
                                                        f1, f2, f3, 
                                                        static_cast<float>(freq)).__rep();

        ent.e_11 = AAPIMathUtils::parabolic_interpolate( entries[i-1].e_11,
                                                        entries[i].e_11,
                                                        entries[i+1].e_11,
                                                        f1, f2, f3, 
                                                        static_cast<float>(freq)).__rep();

        ent.d_e = AAPIMathUtils::parabolic_interpolate( entries[i-1].d_e,
                                                        entries[i].d_e,
                                                        entries[i+1].d_e,
                                                        f1, f2, f3, 
                                                        static_cast<float>(freq)).__rep();
    }

    res = ( gamma * std::complex<float>( ent.e_11 )) - std::complex<float>( ent.d_e );
    res = ( gamma * std::complex<float>( ent.e_00 )) / ( AAPIMathUtils::_cnonz( res ));

    //res = ( gamma.__rep() * ent.e_11 ) - ( ent.d_e );
    //res = ( gamma.__rep() - ent.e_00 ) / ( AAPIMathUtils::_cnonz( res ).__rep() );

    gamma = res;

    return 0;
}

int AAPICalibrator::correct_z(uint32_t freq, std::complex<float>& z)
{
    float r0;
    int ret;
    std::complex<float> gamma;

    r0 = m_config->get_base_r0();
    gamma = gamma_from_z( z, r0 );

    ret = correct_gamma( freq, gamma );
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    if( gamma.real() > 1.f )
    {
        gamma = std::complex<float>( 1.f, gamma.imag() );
    }
    else if( gamma.real() < -1.f )
    {
        gamma = std::complex<float>( -1.f, gamma.imag() );
    }

    if( gamma.imag() > 1.f )
    {
        gamma = std::complex<float>( gamma.real(), 1.f );
    }
    else if( gamma.imag() < -1.f )
    {
        gamma = std::complex<float>( gamma.real(), -1.f );
    }

    z = z_from_gamma( gamma, r0 );
    return 0;
}

uint32_t AAPICalibrator::freq_by_index(size_t index)
{
    if( index >= CALIB_NUM_ENTRIES )
    {
        return 0;
    }

    return AA_BAND_FMIN + static_cast<uint32_t>( index * CALIB_SCAN_STEP );
}

int AAPICalibrator::index_by_freq(uint32_t freq)
{
    if( AA_IS_FREQ_INBAND( freq ) == false )
    {
        return -1;
    }

    return std::round( static_cast<float>(freq) / CALIB_SCAN_STEP ) - ( AA_BAND_FMIN / CALIB_SCAN_STEP );
}

QString AAPICalibrator::get_calibration_dir(bool generate)
{
    if( !generate && !( m_dir.isEmpty() || m_dir.isNull() ) )
    {
        return m_dir;
    }
    return QDir::cleanPath( AAPIConfig::get_config_dir() + QDir::separator() + QString("calibration") );
}

QString AAPICalibrator::get_osl_correction_file_name(int osl_file)
{
    return QDir::cleanPath( get_calibration_dir() + QDir::separator() + QString("osl_correction_%1").arg(osl_file) );
}

QString AAPICalibrator::get_hwerror_correction_file_name()
{
    return QDir::cleanPath( get_calibration_dir() + QDir::separator() + QString("hw_error_correction") );
}

std::complex<float> AAPICalibrator::gamma_from_z(const std::complex<float>& z, float r0)
{
    std::complex<float> z0( r0, 0.f );
    std::complex<float> g = ( z - z0 ) / ( z + z0 );

    if( std::isnan( g.real() ) || std::isnan( g.imag() ) )
    {
        g = std::complex<float>( 0.99999999f, 0.f );
    }
    return g;
}

std::complex<float> AAPICalibrator::z_from_gamma(const std::complex<float>& gamma, float r0)
{
    float gr2, gi2;
    float dg, re, im;

    gr2 = std::pow( gamma.real(), 2 );
    gi2 = std::pow( gamma.imag(), 2 );
    dg = AAPIMathUtils::_nonz( std::pow( 1.f - gamma.real(), 2 ) + gi2 );

    re = r0 * ( 1.f - gr2 - gi2 ) / dg;
    if( re < 0.f )
    {
        // This may happen due to limited computational accuracy 
        re = 0.f;
    }
    
    im = r0 * 2.f * gamma.imag() / dg;

    return std::complex<float>( re, im );
}

} //namespace aapi
