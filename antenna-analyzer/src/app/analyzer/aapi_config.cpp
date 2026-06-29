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
#define AA_CONFIG_FILE_VERSION      0x0001000a


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

    // Allocate all memory once upfront. Loop becomes a pure initialization chain.
    if (vars.reserve(strs.size())) {
        for (uint i = 0; i < strs.size(); i++) {
            vars.append(strs[i]);
        }
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
    explicit AAPIParam(enum AAPiParameter id, const char *name, enum AAPiVariantType type);
    AAPIParam(const AAPIParam& param);
    AAPIParam(AAPIParam&& param) noexcept;

    AAPIParam& operator=(const AAPIParam& param);
    AAPIParam& operator=(AAPIParam&& param) noexcept;

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
    void copyFrom(const AAPIParam& param);
    void moveFrom(AAPIParam&& param) noexcept;
};


///////////////////////////////////////////////////////////////////////////////
// Parameter table definition
///////////////////////////////////////////////////////////////////////////////

const static AAPIParam g_param_table[] =
{
    AAPIParam(AAPiParameter::OSL_FILE_SELECTED, "osl_file_selected", AAPiVariantType::INT)
                    .add_option(0, "A")
                    .add_option(1, "B")
                    .add_option(2, "C")
                    .add_option(3, "D")
                    .add_option(4, "E")
                    .add_option(5, "F")
                    .add_option(6, "G")
                    .add_option(7, "H")
                    .add_option(8, "I")
                    .add_option(9, "J")
                    .add_option(10, "K")
                    .add_option(11, "L")
                    .add_option(12, "M")
                    .add_option(13, "N")
                    .add_option(14, "O")
                    .add_option(15, "P")
                    .add_option(-1, "None")
                    .set_description("Selected OSL file"),

    AAPIParam(AAPiParameter::BASE_R0,   "base_r0", AAPiVariantType::UINT)
                    .add_option(28U)
                    .add_option(50U)
                    .add_option(75U)
                    .add_option(100U)
                    .add_option(150U)
                    .add_option(300U)
                    .set_description("Base impedance (R0) for Smith and VSWR, Ohm"),

    /*AAPIParam(AAPI_PARAM_SYNTH_XTAL_FREQ,   "synth_xtal_freq", AAPI_VT_UINT)
                    .add_option((uint32_t) AAPI_XTAL_25MHZ, "25")
                    .add_option((uint32_t) AAPI_XTAL_27MHZ, "27")
                    .set_description("Synthesizer IC crystal frequency, MHz"),*/

    /*AAPIParam(AAPI_PARAM_SYNTH_XTAL_CORR,   "synth_xtal_corr", AAPI_VT_INT)
                    .set_description("Synthesizer IC crystal correction, Hz")
                    .set_repeat_delay(20),*/

    AAPIParam(AAPiParameter::OSL_R_OPEN,    "osl_r_open", AAPiVariantType::UINT)
                    .add_option(300U, "300 Ohm")
                    .add_option(333U, "333 Ohm")
                    .add_option(500U, "500 Ohm")
                    .add_option(750U, "750 Ohm")
                    .add_option(1000U, "1000 Ohm")
                    .add_option(999999U, "Infinite")
                    .set_description("Open impedance for OSL calibration, Ohm"),

    AAPIParam(AAPiParameter::OSL_R_SHORT,   "osl_r_short", AAPiVariantType::UINT)
                    .add_option(0U, "Short circuit")
                    .add_option(5U, "5 Ohm")
                    .add_option(10U, "10 Ohm")
                    .set_description("Short impedance for OSL calibration, Ohm"),

    AAPIParam(AAPiParameter::OSL_R_LOAD,    "osl_r_load",   AAPiVariantType::UINT)
                    .add_option(50U, "50 Ohm")
                    .add_option(75U, "75 Ohm")
                    .add_option(100U, "100 Ohm")
                    .add_option(150U, "150 Ohm")
                    .set_description("Load impedance for OSL calibration, Ohm"),

    AAPIParam(AAPiParameter::CALIBR_NUM_SCANS,  "calibr_num_scans",  AAPiVariantType::UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average during calibration at each frequency"),

    AAPIParam(AAPiParameter::MEASURE_NUM_SCANS, "measure_num_scans",    AAPiVariantType::UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in measurement window at each frequency"),

    AAPIParam(AAPiParameter::PAN_NUM_SCANS, "pan_num_scans",    AAPiVariantType::UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in panoramic window at each frequency"),

    AAPIParam(AAPiParameter::AUDIO_INPUT_GAIN,  "audio_input_gain", AAPiVariantType::BYTE)
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

    AAPIParam(AAPiParameter::BRIDGE_R_MEASURE,  "bridge_r_measure", AAPiVariantType::FLOAT)
                    .add_option(1.f)
                    .add_option(5.1f)
                    .add_option(10.f)
                    .set_precision(1)
                    .set_is_valid(show_advanced)
                    .set_description("Bridge R-measure value, Ohm"),

    AAPIParam(AAPiParameter::BRIDGE_R_MEASURE_ADD,  "bridge_r_measure_add", AAPiVariantType::FLOAT)
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

    AAPIParam(AAPiParameter::PAN_IS_CENTER_FREQ,    "pan_is_center_freq",   AAPiVariantType::UINT)
                    .add_option(0U, "Start Frequency")
                    .add_option(1U, "Center Frequency")
                    .set_description("Use either start or center frequency in panoramic view."),

    AAPIParam(AAPiParameter::AUDIO_INPUT_DEVICE,    "audio_input_device",   AAPiVariantType::TEXT)
                    .add_audio_devices()
                    .set_is_valid(show_advanced)
                    .set_description("Audio input device for DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPiParameter::DSP_SAMPLE_RATE,   "dsp_sample_rate",  AAPiVariantType::UINT)
                    .add_option(static_cast<uint>(AAPiAudioSampleRate::_48K))
                    .add_option(static_cast<uint>(AAPiAudioSampleRate::_96K))
                    .set_is_valid(show_advanced)
                    .set_description("Select the sample rate to use in DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPiParameter::DSP_NUM_SAMPLES,   "dsp_num_samples",  AAPiVariantType::UINT)
                    .add_option(512U)
                    .add_option(1024U)
                    .add_option(2048U)
                    .set_is_valid(show_advanced)
                    .set_description("Select the number of samples to use in DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPiParameter::UART_DEVICE,   "uart_device",  AAPiVariantType::TEXT)
                    .add_uart_devices()
                    .set_is_valid(show_advanced)
                    .set_description("Select UART for remote control. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPiParameter::UART_BAUDRATE, "uart_baudrate",    AAPiVariantType::UINT)
                    .add_option((uint) UART_BR_9600)
                    .add_option((uint) UART_BR_19200)
                    .add_option((uint) UART_BR_38400)
                    .add_option((uint) UART_BR_57600)
                    .add_option((uint) UART_BR_115200)
                    .set_is_valid(show_advanced)
                    .set_description("Select UART baudrate. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AAPiParameter::PWRSAVE_TIMEOUT,   "pwr_save_timeout", AAPiVariantType::UINT)
                    .add_option(0U, "Off")
                    .add_option(30000U, "30s")
                    .add_option(60000U, "1 min")
                    .add_option(120000U, "2 min")
                    .add_option(180000U, "3 min")
                    .add_option(300000U, "5 min")
                    .set_description("Power saving mode after period of inactivity. Tap to wake up."),

    // unused yet
    AAPIParam(AAPiParameter::USE_3D_HARMONIC,   "use_3rd_harmonic", AAPiVariantType::BYTE)
                    .add_option(0u, "Off")
                    .add_option(1u, "On")
                    .set_description("Allow measurements at 3-rd harmonic (above maximum frequency)."),

    // more details here: http://www.antenna-theory.com/definitions/sparameters.php
    AAPIParam(AAPiParameter::S11_GRAPH_SHOW,    "s11_graph_show",   AAPiVariantType::BYTE)
                    .add_option(0U, "No")
                    .add_option(1U, "Yes")
                    .set_description("Display S11 graph in panoramic view."),

    AAPIParam(AAPiParameter::S1P_FILE_TYPE, "s1p_file_type",    AAPiVariantType::UINT)
                    .add_option((uint32_t) AAPI_S1P_S_MA, "S MA R50")
                    .add_option((uint32_t) AAPI_S1P_S_RI, "S RI R50")
                    .set_description("Touchstone S1P file type saved with screenshot. Default is S MA R 50."),

    AAPIParam(AAPiParameter::SNAPSHOT_FORMAT,   "snapshot_format",  AAPiVariantType::TEXT)
                    .add_option("bmp", "Bitmap")
                    .add_option("png", "PNG")
                    .add_option("jpg", "JPEG")
                    .set_is_valid(show_advanced)
                    .set_description("Image file format for screenshots."),

    AAPIParam(AAPiParameter::SHOW_ADVANCED, "show_advanced",    AAPiVariantType::BYTE)
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
    copyFrom(param);
}

AAPIParam::AAPIParam(AAPIParam&& param) noexcept
{
    moveFrom(std::move(param));
}

AAPIParam& AAPIParam::operator=(const AAPIParam& param)
{
    if (this != &param) {
        copyFrom(param);
    }
    return *this;
}

AAPIParam& AAPIParam::operator=(AAPIParam&& param) noexcept
{
    if (this != &param) {
        moveFrom(std::move(param));
    }
    return *this;
}

void AAPIParam::copyFrom(const AAPIParam& param)
{
    id = param.id;
    type = param.type;
    repeat_delay = param.repeat_delay;
    is_valid = param.is_valid;
    precision = param.precision;
    reboot_required = param.reboot_required;

    // copy data below
    name = param.name;
    description = param.description;
    opt_values = param.opt_values;
    opt_labels = param.opt_labels;
}

void AAPIParam::moveFrom(AAPIParam&& param) noexcept
{
    id = param.id;
    type = param.type;
    repeat_delay = param.repeat_delay;
    is_valid = param.is_valid;
    precision = param.precision;
    reboot_required = param.reboot_required;

    // move data below
    name = std::move(param.name);
    description = std::move(param.description);
    opt_values = std::move(param.opt_values);
    opt_labels = std::move(param.opt_labels);
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
        opt_labels.append(AAPiString{label});
    else
        opt_labels.append(AAPiString());

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

    for (uint i = 0; i < reader->get_num_devices(); i++) {
        if (
            reader->is_format_supported( i, AAPiAudioChannels::Stereo, AAPiAudioSampleRate::_48K, AAPiAudioSampleSize::_16 ) ||
            reader->is_format_supported( i, AAPiAudioChannels::Stereo, AAPiAudioSampleRate::_48K, AAPiAudioSampleSize::_24 ) ||
            reader->is_format_supported( i, AAPiAudioChannels::Stereo, AAPiAudioSampleRate::_96K, AAPiAudioSampleSize::_16 ) ||
            reader->is_format_supported( i, AAPiAudioChannels::Stereo, AAPiAudioSampleRate::_96K, AAPiAudioSampleSize::_24 )
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
    if (ret == AAPI_SUCCESS) {
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
    if (name) {
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
    if (this != &config) {
        copyFrom( config );
    }
    return *this;
}

AAPiString AAPiConfig::get_app_dir()
{
    fs::path targetPath;
    const char* homeEnv = std::getenv("HOME");

    // Ensure HOME is valid, fallback to native /root directory safely if blank
    if (homeEnv && std::strlen(homeEnv) > 0) {
        targetPath = fs::path(homeEnv);
    } else {
        targetPath = fs::path("/root");
    }

    // Using the canonical /= operator prevents argument parsing bugs
    // and guarantees that the folder lands strictly inside your home directory!
    targetPath /= AAPI_APP_NAME;

    return AAPiString{ targetPath.c_str() };
}

AAPiString AAPiConfig::get_config_dir()
{
    fs::path path( (const char *)get_app_dir() );
    path.append("config");

    return AAPiString { path.c_str() };
}

AAPiString AAPiConfig::get_config_path()
{
    fs::path path( (const char *)get_config_dir() );
    path.append("config.json");

    return AAPiString{ path.c_str() };
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
    for (uint i = 0; i < NUM_PARAMS; ++i) {
        m_values[i] = config.m_values[i];
    }
}

void AAPiConfig::set_defaults()
{
    for (uint i = 0; i < NUM_PARAMS; ++i) {
        m_values[i] = AAPiVariant();
    }

    // set defaults for all parameters
    set_version(STRINGIFY(AAPI_VERSION));

    // audio settings
    set_dsp_sample_rate(static_cast<uint>(AAPiAudioSampleRate::_48K));
    set_dsp_sample_size(static_cast<uint>(AAPiAudioSampleSize::_24));
    set_dsp_num_samples(512);

    //set_synth_xtal_freq(AAPI_XTAL_25MHZ);
    //set_synth_xtal_corr(0);

    set_osl_file_selected(-1); /* No file is selected by default */

    set_base_r0(50);
    set_osl_r_open(500);
    set_osl_r_short(5);
    set_osl_r_load(50);
    set_calibr_num_scans(1);
    set_measure_num_scans(1);
    set_pan_num_scans(1);
    set_lo_freq_div_by_2(0);
    set_generator_freq(14'000'000);
    set_pan_min_freq(7'000'000);
    set_pan_freq_span(800'000);
    set_measure_freq(14'000'000);
    set_pan_is_center_freq(0);
    set_bridge_r_measure(5.1f);
    set_bridge_r_measure_add(200.f);
    set_bridge_r_load(51.f);
    set_audio_input_gain(0);
    set_uart_device(""); // Raspberry Pi3 option "/dev/ttyAMA0"
    set_uart_baudrate(UART_BR_38400);
    set_pwr_save_timeout(0);
    set_use_3rd_harmonic(0);
    set_snapshot_format("png");
    set_s11_graph_show(1);
    set_s1p_file_type(AAPI_S1P_S_MA);
    set_show_advanced(0);

    int i = get_index(AAPiParameter::AUDIO_INPUT_DEVICE);
    if (i >= 0 && get_num_opts(i) > 0) {
        const AAPiVariantArray& devices = get_opt_values(i);

        set_audio_input_device( devices[0].toString() );

    } else {
        set_audio_input_device("");
    }
}

AAPiError AAPiConfig::init()
{
    /* set default values as fallback */
    set_defaults();

    AAPiError ret = load();
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    // correct some values
    uint32_t timeout = get_pwr_save_timeout();
    if( timeout > 0 && timeout < 10000 ) {
        set_pwr_save_timeout(0);
    }

    return 0;
}

AAPiError AAPiConfig::load()
{
    AAPiMutexLocker locker(&m_mutex);

    const fs::path dirPath( (const char *)get_config_dir() );
    if (! fs::exists( dirPath )) {
        if (! fs::create_directory( dirPath )) {
            qDebug() << "Failed to create directory.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    std::ifstream file( (const char *)get_config_path() );
    if (! file.is_open()) {
        qDebug() << "Unable to open configuration file.";
        return AAPI_SUCCESS;
    }

    std::string line;
    uint32_t magic = 0;
    uint32_t version = 0;
    bool hasMagic = false;
    bool hasVersion = false;

    // Verify Metadata Headers Exception-Free
    auto extractHexValue = [](const std::string& rawLine, size_t colonPos, unsigned int& outVal) -> bool {
        AAPiString valStr(rawLine.substr(colonPos + 1).c_str());
        return valStr.trim().toUInt(outVal, 16);
    };

    while (std::getline( file, line )) {
        size_t colonPos = line.find(":");
        if (colonPos != std::string::npos) {
            if (!hasMagic && line.find("magic") != std::string::npos) {
                hasMagic = extractHexValue(line, colonPos, magic);

            } else if (!hasVersion && line.find("version") != std::string::npos) {
                hasVersion = extractHexValue(line, colonPos, version);
            }
        }
        if (hasMagic && hasVersion) {
            break;
        }
    }

    if (AA_CONFIG_FILE_MAGIC != magic || AA_CONFIG_FILE_VERSION != version) {
        qDebug() << "Bad configuration file format or version mismatch, ignoring file.";
        file.close();
        return AAPI_SUCCESS;
    }

    // Phase 2: Read Parameter Stream sequentially using your native operator>>
    for (uint i = 0; i < NUM_PARAMS; i++) {
        // This fires friend std::istream& operator>>(std::istream& is, AAPiVariant& var)
        if (!(file >> m_values[i])) {
            qDebug() << "Configuration file truncated prematurely at parameter: " << i;
            break;
        }

        // Safe guard: Clear out trailing newlines or whitespace
        // left over by numerical or byte operators
        while (file.good() && std::isspace(file.peek())) {
            file.get();
        }
    }

    file.close();
    return AAPI_SUCCESS;
}

AAPiError AAPiConfig::flush()
{
    AAPiMutexLocker lock(&m_mutex);

    const fs::path dirPath( (const char *)get_config_dir() );
    if (!fs::exists(dirPath)) {
        if (!fs::create_directories(dirPath)) {
            qDebug() << "Failed to create directory structure.";
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    std::ofstream file( (const char *)get_config_path(), std::ios::out | std::ios::trunc );
    if (! file.is_open()) {
        qDebug() << "Unable to create configuration file.";
        return AAPI_CONF_E_FILE_OPEN_ERROR;
    }

    // Write simple headers
    file << "magic: 0x" << std::hex << AA_CONFIG_FILE_MAGIC << "\n";
    file << "version: 0x" << std::hex << AA_CONFIG_FILE_VERSION << "\n";
    file << std::dec; // Restore base-10 numerical representations

    // Serialize parameter values sequentially using your native operator<<
    for (uint i = 0; i < NUM_PARAMS; i++) {
        // This fires friend std::ostream& operator<<(std::ostream& os, const AAPiVariant& var)
        file << m_values[i] << "\n";
    }

    if (!file) {
        qDebug() << "File streaming write failure occurred.";
        return AAPI_CONF_E_FILE_WRITE_ERROR;
    }
    file.close();

    return AAPI_SUCCESS;
}

uint32_t AAPiConfig::get_num_valid_params() const
{
    uint32_t num = 0;
    for (uint i = 0; i < get_total_params(); ++i) {
        if( is_valid(i) )
            num++;
    }
    return num;
}

AAPiVariant AAPiConfig::get_value(const AAPiConfig *config, AAPiParameter id)
{
    return config->get_value( id );
}

uint32_t AAPiConfig::get_total_params()
{
    return g_param_table_num;
}

int AAPiConfig::get_index(AAPiParameter id)
{
    for (uint i = 0; i < g_param_table_num; ++i) {
        if( get_id(i) == id )
            return i;
    }
    return -1;
}

AAPiParameter AAPiConfig::get_id(int index)
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
    if( g_param_table[ index ].is_valid ) {

        return g_param_table[ index ].is_valid( this );
    }
    return true;
}

uint32_t AAPiConfig::get_num_opts(enum AAPiParameter id) const
{
    int index = get_index( id );
    if (index < 0)
        return 0;

    return get_num_opts( index );
}

uint32_t AAPiConfig::get_num_opts(int index) const
{
    return static_cast< uint32_t > ( g_param_table[ index ].opt_values.size() );
}

const AAPiVariantArray& AAPiConfig::get_opt_values(AAPiParameter id) const
{
    int index = get_index( id );
    if (index < 0)
        throw std::out_of_range("invalid parameter id");

    return get_opt_values( index );
}

const AAPiVariantArray& AAPiConfig::get_opt_values(int index) const
{
    return g_param_table[ index ].opt_values;
}

const AAPiStringArray& AAPiConfig::get_opt_labels(AAPiParameter id) const
{
    int index = get_index( id );
    if (index < 0)
        throw std::out_of_range("invalid parameter id");

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

AAPiVariant AAPiConfig::get_value(AAPiParameter id) const
{
    return m_values[ (uint)id ];
}

void AAPiConfig::set_value(int index, const AAPiVariant& val)
{
    set_value( get_id( index ), val );
}

void AAPiConfig::set_value(AAPiParameter id, const AAPiVariant& val)
{
    AAPiVariant tmp = val;
    int index = get_index( id );
    if ( index >= 0 )
    {
        // registered parameter, type is known
        switch( get_type( index ) ) {
        case AAPiVariantType::BYTE:
        case AAPiVariantType::UINT:
            tmp = tmp.toUInt();
            break;
        case AAPiVariantType::INT:
            tmp = tmp.toInt();
            break;
        case AAPiVariantType::FLOAT:
            tmp = tmp.toFloat();
            break;
        case AAPiVariantType::TEXT:
            tmp = tmp.toString( );
            break;
        case AAPiVariantType::UNKNOWN:
            break;
        }
    }

    m_values[ (uint)id ] = tmp;
}

double AAPiConfig::get_dsp_fft_bin_width() const
{
    return static_cast<double>(get_dsp_sample_rate()) / (get_dsp_num_samples() / 2);
}

uint32_t AAPiConfig::get_dsp_fft_num_pts() const
{
    // IMPORTANT: When dealing with real-valued input signals, the FFT output
    // exhibits symmetry.
    // In such cases, the second half of the output spectrum is a conjugate
    // of the first half, effectively containing redundant information.
    // You might be discarding this redundant half, making it appear as if
    // the output is half the size.

    // The exact number of unique complex output points produced by an r2c FFT is always (N / 2) + 1
    uint32_t num_fft_pts = (get_dsp_num_samples() / 2) + 1;
    return num_fft_pts;
}

double AAPiConfig::get_dsp_fft_bin_freq(uint32_t bin) const
{   
    return get_dsp_fft_bin_width() * bin;
}

uint32_t AAPiConfig::get_dsp_fft_if_bin() const
{
    uint32_t bin = static_cast<uint32_t>( static_cast<double>(get_intermediate_freq()) / get_dsp_fft_bin_width() + 0.5 );
    return bin;
}

uint32_t AAPiConfig::get_intermediate_freq() const
{
    return 10031;
}

double AAPiConfig::get_dsp_digital_preamp() const
{
    return 1.0; // (10.0x = +20dB boost)
}

uint32_t AAPiConfig::get_dsp_settling_delay_ms() const
{
    // Fetch active runtime parameters
    uint32_t num_samples = get_dsp_num_samples(); // e.g., 1024
    uint32_t sample_rate = get_dsp_sample_rate(); // e.g., 48000

    if (sample_rate == 0)
        return 25; // Safe fallback guard

    // Calculate the exact period duration in floating-point milliseconds,
    // then add a tiny 1-2 ms buffer to account for Linux task-switching overhead.
    double exact_period_ms = (static_cast<double>(num_samples) / sample_rate) * 1000.0;

    // Using std::ceil ensures you always wait long enough to capture a full, clean block
    return static_cast<uint32_t>( std::ceil( exact_period_ms )) + 1;
}

/////////////////////////////////////////////////////////////////////////////

AAPiString get_sysfs_property_path(const char *property_name, uint dev_index)
{
    AAPiString ss;

    ss = "/sys/class/vna/aapi";
    ss += dev_index;
    ss += AAPiString{"/"};
    ss += AAPiString{property_name};

    return ss;
}

} // namespace aapi
