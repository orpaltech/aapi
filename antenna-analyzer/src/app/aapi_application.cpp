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

#include <QQmlComponent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include <rhi/qrhi.h>
#include "aapi_application.h"


namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// QAAPiApplication
///////////////////////////////////////////////////////////////////////////////

QAAPiApplication::QAAPiApplication(QAAPiBaseStyle *style, QAAPiMessages *messages, QObject *parent)
    : QObject( parent )
    , m_style(style)
    , m_msgs(messages)
    , m_config(nullptr)
    , m_generator(nullptr)
    , m_signalProcess(nullptr)
    , m_calibrator(nullptr)
    , m_antscope(nullptr)
    , m_warning(AAPI_SUCCESS)
{
}

QAAPiApplication::~QAAPiApplication()
{
    unload();
}

bool QAAPiApplication::isAudioDeviceOkay() const
{
    return m_warning == AAPI_SUCCESS
           || m_warning != AAPI_DSP_E_ADC_DEVICE_NOT_FOUND;
}

bool QAAPiApplication::isAudioFormatOkay() const
{
    return m_warning == AAPI_SUCCESS
           || m_warning != AAPI_DSP_E_ADC_UNSUPPORT_FORMAT;
}

QString QAAPiApplication::getLicenseText() const
{
    QFile licenseFile(":/files/license");
    QString licenseText;

    if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&licenseFile);
        licenseText = in.readAll();
    }

    return licenseText;
}

QString QAAPiApplication::getGraphicsDeviceInfo() const
{
    return m_mainWindow->rhi()->driverInfo().deviceName;
}

int QAAPiApplication::load()
{
    m_warning = AAPI_SUCCESS;

    if( m_generator )
    {
        // Already loaded, just return OK
        return AAPI_SUCCESS;
    }

    int ret;
    QDir dir( getSnapshotDirectory() );
    if( !dir.exists() )
    {
        if( !dir.mkpath( getSnapshotDirectory()) )
            return AAPI_E_CREATE_DIR_FAILED;
    }

    AAPiPtr<AAPiConfig>           config( AAPiConfig::create( false ) );
    AAPiPtr<AAPiGenerator>        generator( AAPiGenerator::create( config, false ) );
    AAPiPtr<AAPiSignalProcessor>  signalProcess( AAPiSignalProcessor::create( config, false ) );
    AAPiPtr<AAPiCalibrator>       calibrator( AAPiCalibrator::create( config, false ) );
    AAPiPtr<AntScopeDevice>       antscope( AntScopeDevice::create( config, generator, false ) );

    QPointer<QAAPiConfigurationView>    configurationView ( new QAAPiConfigurationView( config, this ) );
    QPointer<QAAPiSignalProcessView>    signalProcessView ( new QAAPiSignalProcessView( config, signalProcess, generator, this ) );
    QPointer<QAAPiMeasurementView>      measurementView ( new QAAPiMeasurementView( config, signalProcess, generator, this ) );
    QPointer<QAAPiPanoramicScanView>    panoramicScanView ( new QAAPiPanoramicScanView( config, signalProcess, generator, calibrator, m_style, this ) );
    QPointer<QAAPiOSLCalibrationView>   oslCalibrationView ( new QAAPiOSLCalibrationView( config, signalProcess, generator, calibrator, this ) );
    QPointer<QAAPiHWCalibrationView>    hwCalibrationView ( new QAAPiHWCalibrationView( config, signalProcess, generator, calibrator, this ) );
    QPointer<QAAPiAboutAppView>         aboutView ( new QAAPiAboutAppView( config, this ) );
    QPointer<QAAPiStatusBackend>        appStatus ( new QAAPiStatusBackend( this ) );

    ret = calibrator->init( );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    ret = generator->open( );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    generator->set_intermediate_freq( AAPI_INT_FREQ_HZ );

    ret = signalProcess->start();
    if (AAPI_FAILED( ret ))
    {
        if (ret == AAPI_DSP_E_ADC_UNSUPPORT_FORMAT ||
            ret == AAPI_DSP_E_ADC_DEVICE_NOT_FOUND)
            m_warning = ret;
        else
            return ret;
    }
/*
    ret = antscope->start();
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }
*/
    ret = appStatus->init( );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    m_config        = config.detach();
    m_generator     = generator.detach();
    m_antscope      = antscope.detach();
    m_signalProcess = signalProcess.detach();
    m_calibrator    = calibrator.detach();

    m_configurationView     = configurationView;
    m_signalProcessView     = signalProcessView;
    m_measurementView       = measurementView;
    m_panoramicScanView     = panoramicScanView;
    m_oslCalibrationView    = oslCalibrationView;
    m_hwCalibrationView     = hwCalibrationView;
    m_aboutView             = aboutView;
    m_appStatus             = appStatus;

    // Connect snapshot signal and slot
    QObject::connect( m_panoramicScanView, SIGNAL( snapshotTaken(QString, QImage) ), this, SLOT( save_snapshot(QString, QImage) ));

    // Connect quit, reboot signals and slots
    QObject::connect( m_aboutView, SIGNAL( rebootApplication() ), this, SLOT( reboot_application() ) );
    QObject::connect( m_aboutView, SIGNAL( quitApplication() ), this, SLOT( quit_application() ));

    /*===========================================================*/
/*REMOVE THIS */
   /*QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(quit_application()));
   timer->setSingleShot(true);
   timer->start(2*30000);*/

    return 0;
}

void QAAPiApplication::unload()
{
    if (! m_generator)
    {
        return;
    }

    // Stop digital signal processing
    m_signalProcess->stop();

    // Close generator
    m_generator->close();

    // Stop Antscope
    m_antscope->stop();

    // Release allocated memory
    AAPI_DISPOSE(m_signalProcess);
    AAPI_DISPOSE(m_generator);
    AAPI_DISPOSE(m_antscope);
    AAPI_DISPOSE(m_calibrator);
    AAPI_DISPOSE(m_config);

    // Release views
    m_configurationView     = nullptr;
    m_signalProcessView     = nullptr;
    m_measurementView       = nullptr;
    m_panoramicScanView     = nullptr;
    m_oslCalibrationView    = nullptr;
    m_hwCalibrationView     = nullptr;
    m_aboutView             = nullptr;
    m_appStatus             = nullptr;
}

QString QAAPiApplication::getSnapshotDirectory()
{
    return QDir::cleanPath( AAPiConfig::get_app_dir() + QDir::separator() + "snapshot" );
}

void QAAPiApplication::save_snapshot(QString file_name, QImage snapshot)
{
    AAPiString format = m_config->get_snapshot_format();
    QString date_time = QDateTime::currentDateTimeUtc().toString("yyyyMMdd_HHmmss");
    m_lastSnapshot = QDir::cleanPath( getSnapshotDirectory() + QDir::separator() +
                                    QString("%1_%2.%3").arg( file_name, date_time, (const char *)format ));
    snapshot.save(m_lastSnapshot);
}

void QAAPiApplication::quit_application()
{
    QApplication::exit( EXIT_SUCCESS );
}

void QAAPiApplication::reboot_application()
{
    QApplication::exit( EXIT_REBOOT );
}

} // namespace aapi
