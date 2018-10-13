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

#ifndef AAPI_ANALYZER_APP_H
#define AAPI_ANALYZER_APP_H

#include <QObject>
#include <QPointer>
#include "analyzer/antscope/antscope_device.h"
#include "analyzer/analyzer_dsp.h"
#include "ui/qml_about_view.h"
#include "ui/qml_dsp_view.h"
#include "ui/qml_config_view.h"
#include "ui/qml_measure_view.h"
#include "ui/qml_panvswr_view.h"
#include "ui/qml_oslcal_view.h"
#include "ui/qml_hwcal_view.h"


///////////////////////////////////////////////////////////////////////////////
// Define reboot exit code
///////////////////////////////////////////////////////////////////////////////

#define EXIT_REBOOT -123456789

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// QAAPIApplication
///////////////////////////////////////////////////////////////////////////////

class QAAPIApplication : public QObject
{
    Q_OBJECT

    /* ----------- Properties ------------ */
    Q_PROPERTY(QAAPIQmlDSPView* dsp READ get_dsp_view CONSTANT)
    Q_PROPERTY(QAAPIQmlConfigView* config READ get_config_view CONSTANT)
    Q_PROPERTY(QAAPIQmlMeasureView* measure READ get_measure_view CONSTANT)
    Q_PROPERTY(QAAPIQmlPanVSWRView* panvswr READ get_panvswr_view CONSTANT)
    Q_PROPERTY(QAAPIQmlOSLCalView* oslcal READ get_oslcal_view CONSTANT)
    Q_PROPERTY(QAAPIQmlHWCalView* hwcal READ get_hwcal_view CONSTANT)
    Q_PROPERTY(QAAPIQmlAboutView* about READ get_about_view CONSTANT)
    Q_PROPERTY(QString lastSnapshot READ get_last_snapshot CONSTANT)

public:
    explicit QAAPIApplication(QObject *parent = Q_NULLPTR);
    ~QAAPIApplication();

    int load();
    void unload();


    /* --------- Property accessors ------- */
    QAAPIQmlDSPView* get_dsp_view() { return m_dspView; }
    QAAPIQmlConfigView* get_config_view() { return m_configView; }
    QAAPIQmlMeasureView* get_measure_view() { return m_measureView; }
    QAAPIQmlPanVSWRView* get_panvswr_view() { return m_panVswrView; }
    QAAPIQmlOSLCalView* get_oslcal_view() { return m_oslCalView; }
    QAAPIQmlHWCalView* get_hwcal_view() { return m_hwCalView; }
    QAAPIQmlAboutView* get_about_view() { return m_aboutView; }
    QString get_last_snapshot() { return m_lastSnapshot; }

    static QString get_snapshot_dir();

private:
    aapi_ptr<AAPIConfig>        m_config;
    aapi_ptr<AAPIGenerator>     m_generator;
    aapi_ptr<antscope_device>   m_antscope;
    aapi_ptr<AAPISignalProcessor>   m_processor;
    aapi_ptr<AAPICalibrator>    m_calibrator;
    QString                     m_lastSnapshot;

    /* ------------- Models -------------- */
    QPointer<QAAPIQmlDSPView>       m_dspView;
    QPointer<QAAPIQmlConfigView>    m_configView;
    QPointer<QAAPIQmlMeasureView>   m_measureView;
    QPointer<QAAPIQmlPanVSWRView>   m_panVswrView;
    QPointer<QAAPIQmlOSLCalView>    m_oslCalView;
    QPointer<QAAPIQmlHWCalView>     m_hwCalView;
    QPointer<QAAPIQmlAboutView>     m_aboutView;


public slots:
    void snapshot_taken(QString file_name, QImage snapshot);
    void reboot_application();
    void quit_application();
};

} // namespace aapi

#endif // AAPI_ANALYZER_APP_H
