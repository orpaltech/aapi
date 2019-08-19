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

#include <QQmlComponent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QApplication>
#include <QTimer>
#include "analyzer_app.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// QAAPIApplication
///////////////////////////////////////////////////////////////////////////////

QAAPIApplication::QAAPIApplication(QObject *parent)
    : QObject( parent )
    , m_config( AAPIConfig::create() )
    , m_generator(nullptr)
    , m_dsp(nullptr)
    , m_calibrator(nullptr)
    , m_antscope(nullptr)
{
}

QAAPIApplication::~QAAPIApplication()
{
    unload();
}

int QAAPIApplication::load()
{
    if( m_generator != nullptr )
    {
        // Already loaded, just return OK
        return AAPI_SUCCESS;
    }

    int ret;
    QDir dir( get_snapshot_dir() );
    if( !dir.exists() )
    {
        if( !dir.mkpath( get_snapshot_dir() ) )
            return AAPI_E_CREATE_DIR_FAILED;
    }

    AAPtr<AAPIGenerator>        generator( AAPIGenerator::create( m_config, false ) );
    AAPtr<AAPISignalProcessor>  dsp( AAPISignalProcessor::create( m_config, false ) );
    AAPtr<AAPICalibrator>   calibrator( AAPICalibrator::create( m_config, false ) );
    AAPtr<AntScopeDevice>   antscope( AntScopeDevice::create( m_config, generator, false ) );

    QPointer<QAAPIQmlConfigView>    qml_config_view ( new QAAPIQmlConfigView( m_config ) );
    QPointer<QAAPIQmlDSPView>   qml_dsp_view ( new QAAPIQmlDSPView( m_config, dsp, generator ) );
    QPointer<QAAPIQmlMeasureView>   qml_measure_view ( new QAAPIQmlMeasureView( m_config, dsp, generator ) );
    QPointer<QAAPIQmlPanVSWRView>   qml_panvswr_view ( new QAAPIQmlPanVSWRView( m_config, dsp, generator, calibrator ) );
    QPointer<QAAPIQmlOSLCalView>    qml_oslcal_view ( new QAAPIQmlOSLCalView( m_config, dsp, generator, calibrator ) );
    QPointer<QAAPIQmlHWCalView> qml_hwcal_view ( new QAAPIQmlHWCalView( m_config, dsp, generator, calibrator ) );
    QPointer<QAAPIQmlAboutView> qml_about_view ( new QAAPIQmlAboutView( m_config ) );
    QPointer<QAAPIQmlStatusBackend> qml_status_backend ( new QAAPIQmlStatusBackend() );

    ret = calibrator->init();
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    ret = generator->open();
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    ret = dsp->start();
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }
    
    ret = antscope->start();
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    ret = qml_status_backend->init();
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    m_generator = generator.detach();
    m_antscope  = antscope.detach();
    m_dsp       = dsp.detach();
    m_calibrator    = calibrator.detach();

    m_qml_config_view   = qml_config_view;
    m_qml_dsp_view  = qml_dsp_view;
    m_qml_measure_view  = qml_measure_view;
    m_qml_panvswr_view  = qml_panvswr_view;
    m_qml_oslcal_view   = qml_oslcal_view;
    m_qml_hwcal_view    = qml_hwcal_view;
    m_qml_about_view    = qml_about_view;
    m_qml_status_backend = qml_status_backend;

    // Connect snapshot signal and slot
    QObject::connect( m_qml_panvswr_view, SIGNAL( snapshotTaken(QString, QImage) ),
                      this, SLOT( snapshot_taken( QString, QImage ) ));

    // Connect quit, reboot signals and slots
    QObject::connect( m_qml_about_view, SIGNAL( rebootApplication() ),
                      this, SLOT( reboot_application() ) );

    QObject::connect( m_qml_about_view, SIGNAL( quitApplication() ),
                      this, SLOT( quit_application() ));

    /*===========================================================*/
/*REMOVE THIS */
   QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(quit_application()));
   timer->setSingleShot(true);
   timer->start(2*30000);

    return 0;
}

void QAAPIApplication::unload()
{
    if( m_generator == nullptr )
        return;

    // Stop digital signal processing component
    m_dsp->stop();

    // Close generator component 
    m_generator->close();

    // Stop Antscope component 
    m_antscope->stop();

    // Release allocated memory
    AAPI_DISPOSE(m_dsp);
    AAPI_DISPOSE(m_generator);
    AAPI_DISPOSE(m_antscope);
    AAPI_DISPOSE(m_calibrator);
    AAPI_DISPOSE(m_config);

    // Release views 
    m_qml_config_view = nullptr;
    m_qml_dsp_view = nullptr;
    m_qml_measure_view = nullptr;
    m_qml_panvswr_view = nullptr;
    m_qml_oslcal_view = nullptr;
    m_qml_hwcal_view = nullptr;
    m_qml_about_view = nullptr;
    m_qml_status_backend = nullptr;
}

QString QAAPIApplication::get_snapshot_dir()
{
    return QDir::cleanPath( AAPIConfig::get_app_dir() + QDir::separator() + tr("snapshot") );
}

void QAAPIApplication::snapshot_taken(QString file_name, QImage snapshot)
{
    QString format = m_config->get_snapshot_format();
    QString date_time = QDateTime::currentDateTimeUtc().toString("yyyyMMdd_HHmmss");
    m_last_snapshot = QDir::cleanPath( get_snapshot_dir() + QDir::separator()
                                     + QString("%1_%2.%3").arg( file_name, date_time, format ));
    snapshot.save(m_last_snapshot);
}

void QAAPIApplication::quit_application()
{
    QApplication::exit( EXIT_SUCCESS );
}

void QAAPIApplication::reboot_application()
{
    QApplication::exit( EXIT_REBOOT );
}

} // namespace aapi
