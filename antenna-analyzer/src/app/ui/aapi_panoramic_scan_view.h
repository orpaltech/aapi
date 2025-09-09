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

#ifndef UI_AAPI_PAN_VSWR_VIEW_H
#define UI_AAPI_PAN_VSWR_VIEW_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QtQuick/QQuickItemGrabResult>
#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include "analyzer/aapi_calibrator.h"
#include "aapi_view_backend.h"
#include "aapi_base_style.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiPanoramicScanView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiPanoramicScanView class
///
class QAAPiPanoramicScanView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(PanoramicScanViewBackend)

public:
    explicit QAAPiPanoramicScanView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                    AAPiGenerator *gen, AAPiCalibrator *cal,
                                    QAAPiBaseStyle *style,
                                    QObject *parent = Q_NULLPTR);
    ~QAAPiPanoramicScanView();

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
    int getConfigParams();
    uint32_t getFreqStart() const;
    int startScan(bool fast);
    void updatePlotArea(ChartType chart_type, QRectF rect);
    void setPlotBkgnd(QChart *chart);
    void updateAxisRange();

    virtual int load_view();
    virtual void destroy_view();

// Measurement callback
    virtual int onViewMeasureFinished(AAPiMeasureTask *measure);

private:
    QMap<ChartType, QSize>  m_plotArea;

    QPointer<QObject>       m_smithChart;
    ChartType               m_chartType;

    QPointer<QLineSeries>   m_rxSeries[2];
    QPointer<QLineSeries>   m_vswrSeries;

    AAPiCalibrator          *m_calibrator;
    uint32_t                m_freq1, m_span;
    uint32_t                m_width;

    QAAPiBaseStyle          *m_style;

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

#endif // UI_AAPI_PAN_VSWR_VIEW_H
