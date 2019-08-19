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

#include <unistd.h>
#include <memory.h>
#include <QMutexLocker>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>
#include "analyzer/analyzer_config.h"
#include "serial/uart_device.h"
#include "audio/audio_reader.h"

///////////////////////////////////////////////////////////////////////////////
// Config definitions
///////////////////////////////////////////////////////////////////////////////
#define AA_CONFIG_FILE_MAGIC        0xA0B0C0D0
#define AA_CONFIG_FILE_VERSION      2

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Function definitions
///////////////////////////////////////////////////////////////////////////////

static bool show_advanced(const AAPIConfig *config)
{
    return config->get_show_advanced() > 0;
}

typedef bool (*pfn_is_valid)(const AAPIConfig *config);

///////////////////////////////////////////////////////////////////////////////
// class AAPIParam
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIParam class
///
class AAPIParam
{
public:
    AAPIParam(enum AAPIParamId id, const char *name, enum AAPIParamType type);
    AAPIParam(const AAPIParam& param);

    AAPIParam& operator=(const AAPIParam& param);

    AAPIParam& set_description(const char *description);
    AAPIParam& add_option(QVariant value, const char *label = nullptr);
    AAPIParam& clear_options();
    AAPIParam& set_repeat_delay(uint32_t repeat_delay);
    AAPIParam& set_reboot_required(bool reboot_required);
    AAPIParam& set_is_valid(pfn_is_valid is_valid);
    AAPIParam& set_precision(uint32_t precision);

    AAPIParam& add_audio_devices();

public:
    /**
     * ID of the configuration parameter, see AnalyzerParam enum
     */
    enum AAPIParamId id;

    /**
     * Short parameter name to be displayed
     */
    QString name;

    /**
     * Parameter value type, see AnalyzerParamType enum.
     */
    enum AAPIParamType type;

    /**
     * Number of fractional digits for floating point numbers
     */
    uint32_t precision;

    /**
     * Detailed description of the parameter
     */
    QString description;

    /**
     * Array of values that can be selected for parameter.
     */
    QVector<QVariant> opt_values;

    /**
     * Array of alternative string representations for values that can be selected for parameter.
     * Length of the array is specified in n_values.
     */
    QVector<QString> opt_labels;

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
    AAPIParam(AA_PARAM_OSL_SELECTED, "osl_selected", AA_PT_INT)
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

    AAPIParam(AA_PARAM_BASE_R0, "base_r0", AA_PT_UINT)
                    .add_option(28U)
                    .add_option(50U)
                    .add_option(75U)
                    .add_option(100U)
                    .add_option(150U)
                    .add_option(300U)
                    .set_description("Base impedance (R0) for Smith Chart and VSWR, Ohm"),

    AAPIParam(AA_PARAM_SYNTH_XTAL_FREQ, "synth_xtal_freq", AA_PT_UINT)
                    .add_option((uint32_t) AA_XTALF_25MHZ, "25")
                    .add_option((uint32_t) AA_XTALF_27MHZ, "27")
                    .set_description("Synthesizer IC crystal frequency, MHz"),

    AAPIParam(AA_PARAM_SYNTH_XTAL_CORR, "synth_xtal_corr", AA_PT_INT)
                    .set_description("Synthesizer IC crystal frequency correction, Hz")
                    .set_repeat_delay(20),

    AAPIParam(AA_PARAM_OSL_R_OPEN, "osl_r_open", AA_PT_UINT)
                    .add_option(300U, "300 Ohm")
                    .add_option(333U, "333 Ohm")
                    .add_option(500U, "500 Ohm")
                    .add_option(750U, "750 Ohm")
                    .add_option(1000U, "1000 Ohm")
                    .add_option(999999U, "INFINITE")
                    .set_description("Open impedance for OSL calibration, Ohm"),

    AAPIParam(AA_PARAM_OSL_R_SHORT, "osl_r_short", AA_PT_UINT)
                    .add_option(0U, "Short circuit")
                    .add_option(5U, "5 Ohm")
                    .add_option(10U, "10 Ohm")
                    .set_description("Short impedance for OSL calibration, Ohm"),

    AAPIParam(AA_PARAM_OSL_R_LOAD, "osl_r_load", AA_PT_UINT)
                    .add_option(50U, "50 Ohm")
                    .add_option(75U, "75 Ohm")
                    .add_option(100U, "100 Ohm")
                    .add_option(150U, "150 Ohm")
                    .set_description("Load impedance for OSL calibration, Ohm"),

    AAPIParam(AA_PARAM_OSL_N_SCANS, "osl_num_scans", AA_PT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average during OSL calibration at each frequency"),

    AAPIParam(AA_PARAM_MEASURE_N_SCANS,	"measure_num_scans", AA_PT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in measurement window at each frequency"),

    AAPIParam(AA_PARAM_PAN_N_SCANS,	"pan_num_scans", AA_PT_UINT)
                    .add_option(1U)
                    .add_option(3U)
                    .add_option(5U)
                    .add_option(7U)
                    .add_option(9U)
                    .add_option(11U)
                    .add_option(15U)
                    .set_description("Number of scans to average in panoramic window at each frequency"),

    AAPIParam(AA_PARAM_AUDIO_INPUT_GAIN, "audio_input_gain", AA_PT_BYTE)
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

    AAPIParam(AA_PARAM_BRIDGE_R_MEASURE, "bridge_r_measure", AA_PT_FLOAT)
                    .add_option(1.f)
                    .add_option(5.1f)
                    .add_option(10.f)
                    .set_precision(1)
                    .set_is_valid(show_advanced)
                    .set_description("Bridge R-measure value, Ohm"),

    AAPIParam(AA_PARAM_BRIDGE_R_MEASURE_ADD, "bridge_r_measure_add", AA_PT_FLOAT)
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

    AAPIParam(AA_PARAM_PAN_IS_CENTER_FREQ, "pan_is_center_freq", AA_PT_UINT)
                    .add_option(0U, "Start Frequency")
                    .add_option(1U, "Center Frequency")
                    .set_description("Set either start or center frequency in panoramic view."),

    AAPIParam(AA_PARAM_AUDIO_INPUT_DEVICE, "audio_input_device", AA_PT_TEXT)
                    .add_audio_devices()
                    .set_is_valid(show_advanced)
                    .set_description("Audio input device for DSP. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AA_PARAM_UART_DEVICE, "uart_device", AA_PT_TEXT)
                    .add_option("/dev/ttyAMA0", "Hardware (/dev/ttyAMA0)")
                    .add_option("/dev/ttyS0", "Emulated (/dev/ttyS0)")
                    .set_is_valid(show_advanced)
                    .set_description("Select UART for remote control. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AA_PARAM_UART_BAUDRATE, "uart_baudrate", AA_PT_UINT)
                    .add_option((uint) UART_BR_9600)
                    .add_option((uint) UART_BR_19200)
                    .add_option((uint) UART_BR_38400)
                    .add_option((uint) UART_BR_57600)
                    .add_option((uint) UART_BR_115200)
                    .set_is_valid(show_advanced)
                    .set_description("Select UART baudrate. Requires reboot.")
                    .set_reboot_required(true),

    AAPIParam(AA_PARAM_PWRSAVE_TIMEOUT, "pwr_save_timeout", AA_PT_UINT)
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
    AAPIParam(AA_PARAM_S11_GRAPH_SHOW, "s11_graph_show", AA_PT_UINT)
                    .add_option(0U, "No")
                    .add_option(1U, "Yes")
                    .set_description("Display S11 graph in panoramic view."),

    AAPIParam(AA_PARAM_S1P_FILE_TYPE, "s1p_file_type", AA_PT_UINT)
                    .add_option((uint32_t) AA_S1P_TYPE_S_MA, "S MA R50")
                    .add_option((uint32_t) AA_S1P_TYPE_S_RI, "S RI R50")
                    .set_description("Touchstone S1P file type saved with screenshot. Default is S MA R 50."),

    AAPIParam(AA_PARAM_SNAPSHOT_FORMAT, "snapshot_format", AA_PT_TEXT)
                    .add_option("bmp", "Bitmap")
                    .add_option("png", "PNG")
                    .add_option("jpg", "JPEG")
                    .set_is_valid(show_advanced)
                    .set_description("Image file format for screenshots."),

    AAPIParam(AA_PARAM_SHOW_ADVANCED, "show_advanced", AA_PT_UINT)
                    .add_option(0U, "Off")
                    .add_option(1U, "On")
                    .set_description("Display advanced menu parameters."),

};

const static size_t g_param_table_num = sizeof(g_param_table) / sizeof(AAPIParam);

static AAPIRadioBand g_ham_bands [] = {
    /* HF ITU region 1 bands */
    AAPIRadioBand(1800ul, 2000ul, "160m"),
    AAPIRadioBand(3500ul, 3800ul, "80m"),
    AAPIRadioBand(7000ul, 7300ul, "40m"),
    AAPIRadioBand(10100ul, 10150ul, "30m"),
    AAPIRadioBand(14000ul, 14350ul, "20m"),
    AAPIRadioBand(18068ul, 18168ul, "17m"),
    AAPIRadioBand(21000ul, 21450ul, "15m"),
    AAPIRadioBand(24890ul, 24990ul, "12m"),
    AAPIRadioBand(28000ul, 29700ul, "10m"),
    /* VHF bands */
    AAPIRadioBand(50000ul, 54000ul, "6m"),
    AAPIRadioBand(144000ul, 148000ul, "2m"),
    AAPIRadioBand(222000ul, 225000ul, "1.25m"), // (USA, Canada)
    /* UHF bands */
    AAPIRadioBand(430000ul, 440000ul, "70cm")
};

const static size_t g_ham_bands_num = sizeof(g_ham_bands) / sizeof(g_ham_bands[0]);

///////////////////////////////////////////////////////////////////////////////
// class AAPIParam
///////////////////////////////////////////////////////////////////////////////

AAPIParam::AAPIParam(enum AAPIParamId id, const char *name, enum AAPIParamType type)
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

AAPIParam &AAPIParam::set_description(const char* desc)
{
    this->description = desc;
    return *this;
}

AAPIParam &AAPIParam::add_option(QVariant value, const char* label)
{
    opt_values.append(value);
    if (label)
        opt_labels.append(label);
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
    AAPtr<AAPIAudioReader>  reader( AAPIAudioReader::create( false ) );

    for( uint i = 0; i < reader->get_num_devices(); i++ )
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

///////////////////////////////////////////////////////////////////////////////
// struct AAPIRadioBand
///////////////////////////////////////////////////////////////////////////////

AAPIRadioBand::AAPIRadioBand(uint32_t lo, uint32_t hi, const char *name)
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
// class AAPIConfig
///////////////////////////////////////////////////////////////////////////////

AAPIConfig::AAPIConfig()
    : m_mutex(QMutex::Recursive)
{
    init();
}

AAPIConfig::~AAPIConfig()
{
}

AAPIConfig& AAPIConfig::operator=(const AAPIConfig& config)
{
    if( this != &config )
    {
        copy( config );
    }
    return *this;
}

QString AAPIConfig::get_app_dir()
{
    return QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
                           + QDir::separator() + QString(AA_APP_NAME));
}

QString AAPIConfig::get_config_dir()
{
    return QDir::cleanPath(get_app_dir() + QDir::separator() + QString("config"));
}

QString AAPIConfig::get_config_path()
{
    return QDir::cleanPath(get_config_dir() + QDir::separator() + QString("config.bin"));
}

AAPIRadioBand *AAPIConfig::get_ham_bands()
{
    return g_ham_bands;
}

uint32_t AAPIConfig::get_num_ham_bands()
{
    return g_ham_bands_num;
}

void AAPIConfig::copy (const AAPIConfig& config)
{
    // deep copy
    for( int i = 0; i < NUM_AA_PARAMS; i ++ )
    {
        m_values[i] = config.m_values[i];
    }
}

void AAPIConfig::set_defaults()
{
    int i;

    for( i = 0; i < NUM_AA_PARAMS; i ++ )
    {
        m_values[i] = QVariant();
    }

    // set defaults for all parameters
    set_version(AA_VERSION);
    set_dsp_sample_rate(AUDIO_SRATE_48K);
    set_dsp_sample_size(AUDIO_SSIZE_16);
    set_dsp_nsamples(512);
    set_dsp_fftbin(107);
    set_synth_xtal_freq(AA_XTALF_27MHZ);
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
    set_generator_freq(14000000);
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
    set_s1p_file_type(AA_S1P_TYPE_S_MA);
    set_show_advanced(0);

    i = get_index(AA_PARAM_AUDIO_INPUT_DEVICE);
    if( i >= 0 && get_num_opt(i) > 0 )
    {
        const QVariant *devices = get_opt_values(i);
        set_audio_input_device( devices[0].toString().toLatin1() );
    }
    else
    {
        set_audio_input_device("");
    }
}

int AAPIConfig::init()
{
    /* set default values as fallback */
    set_defaults();

    QMutexLocker locker(&m_mutex);

    QDir dir(get_config_dir());
    if(! dir.exists() )
    {
        if(! dir.mkpath( get_config_dir() ) )
        {
            return AAPI_E_CREATE_DIR_FAILED;
        }
    }

    QFile file( get_config_path() );
    if( file.open( QIODevice::ReadOnly ) )
    {
        /*Read serialized data from the file*/
        QDataStream in( &file );
        /* Read and check the header */
        quint32 magic, version;
        in >> magic;
        in >> version;

        if( AA_CONFIG_FILE_MAGIC    == magic	&&
            AA_CONFIG_FILE_VERSION  == version )
        {
            // set serialization format
            in.setVersion( QDataStream::Qt_5_9 );

            // read parameter values
            for( int i = 0; i < NUM_AA_PARAMS; i++ )
            {
                in >> m_values[i];
            }
        }
        else
        {
            /* bad file format, ignore */
        }

        /* close explicitly */
        file.close();
    }
    else
    {
        /* unable to open config file */
    }

    /* correct some values */
    uint32_t timeout = get_pwr_save_timeout();
    if( timeout > 0 && timeout < 10000 )
    {
        set_pwr_save_timeout(0);
    }

    return 0;
}

int AAPIConfig::flush()
{
    QMutexLocker lock( &m_mutex );

    QDir dir( get_config_dir() );
    if( !dir.exists() )
    {
        if( !dir.mkpath( get_config_dir() ) )
            return AAPI_E_CREATE_DIR_FAILED;
    }

    QFile file( get_config_path() );
    if( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
    {
        return AAPI_CONFIG_E_OPEN_FILE_FAILED;
    }

    QDataStream out( &file ); // we will serialize the data into the file

    // Write a header with a "magic number" and a version
    out << static_cast< quint32 >( AA_CONFIG_FILE_MAGIC );
    out << static_cast< quint32 >( AA_CONFIG_FILE_VERSION );

    // set serialization format
    out.setVersion( QDataStream::Qt_5_9 );

    /* flush param values */
    for( int i = 0; i < NUM_AA_PARAMS; i++ )
    {
        out << m_values[i];
    }

    /* Explicit close */
    file.close();

    return 0;
}

uint32_t AAPIConfig::get_num_params() const
{
    uint32_t num = 0, i;
    for( i = 0; i < get_total_params(); i++ )
    {
        if( is_valid(i) )
            num++;
    }
    return num;
}

QVariant AAPIConfig::get_value(const AAPIConfig *config, enum AAPIParamId param)
{
    return config->get_value( param );
}

uint32_t AAPIConfig::get_total_params()
{
    return g_param_table_num;
}

int AAPIConfig::get_index(enum AAPIParamId id)
{
    for( uint i = 0; i < g_param_table_num; i++ )
    {
        if( get_id(i) == id )
            return i;
    }
    return -1;
}

enum AAPIParamId AAPIConfig::get_id(int index)
{
    return g_param_table[ index ].id;
}

enum AAPIParamType AAPIConfig::get_type(int index)
{
    return g_param_table[ index ].type;
}

QString AAPIConfig::get_name(int index) const
{
    return g_param_table[ index ].name;
}

QString AAPIConfig::get_description(int index) const
{
    return g_param_table[ index ].description;
}

uint32_t AAPIConfig::get_precision(int index) const
{
    return g_param_table[ index ].precision;
}

bool AAPIConfig::is_valid(int index) const
{
    if( g_param_table[ index ].is_valid )
    {
        return g_param_table[ index ].is_valid( this );
    }
    return true;
}

uint32_t AAPIConfig::get_num_opt(enum AAPIParamId id) const
{
    int index = get_index( id );
    if( index < 0 )
        return 0;

    return get_num_opt( index );
}

uint32_t AAPIConfig::get_num_opt(int index) const
{
    return static_cast< uint32_t > ( g_param_table[ index ].opt_values.size() );
}

const QVariant *AAPIConfig::get_opt_values(enum AAPIParamId id) const
{
    int index = get_index( id );
    if( index < 0 )
        return nullptr;

    return get_opt_values( index );
}

const QVariant *AAPIConfig::get_opt_values(int index) const
{
    if( get_num_opt( index ) == 0 )
        return nullptr;

    return g_param_table[ index ].opt_values.data();
}

const QString *AAPIConfig::get_opt_labels(enum AAPIParamId id) const
{
    int index = get_index( id );
    if ( index < 0 )
        return nullptr;

    return get_opt_labels( index );
}

const QString *AAPIConfig::get_opt_labels(int index) const
{
    if( g_param_table[ index ].opt_labels.size() == 0 )
        return nullptr;

    return g_param_table[ index ].opt_labels.data();
}

QVariant AAPIConfig::get_value(int index) const
{
    return get_value( g_param_table[ index ].id );
}

QVariant AAPIConfig::get_value(enum AAPIParamId id) const
{
    if( id < 0 || id >= NUM_AA_PARAMS )
    {
        return QVariant( QVariant::Invalid );
    }

    return m_values[ id ];
}

void AAPIConfig::set_value(int index, QVariant val)
{
    set_value( get_id( index ), val );
}

void AAPIConfig::set_value(enum AAPIParamId id, QVariant val)
{
    if( id < 0 || id >= NUM_AA_PARAMS )
    {
        return;
    }

    int index = get_index( id );
    if( index >= 0 )
    {
        bool res = true;
        QVariant tmp = val;
        // registered parameter, type is known
        switch( get_type( index ) )
        {
        case AA_PT_BYTE:
        case AA_PT_UINT:
            tmp = tmp.toUInt( &res );
            break;
        case AA_PT_INT:
            tmp = tmp.toInt( &res );
            break;
        case AA_PT_FLOAT:
            tmp = tmp.toFloat( &res );
            break;
        case AA_PT_TEXT:
            tmp = tmp.toString();
            break;
        }
        if( res ) {
            val = tmp;
        }
    }

    m_values[ id ] = val;
}

uint32_t AAPIConfig::get_bin_freq(uint32_t bin) const
{
    double bin_width = static_cast<double>( get_dsp_sample_rate() ) / get_dsp_nsamples();
    return static_cast<uint32_t>( bin_width * bin );
}

uint32_t AAPIConfig::get_ifreq() const
{
    return get_bin_freq( get_dsp_fftbin() );
}

} // namespace aapi
