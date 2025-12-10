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

#ifndef AAPI_APPLICATION_H
#define AAPI_APPLICATION_H

#include <QObject>
#include <QPointer>
#include <QQuickWindow>
#include "ui/aapi_measure_view.h"
#include "ui/aapi_configuration_view.h"
#include "ui/aapi_osl_calibration_view.h"
#include "ui/aapi_hw_calibration_view.h"
#include "ui/aapi_about_app_view.h"
#include "ui/aapi_signal_process_view.h"
#include "ui/aapi_panoramic_scan_view.h"
#include "ui/aapi_status_backend.h"
#include "ui/aapi_base_style.h"
#include "ui/aapi_messages.h"
#include "analyzer/antscope/antscope_device.h"
#include "analyzer/aapi_signal_process.h"
#include "analyzer/aapi_device.h"

template <class T>
using QPtr = std::unique_ptr<T>;

///////////////////////////////////////////////////////////////////////////////
// Define reboot exit code
///////////////////////////////////////////////////////////////////////////////

#define EXIT_REBOOT -123456789

class QSocketNotifier;

namespace aapi
{

class QAAPiShutdownManager : public QObject
{
    Q_OBJECT

public:
    QAAPiShutdownManager(QObject *parent = nullptr);
    ~QAAPiShutdownManager();

    int openDevice();
    void setShutdownRequested(bool requested, const QString &reason = QString());
    void performSystemShutdown();

    AAPiDevice *getDevice() const;
    bool isShutdownRequested() const;
    QString shutdownReason() const;

private:
    void syncFilesystems();
    bool checkRootPrivileges();

private:
    bool m_shutdownRequested;
    QString m_shutdownReason;
    AAPiDevice *m_device;
};

///////////////////////////////////////////////////////////////////////////////
// QAAPiApplication
///////////////////////////////////////////////////////////////////////////////

class QAAPiApplication : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(AapiApplication)

    // ----------- Properties ------------
    Q_PROPERTY(QAAPiConfigurationView*  view_configuration      READ    getConfigurationViewBackend CONSTANT)
    Q_PROPERTY(QAAPiSignalProcessView*  view_signal_process     READ    getSignalProcessViewBackend CONSTANT)
    Q_PROPERTY(QAAPiMeasurementView*    view_measurement        READ    getMeasurementViewBackend CONSTANT)
    Q_PROPERTY(QAAPiPanoramicScanView*  view_panoramic_scan     READ    getPanoramicScanViewBackend CONSTANT)
    Q_PROPERTY(QAAPiOSLCalibrationView* view_osl_calibration    READ    getOslCalibrationViewBackend CONSTANT)
    Q_PROPERTY(QAAPiHWCalibrationView*  view_hw_calibration     READ    getHwCalibrationViewBackend CONSTANT)
    Q_PROPERTY(QAAPiAboutAppView*       view_about              READ    getAboutAppViewBackend CONSTANT)
    Q_PROPERTY(QAAPiStatusBackend*      status                  READ    getStatusBackend CONSTANT)
    Q_PROPERTY(QString                  last_snapshot           READ    geLastSnapshot CONSTANT);
    Q_PROPERTY(QAAPiBaseStyle*          style                   READ    getStyle CONSTANT)
    Q_PROPERTY(QAAPiMessages*           messages                READ    getMessages CONSTANT)
    Q_PROPERTY(bool                     audio_device_okay       READ    isAudioDeviceOkay CONSTANT)
    Q_PROPERTY(bool                     audio_format_okay       READ    isAudioFormatOkay CONSTANT)
    Q_PROPERTY(int                      warning                 READ    getWarning CONSTANT)
    Q_PROPERTY(QString                  license_text            READ    getLicenseText CONSTANT)

public:
    explicit QAAPiApplication(QAAPiBaseStyle *style, QAAPiMessages *messages,
                              QAAPiShutdownManager *shutdownManager,
                              QObject *parent = Q_NULLPTR);
    ~QAAPiApplication();

    int load();
    void unload();


    // --------- Property accessors -------
    QAAPiConfigurationView  *getConfigurationViewBackend() const { return m_configurationView.get(); }
    QAAPiSignalProcessView  *getSignalProcessViewBackend() const { return m_signalProcessView.get(); }
    QAAPiMeasurementView    *getMeasurementViewBackend() const { return m_measurementView.get(); }
    QAAPiPanoramicScanView  *getPanoramicScanViewBackend() const { return m_panoramicScanView.get(); }
    QAAPiOSLCalibrationView *getOslCalibrationViewBackend() const { return m_oslCalibrationView.get(); }
    QAAPiHWCalibrationView  *getHwCalibrationViewBackend() const { return m_hwCalibrationView.get(); }
    QAAPiAboutAppView       *getAboutAppViewBackend() const { return m_aboutAppView.get(); }
    QAAPiStatusBackend      *getStatusBackend() const { return m_appStatus.get(); }
    QAAPiBaseStyle          *getStyle() const { return m_style; }
    QAAPiMessages           *getMessages() const { return m_messages; }
    QString                 geLastSnapshot() const { return m_lastSnapshot; }
    bool                    isAudioDeviceOkay() const;
    bool                    isAudioFormatOkay() const;
    int                     getWarning() const { return m_warning; }
    QString                 getLicenseText() const;
    QString                 getGraphicsDeviceInfo() const;
    void                    setMainWindow(QQuickWindow *mainWnd) { m_mainWindow = mainWnd; }

    // --------- Static methods ----------
    static QString  getSnapshotDirectory();

private:
    AAPiPtr<AAPiConfig>             m_config;
    AAPiPtr<AAPiGenerator>          m_generator;
    AAPiPtr<AntScopeDevice>         m_antscope;
    AAPiPtr<AAPiSignalProcessor>    m_signalProcess;
    AAPiPtr<AAPiCalibrator>         m_calibrator;
    //AAPiPtr<AAPiDevice>             m_device;
    QString                         m_lastSnapshot;

    QAAPiBaseStyle                  *m_style;
    QAAPiMessages                   *m_messages;

    int                             m_warning;

    QQuickWindow                    *m_mainWindow;

    QAAPiShutdownManager            *m_shutdownMgr;
    QPtr<QSocketNotifier>           m_deviceNotifier;

    // ------------- Models --------------
    QPtr<QAAPiConfigurationView>    m_configurationView;
    QPtr<QAAPiSignalProcessView>    m_signalProcessView;
    QPtr<QAAPiMeasurementView>      m_measurementView;
    QPtr<QAAPiPanoramicScanView>    m_panoramicScanView;
    QPtr<QAAPiOSLCalibrationView>   m_oslCalibrationView;
    QPtr<QAAPiHWCalibrationView>    m_hwCalibrationView;
    QPtr<QAAPiAboutAppView>         m_aboutAppView;
    QPtr<QAAPiStatusBackend>        m_appStatus;

private:
    void initiateShutdown(const QString &reason);

Q_SIGNALS:
    void quitApplication();

public Q_SLOTS:
    void process_snapshot(QString file_name, QImage snapshot);
    void reboot_application();
    void quit_application();
    void device_status_change();
};

} // namespace aapi

#endif // AAPI_APPLICATION_H
