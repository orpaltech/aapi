/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "analyzer_app.h"

#define BACKEND_URI "com.orpaltech.aapi"

const QString ERROR_CREATE_BACKEND("Unable to create backend object");

int main( int argc, char *argv[] )
{
    int ret;

    do
    {
        QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling, true );
        QApplication app( argc, argv );
        QAAPIApplication *analyzer = new QAAPIApplication( &app );

        ret = analyzer->load();
        if( AAPI_FAILED( ret ) )
        {
            ret = EXIT_FAILURE;
            break;
        }

        /* Register QML types */
        ret = qmlRegisterUncreatableType<QAAPIQmlView>(BACKEND_URI, 1, 0, "ViewBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlDSPView>(BACKEND_URI, 1, 0, "DSPViewBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlConfigView>(BACKEND_URI, 1, 0, "ConfigViewBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlMeasureView>(BACKEND_URI, 1, 0, "MeasurementBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlPanVSWRView>(BACKEND_URI, 1, 0, "PanVSWRBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlOSLCalView>(BACKEND_URI, 1, 0, "OSLCalibrationBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlHWCalView>(BACKEND_URI, 1, 0, "HWCalibrationBackend", ERROR_CREATE_BACKEND);

        ret = qmlRegisterUncreatableType<QAAPIQmlAboutView>(BACKEND_URI, 1, 0, "AboutViewBackend", ERROR_CREATE_BACKEND);

        do
        {
            /* Set analyzer object as a context property */
            QQmlApplicationEngine engine;
            engine.rootContext()->setContextProperty( "analyzer",
                                                      static_cast< QObject*>( analyzer ));

            engine.load( QUrl( QStringLiteral( "qrc:/main.qml" ) ) );
            if( engine.rootObjects().isEmpty() )
            {
                ret = EXIT_FAILURE;
                break;
            }

            /* Enter application main loop */
            ret = app.exec();
        } while(false);

        /* Terminate analyzer */
        analyzer->unload();
    } while (ret == EXIT_REBOOT); /* Check if reboot requested */

    return ret;
}
