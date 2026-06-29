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
#include <QDebug>

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
// class AAPICalibrator
///////////////////////////////////////////////////////////////////////////////
/// \brief AAPiCalibrator::create
/// \param addRef
/// \return
///

IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(AAPiCalibrator)


AAPiCalibrator::AAPiCalibrator()
    : m_osl_file(-1)
    , m_osl_status(0)
    , m_hw_err_status(0)
{
}

AAPiCalibrator::~AAPiCalibrator()
{
}

AAPiError AAPiCalibrator::init()
{
    m_osl_file      = m_config->get_osl_file_selected();
    m_osl_status    = FILE_STATUS_INVALID;
    m_hw_err_status = FILE_STATUS_INVALID;

    const fs::path dirPath( (const char *)get_calibration_dir() );
    if (! fs::exists( dirPath )) {
        if (! fs::create_directory( dirPath )) {
            qDebug() << "Failed to create directory.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    // load OSL correction file
    AAPiError ret = load_osl_correction_file( m_osl_file );
    if ( AAPI_FAILED( ret )) {
        // Ignore error code
    }

    // load hardware error correction file
    ret = load_hw_err_correction_file();
    if (AAPI_FAILED( ret )) {
        // Ignore error code
    }

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::set_osl_file(int file)
{
    if (! is_valid_file( file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if ( m_osl_file != file ) {
        // Invalidate state as the file has changed
        m_osl_status    &= ~FILE_STATUS_VALID;
        m_osl_file      = file;
    }

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::set_osl_z(uint8_t scan_type, int index, const AAPiComplex& z)
{
    if (! is_valid_file( m_osl_file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if (index < 0 || index >= NUM_ENTRIES) {
        return AAPI_E_INVALID_ARG;
    }

    double r0 = m_config->get_base_r0();
    auto gamma = gamma_from_z( z, r0 ).__rep();

    switch ((FileStatus)scan_type) {
    case FileStatus::SCAN_SHORT:
        m_osl_correction [index].g_short = gamma;
        break;

    case FileStatus::SCAN_LOAD:
        m_osl_correction [index].g_load = gamma;
        break;

    case FileStatus::SCAN_OPEN:
        m_osl_correction [index].g_open = gamma;
        break;

    default:
        return AAPI_E_INVALID_ARG;
    }

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::osl_scan_begin(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Must clear the global valid status and this particualar scan type
    uint8_t remove_mask = ( scan_type | FILE_STATUS_VALID );

    // Invalidate state before starting a new scan
    m_osl_status    &= ~remove_mask;

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::osl_scan_finalize(uint8_t scan_type)
{
    if (! is_valid_file( m_osl_file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Update status to indicate that this particular scan type has completed
    m_osl_status    |= scan_type;

    return AAPI_SUCCESS;
}

void AAPiCalibrator::hw_err_scan_begin()
{
    // Set correction status to invalid in the beginning
    m_hw_err_status = FILE_STATUS_INVALID;
}

AAPiError AAPiCalibrator::set_hw_err_entry(int index, double mag0, double phas0)
{
    if ( index < 0 || index >= NUM_ENTRIES ) {
        return AAPI_E_INVALID_ARG;
    }

    m_hw_err_correction [index].mag_0 = mag0;
    m_hw_err_correction [index].phas_0 = phas0;

    return AAPI_SUCCESS;
}

void AAPiCalibrator::hw_err_scan_finalize()
{
    // For hardware error correction we simply set status to valid
    // as soon as then scan is finished
    //
    m_hw_err_status = FILE_STATUS_VALID;
}

bool AAPiCalibrator::osl_file_exists(int f) const
{
    if (! is_valid_file( f )) {
        return false;
    }

    const fs::path filePath( (const char *)get_osl_correction_file_name(f) );
    return fs::exists( filePath );
}

AAPiError AAPiCalibrator::load_osl_correction_file(int f)
{
    if (! is_valid_file( f )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    std::ifstream file( (const char *)get_osl_correction_file_name(f), std::ios::in | std::ios::binary);
    if (! file.is_open( )) {
        qDebug() << "Unable to open OSL correction file: " << f;
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    quint32 magic = 0, version = 0, length = 0;

    // Read and validate the file header parameters sequentially
    if (file.read(reinterpret_cast<char*>(&magic), sizeof(quint32)) &&
        file.read(reinterpret_cast<char*>(&version), sizeof(quint32)) &&
        file.read(reinterpret_cast<char*>(&length), sizeof(quint32))) {

        // Compute the expected unpadded pure data payload size
        // 3 complex structures per entry * 2 doubles * 8 bytes
        constexpr size_t entry_payload_size = sizeof(AAPiComplexType) * 3;
        constexpr size_t total_expected_payload = entry_payload_size * NUM_ENTRIES;

        if (OSL_CORR_FILE_MAGIC   == magic &&
            OSL_CORR_FILE_VERSION == version &&
            total_expected_payload == length) {

            // To isolate active calibration memory loops from partial disk corruption,
            // we read directly into a local stack/heap vector buffer first
            std::vector<OSLCorrectionEntry> temp_buffer(NUM_ENTRIES);
            bool read_success = true;

            for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
                if (!(file >> temp_buffer[i])) {
                    read_success = false;
                    break;
                }
            }

            // Unconditional success path: Apply safe data transfer element-wise
            if (read_success && file.good()) {
                for (uint i = 0; i < NUM_ENTRIES; ++i) {
                    m_osl_correction[i] = temp_buffer[i];
                }

                m_osl_status = FILE_STATUS_VALID;
                m_osl_file   = f;

                qDebug() << "Successfully loaded OSL correction file: " << f;
                return AAPI_SUCCESS;
            }
        }
    }

    // Fallback failure track: Explicitly invalidate active variables
    // and wipe data values to zero, neutralizing alignment boundaries cleanly
    m_osl_status = FILE_STATUS_INVALID;
    m_osl_file   = -1;
    for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
        m_osl_correction[i].e_00 = AAPiComplex(0.0, 0.0).__rep();
        m_osl_correction[i].e_11 = AAPiComplex(0.0, 0.0).__rep();
        m_osl_correction[i].d_e  = AAPiComplex(0.0, 0.0).__rep();
    }

    qDebug() << "Invalid or truncated OSL correction file: " << f;
    return AAPI_CAL_E_FILE_INVALID;
}

AAPiError AAPiCalibrator::flush_osl_correction_file()
{
    if (! is_valid_file( m_osl_file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }
    if (get_osl_status() != FILE_STATUS_VALID) {
        return AAPI_CAL_E_NOT_CALCULATED;
    }

    AAPiString finalFileName = get_osl_correction_file_name( m_osl_file );
    std::string tempFileName = std::string((const char*)finalFileName) + ".tmp";

    std::ofstream file( tempFileName, std::ios::out | std::ios::binary );
    if (! file.is_open( )) {
        qDebug() << "Unable to create temporary OSL correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Data payload size calculation matches the raw data volume excluding alignment gaps
    constexpr quint32 entry_payload_size = sizeof(AAPiComplexType) * 3;
    constexpr quint32 pure_data_size = entry_payload_size * NUM_ENTRIES;

    quint32 magic   = static_cast<quint32>( OSL_CORR_FILE_MAGIC );
    quint32 version = static_cast<quint32>( OSL_CORR_FILE_VERSION );

    // Serialize file headers safely
    file.write( reinterpret_cast<const char*>(&magic), sizeof(quint32) );
    file.write( reinterpret_cast<const char*>(&version), sizeof(quint32) );
    file.write( reinterpret_cast<const char*>(&pure_data_size), sizeof(quint32) );

    // Write your OSL calibration items one-by-one, stripping out compile padding
    for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
        file << m_osl_correction[i];
    }

    if ( !file.good() ) {
        file.close();
        fs::remove( tempFileName ); // Clear corrupt tracking fragments
        qDebug() << "Failed to save OSL correction data.";
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    file.flush();
    file.close();

    // Atomic exchange protects previous file blocks if disk failure occur
    std::error_code ec;
    fs::rename(tempFileName, (const char*)finalFileName, ec);
    if (ec) {
        qDebug() << "Failed to replace calibration file: " << ec.message().c_str();
        fs::remove(tempFileName);
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    qDebug() << "Successfully flushed OSL correction file: " << m_osl_file;
    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::load_hw_err_correction_file()
{
    std::ifstream file( (const char *)get_hw_err_correction_file_name(), std::ios::in | std::ios::binary);
    if (! file.is_open()) {
        qDebug() << "Unable to open HW error correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    quint32 magic = 0, version = 0, length = 0;

    // Read and validate the file header parameters sequentially
    if (file.read(reinterpret_cast<char*>(&magic), sizeof(quint32)) &&
        file.read(reinterpret_cast<char*>(&version), sizeof(quint32)) &&
        file.read(reinterpret_cast<char*>(&length), sizeof(quint32))) {

        // Compute the expected unpadded pure data payload size
        // 2 doubles per entry * 8 bytes
        constexpr size_t entry_payload_size = sizeof(double) * 2;
        constexpr size_t total_payload = entry_payload_size * NUM_ENTRIES;

        if (HWE_CORR_FILE_MAGIC   == magic &&
            HWE_CORR_FILE_VERSION == version &&
            total_payload == length) {

            // Read directly into a safe local vector buffer first
            // to protect active variables from partial disk corruption
            std::vector<HwErrCorrectionEntry> temp_buffer(NUM_ENTRIES);
            bool read_success = true;

            for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
                if (!(file >> temp_buffer[i])) {
                    read_success = false;
                    break;
                }
            }

            // Unconditional success path: Apply safe data transfer element-wise
            if (read_success && file.good()) {
                for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
                    m_hw_err_correction[i] = temp_buffer[i];
                }
                m_hw_err_status = FILE_STATUS_VALID;

                qDebug() << "Successfully loaded HW error correction file.";
                return AAPI_SUCCESS;
            }
        }
    }

    // Fallback failure track: Explicitly invalidate active variables
    // and wipe data values to zero safely, avoiding direct memset
    m_hw_err_status = FILE_STATUS_INVALID;
    for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
        m_hw_err_correction[i].mag_0  = 0.0;
        m_hw_err_correction[i].phas_0 = 0.0;
    }

    qDebug() << "Invalid or truncated HW error correction file";
    return AAPI_CAL_E_FILE_INVALID;
}

AAPiError AAPiCalibrator::flush_hw_err_correction_file()
{
    if (get_hw_err_status() != FILE_STATUS_VALID) {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    AAPiString finalFileName = get_hw_err_correction_file_name();
    std::string tempFileName = std::string((const char*)finalFileName) + ".tmp";

    // Open your temporary working file cleanly
    std::ofstream file( tempFileName, std::ios::out | std::ios::binary);
    if (! file.is_open()) {
        qDebug() << "Unable to create HW error correction file.";
        return AAPI_CAL_E_OPEN_FILE_FAILED;
    }

    // Data payload size calculation matches the raw data volume excluding alignment gaps
    constexpr quint32 entry_payload_size = sizeof(double) * 2;
    constexpr quint32 pure_data_size = entry_payload_size * NUM_ENTRIES;

    quint32 magic   = static_cast<quint32>(HWE_CORR_FILE_MAGIC);
    quint32 version = static_cast<quint32>(HWE_CORR_FILE_VERSION);

    // Serialize file headers safely
    file.write(reinterpret_cast<const char*>(&magic), sizeof(quint32));
    file.write(reinterpret_cast<const char*>(&version), sizeof(quint32));
    file.write(reinterpret_cast<const char*>(&pure_data_size), sizeof(quint32));

    // Write your HW calibration items one-by-one, stripping out compile padding
    for (uint32_t i = 0; i < NUM_ENTRIES; ++i) {
        file << m_hw_err_correction[i];
    }

    if (! file.good()) {
        file.close();
        fs::remove(tempFileName); // Clear corrupt tracking fragments
        qDebug() << "Failed to save HW error correction file.";
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    file.flush();
    file.close();

    // Atomic filesystem exchange protects previous files if disk drops occur
    std::error_code ec;
    fs::rename(tempFileName, (const char*)finalFileName, ec);
    if (ec) {
        qDebug() << "Failed to replace HW error file: " << ec.message().c_str();
        fs::remove(tempFileName);
        return AAPI_CAL_E_WRITE_FILE_FAILED;
    }

    return AAPI_SUCCESS;
}


static constexpr AAPiComplex Z_1    = AAPiComplex( 1.f, 0.f );
static constexpr AAPiComplex Z_N1   = AAPiComplex( -1.f, 0.f );


AAPiError AAPiCalibrator::calc_osl_entries()
{
    if (! is_valid_file( m_osl_file )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    // Check if we fully scanned (all 3 cases)
    if (! is_osl_fully_scanned ()) {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    double r0       = m_config->get_base_r0(); // System-wide base impedance, usually 50 Ohm
    double r_load   = m_config->get_osl_r_load();
    double r_short  = m_config->get_osl_r_short();
    double r_open   = m_config->get_osl_r_open();

    AAPiComplex g_load (( r_load  - r0 ) / ( r_load  + r0 ), 0.0f );
    AAPiComplex g_short(( r_short - r0 ) / ( r_short + r0 ), 0.0f );
    AAPiComplex g_open (( r_open  - r0 ) / ( r_open  + r0 ), 0.0f );

    // Calculate calibration coefficients from measured reflection coefficients
    for (uint i = 0; i < NUM_ENTRIES; ++i) {

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
    m_osl_status = FILE_STATUS_VALID;

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::correct_hw_err(uint32_t freq, double& mag_ratio, double& phas_diff)
{
    int i = get_index_by_freq( freq );
    if ( i < 0 ) {
        return AAPI_CAL_E_FREQ_OUT_OF_RANGE;
    }

    if (get_hw_err_status() != FILE_STATUS_VALID ) {
        return AAPI_CAL_E_SCAN_INCOMPLETE;
    }

    // Perform the correction
    mag_ratio *= m_hw_err_correction [i].mag_0;
    phas_diff -= m_hw_err_correction [i].phas_0;

    return AAPI_SUCCESS;
}

AAPiError AAPiCalibrator::correct_gamma(uint32_t freq, AAPiComplex& gamma)
{
    if ( ! AAPiConfig::is_freq_in_band( freq ) ) {
        // We can't analyze on out-of-band frequencies
        return AAPI_CAL_E_FREQ_OUT_OF_RANGE;
    }

    int f = get_osl_file();
    AAPiComplex res;
    int ret;
    OSLCorrectionEntry ent;
    OSLCorrectionEntry *entries = m_osl_correction;

    if (! is_valid_file( f )) {
        f = m_config->get_osl_file_selected();
    }
    if (! is_valid_file( f )) {
        return AAPI_CAL_E_FILE_NOT_SELECTED;
    }

    if (get_osl_status() != FILE_STATUS_VALID) {

        ret = load_osl_correction_file( f );
        if (AAPI_FAILED( ret )) {
            return ret;
        }
    }
    
    int i = ( freq - AAPI_BAND_FREQ_MIN ) / AAPI_CAL_SCAN_STEP; // Nearest lower OSL entry

    if (0 == ( freq % AAPI_CAL_SCAN_STEP )) {

        // We already have exact value for the freq 
        ent = entries[ i ];

    } else if ( i == 0 ) {
        // Linear interpolate 2 OSL factors from 2 nearby entries
        double prop = static_cast<double>( freq - AAPI_BAND_FREQ_MIN ) / AAPI_CAL_SCAN_STEP;

        // Convert to AAPiComplex for the math, then call .__rep() to store back into AAPiComplexType
        ent.e_00 = (( AAPiComplex(entries[1].e_00) - AAPiComplex(entries[0].e_00) ) * prop + AAPiComplex(entries[0].e_00)).__rep();
        ent.e_11 = (( AAPiComplex(entries[1].e_11) - AAPiComplex(entries[0].e_11) ) * prop + AAPiComplex(entries[0].e_11)).__rep();
        ent.d_e  = (( AAPiComplex(entries[1].d_e) - AAPiComplex(entries[0].d_e) ) * prop + AAPiComplex(entries[0].d_e)).__rep();

    } else {
        // We have 3 OSL points near freq, use parabolic interpolation 
        double f1, f2, f3;
        f1 = static_cast<double>( i - 1 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FREQ_MIN;
        f2 = static_cast<double>( i + 0 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FREQ_MIN;
        f3 = static_cast<double>( i + 1 ) * AAPI_CAL_SCAN_STEP + AAPI_BAND_FREQ_MIN;

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


    // Formula: Gamma_Actual = (Gamma_Meas - e00) / (Gamma_Meas * e11 - d_e)
    AAPiComplex numerator = gamma - AAPiComplex(ent.e_00);
    AAPiComplex denominator = (gamma * AAPiComplex(ent.e_11)) - AAPiComplex(ent.d_e);
    gamma = numerator / MathUtils::_cnonz(denominator);

    return 0;
}

AAPiError AAPiCalibrator::correct_z(uint32_t freq, AAPiComplex& z)
{
    double r0 = m_config->get_base_r0();
    AAPiComplex gamma = gamma_from_z( z, r0 );

    AAPiError ret = correct_gamma( freq, gamma );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    // It stabilizes the dg denominator calculations inside z_from_gamma
    // when measurements hover right on the edge of open/short states!
    if (gamma.real() > 1.) {
        gamma = AAPiComplex( 1., gamma.imag() );

    } else if (gamma.real() < -1.) {
        gamma = AAPiComplex( -1., gamma.imag() );
    }

    if (gamma.imag() > 1.) {
        gamma = AAPiComplex( gamma.real(), 1. );

    } else if (gamma.imag() < -1.) {
        gamma = AAPiComplex( gamma.real(), -1. );
    }

    z = z_from_gamma( gamma, r0 );

    return AAPI_SUCCESS;
}

AAPiString AAPiCalibrator::get_calibration_dir()
{
    fs::path dirPath = (const char *)AAPiConfig::get_app_dir();
    dirPath.append("calibration");

    return AAPiString{dirPath.c_str()};
}

AAPiString AAPiCalibrator::get_osl_correction_file_name(int osl_file)
{
    fs::path filePath = (const char *)get_calibration_dir();
    AAPiString fileName = AAPiString("osl_") + AAPiVariant( osl_file ).toString() + AAPiString{".corr"};
    filePath.append((const char *)fileName);

    return AAPiString{filePath.c_str()};
}

AAPiString AAPiCalibrator::get_hw_err_correction_file_name()
{
    fs::path filePath = (const char *)get_calibration_dir();
    AAPiString fileName{"hw_error.corr" };
    filePath.append((const char*)fileName);

    return AAPiString{ filePath.c_str()};
}

AAPiComplex AAPiCalibrator::gamma_from_z(const AAPiComplex& z, double r0)
{
    AAPiComplex z0( r0, 0. );

    // Defensively protect the complex denominator using your constexpr utility.
    // This guarantees the system can never trigger a hardware floating-point division exception.
    AAPiComplex denominator = z + z0;
    AAPiComplex g = (z - z0) / MathUtils::_cnonz(denominator);

    // If an absolute mathematical anomaly still slips past, we clamp the result cleanly
    // to the dead center of the Smith Chart (0.0).
    // This prevents giant visual "jumps" across your screen canvas layout!
    if (std::isnan(g.real()) || std::isnan(g.imag()) ||
        std::isinf(g.real()) || std::isinf(g.imag())) {
        g = AAPiComplex( 0., 0. );
    }

    return g;
}

AAPiComplex AAPiCalibrator::z_from_gamma(const AAPiComplex& gamma, double r0)
{
    double gamma_re_2 = MathUtils::sqr( gamma.real() );
    double gamma_im_2 = MathUtils::sqr( gamma.imag() );

    // Protect against division-by-zero when gamma hits the (1.0, 0.0) Open Circuit point
    double dg = MathUtils::_nonz( MathUtils::sqr(1.0 - gamma.real()) + gamma_im_2 );

    double re = r0 * ( 1. - gamma_re_2 - gamma_im_2 ) / dg;
    if ( re < 0. ) {
        // This may happen due to limited computational accuracy
        re = 0.;
    }

    double im = r0 * 2. * gamma.imag() / dg;

    // If an open circuit causes the real or imaginary vectors to skyrocket,
    // we clamp them to a safe maximum boundary to protect our UI legend strings!
    constexpr double MAX_IMPEDANCE_OHMS = 100000.;

    re = std::min(re, MAX_IMPEDANCE_OHMS);
    im = std::clamp(im, -MAX_IMPEDANCE_OHMS, MAX_IMPEDANCE_OHMS);

    return AAPiComplex( re, im );
}

std::ostream& operator<<(std::ostream& os, const AAPiCalibrator::OSLCorrectionEntry& entry)
{
    // Serialize using the primary structural parameters
    os.write(reinterpret_cast<const char*>(&entry.e_00), sizeof(entry.e_00));
    os.write(reinterpret_cast<const char*>(&entry.e_11), sizeof(entry.e_11));
    os.write(reinterpret_cast<const char*>(&entry.d_e), sizeof(entry.d_e));

    return os;
}

std::istream& operator>>(std::istream& is, AAPiCalibrator::OSLCorrectionEntry& entry)
{
    // Deserializing into the same addresses populates both union aliases simultaneously
    is.read(reinterpret_cast<char*>(&entry.e_00), sizeof(entry.e_00));
    is.read(reinterpret_cast<char*>(&entry.e_11), sizeof(entry.e_11));
    is.read(reinterpret_cast<char*>(&entry.d_e), sizeof(entry.d_e));

    return is;
}

std::ostream& operator<<(std::ostream& os, const AAPiCalibrator::HwErrCorrectionEntry& entry)
{
    os.write(reinterpret_cast<const char*>(&entry.mag_0), sizeof(entry.mag_0));
    os.write(reinterpret_cast<const char*>(&entry.phas_0), sizeof(entry.phas_0));

    return os;
}

std::istream& operator>>(std::istream& is, AAPiCalibrator::HwErrCorrectionEntry& entry)
{
    is.read(reinterpret_cast<char*>(&entry.mag_0), sizeof(entry.mag_0));
    is.read(reinterpret_cast<char*>(&entry.phas_0), sizeof(entry.phas_0));

    return is;
}

} //namespace aapi
