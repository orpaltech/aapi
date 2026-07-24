/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

#define STRINGIFY_DIRECT(x) #x
#define STRINGIFY(x) STRINGIFY_DIRECT(x)

/**
     * Synthesizer IC crystal frequency, Hz
     */
//AAPI_PARAM_SYNTH_XTAL_FREQ,

    /**
     * Synthesizer IC crystal correction (signed, int16_t)
     */
//AAPI_PARAM_SYNTH_XTAL_CORR,

/*
 * Frequency range of the analyzer
 */
#define AAPI_BAND_FREQ_MIN  500'000UL       // must be multiple 100000
#define AAPI_BAND_FREQ_MAX  159'000'000UL   // must be multiple of 100000

#if (AAPI_BAND_FREQ_MAX % 100000) != 0 || AAPI_BAND_FREQ_MAX <= AAPI_BAND_FREQ_MIN || (AAPI_BAND_FREQ_MIN % 100000) != 0
    #error "Incorrect band limit settings"
#endif

/* Maximum number of measurements */
#define AAPI_MAX_MEASURE_SCANS  20U

// Texas Instruments PCM1803 requires a full-scale input of exactly 3.0Vp-p (Peak-to-Peak).
// Define the true conversion factor that accounts for the 1500mV peak ceiling
// AND the RMS-to-Peak trigonometric conversion factor (1500.0 * 1.4142135)
#define AAPI_MAG_CORR_FACTOR(preamp)    (2121.32 / (preamp))

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

enum class AAPiParameter {
	/**
	 * 4 characters of version string
	 */
    VERSION,

	/**
     * The audio sample rate to use in analog-to-digital conversion, Samples/Sec
	 */
    DSP_SAMPLE_RATE,

    /**
     * DSP sample size, bits/sample
     */
    DSP_SAMPLE_SIZE,

	/**
	 * Number of samples for DSP
	 */
    DSP_NUM_SAMPLES,

	/**
	 * Initial frequency for panoramic window
	 */
    PAN_MIN_FREQ,

	/**
	 * Span for panoramic window
	 */
    PAN_FREQ_SPAN,

	/**
	 * Way of setting panoramic window:
     * 		0: F1+bandspan,
     * 		1: F1 +/- Bandspan/2
	 */
    PAN_IS_CENTER_FREQ,

	/**
	 * Measurement window frequency
	 */
    MEASURE_FREQ,

	/**
     * Base R0 for G measurements
	 */
    BASE_R0,

	/**
	 * Selected OSL file
	 */
    OSL_FILE_SELECTED,

	/**
	 * R-LOAD for OSL calibration
	 */
    OSL_R_LOAD,

	/**
	 * R-SHORT for OSL calibration
	 */
    OSL_R_SHORT,

	/**
	 * R-OPEN for OSL calibration
	 */
    OSL_R_OPEN,

	/**
     * Number of scans to average during calibration
	 */
    CALIBR_NUM_SCANS,

	/**
	 * Number of scans to average in measurement window
	 */
    MEASURE_NUM_SCANS,

	/**
	 * Number of scans to average in panoramic window
	 */
    PAN_NUM_SCANS,

	/**
     * Linear audio input gain, dB
	 */
    AUDIO_INPUT_GAIN,

	/**
	 * LO frequency is divided by two in quadrature mixer
	 */
    LO_FREQ_DIV_BY_2,

	/**
	 * Frequency for generator window, Hz
	 */
    GENERATOR_FREQ,

	/**
	 * Value of measurement resistor in bridge, float32
	 */
    BRIDGE_R_MEASURE,

	/**
	 * Value of series resistor in bridge, float32
	 */
    BRIDGE_R_MEASURE_ADD,

	/**
	 * Value of load resistor in bridge, float32
	 */
    BRIDGE_R_LOAD,

	/**
	 * Serial port (UART) to be used
	 */
    UART_DEVICE,

	/**
	 * Serial (COM) port speed, bps
	 */
    UART_BAUDRATE,

	/**
     * 2-channel audio input device for use in analog-to-digital conversion.
	 */
    AUDIO_INPUT_DEVICE,

	/**
     * Time in milliseconds after which the power saving mode is activated (0 - disabled)
	 */
    PWRSAVE_TIMEOUT,

	/**
     * Use 3-rd harmonic for measurements frequencies above BAND_FMAX
	 * 		1: enable
	 * 		0: disable
	 */
    USE_3D_HARMONIC,

	/**
	 * Show S11 graph in the panoramic window
	 * 		1: show
	 * 		0: hide
	 */
    S11_GRAPH_SHOW,

	/**
	 * Type of Touchstone S1P file saved with panoramic screenshot
	 */
    S1P_FILE_TYPE,

	/**
	 * Show advanced options in configuration menu
	 */
    SHOW_ADVANCED,

	/**
     * Image format for screenshots ("bmp", "png", "jpg")
	 */
    SNAPSHOT_FORMAT,

    NUM_PARAMS
};

enum AAPiS1pType {
    AAPI_S1P_S_MA = 0u,
    AAPI_S1P_S_RI = 1u
};

/*enum AAPiXtalRate {
    AAPI_XTAL_25MHZ	= 25000000u,
    AAPI_XTAL_27MHZ	= 27000000u
};*/

enum AAPiConfigError {
    AAPI_CONF_E_UNSPECIFIED         = (AAPI_CONF_ERROR_START - 0),
    AAPI_CONF_E_FILE_OPEN_ERROR     = (AAPI_CONF_ERROR_START - 1),
    AAPI_CONF_E_FILE_WRITE_ERROR    = (AAPI_CONF_ERROR_START - 2)
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
    AAPiConfig(AAPiConfig&& config) noexcept;
    ~AAPiConfig();

    static constexpr uint NUM_PARAMS = (uint)AAPiParameter::NUM_PARAMS;

public:
    AAPiConfig& operator=(const AAPiConfig& config);
    AAPiConfig& operator=(AAPiConfig&& config) noexcept;

    uint32_t get_num_valid_params() const;

    AAPiString get_name(int param_index) const;
    AAPiString get_description(int param_index) const;
    uint32_t get_precision(int param_index) const;
    bool is_valid(int param_index) const;

    uint32_t get_num_opts(enum AAPiParameter id) const;
    uint32_t get_num_opts(int param_index) const;
    const AAPiVariantArray& get_opt_values(AAPiParameter id) const;
    const AAPiVariantArray& get_opt_values(int param_index) const;
    const AAPiStringArray& get_opt_labels(AAPiParameter id) const;
    const AAPiStringArray& get_opt_labels(int param_index) const;

    AAPiVariant get_value(AAPiParameter id) const;
    AAPiVariant get_value(int param_index) const;

    void set_value(AAPiParameter id, const AAPiVariant& val);
    void set_value(int param_index, const AAPiVariant& val);

public:
    AAPI_PARAM_STRING_ACCESSOR(AAPiParameter::VERSION, version)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::DSP_SAMPLE_RATE, dsp_sample_rate)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::DSP_SAMPLE_SIZE, dsp_sample_size)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::DSP_NUM_SAMPLES, dsp_num_samples)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::PAN_MIN_FREQ, pan_min_freq)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::PAN_FREQ_SPAN, pan_freq_span)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::MEASURE_FREQ, measure_freq)
    //AAPI_PARAM_UINT32_ACCESSOR(AAPI_PARAM_SYNTH_XTAL_FREQ, synth_xtal_freq)
    //AAPI_PARAM_INT32_ACCESSOR(AAPI_PARAM_SYNTH_XTAL_CORR, synth_xtal_corr)
    AAPI_PARAM_INT32_ACCESSOR(AAPiParameter::OSL_FILE_SELECTED, osl_file_selected)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::BASE_R0, base_r0)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::OSL_R_LOAD, osl_r_load)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::OSL_R_SHORT, osl_r_short)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::OSL_R_OPEN, osl_r_open)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::CALIBR_NUM_SCANS, calibr_num_scans)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::MEASURE_NUM_SCANS, measure_num_scans)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::PAN_NUM_SCANS, pan_num_scans)
    AAPI_PARAM_STRING_ACCESSOR(AAPiParameter::AUDIO_INPUT_DEVICE, audio_input_device)
    AAPI_PARAM_UINT8_ACCESSOR(AAPiParameter::AUDIO_INPUT_GAIN, audio_input_gain)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::LO_FREQ_DIV_BY_2, lo_freq_div_by_2)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::GENERATOR_FREQ, generator_freq)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::PAN_IS_CENTER_FREQ, pan_is_center_freq)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPiParameter::BRIDGE_R_MEASURE, bridge_r_measure)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPiParameter::BRIDGE_R_MEASURE_ADD, bridge_r_measure_add)
    AAPI_PARAM_FLOAT_ACCESSOR(AAPiParameter::BRIDGE_R_LOAD, bridge_r_load)
    AAPI_PARAM_STRING_ACCESSOR(AAPiParameter::UART_DEVICE, uart_device)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::UART_BAUDRATE, uart_baudrate)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::PWRSAVE_TIMEOUT, pwr_save_timeout)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::USE_3D_HARMONIC, use_3rd_harmonic)
    AAPI_PARAM_STRING_ACCESSOR(AAPiParameter::SNAPSHOT_FORMAT, snapshot_format)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::S11_GRAPH_SHOW, s11_graph_show)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::S1P_FILE_TYPE, s1p_file_type)
    AAPI_PARAM_UINT32_ACCESSOR(AAPiParameter::SHOW_ADVANCED, show_advanced)


public:
    AAPiError init();
    AAPiError load();
    AAPiError flush();

    // DSP-related helper properties
    uint32_t get_intermediate_freq() const;
    uint32_t get_dsp_fft_if_bin() const;    // The bin corresponding to the intermediate frequency
    uint32_t get_dsp_fft_num_pts() const;   // The number of useful FFT points
    double get_dsp_fft_bin_width() const;   // The frequency range between two FFT bins
    double get_dsp_fft_bin_freq(uint32_t bin) const;    // The frequency corresponding to the bin
    double get_dsp_digital_preamp() const;
    uint32_t get_dsp_settling_delay_ms() const;

public:
    static int get_index(AAPiParameter id);
    static AAPiParameter get_id(int param_index);
    static enum AAPiVariantType get_type(int param_index);
    static AAPiVariant get_value(const AAPiConfig *config, AAPiParameter id);
    static uint32_t get_total_params();

    static AAPiString get_app_dir();
    static AAPiString get_config_dir();
    static AAPiString get_config_path();

    static AAPiRadioBand *get_ham_bands();
    static uint32_t get_num_ham_bands();

    static constexpr bool is_freq_in_band(uint32_t freq) { return (freq >= AAPI_BAND_FREQ_MIN && freq <= AAPI_BAND_FREQ_MAX); }

private:
    void set_defaults();
    void copyFrom(const AAPiConfig& config);
    void moveFrom(AAPiConfig&& config);

private:
    AAPiVariant m_values[ NUM_PARAMS ];
    AAPiMutex   m_mutex;
};


AAPiString get_sysfs_property_path(const char *property_name, uint dev_index = 0);

} // namespace aapi

#endif // AAPI_CONFIG_H
