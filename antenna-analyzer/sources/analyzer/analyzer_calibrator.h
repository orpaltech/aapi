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

#ifndef AAPI_ANALYZER_CALIBRATOR_H
#define AAPI_ANALYZER_CALIBRATOR_H

#include <complex>
#include "analyzer/analyzer_config.h"

///////////////////////////////////////////////////////////////////////////////
// Calibrator definitions
///////////////////////////////////////////////////////////////////////////////
#define CALIB_SCAN_STEP             100000
#define CALIB_NUM_ENTRIES           (((AA_BAND_FMAX) - (AA_BAND_FMIN)) / CALIB_SCAN_STEP + 1)
#define CALIB_MAX_FILES             16
#define CALIB_IS_VALID_FILE(f)      ((f) >= 0 && (f) < CALIB_MAX_FILES)

namespace aapi
{

enum AAPICalibratorError {
    AAPI_CAL_E_FILE_NOT_SELECTED     =   AAPI_CALIB_ERROR_START,
    AAPI_CAL_E_OPEN_FILE_FAILED      =   AAPI_CALIB_ERROR_START - 1,
    AAPI_CAL_E_WRITE_FILE_FAILED     =   AAPI_CALIB_ERROR_START - 2,
    AAPI_CAL_E_FILE_INVALID          =   AAPI_CALIB_ERROR_START - 3,
    AAPI_CAL_E_SCAN_INCOMPLETE       =   AAPI_CALIB_ERROR_START - 4,
    AAPI_CAL_E_FREQ_OUT_OF_RANGE     =   AAPI_CALIB_ERROR_START - 5,
    AAPI_CAL_E_NOT_CALCULATED        =   AAPI_CALIB_ERROR_START - 6,
};

///////////////////////////////////////////////////////////////////////////////
// Struct & Enum definitions
///////////////////////////////////////////////////////////////////////////////
typedef std::complex<float>::_ComplexT _ComplexF;

typedef union {
    struct {
        _ComplexF e_00;
        _ComplexF e_11;
        _ComplexF d_e;
    };
    struct {
        _ComplexF g_short;  /* measured gamma for short circuit */
        _ComplexF g_load;   /* measured gamma for normal load (50 Ohm) */
        _ComplexF g_open;   /* measured gamma for open circuit */
    };
} OSLCorrectionEntry;

typedef struct {
    float mag_0;    /* magnitude correction value */
    float phas_0;   /* phase correction value */
} HWErrorCorrectionEntry;

typedef enum {
    CALIB_FILE_INVALID  = 0x00,
    CALIB_FILE_VALID  = 0x01,
    CALIB_FILE_SHORT_SCANNED  = 0x02,
    CALIB_FILE_LOAD_SCANNED   = 0x04,
    CALIB_FILE_OPEN_SCANNED   = 0x08,
    CALIB_FILE_FULLY_SCANNED = (CALIB_FILE_SHORT_SCANNED | CALIB_FILE_LOAD_SCANNED | CALIB_FILE_OPEN_SCANNED)
} CalibratorFileStatus;

///////////////////////////////////////////////////////////////////////////////
// class AAPICalibrator
///////////////////////////////////////////////////////////////////////////////

class AAPICalibrator : public AAPIObject
{
    DECLARE_AAPI_OBJECT(AAPICalibrator)

    static AAPICalibrator *create(AAPIConfig *config, bool add_ref = true);
protected:
    AAPICalibrator();
    ~AAPICalibrator();

public:
    int init();

    int get_osl_file() const { return m_osl_file; }
    int set_osl_file(int f);
    uint8_t get_osl_status() const { return m_osl_status; }

    int scan_osl_begin(uint8_t scan_type);
    int scan_osl_ended(uint8_t scan_type);
    int set_osl_z(uint8_t scan_type, int index, const std::complex<float>& z);
    int calc_osl_entries();

    void scan_hwerror_begin();
    int set_hwerror_entry(int index, float mag0, float phas0);
    void scan_hwerror_ended();

    bool osl_file_exists(int f) const;

    int load_osl_correction_file(int f);
    int save_osl_correction_file();

    int load_hwerror_correction_file();
    int save_hwerror_correction_file();

    void correct_hwerror(uint32_t freq, float *mag_ratio, float *phas_diff);
    int correct_z(uint32_t freq, std::complex<float>& z);
    int correct_gamma(uint32_t freq, std::complex<float>& g);

public:
    static bool is_valid_file(int f);
    static uint32_t freq_by_index(size_t index);
    static int index_by_freq(uint32_t freq);

    static QString get_calibration_dir(bool generate = false);
    static QString get_osl_correction_file_name(int osl_file);
    static QString get_hwerror_correction_file_name();

    static std::complex<float> gamma_from_z(const std::complex<float>& z, float r0);
    static std::complex<float> z_from_gamma(const std::complex<float>& g, float r0);

private:
    AAPIConfig          *m_config;
    static QString      m_dir;
    int                 m_osl_file;
    uint8_t             m_osl_status;
    uint8_t             m_hwerror_status;
    OSLCorrectionEntry  m_osl_correction_entries [ CALIB_NUM_ENTRIES ];
    HWErrorCorrectionEntry  m_hwerror_correction_entries [ CALIB_NUM_ENTRIES ];
};

} //namespace aapi

#endif // AAPI_ANALYZER_CALIBRATOR_H
