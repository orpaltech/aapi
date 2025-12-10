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

#ifndef AAPI_CONFIG_H
#define AAPI_CONFIG_H

#include <QMutex>
#include <QMutexLocker>
#include "aapi_error.h"
#include "aapi_object.h"
#include "utils/simple_variant.h"

///////////////////////////////////////////////////////////////////////////////
// Configuration definitions
///////////////////////////////////////////////////////////////////////////////


#define AAPI_SYSFS_PATH         "/sys/class/vna/aapi0"
#define AAPI_SYSFS_ENABLE       "enable"
#define AAPI_SYSFS_MEASURE_FREQ "measure_freq"
#define AAPI_SYSFS_INT_FREQ     "intermediate_freq"

/* The selected intermediate frequency */
#define AAPI_INT_FREQ_HZ    12281U

/*
 * Frequency range of the analyzer
 */
#define AAPI_BAND_FMIN      500000UL    //BAND_FMIN must be multiple 100000
#define AAPI_BAND_FMAX      158000000UL //BAND_FMAX must be multiple of 100000

#if (AAPI_BAND_FMAX % 100000) != 0 || AAPI_BAND_FMAX <= AAPI_BAND_FMIN || (AAPI_BAND_FMIN % 100000) != 0
    #error "Incorrect band limit settings"
#endif

#define AAPI_IS_FREQ_INBAND(f)      ((f) >= AAPI_BAND_FMIN && (f) <= AAPI_BAND_FMAX)

/* Maximum number of measurements */
#define AAPI_MEASURE_MAX_SCANS      20U

#define AAPI_MAG_CORR_FACTOR        1.  // TODO

///////////////////////////////////////////////////////////////////////////////
// Parameter accessor method declarations
///////////////////////////////////////////////////////////////////////////////

#define AAPI_PARAM_UINT32_ACCESSOR_R(_id, _name)    \
    uint32_t get_ ## _name () const {               \
        return get_value( _id ).toUInt();           \
}

#define AAPI_PARAM_UINT32_ACCESSOR_W(_id, _name)    \
    void set_ ## _name (uint32_t value) {           \
        set_value( _id, value );                    \
    }

#define AAPI_PARAM_INT32_ACCESSOR(_id, _name)   \
    void set_ ## _name (int32_t value) {        \
        set_value( _id, value );                \
    }                                           \
    int32_t get_ ## _name () const {            \
        return get_value( _id ).toInt();        \
	}

#define AAPI_PARAM_UINT32_ACCESSOR(_id, _name)  \
    AAPI_PARAM_UINT32_ACCESSOR_R(_id, _name)    \
    AAPI_PARAM_UINT32_ACCESSOR_W(_id, _name)

#define AAPI_PARAM_UINT8_ACCESSOR(_id, _name)   \
    void set_ ## _name (uint8_t value) {        \
        set_value( _id,                         \
            static_cast<uint32_t> (value));     \
    }                                           \
    uint8_t get_ ## _name () const {            \
        return get_value( _id ).toByte();       \
	}

#define AAPI_PARAM_FLOAT_ACCESSOR(_id, _name)   \
    void set_ ## _name (float value) {      \
        set_value( _id, value );            \
    }                                       \
    float get_ ## _name () const {          \
        return get_value( _id ).toFloat();  \
	}

#define AAPI_PARAM_STRING_ACCESSOR(_id, _name)  \
    void set_ ## _name (const char* value) {    \
        set_value( _id, value );                \
    }                                           \
    AAPiString get_ ## _name () const {         \
        return get_value( _id ).toString();     \
	}

namespace aapi
{

typedef QRecursiveMutex                 AAPiMutex;
typedef QMutexLocker<QRecursiveMutex>   AAPiMutexLocker;

///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////

enum AAPiParameter {
	/**
	 * 4 characters of version string
	 */
    AAPI_PARAM_VERSION,

	/**
     * The audio sample rate to use in analog-to-digital conversion, Samples/Sec
	 */
    AAPI_PARAM_DSP_SAMPLE_RATE,

    /**
     * DSP sample size, bits/sample
     */
    AAPI_PARAM_DSP_SAMPLE_SIZE,

	/**
	 * Number of samples for DSP
	 */
    AAPI_PARAM_DSP_NUM_SAMPLES,

	/**
	 * Initial frequency for panoramic window
	 */
    AAPI_PARAM_PAN_FREQ1,

	/**
	 * Span for panoramic window
	 */
    AAPI_PARAM_PAN_SPAN,

	/**
	 * Way of setting panoramic window:
     * 		0: F1+bandspan,
     * 		1: F1 +/- Bandspan/2
	 */
    AAPI_PARAM_PAN_IS_CENTER_FREQ,

	/**
	 * Measurement window frequency
	 */
    AAPI_PARAM_MEASURE_FREQ,

	/**
     * Synthesizer IC Xtal frequency, Hz
	 */
    AAPI_PARAM_SYNTH_XTAL_FREQ,

	/**
     * Synthesizer IC Xtal correction (signed, int16_t)
	 */
    AAPI_PARAM_SYNTH_XTAL_CORR,

	/**
     * Base R0 for G measurements
	 */
    AAPI_PARAM_BASE_R0,

	/**
	 * Selected OSL file
	 */
    AAPI_PARAM_OSL_SELECTED,

	/**
	 * R-LOAD for OSL calibration
	 */
    AAPI_PARAM_OSL_R_LOAD,

	/**
	 * R-SHORT for OSL calibration
	 */
    AAPI_PARAM_OSL_R_SHORT,

	/**
	 * R-OPEN for OSL calibration
	 */
    AAPI_PARAM_OSL_R_OPEN,

	/**
	 * Number of scans to average during OSL
	 */
    AAPI_PARAM_OSL_N_SCANS,

	/**
	 * Number of scans to average in measurement window
	 */
    AAPI_PARAM_MEASURE_N_SCANS,

	/**
	 * Number of scans to average in panoramic window
	 */
    AAPI_PARAM_PAN_N_SCANS,

	/**
     * Linear audio input gain, dB
	 */
    AAPI_PARAM_AUDIO_INPUT_GAIN,

	/**
	 * LO frequency is divided by two in quadrature mixer
	 */
    AAPI_PARAM_LO_FREQ_DIV_BY_2,

	/**
	 * Frequency for generator window, Hz
	 */
    AAPI_PARAM_GENERATOR_FREQ,

	/**
	 * Value of measurement resistor in bridge, float32
	 */
    AAPI_PARAM_BRIDGE_R_MEASURE,

	/**
	 * Value of series resistor in bridge, float32
	 */
    AAPI_PARAM_BRIDGE_R_MEASURE_ADD,

	/**
	 * Value of load resistor in bridge, float32
	 */
    AAPI_PARAM_BRIDGE_R_LOAD,

	/**
	 * Serial port (UART) to be used
	 */
    AAPI_PARAM_UART_DEVICE,

	/**
	 * Serial (COM) port speed, bps
	 */
    AAPI_PARAM_UART_BAUDRATE,

	/**
     * 2-channel audio input device for use in analog-to-digital conversion.
	 */
    AAPI_PARAM_AUDIO_INPUT_DEVICE,

	/**
     * Time in milliseconds after which the power saving mode is activated (0 - disabled)
	 */
    AAPI_PARAM_PWRSAVE_TIMEOUT,

	/**
	 * Enable setting frequency on 3rd harmonic above BAND_FMAX
	 * 		1: enable
	 * 		0: disable
	 */
    AAPI_PARAM_3RD_HARM_ENABLE,

	/**
	 * Show S11 graph in the panoramic window
	 * 		1: show
	 * 		0: hide
	 */
    AAPI_PARAM_S11_GRAPH_SHOW,

	/**
	 * Type of Touchstone S1P file saved with panoramic screenshot
	 */
    AAPI_PARAM_S1P_FILE_TYPE,

	/**
	 * Show advanced options in configuration menu
	 */
    AAPI_PARAM_SHOW_ADVANCED,

	/**
     * Image format for screenshots ("bmp", "png", "jpg")
	 */
    AAPI_PARAM_SNAPSHOT_FORMAT,

    NUM_AAPI_PARAMS
};

enum AAPiS1pType {
    AAPI_S1P_S_MA = 0u,
    AAPI_S1P_S_RI = 1u
};

enum AAPiXtalRate {
    AAPI_XTAL_25MHZ	= 25000000u,
    AAPI_XTAL_27MHZ	= 27000000u
};

enum AAPiConfigError {
    AAPI_CONF_E_UNSPECIFIED         = (AAPI_CONF_ERROR_START - 0),
    AAPI_CONF_E_FILE_OPEN_FAILED    = (AAPI_CONF_ERROR_START - 1)
};

///////////////////////////////////////////////////////////////////////////////
// struct AAPiRadioBand
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiRadioBand struct
///
struct AAPiRadioBand
{
    uint32_t lo;
    uint32_t hi;
    char name[30];

    AAPiRadioBand(uint32_t lo, uint32_t hi, const char *name);
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiConfig
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiConfig class
///
class AAPiConfig : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AAPiConfig)

protected:
    AAPiConfig();
    AAPiConfig(const AAPiConfig& config);
    ~AAPiConfig();

public:
    AAPiConfig& operator=(const AAPiConfig& config);

    uint32_t get_num_params() const;

    AAPiString get_name(int param_index) const;
    AAPiString get_description(int param_index) const;
    uint32_t get_precision(int param_index) const;
    bool is_valid(int param_index) const;

    uint32_t get_num_opts(enum AAPiParameter id) const;
    uint32_t get_num_opts(int param_index) const;
    const AAPiVariantArray& get_opt_values(enum AAPiParameter id) const;
    const AAPiVariantArray& get_opt_values(int param_index) const;
    const AAPiStringArray& get_opt_labels(enum AAPiParameter id) const;
    const AAPiStringArray& get_opt_labels(int param_index) const;

    AAPiVariant get_value(enum AAPiParameter id) const;
    AAPiVariant get_value(int param_index) const;

    void set_value(enum AAPiParameter id, const AAPiVariant& val);
    void set_value(int param_index, const AAPiVariant& val);

public:
    AAPI_PARAM_STRING_ACCESSOR(AAPI_PARAM_VERSION, version)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_DSP_SAMPLE_RATE, dsp_sample_rate)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_DSP_SAMPLE_SIZE, dsp_sample_size)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_DSP_NUM_SAMPLES, dsp_num_samples)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_PAN_FREQ1, pan_freq1)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_PAN_SPAN, pan_span)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_MEASURE_FREQ, measure_freq)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_SYNTH_XTAL_FREQ, synth_xtal_freq)
    AAPI_PARAM_INT32_ACCESSOR(AAPI_PARAM_SYNTH_XTAL_CORR, synth_xtal_corr)
    AAPI_PARAM_INT32_ACCESSOR(AAPI_PARAM_OSL_SELECTED, osl_selected)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_BASE_R0, base_r0)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_OSL_R_LOAD, osl_r_load)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_OSL_R_SHORT, osl_r_short)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_OSL_R_OPEN, osl_r_open)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_OSL_N_SCANS, osl_n_scans)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_MEASURE_N_SCANS, measure_n_scans)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_PAN_N_SCANS, pan_n_scans)
    AAPI_PARAM_STRING_ACCESSOR(AAPI_PARAM_AUDIO_INPUT_DEVICE, audio_input_device)
    AAPI_PARAM_UINT8_ACCESSOR(AAPI_PARAM_AUDIO_INPUT_GAIN, audio_input_gain)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_LO_FREQ_DIV_BY_2, lo_freq_div_by_2)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_GENERATOR_FREQ, generator_freq)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_PAN_IS_CENTER_FREQ, pan_is_center_freq)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPI_PARAM_BRIDGE_R_MEASURE, bridge_r_measure)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPI_PARAM_BRIDGE_R_MEASURE_ADD, bridge_r_measure_add)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPI_PARAM_BRIDGE_R_LOAD, bridge_r_load)
    AAPI_PARAM_STRING_ACCESSOR(AAPI_PARAM_UART_DEVICE, uart_device)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_UART_BAUDRATE, uart_baudrate)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_PWRSAVE_TIMEOUT, pwr_save_timeout)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_3RD_HARM_ENABLE, 3rd_harm_enable)
    AAPI_PARAM_STRING_ACCESSOR(AAPI_PARAM_SNAPSHOT_FORMAT, snapshot_format)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_S11_GRAPH_SHOW, s11_graph_show)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_S1P_FILE_TYPE, s1p_file_type)
    AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_SHOW_ADVANCED, show_advanced)


public:
    int init();
	int flush();

    // DSP-related helper properties
    uint32_t get_dsp_fft_if_bin() const;    // The bin corresponding to the intermediate frequency
    uint32_t get_dsp_fft_num_pts() const;   // The number of useful FFT points
    double get_dsp_fft_bin_width() const;   // The frequency range between two FFT bins
    double get_dsp_fft_bin_freq(uint32_t bin) const;    // The frequency corresponding to the bin

public:
    static int get_index(enum AAPiParameter id);
    static enum AAPiParameter get_id(int param_index);
    static enum AAPiVariantType get_type(int param_index);
    static AAPiVariant get_value(const AAPiConfig *config, enum AAPiParameter id);
    static uint32_t get_total_params();

    static AAPiString get_app_dir();
    static AAPiString get_config_dir();
    static AAPiString get_config_path();

    static AAPiRadioBand *get_ham_bands();
    static uint32_t get_num_ham_bands();

private:
    void set_defaults();
    void copyFrom(const AAPiConfig& config);

private:
    AAPiVariant m_values[ NUM_AAPI_PARAMS ];
    AAPiMutex   m_mutex;
};

/*
    AAPI_PT_BYTE,   //8-bit unsigned
    AAPI_PT_UINT,   //32-bit unsigned
    AAPI_PT_INT,    //32-bit signed
    AAPI_PT_FLOAT,  //32-bit float
    AAPI_PT_TEXT,   //char*
 */

AAPiString get_sysfs_property_path(const char *property_name);

} // namespace aapi

#endif // AAPI_CONFIG_H
