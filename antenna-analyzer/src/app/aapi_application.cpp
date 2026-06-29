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

#include <QQmlComponent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QTimer>
#include <QApplication>
#include <QSettings>
#include <QSocketNotifier>
#include <QQuickItem>
#include <rhi/qrhi.h>
#include <sys/reboot.h>
#include <unistd.h>
#include "aapi_application.h"


namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class QAAPiShutdownManager
///////////////////////////////////////////////////////////////////////////////
/// \brief QAAPiShutdownManager::QAAPiShutdownManager
/// \param parent
///
QAAPiShutdownManager::QAAPiShutdownManager(QObject *parent)
    : QObject(parent)
    , m_shutdownRequested(false)
{
    qInfo() << "ShutdownManager created";
}

QAAPiShutdownManager::~QAAPiShutdownManager()
{
    if (m_device) {
        m_device->close();
    }

    qInfo() << "ShutdownManager destroyed";
}

int QAAPiShutdownManager::openDevice(uint dev_index)
{
    AAPiPtr<AAPiDevice> device( AAPiDevice::create(false) );
    AAPiString path;

    path = "/dev/aapi";
    path += dev_index;

    int ret = device->open( path );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    m_device = device;
    return AAPI_SUCCESS;
}

void QAAPiShutdownManager::setShutdownRequested(bool requested, const QString &reason)
{
    m_shutdownRequested = requested;
    m_shutdownReason = reason;

    if ( requested ) {
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
    if (geteuid() != 0) {
        qCritical() << "ERROR: Not running as root!";
        qCritical() << "Cannot execute system poweroff";
        return false;
    }
    return true;
}

void QAAPiShutdownManager::performSystemShutdown()
{
    if (!m_shutdownRequested) {
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
    if (::reboot(RB_POWER_OFF) < 0) {

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

    if (licenseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
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

    if( m_generator ) {
        // Already loaded, just return OK
        return AAPI_SUCCESS;
    }

    int ret;
    QDir dir( getSnapshotDirectory() );
    if( !dir.exists() ) {
        if( !dir.mkpath( getSnapshotDirectory()) )
            return AAPI_E_CREATE_DIR_FAILED;
    }

    AAPiPtr<AAPiDevice>             device      ( AAPiDevice::create( false ));
    AAPiPtr<AAPiConfig>             config      ( AAPiConfig::create( false ));
    AAPiPtr<AAPiGenerator>          generator   ( AAPiGenerator::create( config, false ));
    AAPiPtr<AAPiCalibrator>         calibrator  ( AAPiCalibrator::create( config, false ));
    AAPiPtr<AAPiSignalProcessor>    processor   ( AAPiSignalProcessor::create( config, false ));
    AAPiPtr<AntScopeDevice>         antscope    ( AntScopeDevice::create( config, generator, false ));

    QPointer<QAAPiConfigurationView>  configurationView     ( new QAAPiConfigurationView( config, this ));
    QPointer<QAAPiSignalProcessView>  signalProcessView     ( new QAAPiSignalProcessView( config, processor, generator, this ) );
    QPointer<QAAPiMeasurementView>    measurementView       ( new QAAPiMeasurementView( config, processor, generator, calibrator, m_messages, this ) );
    QPointer<QAAPiPanoramicScanView>  panoramicScanView     ( new QAAPiPanoramicScanView( config, processor, generator, calibrator, m_style, m_messages, this ) );
    QPointer<QAAPiOSLCalibrationView> oslCalibrationView    ( new QAAPiOSLCalibrationView( config, processor, generator, calibrator, this ) );
    QPointer<QAAPiHWCalibrationView>  hwCalibrationView     ( new QAAPiHWCalibrationView( config, processor, generator, calibrator, this ) );
    QPointer<QAAPiAboutAppView>       aboutAppView          ( new QAAPiAboutAppView( config, this ) );
    QPointer<QAAPiStatusBackend>      appStatus             ( new QAAPiStatusBackend( this ) );


    ret = config->init( );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    ret = calibrator->init( );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    ret = generator->open( );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    ret = processor->start();
    if (AAPI_FAILED( ret )) {
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
    if (AAPI_FAILED( ret )) {
        return ret;
    }


    m_config        = std::move(config);
    m_generator     = std::move(generator);
    m_antscope      = std::move(antscope);
    m_calibrator    = std::move(calibrator);
    m_processor     = std::move(processor);

    m_configurationView     = std::move(configurationView);
    m_signalProcessView     = std::move(signalProcessView);
    m_measurementView       = std::move(measurementView);
    m_panoramicScanView     = std::move(panoramicScanView);
    m_oslCalibrationView    = std::move(oslCalibrationView);
    m_hwCalibrationView     = std::move(hwCalibrationView);
    m_aboutAppView          = std::move(aboutAppView);
    m_appStatus             = std::move(appStatus);

    // Set up QSocketNotifiers to monitor the read ends
    m_deviceNotifier = QPointer<QSocketNotifier>(new QSocketNotifier( m_shutdownMgr->getDevice()->get_event_handle(), QSocketNotifier::Read, this ));
    QObject::connect( m_deviceNotifier.get(), &QSocketNotifier::activated,
                     this, &QAAPiApplication::handleDeviceStatusChange );

    // Connect snapshot signal and slot
    QObject::connect( m_panoramicScanView.get(), &QAAPiPanoramicScanView::snapshotTaken,
                     this, &QAAPiApplication::handleSnapshotTaken );

    // Connect quit, reboot signals and slots
    QObject::connect( m_aboutAppView.get(), &QAAPiAboutAppView::raiseRebootApplication,
                     this, &QAAPiApplication::handleRebootApplication );
    QObject::connect( m_aboutAppView.get(), &QAAPiAboutAppView::raiseQuitApplication,
                     this, &QAAPiApplication::handleQuitApplication );

    QObject::connect( this, &QAAPiApplication::raiseQuitApplication,
                     this, &QAAPiApplication::handleQuitApplication );

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

    if (! m_generator) {
        return;
    }

    // Stop digital signal processing
    m_processor->stop();

    // Close generator
    m_generator->close();

    // Stop Antscope
    m_antscope->stop();

    // Release allocated memory
    m_processor = nullptr;
    m_generator = nullptr;
    m_calibrator = nullptr;
    m_config = nullptr;
    m_antscope = nullptr;

    // Disable device monitoring
    if (m_deviceNotifier) {
        m_deviceNotifier->setEnabled(false);
        m_deviceNotifier = nullptr;
    }

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
    if (m_deviceNotifier) {
        m_deviceNotifier->setEnabled(false);
        qDebug() << "Device monitoring disabled";
    }

    // Notify ShutdownManager that shutdown is requested
    m_shutdownMgr->setShutdownRequested(true, reason);

    emit raiseQuitApplication();
}

void QAAPiApplication::handleSnapshotTaken(QString file_name, QImage snapshot)
{
    // Ensure the directory path exists on disk before attempting to write
    QString snapshotDir = getSnapshotDirectory();
    QDir dir(snapshotDir);
    if (!dir.exists()) {
        dir.mkpath("."); // Creates the directory structure recursively if missing
    }

    QString formatStr = (const char*)m_config->get_snapshot_format();
    QString date_time = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    m_lastSnapshot = QDir::cleanPath(snapshotDir + QDir::separator() +
                                     QString("%1_%2.%3").arg(file_name, date_time, formatStr));

    // Spawns a lightweight, detached background thread worker.
    QThread* workerThread = QThread::create([snapshot, targetPath = m_lastSnapshot]() {
        bool success = snapshot.save(targetPath);
        if (!success) {
            qWarning() << "CRITICAL: Snapshot failed to write to disk path:" << targetPath;
        }
    });

    // Automatically de-allocates the system OS thread handle structures when execution finishes
    connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();  // Fire-and-forget
}

void QAAPiApplication::handleDeviceStatusChange()
{
    qDebug() << "device status has changed.";

    struct AAPiDeviceStatus status;
    int ret = m_shutdownMgr->getDevice()->read_status(&status);
    if (ret < 0) {
        qCritical() << "error reading device status: ret=" << ret;
        return;
    }

    qDebug() << "check if shutdown is requested by the device.";

    if (status.is_shutdown) {
        initiateShutdown("power off");
    }
}

void QAAPiApplication::handleQuitApplication()
{
    QApplication::exit( EXIT_SUCCESS );
}

void QAAPiApplication::handleRebootApplication()
{
    QApplication::exit( EXIT_REBOOT );
}

} // namespace aapi
