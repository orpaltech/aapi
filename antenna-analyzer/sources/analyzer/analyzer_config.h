/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef AAPI_CONFIG_H
#define AAPI_CONFIG_H

#include <QVariant>
#include <QString>
#include <QVector>
#include <QMutex>
#include "error_codes.h"
#include "analyzer_object.h"

///////////////////////////////////////////////////////////////////////////////
// Configuration definitions
///////////////////////////////////////////////////////////////////////////////

#define AA_VERSION              "1.0"
#define AA_APP_NAME             "orpal-aapi"

/*
 * Frequency range of the analyzer
 */
#define AA_BAND_FMIN 500000ul    //BAND_FMIN must be multiple 100000
#define AA_BAND_FMAX 158000000ul //BAND_FMAX must be multiple of 100000

#if (AA_BAND_FMAX % 100000) != 0 || AA_BAND_FMAX <= AA_BAND_FMIN || (AA_BAND_FMIN % 100000) != 0
    #error "Incorrect band limit settings"
#endif

#define AA_IS_FREQ_INBAND(f)    ((f) >= AA_BAND_FMIN && (f) <= AA_BAND_FMAX)

/* Maximum number of measurements */
#define AA_MEASURE_MAX_SCANS    20u

#define AA_MAG_CORR_FACTOR  1.f // TODO

///////////////////////////////////////////////////////////////////////////////
// Parameter accessor method declarations
///////////////////////////////////////////////////////////////////////////////

#define AA_PARAM_INT32_ACCESSOR(_id, _name) \
    void set_ ## _name (int32_t value) {    \
        set_value( _id, value );            \
    }                                       \
    int32_t get_ ## _name () const {        \
        return get_value( _id ).toInt();    \
	}

#define AA_PARAM_UINT32_ACCESSOR(_id, _name)    \
    void set_ ## _name (uint32_t value) {       \
        set_value( _id, value );                \
    }                                           \
    uint32_t get_ ## _name () const {           \
        return get_value( _id ).toUInt();       \
	}

#define AA_PARAM_UINT8_ACCESSOR(_id, _name)     \
    void set_ ## _name (uint8_t value) {        \
        set_value( _id,                         \
            static_cast<uint32_t> (value));     \
    }                                           \
    uint8_t get_ ## _name () const {            \
        return static_cast<uint8_t>             \
                (get_value( _id ).toUInt());    \
	}

#define AA_PARAM_FLOAT_ACCESSOR(_id, _name) \
    void set_ ## _name (float value) {      \
        set_value( _id, value );            \
    }                                       \
    float get_ ## _name () const {          \
        return get_value( _id ).toFloat();  \
	}

#define AA_PARAM_STRING_ACCESSOR(_id, _name)    \
    void set_ ## _name (const char* value) {    \
        set_value( _id, value );                \
    }                                           \
    QString get_ ## _name () const {            \
        return get_value( _id ).toString();     \
	}

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////

enum AAPIParamId {
	/**
	 * 4 characters of version string
	 */
	AA_PARAM_VERSION,

	/**
	 * DSP sample rate, Samples/Sec
	 */
    AA_PARAM_DSP_SAMPLE_RATE,

    /**
     * DSP sample size, bits/sample
     */
    AA_PARAM_DSP_SAMPLE_SIZE,

	/**
	 * Number of samples for DSP
	 */
	AA_PARAM_DSP_NSAMPLES,

	/**
	 * FFT bin number for DSP
	 * Bin 107 determines 10031 Hz intermediate frequency at 512 samples at 48 kHz.
	 */
	AA_PARAM_DSP_FFTBIN,

	/**
	 * Initial frequency for panoramic window
	 */
    AA_PARAM_PAN_FREQ1,

	/**
	 * Span for panoramic window
	 */
	AA_PARAM_PAN_SPAN,

	/**
	 * Way of setting panoramic window:
     * 		0: F1+bandspan,
     * 		1: F1 +/- Bandspan/2
	 */
    AA_PARAM_PAN_IS_CENTER_FREQ,

	/**
	 * Measurement window frequency
	 */
	AA_PARAM_MEASURE_FREQ,

	/**
     * Synthesizer IC Xtal frequency, Hz
	 */
    AA_PARAM_SYNTH_XTAL_FREQ,

	/**
     * Synthesizer IC Xtal correction (signed, int16_t)
	 */
    AA_PARAM_SYNTH_XTAL_CORR,

	/**
     * Base R0 for G measurements
	 */
    AA_PARAM_BASE_R0,

	/**
	 * Selected OSL file
	 */
    AA_PARAM_OSL_SELECTED,

	/**
	 * R-LOAD for OSL calibration
	 */
	AA_PARAM_OSL_R_LOAD,

	/**
	 * R-SHORT for OSL calibration
	 */
	AA_PARAM_OSL_R_SHORT,

	/**
	 * R-OPEN for OSL calibration
	 */
	AA_PARAM_OSL_R_OPEN,

	/**
	 * Number of scans to average during OSL
	 */
	AA_PARAM_OSL_N_SCANS,

	/**
	 * Number of scans to average in measurement window
	 */
	AA_PARAM_MEASURE_N_SCANS,

	/**
	 * Number of scans to average in panoramic window
	 */
	AA_PARAM_PAN_N_SCANS,

	/**
     * Linear audio input gain, dB
	 */
    AA_PARAM_AUDIO_INPUT_GAIN,

	/**
	 * LO frequency is divided by two in quadrature mixer
	 */
	AA_PARAM_LO_FREQ_DIV_BY_2,

	/**
	 * Frequency for generator window, Hz
	 */
	AA_PARAM_GENERATOR_FREQ,

	/**
	 * Value of measurement resistor in bridge, float32
	 */
    AA_PARAM_BRIDGE_R_MEASURE,

	/**
	 * Value of series resistor in bridge, float32
	 */
    AA_PARAM_BRIDGE_R_MEASURE_ADD,

	/**
	 * Value of load resistor in bridge, float32
	 */
	AA_PARAM_BRIDGE_R_LOAD,

	/**
	 * Serial port (UART) to be used
	 */
    AA_PARAM_UART_DEVICE,

	/**
	 * Serial (COM) port speed, bps
	 */
    AA_PARAM_UART_BAUDRATE,

	/**
	 * 2-channel audio input device to be used for analog-to-digital conversion.
	 */
    AA_PARAM_AUDIO_INPUT_DEVICE,

	/**
     * Time in milliseconds after which the power saving mode is activated (0 - disabled)
	 */
    AA_PARAM_PWRSAVE_TIMEOUT,

	/**
	 * Enable setting frequency on 3rd harmonic above BAND_FMAX
	 * 		1: enable
	 * 		0: disable
	 */
	AA_PARAM_3RD_HARM_ENABLE,

	/**
	 * Show S11 graph in the panoramic window
	 * 		1: show
	 * 		0: hide
	 */
	AA_PARAM_S11_GRAPH_SHOW,

	/**
	 * Type of Touchstone S1P file saved with panoramic screenshot
	 */
	AA_PARAM_S1P_FILE_TYPE,

	/**
	 * Show advanced options in configuration menu
	 */
	AA_PARAM_SHOW_ADVANCED,

	/**
     * Image format for screenshots ("bmp", "png", "jpg")
	 */
    AA_PARAM_SNAPSHOT_FORMAT,


    NUM_AA_PARAMS
};

enum AAPIParamType {
    AA_PT_BYTE, 	//8-bit unsigned
    AA_PT_UINT, 	//32-bit unsigned
    AA_PT_INT,   	//32-bit signed
    AA_PT_FLOAT,	//32-bit float
    AA_PT_TEXT,     //char*
};

enum AAPIS1pType {
    AA_S1P_TYPE_S_MA = 0u,
    AA_S1P_TYPE_S_RI = 1u
};

enum AAPIXtalRate {
	AA_XTALF_25MHZ	= 25000000u,
	AA_XTALF_27MHZ	= 27000000u
};

enum AAPIConfigError {
    AAPI_CONFIG_E_OPEN_FILE_FAILED	= CONFIG_ERROR_START
};

///////////////////////////////////////////////////////////////////////////////
// struct AAPIRadioBand
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIRadioBand struct
///
struct AAPIRadioBand
{
    uint32_t lo;
    uint32_t hi;
    char name[30];

    AAPIRadioBand(uint32_t lo, uint32_t hi, const char *name);
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIConfig
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIConfig class
///
class AAPIConfig : public AAPIObject
{
    DECLARE_AAPI_OBJECT(AAPIConfig)

protected:
    AAPIConfig();
	~AAPIConfig();

public:
    AAPIConfig& operator=(const AAPIConfig& config);

    uint32_t get_num_params() const;

    QString get_name(int index) const;
    QString get_description(int index) const;
    uint32_t get_precision(int index) const;
    bool is_valid(int index) const;

    uint32_t get_num_opt(enum AAPIParamId id) const;
    uint32_t get_num_opt(int index) const;
    const QVariant *get_opt_values(enum AAPIParamId id) const;
    const QVariant *get_opt_values(int index) const;
    const QString *get_opt_labels(enum AAPIParamId id) const;
    const QString *get_opt_labels(int index) const;

    QVariant get_value(enum AAPIParamId id) const;
    QVariant get_value(int index) const;

    void set_value(enum AAPIParamId id, QVariant val);
    void set_value(int index, QVariant val);

    uint32_t get_bin_freq(uint32_t bin) const;
    /* Intermediate frequency */
    uint32_t get_ifreq() const;

	AA_PARAM_STRING_ACCESSOR(AA_PARAM_VERSION, version)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_DSP_SAMPLE_RATE, dsp_sample_rate)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_DSP_SAMPLE_SIZE, dsp_sample_size)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_DSP_NSAMPLES, dsp_nsamples)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_DSP_FFTBIN, dsp_fftbin)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_PAN_FREQ1, pan_freq1)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_PAN_SPAN, pan_span)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_MEASURE_FREQ, measure_freq)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_SYNTH_XTAL_FREQ, synth_xtal_freq)
    AA_PARAM_INT32_ACCESSOR(AA_PARAM_SYNTH_XTAL_CORR, synth_xtal_corr)
    AA_PARAM_INT32_ACCESSOR(AA_PARAM_OSL_SELECTED, osl_selected)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_BASE_R0, base_r0)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_OSL_R_LOAD, osl_r_load)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_OSL_R_SHORT, osl_r_short)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_OSL_R_OPEN, osl_r_open)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_OSL_N_SCANS, osl_n_scans)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_MEASURE_N_SCANS, measure_n_scans)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_PAN_N_SCANS, pan_n_scans)
    AA_PARAM_STRING_ACCESSOR(AA_PARAM_AUDIO_INPUT_DEVICE, audio_input_device)
    AA_PARAM_UINT8_ACCESSOR(AA_PARAM_AUDIO_INPUT_GAIN, audio_input_gain)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_LO_FREQ_DIV_BY_2, lo_freq_div_by_2)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_GENERATOR_FREQ, generator_freq)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_PAN_IS_CENTER_FREQ, pan_is_center_freq)
    AA_PARAM_FLOAT_ACCESSOR(AA_PARAM_BRIDGE_R_MEASURE, bridge_r_measure)
    AA_PARAM_FLOAT_ACCESSOR(AA_PARAM_BRIDGE_R_MEASURE_ADD, bridge_r_measure_add)
	AA_PARAM_FLOAT_ACCESSOR(AA_PARAM_BRIDGE_R_LOAD, bridge_r_load)
	AA_PARAM_STRING_ACCESSOR(AA_PARAM_UART_DEVICE, uart_device)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_UART_BAUDRATE, uart_baudrate)
    AA_PARAM_UINT32_ACCESSOR(AA_PARAM_PWRSAVE_TIMEOUT, pwr_save_timeout)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_3RD_HARM_ENABLE, 3rd_harm_enable)
    AA_PARAM_STRING_ACCESSOR(AA_PARAM_SNAPSHOT_FORMAT, snapshot_format)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_S11_GRAPH_SHOW, s11_graph_show)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_S1P_FILE_TYPE, s1p_file_type)
	AA_PARAM_UINT32_ACCESSOR(AA_PARAM_SHOW_ADVANCED, show_advanced)

    int init();
	int flush();

    static int get_index(enum AAPIParamId id);
    static enum AAPIParamId get_id(int index);
    static enum AAPIParamType get_type(int index);
    static QVariant get_value(const AAPIConfig *config, enum AAPIParamId param);
    static uint32_t get_total_params();

    static QString get_app_dir();
    static QString get_config_dir();
    static QString get_config_path();

    static AAPIRadioBand *get_ham_bands();
    static uint32_t get_num_ham_bands();

private:
    void set_defaults();
    void copy(const AAPIConfig& config);

private:
    QVariant    m_values[ NUM_AA_PARAMS ];
    QMutex		m_mutex;
};

} // namespace aapi

#endif // AAPI_CONFIG_H
