/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QML_PANVSWR_VIEW_H
#define QML_PANVSWR_VIEW_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QtQuick/QQuickItemGrabResult>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include "analyzer/analyzer_calibrator.h"
#include "ui/qml_view_backend.h"

QT_CHARTS_USE_NAMESPACE

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlPanVSWRView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlPanVSWRView class
///
class QAAPIQmlPanVSWRView :
        public QAAPIQmlView
{
    Q_OBJECT

public:
    explicit QAAPIQmlPanVSWRView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                 AAPIGenerator *gen, AAPICalibrator *cal,
                                 QObject *parent = Q_NULLPTR);
    ~QAAPIQmlPanVSWRView();

    enum ChartType {
        CHART_VSWR,
        CHART_RX,
        CHART_SMITH,
        CHART_S11
    };

    enum BandSpan {
        SPAN_200 = 200,
        SPAN_400 = 400,
        SPAN_800 = 800,
        SPAN_1600 = 1600,
        SPAN_2M = 2000,
        SPAN_4M = 4000,
        SPAN_8M = 8000,
        SPAN_16M = 16000,
        SPAN_20M = 20000,
        SPAN_40M = 40000
    };

private:
    int get_params();
    uint32_t get_freq_start() const;
    int start_scan(bool fast);
    void update_plot_area(enum ChartType chart_type, QRectF rect);
    void set_plot_bgnd(QChart *chart);
    void update_axis_range();

    /* Measurement callback */
    virtual int on_measure_finished(AAPIMeasure *measure);

    virtual int load_view();
    virtual void destroy_view();

private:
    QMap<ChartType, QSize> m_plotArea;

    QPointer<QObject>   m_SmithChart;
    enum ChartType      m_chartType;

    QPointer<QLineSeries>   m_RXSeries[2];
    QPointer<QLineSeries>   m_VSWRSeries;

    aapi_ptr<AAPICalibrator> m_calibrator;
    uint32_t    m_freq1, m_span, m_width;

signals:
    void pointGamma(QPointF pt);
    void scanStarted(int numSteps);
    void scanComplete();
    void scanError(QString message);
    void scanNoSignal();

public slots:
    void rx_setup(QLineSeries *RSeries, QLineSeries *XSeries);
    void vswr_setup(QLineSeries *series);
    void smith_setup(QObject *chart);

    void tab_changed(int index);

    void rx_fast_scan();
    void rx_slow_scan();

    void vswr_fast_scan();
    void vswr_slow_scan();

    void smith_fast_scan();
    void smith_slow_scan();

    void rx_snapshot(QQuickItemGrabResult *result);
    void vswr_snapshot(QQuickItemGrabResult *result);
    void smith_snapshot(QQuickItemGrabResult *result);

    void rx_plot_area(QRectF rect);
    void vswr_plot_area(QRectF rect);
    void smith_plot_area(QRectF rect);

};

#endif // QML_PANVSWR_VIEW_H
