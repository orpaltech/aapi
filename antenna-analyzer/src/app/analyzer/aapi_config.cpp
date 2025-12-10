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

#include <unistd.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "analyzer/aapi_config.h"
#include "serial/aapi_uart_device.h"
#include "audio/audio_reader.h"
namespace fs = std::filesystem;
#include <QDebug>

///////////////////////////////////////////////////////////////////////////////
// Config definitions
///////////////////////////////////////////////////////////////////////////////
#define AA_CONFIG_FILE_MAGIC        0xE0B0A010
#define AA_CONFIG_FILE_VERSION      0x00040002

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Function definitions
///////////////////////////////////////////////////////////////////////////////

static bool show_advanced(const AAPiConfig *config)
{
    return config->get_show_advanced() > 0;
}

typedef bool (*pfn_is_valid)(const AAPiConfig *config);

static AAPiVariantArray strings_to_variants(const AAPiStringArray& strs)
{
    AAPiVariantArray vars;
    for (int i=0; i < strs.size(); i++)
    {
        vars.append(strs[i]);
    }
    return vars;
}

///////////////////////////////////////////////////////////////////////////////
// class AAPIParam
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIParam class
///
class AAPIParam
{
public:
    AAPIParam(enum AAPiParameter id, const char *name, enum AAPiVariantType type);
    AAPIParam(const AAPIParam& param);

    AAPIParam& operator=(const AAPIParam& param);

    AAPIParam& set_description(const char *description);
    AAPIParam& add_option(AAPiVariant value, const char *label = nullptr);
    AAPIParam& add_options(const AAPiVariantArray& values, const AAPiStringArray& labels);
    AAPIParam& clear_options();
    AAPIParam& set_repeat_delay(uint32_t repeat_delay);
    AAPIParam& set_reboot_required(bool reboot_required);
    AAPIParam& set_is_valid(pfn_is_valid is_valid);
    AAPIParam& set_precision(uint32_t precision);

    AAPIParam& add_audio_devices();
    AAPIParam& add_uart_devices();

public:
    /**
     * ID of the configuration parameter, see AnalyzerParam enum
     */
    enum AAPiParameter id;

    /**
     * Short parameter name to be displayed
     */
    AAPiString name;

    /**
     * Parameter value type, see AnalyzerParamType enum.
     */
    enum AAPiVariantType type;

    /**
     * Number of fractional digits for floating point numbers
     */
    uint32_t precision;

    /**
     * Detailed description of the parameter
     */
    AAPiString description;

    /**
     * Array of values that can be selected for parameter.
     */
    AAPiVariantArray opt_values;

    /**
     * Array of alternative string representations for values that can be selected for parameter.
     * Length of the array is specified in n_values.
     */
    AAPiStringArray opt_labels;

    /**
     * Nonzero if continuous tap of value should be detected.
     * Number of ms to sleep between callbacks.
     */
    uint32_t repeat_delay;

    /**
     * Optional callback that can be defined.
     * Returning false from this function will force parameter to be hidden.
     */
    pfn_is_valid is_valid;

    /**
     * Nonzero if reset is required to apply a parameter value
     */
    bool reboot_required;

private:
    void copy(const AAPIParam& param);
};


///////////////////////////////////////////////////////////////////////////////
// Parameter table definition
///////////////////////////////////////////////////////////////////////////////

const static AAPIParam g_param_table[] =
{
    AAPIParam(AAPI_PARAM_OSL_SELECTED,  "osl_selected", AAPI_VT_INT)
                    .add_option((int)0, "A")
                    .add_option((int)1, "B")
                    .add_option((int)2, "C")
                    .add_option((int)3, "D")
                    .add_option((int)4, "E")
                    .add_option((int)5, "F")
                    .add_option((int)6, "G")
                    .add_option((int)7, "H")
                    .add_option((int)8, "I")
                    .add_option((int)9, "J")
                    .add_option((int)10, "K")
                    .add_option((int)11, "L")
                    .add_option((int)12, "M")
                    .add_option((int)13, "N")
                    .add_option((int)14, "O")
                    .add_option((int)15, "P")
                    .add_option((int)-1, "None")
                    .set_description("Selected OSL file"),

    AAPIParam(AAPI_PARAM_BASE_R0,   "base_r0", AAPI_VT_UINT)
                    .add_option(28U)
                    .add_option(50U)
                    .add_option(75U)
                    .add_option(100U)
                    .add_option(150U)
                    .add_option(300U)
                    .set_description("Base impedance (R0) for Smith and VSWR, Ohm"),

    AAPIParam(AAPI_PARAM_SYNTH_XTAL_FREQ,   "synth_xtal_freq", AAPI_VT_UINT)
                    .add_option((uint32_t) AAPI_XTAL_25MHZ, "25")
                    .add_option((uint32_t) AAPI_XTAL_27MHZ, "27")
                    .set_description("Synthesizer IC crystal frequency, MHz"),

    AAPIParam(AAPI_PARAM_SYNTH_XTAL_CORR,   "synth_xtal_corr", AAPI_VT_INT)
                    .set_description("Synthesizer IC crystal correction, Hz")
                    .set_repeat_delay(20),

    AAPIParam(AAPI_PARAM_OSL_R_OPEN,    "osl_r_open", AAPI_VT_UINT)
                    .add_option(300U, "300 Ohm")
                    .add_option(333U, "333 Ohm")
                    .add_option(500U, "500 Ohm")
                    .add_option(750U, "750 Ohm")
                    .add_option(1000U, "1000 Ohm")
                    .add_option(999999U, "INFINITE")
                    .set_description("Open impedance for OSL calibration, Ohm"),

    AAPIParam(AAPI_PARAM_OSL_R_SHORT,   "osl_r_short", AAPI_VT_UINT)
                    .add_option(0U, "Short circuit")
                    .add_option(5U, "5 Ohm")
                    .add_option(10U, "10 Ohm")
                    .set_description("Short impedance for OSL calibration, Ohm"),

    AAPIParam(AAPI_PARAM_OSL_R_LOAD,    "osl_r_load", AAPI_VT_UINT)
                    .add_option(50U, "50 Ohm")
                    .add_option(75U, "75 Ohm")
                    .add_option(100U, "100 Ohm")
                    .add_option(150U, "150 Ohm")
                    .set_description("Load impedance for OSL calibration, Ohm"),

    AAPIParam(AAPI_PARAM_OSL_N_SCANS,   "osl_num_scans", AAPI_VT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average during OSL calibration at each frequency"),

    AAPIParam(AAPI_PARAM_MEASURE_N_SCANS,   "measure_num_scans", AAPI_VT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in measurement window at each frequency"),

    AAPIParam(AAPI_PARAM_PAN_N_SCANS,	"pan_num_scans", AAPI_VT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in panoramic window at each frequency"),

    AAPIParam(AAPI_PARAM_AUDIO_INPUT_GAIN, "audio_input_gain", AAPI_VT_BYTE)
                    .add_option(0U)
                    .add_option(3U)
                    .add_option(6U)
                    .add_option(9U)
                    .add_option(12U)
                    .add_option(15U)
                    .add_option(18U)
                    .add_option(21U)
                    .add_option(24U)
                    .add_option(27U)
                    .add_option(30U)
                    .set_is_valid(show_advanced)
                    .set_description("Audio line input attenuation, dB. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPI_PARAM_BRIDGE_R_MEASURE, "bridge_r_measure", AAPI_VT_FLOAT)
                    .add_option(1.f)
                    .add_option(5.1f)
                    .add_option(10.f)
                    .set_precision(1)
                    .set_is_valid(show_advanced)
                    .set_description("Bridge R-measure value, Ohm"),

    AAPIParam(AAPI_PARAM_BRIDGE_R_MEASURE_ADD, "bridge_r_measure_add", AAPI_VT_FLOAT)
                    .add_option(33.f)
                    .add_option(51.f)
                    .add_option(75.f)
                    .add_option(100.f)
                    .add_option(120.f)
                    .add_option(150.f)
                    .add_option(200.f)
                    .set_precision(1)
                    .set_is_valid(show_advanced)
                    .set_description("Bridge R-add value, Ohm"),

    AAPIParam(AAPI_PARAM_PAN_IS_CENTER_FREQ, "pan_is_center_freq", AAPI_VT_UINT)
                    .add_option(0U, "Start Frequency")
                    .add_option(1U, "Center Frequency")
                    .set_description("Use either start or center frequency in panoramic view."),

    AAPIParam(AAPI_PARAM_AUDIO_INPUT_DEVICE, "audio_input_device", AAPI_VT_TEXT)
                    .add_audio_devices()
                    .set_is_valid(show_advanced)
                    .set_description("Audio input device for DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPI_PARAM_DSP_SAMPLE_RATE, "dsp_sample_rate", AAPI_VT_UINT)
                    .add_option((uint) AUDIO_SRATE_48K)
                    .add_option((uint) AUDIO_SRATE_96K)
                    .set_is_valid(show_advanced)
                    .set_description("Select the sample rate to use in DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPI_PARAM_UART_DEVICE, "uart_device", AAPI_VT_TEXT)
                    .add_uart_devices()
                    .set_is_valid(show_advanced)
                    .set_description("Select UART for remote control. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPI_PARAM_UART_BAUDRATE, "uart_baudrate", AAPI_VT_UINT)
                    .add_option((uint) UART_BR_9600)
                    .add_option((uint) UART_BR_19200)
                    .add_option((uint) UART_BR_38400)
                    .add_option((uint) UART_BR_57600)
                    .add_option((uint) UART_BR_115200)
                    .set_is_valid(show_advanced)
                    .set_description("Select UART baudrate. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPI_PARAM_PWRSAVE_TIMEOUT, "pwr_save_timeout", AAPI_VT_UINT)
                    .add_option(0U, "Off")
                    .add_option(30000U, "30s")
                    .add_option(60000U, "1 min")
                    .add_option(120000U, "2 min")
                    .add_option(180000U, "3 min")
                    .add_option(300000U, "5 min")
                    .set_description("Power saving mode after period of inactivity. Tap to wake up."),

    // unused yet
    /*AAPIParam(AA_PARAM_3RD_HARMONIC, "3rd_harmonic", AA_PT_UINT32)
                    .addOpt(0u, "Off").addOpt(1u, "On")
                    .setDescription("Allow measurements on 3rd harmonic (above maximum frequency)"),*/

    // more details here: http://www.antenna-theory.com/definitions/sparameters.php
    AAPIParam(AAPI_PARAM_S11_GRAPH_SHOW, "s11_graph_show", AAPI_VT_UINT)
                    .add_option(0U, "No")
                    .add_option(1U, "Yes")
                    .set_description("Display S11 graph in panoramic view."),

    AAPIParam(AAPI_PARAM_S1P_FILE_TYPE, "s1p_file_type", AAPI_VT_UINT)
                    .add_option((uint32_t) AAPI_S1P_S_MA, "S MA R50")
                    .add_option((uint32_t) AAPI_S1P_S_RI, "S RI R50")
                    .set_description("Touchstone S1P file type saved with screenshot. Default is S MA R 50."),

    AAPIParam(AAPI_PARAM_SNAPSHOT_FORMAT, "snapshot_format", AAPI_VT_TEXT)
                    .add_option("bmp", "Bitmap")
                    .add_option("png", "PNG")
                    .add_option("jpg", "JPEG")
                    .set_is_valid(show_advanced)
                    .set_description("Image file format for screenshots."),

    AAPIParam(AAPI_PARAM_SHOW_ADVANCED, "show_advanced", AAPI_VT_UINT)
                    .add_option(0U, "Off")
                    .add_option(1U, "On")
                    .set_description("Display advanced menu parameters."),

};

const static size_t g_param_table_num = sizeof(g_param_table) / sizeof(AAPIParam);

static AAPiRadioBand g_ham_bands [] = {
    /* HF ITU region 1 bands */
    AAPiRadioBand(1800ul, 2000ul, "160m"),
    AAPiRadioBand(3500ul, 3800ul, "80m"),
    AAPiRadioBand(7000ul, 7300ul, "40m"),
    AAPiRadioBand(10100ul, 10150ul, "30m"),
    AAPiRadioBand(14000ul, 14350ul, "20m"),
    AAPiRadioBand(18068ul, 18168ul, "17m"),
    AAPiRadioBand(21000ul, 21450ul, "15m"),
    AAPiRadioBand(24890ul, 24990ul, "12m"),
    AAPiRadioBand(28000ul, 29700ul, "10m"),
    /* VHF bands */
    AAPiRadioBand(50000ul, 54000ul, "6m"),
    AAPiRadioBand(144000ul, 148000ul, "2m"),
    AAPiRadioBand(222000ul, 225000ul, "1.25m"), // (USA, Canada)
    /* UHF bands */
    AAPiRadioBand(430000ul, 440000ul, "70cm")
};

const static size_t g_ham_bands_num = sizeof(g_ham_bands) / sizeof(g_ham_bands[0]);

///////////////////////////////////////////////////////////////////////////////
// class AAPIParam
///////////////////////////////////////////////////////////////////////////////

AAPIParam::AAPIParam(enum AAPiParameter id, const char *name, enum AAPiVariantType type)
{
    this->id = id;
    this->name = name;
    this->type = type;
    this->repeat_delay = 0;
    this->reboot_required = false;
    this->is_valid = nullptr;
    this->precision = 0;
}

AAPIParam::AAPIParam(const AAPIParam& param)
{
    copy(param);
}

AAPIParam& AAPIParam::operator=(const AAPIParam& param)
{
    if (this != &param)
    {
        copy(param);
    }
    return *this;
}

void AAPIParam::copy(const AAPIParam& param)
{
    id = param.id;
    type = param.type;
    repeat_delay = param.repeat_delay;
    is_valid = param.is_valid;
    precision = param.precision;
    reboot_required = param.reboot_required;
    // shallow copy
    name = param.name;
    description = param.description;
    opt_values = param.opt_values;
    opt_labels = param.opt_labels;
}

AAPIParam &AAPIParam::set_description(const char *desc)
{
    this->description = desc;
    return *this;
}

AAPIParam &AAPIParam::add_option(AAPiVariant value, const char *label)
{
    opt_values.append(value);

    if (label)
        opt_labels.append(label);
    else
        opt_labels.append("");

    return *this;
}

AAPIParam &AAPIParam::add_options(const AAPiVariantArray& values, const AAPiStringArray& labels)
{
    opt_values = values;
    opt_labels = labels;
    return *this;
}

AAPIParam &AAPIParam::clear_options()
{
    opt_values.clear();
    opt_labels.clear();
    return *this;
}

 AAPIParam &AAPIParam::set_repeat_delay(uint32_t repeat_delay)
{
    this->repeat_delay = repeat_delay;
    return *this;
}

AAPIParam &AAPIParam::set_reboot_required(bool reboot_required)
{
    this->reboot_required = reboot_required;
    return *this;
}

AAPIParam &AAPIParam::set_is_valid(pfn_is_valid is_valid)
{
    this->is_valid = is_valid;
    return *this;
}

AAPIParam &AAPIParam::set_precision(uint32_t precision)
{
    this->precision = precision;
    return *this;
}

AAPIParam &AAPIParam::add_audio_devices()
{
    AAPiPtr<AAPiAudioReader>  reader( AAPiAudioReader::create( false ) );

    for (uint i = 0; i < reader->get_num_devices(); i++)
    {
        if (
            reader->is_format_supported( i, AUDIO_CHANNELS_2, AUDIO_SRATE_48K, AUDIO_SSIZE_16 ) ||
            reader->is_format_supported( i, AUDIO_CHANNELS_2, AUDIO_SRATE_48K, AUDIO_SSIZE_24 ) ||
            reader->is_format_supported( i, AUDIO_CHANNELS_2, AUDIO_SRATE_96K, AUDIO_SSIZE_16 ) ||
            reader->is_format_supported( i, AUDIO_CHANNELS_2, AUDIO_SRATE_96K, AUDIO_SSIZE_24 )
            )
        {
            // Device satisfies the requirements
            add_option( reader->get_device_id(i), reader->get_device_name(i) );
        }
    }

    return *this;
}

AAPIParam &AAPIParam::add_uart_devices()
{
    AAPiStringArray uarts;
    int ret;

    ret = AAPiUartDevice::enumerate(uarts);
    if (ret == AAPI_SUCCESS)
    {
        add_options(strings_to_variants(uarts), uarts);
    }

    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// struct AAPiRadioBand
///////////////////////////////////////////////////////////////////////////////

AAPiRadioBand::AAPiRadioBand(uint32_t lo, uint32_t hi, const char *name)
{
    this->lo = lo;
    this->hi = hi;
    size_t len = 0;
    if (name)
    {
        len = strlen(name);
        size_t max = sizeof(this->name)-1;
        if (len > max)
            len = max;
        strncpy(this->name, name, len);
    }
    this->name[len] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// class AAPiConfig
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT(AAPiConfig)

AAPiConfig::AAPiConfig()
{
    init( );
}

AAPiConfig::AAPiConfig(const AAPiConfig& config)
{
    copyFrom( config );
}

AAPiConfig::~AAPiConfig()
{
}

AAPiConfig& AAPiConfig::operator=(const AAPiConfig& config)
{
    if( this != &config )
    {
        copyFrom( config );
    }
    return *this;
}

AAPiString AAPiConfig::get_app_dir()
{
    AAPiString homeDir = std::getenv("HOME");
    fs::path path( (const char *)homeDir );
    path.append( AAPI_APP_NAME );
    return path.c_str();
}

AAPiString AAPiConfig::get_config_dir()
{
    fs::path path( (const char *)get_app_dir() );
    path.append("config");
    return path.c_str();
}

AAPiString AAPiConfig::get_config_path()
{
    fs::path path( (const char *)get_config_dir() );
    path.append("config.bin");
    return path.c_str();
}

AAPiRadioBand *AAPiConfig::get_ham_bands()
{
    return g_ham_bands;
}

uint32_t AAPiConfig::get_num_ham_bands()
{
    return g_ham_bands_num;
}

void AAPiConfig::copyFrom (const AAPiConfig& config)
{
    // deep copy
    for( int i = 0; i < NUM_AAPI_PARAMS; i ++ )
    {
        m_values[i] = config.m_values[i];
    }
}

void AAPiConfig::set_defaults()
{
    int i;

    for( i = 0; i < NUM_AAPI_PARAMS; i ++ )
    {
        m_values[i] = AAPiVariant();
    }

    // set defaults for all parameters
    set_version(AAPI_VERSION);

    // audio settings
    set_dsp_sample_rate(AUDIO_SRATE_48K);
    set_dsp_sample_size(AUDIO_SSIZE_24);
    set_dsp_num_samples(1024);

    set_synth_xtal_freq(AAPI_XTAL_25MHZ);
    set_synth_xtal_corr(0);

    set_osl_selected(-1); /*TODO: set back to -1*/

    set_base_r0(50);
    set_osl_r_load(50);
    set_osl_r_short(5);
    set_osl_r_open(500);
    set_osl_n_scans(1); /* TODO: set back to 1 */
    set_measure_n_scans(1);
    set_pan_n_scans(1);
    set_lo_freq_div_by_2(0);
    set_generator_freq(12288000);
    set_pan_freq1(7000000);
    set_pan_span(8000/*000*/);
    set_measure_freq(14000000);
    set_pan_is_center_freq(0);
    set_bridge_r_measure(5.1f);
    set_bridge_r_measure_add(200.f);
    set_bridge_r_load(52.f);
    set_audio_input_gain(0);
    set_uart_device(""); // Raspberry Pi3 option "/dev/ttyAMA0"
    set_uart_baudrate(UART_BR_38400);
    set_pwr_save_timeout(0);
    set_3rd_harm_enable(0);
    set_snapshot_format("png");
    set_s11_graph_show(1);
    set_s1p_file_type(AAPI_S1P_S_MA);
    set_show_advanced(0);

    i = get_index(AAPI_PARAM_AUDIO_INPUT_DEVICE);
    if( i >= 0 && get_num_opts(i) > 0 )
    {
        const AAPiVariantArray& devices = get_opt_values(i);
        set_audio_input_device( devices[0].toString() );
    }
    else
    {
        set_audio_input_device("");
    }
}

int AAPiConfig::init()
{
    /* set default values as fallback */
    set_defaults();

    AAPiMutexLocker locker(&m_mutex);

    const fs::path dirPath( (const char *)get_config_dir() );
    if (! fs::exists( dirPath ))
    {
        if (! fs::create_directory( dirPath ))
        {
            qDebug() << "Failed to create directory.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    std::ifstream file( get_config_path(), std::ios::in | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to open configuration file.";
        return AAPI_SUCCESS;
    }

    // Read and check the header
    uint32_t magic, version;
    file >> magic;
    file >> version;

    if( AA_CONFIG_FILE_MAGIC    == magic	&&
        AA_CONFIG_FILE_VERSION  == version )
    {
        // read parameter values
        for( int i = 0; i < NUM_AAPI_PARAMS; i++ )
        {
            file >> m_values[i];
        }
    }
    else
    {
        qDebug() << "Bad configuration file format, ignore..";
    }

    // close explicitly
    file.close();

    // correct some values
    uint32_t timeout = get_pwr_save_timeout();
    if( timeout > 0 && timeout < 10000 )
    {
        set_pwr_save_timeout(0);
    }

    return 0;
}

int AAPiConfig::flush()
{
    AAPiMutexLocker lock(&m_mutex);

    const fs::path dirPath( (const char *)get_config_dir() );
    if (! fs::exists( dirPath ))
    {
        if (! fs::create_directory( dirPath ))
        {
            qDebug() << "Failed to create directory.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    std::ofstream file( get_config_path(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (! file.is_open( ))
    {
        qDebug() << "Unable to create configuration file.";
        return AAPI_CONF_E_FILE_OPEN_FAILED;
    }

    // we will serialize the data into the file

    // Write a header with a "magic number" and a version
    file << static_cast< uint >( AA_CONFIG_FILE_MAGIC );
    file << static_cast< uint >( AA_CONFIG_FILE_VERSION );

    // flush param values
    for ( int i = 0; i < NUM_AAPI_PARAMS; i++ )
    {
        file << m_values[i];
    }

    // Explicit close
    file.close();

    return 0;
}

uint32_t AAPiConfig::get_num_params() const
{
    uint32_t num = 0, i;
    for( i = 0; i < get_total_params(); i++ )
    {
        if( is_valid(i) )
            num++;
    }
    return num;
}

AAPiVariant AAPiConfig::get_value(const AAPiConfig *config, enum AAPiParameter id)
{
    return config->get_value( id );
}

uint32_t AAPiConfig::get_total_params()
{
    return g_param_table_num;
}

int AAPiConfig::get_index(enum AAPiParameter id)
{
    for( uint i = 0; i < g_param_table_num; i++ )
    {
        if( get_id(i) == id )
            return i;
    }
    return -1;
}

enum AAPiParameter AAPiConfig::get_id(int index)
{
    return g_param_table[ index ].id;
}

enum AAPiVariantType AAPiConfig::get_type(int index)
{
    return g_param_table[ index ].type;
}

AAPiString AAPiConfig::get_name(int index) const
{
    return g_param_table[ index ].name;
}

AAPiString AAPiConfig::get_description(int index) const
{
    return g_param_table[ index ].description;
}

uint32_t AAPiConfig::get_precision(int index) const
{
    return g_param_table[ index ].precision;
}

bool AAPiConfig::is_valid(int index) const
{
    if( g_param_table[ index ].is_valid )
    {
        return g_param_table[ index ].is_valid( this );
    }
    return true;
}

uint32_t AAPiConfig::get_num_opts(enum AAPiParameter id) const
{
    int index = get_index( id );
    if( index < 0 )
        return 0;

    return get_num_opts( index );
}

uint32_t AAPiConfig::get_num_opts(int index) const
{
    return static_cast< uint32_t > ( g_param_table[ index ].opt_values.size() );
}

const AAPiVariantArray& AAPiConfig::get_opt_values(enum AAPiParameter id) const
{
    int index = get_index( id );
    if( index < 0 )
        throw;

    return get_opt_values( index );
}

const AAPiVariantArray& AAPiConfig::get_opt_values(int index) const
{
    return g_param_table[ index ].opt_values;
}

const AAPiStringArray& AAPiConfig::get_opt_labels(enum AAPiParameter id) const
{
    int index = get_index( id );
    if ( index < 0 )
        throw;

    return get_opt_labels( index );
}

const AAPiStringArray& AAPiConfig::get_opt_labels(int index) const
{
    return g_param_table[ index ].opt_labels;
}

AAPiVariant AAPiConfig::get_value(int index) const
{
    return get_value( g_param_table[ index ].id );
}

AAPiVariant AAPiConfig::get_value(enum AAPiParameter id) const
{
    if( id < 0 || id >= NUM_AAPI_PARAMS )
    {
        return AAPiVariant::Unknown;
    }

    return m_values[ id ];
}

void AAPiConfig::set_value(int index, const AAPiVariant& val)
{
    set_value( get_id( index ), val );
}

void AAPiConfig::set_value(enum AAPiParameter id, const AAPiVariant& val)
{
    if( id < 0 || id >= NUM_AAPI_PARAMS )
    {
        return;
    }

    AAPiVariant tmp = val;
    int index = get_index( id );
    if( index >= 0 )
    {
        // registered parameter, type is known
        switch( get_type( index ) )
        {
        case AAPI_VT_BYTE:
        case AAPI_VT_UINT:
            tmp = tmp.toUInt();
            break;
        case AAPI_VT_INT:
            tmp = tmp.toInt();
            break;
        case AAPI_VT_FLOAT:
            tmp = tmp.toFloat();
            break;
        case AAPI_VT_TEXT:
            tmp = tmp.toString( );
            break;
        case AAPI_VT_UNKNOWN:
            break;
        }
    }

    m_values[ id ] = tmp;
}

double AAPiConfig::get_dsp_fft_bin_width() const
{
    return static_cast<double>( get_dsp_sample_rate() / 2 ) / get_dsp_fft_num_pts();
}

uint32_t AAPiConfig::get_dsp_fft_num_pts() const
{
    // The number of useful FFT points is always 1/2 of the input samples.

    // IMPORTANT: When dealing with real-valued input signals, the FFT output
    // exhibits symmetry.
    // In such cases, the second half of the output spectrum is a conjugate
    // of the first half, effectively containing redundant information.
    // You might be discarding this redundant half, making it appear as if
    // the output is half the size.

    uint32_t num_fft_pts = get_dsp_num_samples() / 2;
    return num_fft_pts;
}

double AAPiConfig::get_dsp_fft_bin_freq(uint32_t bin) const
{   
    return get_dsp_fft_bin_width() * bin;
}

uint32_t AAPiConfig::get_dsp_fft_if_bin() const
{
    // with number of samples 1024 it must be 131 at 96KHz; 262 at 48KHz
    uint32_t bin = (uint32_t)( AAPI_INT_FREQ_HZ / get_dsp_fft_bin_width() + 0.5 );
    return bin;
}

/////////////////////////////////////////////////////////////////////////////

AAPiString get_sysfs_property_path(const char *property_name)
{
    AAPiString ss;

    ss = AAPI_SYSFS_PATH;
    ss += "/";
    ss += property_name;

    return ss;
}

} // namespace aapi
