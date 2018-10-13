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
#include "analyzer/callback_holder.h"

///////////////////////////////////////////////////////////////////////////////
// OSL Definitions
///////////////////////////////////////////////////////////////////////////////
#define CALIB_SCAN_STEP             100000
#define CALIB_NUM_ENTRIES           (((AA_BAND_FMAX) - (AA_BAND_FMIN)) / CALIB_SCAN_STEP + 1)
#define OSL_MAX_FILES               16
#define OSL_IS_VALID_FILE(f)        ((f) >= 0 && (f) < OSL_MAX_FILES)

namespace aapi
{

enum AAPICalibratorError {
    AAPICAL_E_FILE_NOT_SELECTED     =   AAPI_CALIB_ERROR_START,
    AAPICAL_E_OPEN_FILE_FAILED      =   AAPI_CALIB_ERROR_START - 1,
    AAPICAL_E_WRITE_FILE_FAILED     =   AAPI_CALIB_ERROR_START - 2,
    AAPICAL_E_FILE_INVALID          =   AAPI_CALIB_ERROR_START - 3,
    AAPICAL_E_SCAN_INCOMPLETE       =   AAPI_CALIB_ERROR_START - 4,
    AAPICAL_E_FREQ_OUT_OF_RANGE     =   AAPI_CALIB_ERROR_START - 5,
    AAPICAL_E_NOT_CALCULATED        =   AAPI_CALIB_ERROR_START - 6,
};

///////////////////////////////////////////////////////////////////////////////
// Struct & Enum definitions
///////////////////////////////////////////////////////////////////////////////
typedef std::complex<float>::_ComplexT _ComplexF;

typedef union __tag_osl_corr_entry {
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
} AAPIOSLCorrEntry;

typedef struct __tag_hwerr_corr_entry {
    float mag_0;    /* magnitude correction value */
    float phas_0;   /* phase correction value */
} AAPIHWErrCorrEntry;

typedef enum __tag_osl_file_status {
    OSL_FILE_INVALID  = 0x00,
    OSL_FILE_VALID  = 0x01,
    OSL_FILE_SHORT_SCANNED  = 0x02,
    OSL_FILE_LOAD_SCANNED   = 0x04,
    OSL_FILE_OPEN_SCANNED   = 0x08,
    OSL_FILE_FULLY_SCANNED = (OSL_FILE_SHORT_SCANNED | OSL_FILE_LOAD_SCANNED | OSL_FILE_OPEN_SCANNED)
} osl_file_status;

///////////////////////////////////////////////////////////////////////////////
// class AAPICalibrator
///////////////////////////////////////////////////////////////////////////////

class AAPICalibrator :
        public AAPIObject
{
    DECLARE_AAPI_OBJECT(AAPICalibrator)

    static AAPICalibrator *create(AAPIConfig *config, bool add_ref = true);
protected:
    AAPICalibrator();
public:
    ~AAPICalibrator();

    int init();
    int get_osl_file() const { return m_OSLFile; }
    int set_osl_file(int osl_file);
    uint8_t get_osl_status() const { return m_OSLCorrStatus; }

    int scan_osl_begin(uint8_t scan_type);
    int scan_osl_ended(uint8_t scan_type);
    int set_osl_z(uint8_t scan_type, int index, const std::complex<float>& z);
    int calc_osl_entries();

    void scan_hwerr_begin();
    int set_hwerr_entry(int index, float mag0, float phas0);
    void scan_hwerr_ended();

    bool osl_file_exists(int osl_file) const;

    int load_osl_file(int osl_file);
    int save_osl_file();

    int load_hwerr_file();
    int save_hwerr_file();

    void correct_hwerr(uint32_t freq, float *mag_ratio, float *phas_diff);
    int correct_z(uint32_t freq, std::complex<float>& z);
    int correct_gamma(uint32_t freq, std::complex<float>& g);

    static bool is_valid_file(int osl_file);
    static uint32_t freq_by_index(size_t index);
    static int index_by_freq(uint32_t freq);

    static QString get_osl_dir(bool generate = false);
    static QString get_osl_corr_path(int osl_file);
    static QString get_hwerr_corr_path();

    static std::complex<float> gamma_from_z(const std::complex<float>& z, float r0);
    static std::complex<float> z_from_gamma(const std::complex<float>& g, float r0);

private:
    aapi_ptr<AAPIConfig> m_config;

    static QString  m_dir;
    int             m_OSLFile;
    uint8_t         m_OSLCorrStatus;
    uint8_t         m_HWErrCorrStatus;
    AAPIOSLCorrEntry    m_OSLCorrEntries [ CALIB_NUM_ENTRIES ];
    AAPIHWErrCorrEntry  m_HWErrCorrEntries [ CALIB_NUM_ENTRIES ];
};

} //namespace aapi

#endif // AAPI_ANALYZER_CALIBRATOR_H
