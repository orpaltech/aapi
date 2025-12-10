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

#include "utils/aapi_complex.h"
#include "analyzer/aapi_config.h"

///////////////////////////////////////////////////////////////////////////////
// Calibrator definitions
///////////////////////////////////////////////////////////////////////////////
#define AAPI_CAL_SCAN_STEP          100000UL
#define AAPI_CAL_NUM_ENTRIES        (((AAPI_BAND_FMAX) - (AAPI_BAND_FMIN)) / AAPI_CAL_SCAN_STEP + 1)
#define AAPI_CAL_MAX_FILES          16UL
#define AAPI_CAL_FILE_IS_VALID(f)   ((f) >= 0 && (f) < AAPI_CAL_MAX_FILES)


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
// Struct & Enum definitions
///////////////////////////////////////////////////////////////////////////////

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
} HWECorrectionEntry;

typedef enum {
    AAPI_CAL_FILE_INVALID       = 0x00,
    AAPI_CAL_FILE_VALID         = 0x01,
    AAPI_CAL_FILE_SHORT_SCANNED = 0x02,
    AAPI_CAL_FILE_LOAD_SCANNED  = 0x04,
    AAPI_CAL_FILE_OPEN_SCANNED  = 0x08,
    AAPI_CAL_FILE_FULLY_SCANNED = (AAPI_CAL_FILE_SHORT_SCANNED | AAPI_CAL_FILE_LOAD_SCANNED | AAPI_CAL_FILE_OPEN_SCANNED)
} AAPiCalibratorFileStatus;

///////////////////////////////////////////////////////////////////////////////
// class AAPiCalibrator
///////////////////////////////////////////////////////////////////////////////

class AAPiCalibrator : public AAPiObject
{
    DECLARE_AAPI_OBJECT_WITH_CONFIG(AAPiCalibrator)

protected:
    AAPiCalibrator();
    ~AAPiCalibrator();

public:
    int init();

    int get_osl_file() const { return m_osl_file; }
    int set_osl_file(int f);
    uint8_t get_osl_status() const { return m_osl_status; }

    int scan_osl_begin(uint8_t scan_type);
    int scan_osl_ended(uint8_t scan_type);
    int set_osl_z(uint8_t scan_type, int index, const AAPiComplex& z);
    int calc_osl_entries();

    void scan_hwe_begin();
    int set_hwe_entry(int index, double mag0, double phas0);
    void scan_hwe_ended();

    bool osl_file_exists(int f) const;

    int load_osl_correction_file(int f);
    int flush_osl_correction_file();

    int load_hwe_correction_file();
    int flush_hwe_correction_file();

    void correct_hwe(uint32_t freq, double *mag_ratio, double *phas_diff);
    int correct_z(uint32_t freq, AAPiComplex& z);
    int correct_gamma(uint32_t freq, AAPiComplex& g);

public:
    static bool is_valid_file(int f);
    static uint32_t freq_by_index(size_t index);
    static int index_by_freq(uint32_t freq);

    static AAPiString get_calibration_dir(bool generate = false);
    static AAPiString get_osl_correction_file_name(int osl_file);
    static AAPiString get_hwe_correction_file_name();

    static AAPiComplex gamma_from_z(const AAPiComplex& z, double r0);
    static AAPiComplex z_from_gamma(const AAPiComplex& g, double r0);

private:
    static AAPiString   m_dir;
    int                 m_osl_file;
    uint8_t             m_osl_status;
    uint8_t             m_hwe_status;
    OSLCorrectionEntry  m_osl_correction [ AAPI_CAL_NUM_ENTRIES ];
    HWECorrectionEntry  m_hwe_correction [ AAPI_CAL_NUM_ENTRIES ];
};

} //namespace aapi

#endif // AAPI_CALIBRATOR_H
