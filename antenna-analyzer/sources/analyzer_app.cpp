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
    : QObject(parent)
    , m_config(AAPIConfig::create(false))
{
}

QAAPIApplication::~QAAPIApplication()
{
    unload();
}

int QAAPIApplication::load()
{
    if( m_generator )
    {
        return AAPI_E_SUCCESS;
    }

    QDir dir( get_snapshot_dir() );
    if( !dir.exists() )
    {
        if( !dir.mkpath(get_snapshot_dir()) )
            return AAPI_E_CREATE_DIRECTORY;
    }

    aapi_ptr<AAPIGenerator>     generator;  generator.attach(AAPIGenerator::create( m_config ));
    aapi_ptr<antscope_device>   antscope;   antscope.attach(antscope_device::create( m_config, generator ));
    aapi_ptr<AAPISignalProcessor>   processor;  processor.attach(AAPISignalProcessor::create( m_config ));
    aapi_ptr<AAPICalibrator> calibrator;    calibrator.attach(AAPICalibrator::create( m_config ));

    QPointer<QAAPIQmlConfigView> qmlConfig ( new QAAPIQmlConfigView( m_config ) );

    QPointer<QAAPIQmlDSPView> qmlDSP ( new QAAPIQmlDSPView( m_config, processor, generator) );

    QPointer<QAAPIQmlMeasureView> qmlMeasure ( new QAAPIQmlMeasureView( m_config, processor, generator) );

    QPointer<QAAPIQmlPanVSWRView> qmlPanVSWR ( new QAAPIQmlPanVSWRView( m_config, processor, generator, calibrator) );

    QPointer<QAAPIQmlOSLCalView> qmlOSLCal ( new QAAPIQmlOSLCalView( m_config, processor, generator, calibrator) );

    QPointer<QAAPIQmlHWCalView> qmlHWCal ( new QAAPIQmlHWCalView( m_config, processor, generator, calibrator) );

    QPointer<QAAPIQmlAboutView> qmlAbout ( new QAAPIQmlAboutView( m_config ) );

    int ret = calibrator->init();
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    ret = generator->open();
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    ret = antscope->start();
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    ret = processor->start();
    if( AAPI_FAILED(ret) )
    {
        return ret;
    }

    m_generator   = generator;
    m_antscope    = antscope;
    m_processor   = processor;
    m_calibrator  = calibrator;

    m_dspView     = qmlDSP;
    m_configView  = qmlConfig;
    m_measureView = qmlMeasure;
    m_panVswrView = qmlPanVSWR;
    m_oslCalView  = qmlOSLCal;
    m_hwCalView   = qmlHWCal;
    m_aboutView   = qmlAbout;

    // Connect snapshot signal and slot
    QObject::connect( m_panVswrView, SIGNAL( snapshotTaken(QString, QImage) ),
                      this, SLOT( snapshot_taken( QString, QImage ) ));

    // Connect quit, reboot signals and slots
    QObject::connect( m_aboutView, SIGNAL( rebootApplication() ),
                      this, SLOT( reboot_application() ) );

    QObject::connect( m_aboutView, SIGNAL( quitApplication() ),
                      this, SLOT( quit_application() ));

    /*===========================================================*/
/*REMOVE THIS */
    QTimer *timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(quit_application()));
   timer->setSingleShot(true);
   timer->start(30000);

    return 0;
}

void QAAPIApplication::unload()
{
    if (!m_generator)
        return;

    /* Stop digital signal processing component*/
    m_processor->stop();

    /* Stop Antscope component */
    m_antscope->stop();

    /* Close generator component */
    m_generator->close();

    /* Release allocated memory*/
    m_processor = nullptr;
    m_generator = nullptr;
    m_antscope = nullptr;
    m_calibrator = nullptr;

    /* Release views */
    m_dspView = nullptr;
    m_configView = nullptr;
    m_measureView = nullptr;
    m_panVswrView = nullptr;
    m_oslCalView = nullptr;
    m_hwCalView = nullptr;
    m_aboutView = nullptr;
}

QString QAAPIApplication::get_snapshot_dir()
{
    return QDir::cleanPath(AAPIConfig::get_app_dir() + QDir::separator() + tr("snapshot"));
}

void QAAPIApplication::snapshot_taken(QString file_name, QImage snapshot)
{
    QString format = m_config->get_snapshot_format();
    QString date_time = QDateTime::currentDateTimeUtc().toString("yyyyMMdd_HHmmss");
    m_lastSnapshot = QDir::cleanPath( get_snapshot_dir() + QDir::separator()
                                     + QString("%1_%2.%3").arg( file_name, date_time, format ));
    snapshot.save(m_lastSnapshot);
}

void QAAPIApplication::quit_application()
{
    QApplication::exit(EXIT_SUCCESS);
}

void QAAPIApplication::reboot_application()
{
    QApplication::exit(EXIT_REBOOT);
}

} // namespace aapi
