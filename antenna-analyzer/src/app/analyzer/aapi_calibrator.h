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

#ifndef AAPI_CALIBRATOR_H
#define AAPI_CALIBRATOR_H

#include "aapi_object_with_config.h"
#include "utils/aapi_complex.h"

///////////////////////////////////////////////////////////////////////////////
// Calibrator definitions
///////////////////////////////////////////////////////////////////////////////
#define AAPI_CAL_SCAN_STEP          100'000UL
#define AAPI_CAL_MAX_FILES          16UL


namespace aapi
{

enum AAPiCalibratorError {
    AAPI_CAL_E_UNSPECIFIED          = (AAPI_CALIBRATOR_ERROR_START - 0),
    AAPI_CAL_E_FILE_NOT_SELECTED    = (AAPI_CALIBRATOR_ERROR_START - 1),
    AAPI_CAL_E_OPEN_FILE_FAILED     = (AAPI_CALIBRATOR_ERROR_START - 2),
    AAPI_CAL_E_WRITE_FILE_FAILED    = (AAPI_CALIBRATOR_ERROR_START - 3),
    AAPI_CAL_E_FILE_INVALID         = (AAPI_CALIBRATOR_ERROR_START - 4),
    AAPI_CAL_E_SCAN_INCOMPLETE      = (AAPI_CALIBRATOR_ERROR_START - 5),
    AAPI_CAL_E_FREQ_OUT_OF_RANGE    = (AAPI_CALIBRATOR_ERROR_START - 6),
    AAPI_CAL_E_NOT_CALCULATED       = (AAPI_CALIBRATOR_ERROR_START - 7),
};


///////////////////////////////////////////////////////////////////////////////
// class AAPiCalibrator
///////////////////////////////////////////////////////////////////////////////

class AAPiCalibrator : public AAPiObject
{
    DECLARE_AAPI_OBJECT_WITH_CONFIG(AAPiCalibrator)

public:
    static constexpr int NUM_ENTRIES = ((AAPI_BAND_FREQ_MAX - AAPI_BAND_FREQ_MIN) / AAPI_CAL_SCAN_STEP + 1);

    enum class FileStatus {
        INVALID    = 0x00U,
        VALID      = 0x01U,
        SCAN_SHORT = 0x02U,
        SCAN_LOAD  = 0x04U,
        SCAN_OPEN  = 0x08U,
        FULL_SCAN  = (SCAN_SHORT | SCAN_LOAD | SCAN_OPEN)
    };

    static constexpr uint8_t FILE_STATUS_VALID = static_cast<uint8_t>(FileStatus::VALID);
    static constexpr uint8_t FILE_STATUS_INVALID = static_cast<uint8_t>(FileStatus::INVALID);
    static constexpr uint8_t FILE_STATUS_SCAN_OPEN = static_cast<uint8_t>(FileStatus::SCAN_OPEN);
    static constexpr uint8_t FILE_STATUS_SCAN_SHORT = static_cast<uint8_t>(FileStatus::SCAN_SHORT);
    static constexpr uint8_t FILE_STATUS_SCAN_LOAD = static_cast<uint8_t>(FileStatus::SCAN_LOAD);
    static constexpr uint8_t FILE_STATUS_FULL_SCAN = static_cast<uint8_t>(FileStatus::FULL_SCAN);

    typedef union {
        struct {
            AAPiComplexType e_00;
            AAPiComplexType e_11;
            AAPiComplexType d_e;
        };
        struct {
            AAPiComplexType g_short;  /* measured gamma for short circuit */
            AAPiComplexType g_load;   /* measured gamma for normal load (50 Ohm) */
            AAPiComplexType g_open;   /* measured gamma for open circuit */
        };
    } OSLCorrectionEntry;

    typedef struct {
        double mag_0;    /* magnitude correction value */
        double phas_0;   /* phase correction value */
    } HwErrCorrectionEntry;

protected:
    AAPiCalibrator();
    ~AAPiCalibrator();

public:
    AAPiError init();

    int get_osl_file() const { return m_osl_file; }
    AAPiError set_osl_file(int osl_file);
    bool osl_file_exists(int f) const;
    AAPiError load_osl_correction_file(int f);
    AAPiError flush_osl_correction_file();
    uint8_t get_osl_status() const { return m_osl_status; }
    bool is_osl_scan_complete(uint8_t scan_type) const { return ( get_osl_status() & scan_type ) == scan_type; }
    // Check if we we're fully scanned (all 3 cases)
    bool is_osl_fully_scanned() const { return is_osl_scan_complete( FILE_STATUS_FULL_SCAN ); }
    // Marks the beginning of this particualr scan type (open, short, or load)
    AAPiError osl_scan_begin(uint8_t scan_type);
    // Finalizes this particular scan type.
    // NOTE: The calibration is still considered incomplete until calc_osl_entries() is successfully executed
    AAPiError osl_scan_finalize(uint8_t scan_type);
    AAPiError calc_osl_entries();
    AAPiError set_osl_z(uint8_t scan_type, int index, const AAPiComplex& z);

    uint8_t get_hw_err_status() const { return m_hw_err_status; }
    AAPiError load_hw_err_correction_file();
    AAPiError flush_hw_err_correction_file();
    void hw_err_scan_begin();
    void hw_err_scan_finalize();
    AAPiError set_hw_err_entry(int index, double mag0, double phas0);

    // Correction methods
    AAPiError correct_hw_err(uint32_t freq, double& mag_ratio, double& phas_diff);
    AAPiError correct_z(uint32_t freq, AAPiComplex& z);
    AAPiError correct_gamma(uint32_t freq, AAPiComplex& g);

public:
    static constexpr bool is_valid_file(int f) { return ((f) >= 0 && (f) < AAPI_CAL_MAX_FILES); }

    static constexpr uint32_t get_freq_by_index(uint index)
    {
        return (index < NUM_ENTRIES) ? (AAPI_BAND_FREQ_MIN + static_cast<uint32_t>( index * AAPI_CAL_SCAN_STEP ) ) : 0;
    }
    static constexpr int get_index_by_freq(uint32_t freq)
    {
        if ( ! AAPiConfig::is_freq_in_band( freq ) ) {
            return -1;
        }
        return std::round( static_cast<double>(freq) / AAPI_CAL_SCAN_STEP ) - ( AAPI_BAND_FREQ_MIN / AAPI_CAL_SCAN_STEP );
    }

    static AAPiString get_calibration_dir();
    static AAPiString get_osl_correction_file_name(int osl_file);
    static AAPiString get_hw_err_correction_file_name();

    static AAPiComplex gamma_from_z(const AAPiComplex& z, double r0);
    static AAPiComplex z_from_gamma(const AAPiComplex& g, double r0);

private:
    friend std::ostream& operator<<(std::ostream& os, const OSLCorrectionEntry& entry);
    friend std::istream& operator>>(std::istream& is, OSLCorrectionEntry& entry);
    friend std::ostream& operator<<(std::ostream& os, const HwErrCorrectionEntry& entry);
    friend std::istream& operator>>(std::istream& is, HwErrCorrectionEntry& entry);

private:
    int                     m_osl_file;
    uint8_t                 m_osl_status;
    // Aligns the calibration arrays perfectly with the ARM CPU hardware layout,
    // maximizing data throughput during high-speed parabolic interpolations
    alignas(64)
    OSLCorrectionEntry      m_osl_correction [ NUM_ENTRIES ];

    uint8_t                 m_hw_err_status;
    // Forces the compiler to map this large array to a strict 8-byte boundary
    alignas(64)
    HwErrCorrectionEntry    m_hw_err_correction [ NUM_ENTRIES ];
};

std::ostream& operator<<(std::ostream& os, const AAPiCalibrator::OSLCorrectionEntry& entry);
std::istream& operator>>(std::istream& is, AAPiCalibrator::OSLCorrectionEntry& entry);
std::ostream& operator<<(std::ostream& os, const AAPiCalibrator::HwErrCorrectionEntry& entry);
std::istream& operator>>(std::istream& is, AAPiCalibrator::HwErrCorrectionEntry& entry);

} //namespace aapi

#endif // AAPI_CALIBRATOR_H
