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

#include <QFile>
#include <QGuiApplication>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <qapplication.h>
#include "aapi_application.h"
#include "ui/aapi_aquish_style.h"
#include "ui/aapi_messages.h"


#define AAPI_QML_NAMESPACE  "ru.orpaltech.aapi"
#define AAPI_QML_VER_MAJOR  1
#define AAPI_QML_VER_MINOR  0
#define AAPI_QML_VER    AAPI_QML_VER_MAJOR,AAPI_QML_VER_MINOR

// Define your log file path
static QString logFilePath = QString("/var/log/aapi_%1.log").arg(QDateTime::currentDateTime().toString("ddMMyyyy-hhmmss"));

void logFileMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file(logFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

        switch (type)
        {
        case QtDebugMsg:
            out << "[DEBUG] ";
            break;
        case QtInfoMsg:
            out << "[INFO] ";
            break;
        case QtWarningMsg:
            out << "[WARNING] ";
            break;
        case QtCriticalMsg:
            out << "[CRITICAL] ";
            break;
        case QtFatalMsg:
            out << "[FATAL] ";
            break;
        }

        out << msg << " (" << context.file << ":" << context.line << ", " << context.function << ")\n";
        file.close();
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_MESSAGE_PATTERN", QByteArray("[%{time process} %{type}] %{appname} %{category} %{function} - %{message}"));

    qInstallMessageHandler(logFileMessageOutput);

    QAAPiShutdownManager shutdownManager;
    ret = shutdownManager.openDevice();
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    do
    {
        QApplication app( argc, argv );
        app.setApplicationName("AA-Pi");
        app.setApplicationDisplayName("Antenna Analyzer Pi");
        app.setApplicationVersion(AAPI_VERSION);
        app.setOrganizationName("ORPAL Technology, Inc.");

        QAAPiBaseStyle  *style = new QAAPiAquishStyle( &app );
        QAAPiMessages   *mgs = new QAAPiMessages( &app );
        QAAPiApplication *aapi = new QAAPiApplication( style, mgs, &shutdownManager, &app );

        QScreen *screen = QApplication::primaryScreen();

        qmlRegisterUncreatableType<QAAPiStatusBackend>      (AAPI_QML_NAMESPACE, AAPI_QML_VER, "ApplicationStatusBackend",  AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiViewBackend>        (AAPI_QML_NAMESPACE, AAPI_QML_VER, "ViewBackend",               AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiAboutAppView>       (AAPI_QML_NAMESPACE, AAPI_QML_VER, "AboutAppViewBackend",       AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiConfigurationView>  (AAPI_QML_NAMESPACE, AAPI_QML_VER, "ConfigurationViewBackend",  AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiHWCalibrationView>  (AAPI_QML_NAMESPACE, AAPI_QML_VER, "HWCalibrationViewBackend",  AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiOSLCalibrationView> (AAPI_QML_NAMESPACE, AAPI_QML_VER, "OSLCalibrationViewBackend", AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiMeasurementView>    (AAPI_QML_NAMESPACE, AAPI_QML_VER, "MeasurementViewBackend",    AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiPanoramicScanView>  (AAPI_QML_NAMESPACE, AAPI_QML_VER, "PanoramicScanViewBackend",  AAPI_QML_UNCREATABLE_REASON);
        qmlRegisterUncreatableType<QAAPiSignalProcessView>  (AAPI_QML_NAMESPACE, AAPI_QML_VER, "SignalProcessViewBackend",  AAPI_QML_UNCREATABLE_REASON);


        do
        {
            // Set analyzer object as a context property
            QQmlApplicationEngine engine;
            engine.rootContext()->setContextProperty( "aapi", aapi );

            QObject::connect(&engine,
                            &QQmlApplicationEngine::objectCreationFailed,
                            &app,
                            []() { QCoreApplication::exit(-1); },
                            Qt::QueuedConnection);

#ifdef AAPI_DESIGN_MODE
            engine.rootContext()->setContextProperty( "AAPI_DESIGN_MODE", true );
#else
            engine.rootContext()->setContextProperty( "AAPI_DESIGN_MODE", false );
#endif

            ret = aapi->load( );
            if (AAPI_FAILED( ret ))
            {
                ret = EXIT_FAILURE;
                break;
            }

            engine.loadFromModule("aapi", "Main");
            if (engine.rootObjects().isEmpty())
            {
                ret = EXIT_FAILURE;
                break;
            }

            // Get the first root object, assuming it's main window
            QObject *rootObj = engine.rootObjects().first();

            // Cast to QQuickWindow if it's a window type
            aapi->setMainWindow(qobject_cast<QQuickWindow*>(rootObj));

            // Enter application main loop
            ret = app.exec();
        } while(false);

    } while (ret == EXIT_REBOOT); // Check if reboot requested

    // At this point:
    // - app.exec() has returned
    // - All Qt objects cleaned up

    // Check if shutdown was requested and perform system shutdown
    if (shutdownManager.isShutdownRequested())
    {       
        qInfo() << "Shutdown was requested, proceeding with system poweroff";
        shutdownManager.performSystemShutdown();

        // Should not reach here if poweroff succeeded
        qCritical() << "System poweroff failed or not executed";
    }
    else
    {
        qInfo() << "No shutdown requested, normal exit";
    }

    qInfo() << "========================================";
    qInfo() << "Application exiting normally";
    qInfo() << "========================================";

    return ret;
}
