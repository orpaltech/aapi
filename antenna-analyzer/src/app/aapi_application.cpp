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
#include <QSocketNotifier>
#include <rhi/qrhi.h>
#include <sys/reboot.h>
#include <unistd.h>
#include "aapi_application.h"


namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// QAAPiShutdownManager
///////////////////////////////////////////////////////////////////////////////

QAAPiShutdownManager::QAAPiShutdownManager(QObject *parent)
    : QObject(parent)
    , m_shutdownRequested(false)
    , m_device(nullptr)
{
    qInfo() << "ShutdownManager created";
}

QAAPiShutdownManager::~QAAPiShutdownManager()
{
    if (m_device)
    {
        m_device->close();
        m_device->release();
    }

    qInfo() << "ShutdownManager destroyed";
}

int QAAPiShutdownManager::openDevice()
{
    AAPiPtr<AAPiDevice> device = AAPiDevice::create(false);
    int ret = device->open( "/dev/aapi0");
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    m_device = device.detach();
    return AAPI_SUCCESS;
}

void QAAPiShutdownManager::setShutdownRequested(bool requested, const QString &reason)
{
    m_shutdownRequested = requested;
    m_shutdownReason = reason;

    if (requested)
    {
        qWarning() << "Shutdown requested:" << reason;
    }
}

AAPiDevice *QAAPiShutdownManager::getDevice() const
{
    return m_device;
}

bool QAAPiShutdownManager::isShutdownRequested() const
{
    return m_shutdownRequested;
}

QString QAAPiShutdownManager::shutdownReason() const
{
    return m_shutdownReason;
}

void QAAPiShutdownManager::syncFilesystems()
{
    qInfo() << "Syncing filesystems...";
    sync();
    sync();
    sync();

    // Give time for sync to complete
    usleep(500000); // 500ms

    qInfo() << "Filesystem sync complete";
}

bool QAAPiShutdownManager::checkRootPrivileges()
{
    if (geteuid() != 0)
    {
        qCritical() << "ERROR: Not running as root!";
        qCritical() << "Cannot execute system poweroff";
        return false;
    }
    return true;
}

void QAAPiShutdownManager::performSystemShutdown()
{
    if (!m_shutdownRequested)
    {
        qInfo() << "Shutdown not requested, skipping system poweroff";
        return;
    }

    qCritical() << "========================================";
    qCritical() << "PERFORMING SYSTEM SHUTDOWN";
    qCritical() << "Reason:" << m_shutdownReason;
    qCritical() << "========================================";

    // Sync filesystems
    syncFilesystems();

    // Check privileges
    if (!checkRootPrivileges())
        return;

    qCritical() << "Calling reboot(RB_POWER_OFF)...";
    qCritical() << "This will trigger kernel's pm_power_off function";

    // Call reboot syscall - triggers pm_power_off in kernel
    if (::reboot(RB_POWER_OFF) < 0)
    {
        qCritical() << "reboot() failed:" << strerror(errno);
        qCritical() << "Error code:" << errno;

        // Fallback to shutdown command
        qWarning() << "Trying fallback: poweroff command";
        ::system("/sbin/poweroff");
    }

    // Should not reach here
    qCritical() << "Still running after poweroff command!";
}

///////////////////////////////////////////////////////////////////////////////
// QAAPiApplication
///////////////////////////////////////////////////////////////////////////////

QAAPiApplication::QAAPiApplication(QAAPiBaseStyle *style, QAAPiMessages *messages,
                                   QAAPiShutdownManager *shutdownMgr,
                                   QObject *parent)
    : QObject( parent )
    , m_style(style)
    , m_messages(messages)
    , m_config(nullptr)
    , m_generator(nullptr)
    , m_signalProcess(nullptr)
    , m_calibrator(nullptr)
    , m_antscope(nullptr)
    , m_warning(AAPI_SUCCESS)
    , m_shutdownMgr(shutdownMgr)
    , m_deviceNotifier(nullptr)
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
    qInfo() << "load AAPi application";

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

    //AAPiPtr<AAPiDevice>               device          = AAPiDevice::create( false );
    AAPiPtr<AAPiConfig>             config          = AAPiConfig::create( false );
    AAPiPtr<AAPiGenerator>          generator       = AAPiGenerator::create( config, false );
    AAPiPtr<AAPiCalibrator>         calibrator      = AAPiCalibrator::create( config, false );
    AAPiPtr<AAPiSignalProcessor>    signalProcess   = AAPiSignalProcessor::create( config, false );
    AAPiPtr<AntScopeDevice>         antscope        = AntScopeDevice::create( config, generator, false );

    QPtr<QAAPiConfigurationView>  configurationView ( new QAAPiConfigurationView( config, this ) );
    QPtr<QAAPiSignalProcessView>  signalProcessView ( new QAAPiSignalProcessView( config, signalProcess, generator, this ) );
    QPtr<QAAPiMeasurementView>    measurementView ( new QAAPiMeasurementView( config, signalProcess, generator, this ) );
    QPtr<QAAPiPanoramicScanView>  panoramicScanView ( new QAAPiPanoramicScanView( config, signalProcess, generator, calibrator, m_style, this ) );
    QPtr<QAAPiOSLCalibrationView> oslCalibrationView ( new QAAPiOSLCalibrationView( config, signalProcess, generator, calibrator, this ) );
    QPtr<QAAPiHWCalibrationView>  hwCalibrationView ( new QAAPiHWCalibrationView( config, signalProcess, generator, calibrator, this ) );
    QPtr<QAAPiAboutAppView>       aboutAppView ( new QAAPiAboutAppView( config, this ) );
    QPtr<QAAPiStatusBackend>      appStatus ( new QAAPiStatusBackend( this ) );


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

    /*ret = device->open( "/dev/aapi0");
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    m_device                = device*/

    m_config        = config;
    m_generator     = generator;
    m_antscope      = antscope;
    m_calibrator    = calibrator;
    m_signalProcess = signalProcess;

    m_configurationView     = std::move(configurationView);
    m_signalProcessView     = std::move(signalProcessView);
    m_measurementView       = std::move(measurementView);
    m_panoramicScanView     = std::move(panoramicScanView);
    m_oslCalibrationView    = std::move(oslCalibrationView);
    m_hwCalibrationView     = std::move(hwCalibrationView);
    m_aboutAppView          = std::move(aboutAppView);
    m_appStatus             = std::move(appStatus);

    // Set up QSocketNotifiers to monitor the read ends
    m_deviceNotifier = QPtr<QSocketNotifier>(new QSocketNotifier( m_shutdownMgr->getDevice()->get_event_handle(), QSocketNotifier::Read, this ));
    QObject::connect( m_deviceNotifier.get(), &QSocketNotifier::activated, this, &QAAPiApplication::device_status_change );

    // Connect snapshot signal and slot
    QObject::connect( m_panoramicScanView.get(), &QAAPiPanoramicScanView::snapshotTaken, this, &QAAPiApplication::process_snapshot );

    // Connect quit, reboot signals and slots
    QObject::connect( m_aboutAppView.get(), &QAAPiAboutAppView::rebootApplication, this, &QAAPiApplication::reboot_application );
    QObject::connect( m_aboutAppView.get(), &QAAPiAboutAppView::quitApplication, this, &QAAPiApplication::quit_application );

    QObject::connect( this, &QAAPiApplication::quitApplication, this, &QAAPiApplication::quit_application );

    /*===========================================================*/
/*REMOVE THIS */
   /*QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(quit_application()));
   timer->setSingleShot(true);
   timer->start(2*30000);*/

    return AAPI_SUCCESS;
}

void QAAPiApplication::unload()
{
    qInfo() << "unload AAPi application";

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
    m_signalProcess = nullptr;
    m_generator = nullptr;
    m_antscope = nullptr;
    m_calibrator = nullptr;
    m_config = nullptr;

    // Disable device monitoring
    if (m_deviceNotifier)
    {
        m_deviceNotifier->setEnabled(false);
        m_deviceNotifier = nullptr;
    }

    //m_device = nullptr;

    // Release views
    m_configurationView = nullptr;
    m_signalProcessView = nullptr;
    m_measurementView = nullptr;
    m_panoramicScanView = nullptr;
    m_oslCalibrationView = nullptr;
    m_hwCalibrationView = nullptr;
    m_aboutAppView = nullptr;
    m_appStatus = nullptr;
}

QString QAAPiApplication::getSnapshotDirectory()
{
    return QDir::cleanPath( AAPiConfig::get_app_dir() + QDir::separator() + "snapshot" );
}

void QAAPiApplication::initiateShutdown(const QString &reason)
{
    qCritical() << "========================================";
    qCritical() << "INITIATING GRACEFUL SHUTDOWN";
    qCritical() << "Reason:" << reason;
    qCritical() << "========================================";

    // Disable device monitoring
    if (m_deviceNotifier)
    {
        m_deviceNotifier->setEnabled(false);
        qDebug() << "Device monitoring disabled";
    }

    // Notify ShutdownManager that shutdown is requested
    m_shutdownMgr->setShutdownRequested(true, reason);

    emit quitApplication();
}

void QAAPiApplication::process_snapshot(QString file_name, QImage snapshot)
{
    AAPiString format = m_config->get_snapshot_format();
    QString date_time = QDateTime::currentDateTimeUtc().toString("yyyyMMdd_HHmmss");
    m_lastSnapshot = QDir::cleanPath( getSnapshotDirectory() + QDir::separator() +
                                    QString("%1_%2.%3").arg( file_name, date_time, (const char *)format ));
    snapshot.save(m_lastSnapshot);
}

void QAAPiApplication::device_status_change()
{
    qDebug() << "device status has changed.";

    struct AAPiDeviceStatus status;
    int ret = m_shutdownMgr->getDevice()->read_status(&status);
    if (ret < 0)
    {
        qCritical() << "error reading device status: ret=" << ret;
        return;
    }

    qDebug() << "check if shutdown is requested by the device.";

    if (status.is_shutdown)
    {
        initiateShutdown("power off");
    }
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
