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
#include <QPoint>
#include <QPointer>
#include <QtQuick/QQuickItemGrabResult>
#include <QtGraphs/QLineSeries>
#include <QtGraphs/QValueAxis>
#include "analyzer/aapi_calibrator.h"
#include "aapi_view_backend.h"
#include "aapi_base_style.h"
#include "aapi_messages.h"

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

    /* Properties */
    Q_PROPERTY(quint32 last_measure_freq READ getLastMeasureFreq NOTIFY legendChanged)
    Q_PROPERTY(QString last_rx_string READ getLastRxString NOTIFY legendChanged)
    Q_PROPERTY(double last_vswr_val READ getLastVswrVal NOTIFY legendChanged)

    Q_PROPERTY(QVariantList ham_radio_bands READ getHamRadioBands NOTIFY hamRadioBandsChanged)

    Q_PROPERTY(bool is_min_freq_center READ getIsMinFreqCenter WRITE setIsMinFreqCenter NOTIFY isMinFreqCenterChanged)
    Q_PROPERTY(quint64 min_freq READ getMinFreq WRITE setMinFreq NOTIFY minFreqChanged)

    Q_PROPERTY(quint32 start_freq READ getStartFreqKHz NOTIFY startFreqChanged)
    Q_PROPERTY(quint32 band_span READ getBandSpanKHz WRITE setBandSpanKHz NOTIFY bandSpanChanged)

public:
    explicit QAAPiPanoramicScanView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                    AAPiGenerator *gen, AAPiCalibrator *cal,
                                    QAAPiBaseStyle *style, QAAPiMessages *msgs,
                                    QObject *parent = Q_NULLPTR);
    ~QAAPiPanoramicScanView();

    enum class ChartType {
        VSWR = 0,
        RX,
        SMITH,
        S11
    };

    enum class SweepPoints {
        Fast      = 100,  // Lightning fast tuning/alignment updates
        Standard  = 200,  // The sweet spot for quick band scans
        HighRes   = 400,  // High resolution for sharp filter notches
        MaxVisual = 800   // Absolute ceiling matching a standard tablet screen width
    };

    enum class BandSpan : uint32_t {
        _200K = 200U,
        _400K = 400U,
        _800K = 800U,
        _1600K = 1'600U,
        _2M = 2'000U,
        _4M = 4'000U,
        _8M = 8'000U,
        _16M = 16'000U,
        _20M = 20'000U,
        _40M = 40'000U,
        _80M = 80'000U
    };

    Q_ENUM(BandSpan)

    enum TuneDirection {
        TUNE_DOWN_LARGE  = -3, // <<< (e.g., -100 kHz)
        TUNE_DOWN_MEDIUM = -2, // <<  (e.g., -10 kHz)
        TUNE_DOWN_SMALL  = -1, // <   (e.g., -1 kHz)
        TUNE_UP_SMALL    = 1,  // >   (e.g., +1 kHz)
        TUNE_UP_MEDIUM   = 2,  // >>  (e.g., +10 kHz)
        TUNE_UP_LARGE    = 3   // >>> (e.g., +100 kHz)
    };
    Q_ENUM(TuneDirection)

    // A simple structure to pass coordinate blocks to QML
    struct BandOverlay {
        uint32_t lo;
        uint32_t hi;
    };

    uint32_t getLastMeasureFreq() const { return m_lastFreq; }
    QString getLastRxString() const { return m_lastRxStr; }
    double getLastVswrVal() const { return m_lastVswr; }
    quint32 getBandSpanKHz() const { return m_bandSpanKHz; }
    void setBandSpanKHz(quint32 band_span);
    quint32 getStartFreqKHz() const;
    quint32 getMinFreq() const { return m_minFreq; }
    void setMinFreq(quint32 min_freq);
    QVariantList getHamRadioBands() const { return m_hamRadioBands; }
    bool getIsMinFreqCenter() const { return m_config->get_pan_is_center_freq(); }
    void setIsMinFreqCenter(bool is_center);

protected:
// QAAPiViewBackend
    AAPiError onViewLoad() override;
    void onViewDestroy() override;
    AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) override;
    void onViewMeasureError(AAPiError error) override;

private:
    int validateConfig();
    int startSweep(SweepPoints numPoints);
    void updatePlotArea(ChartType chart_type, const QRectF& rect);
    void updateAxisXRange();
    void updateAxisYRange();
    void setDefaultYRange(ChartType chart_type);

    constexpr bool isBandSpanValid(uint32_t band_span) const {
        return (band_span >= static_cast<uint32_t>(BandSpan::_200K) && band_span <= static_cast<uint32_t>(BandSpan::_80M));
    }

private:
    //QMap<ChartType, uint>   m_plotSize;
    ChartType               m_chartType;

    QPointer<QObject>       m_smithChart;
    QList<QPointF>          m_smithTracePoints;

    QPointer<QXYSeries>     m_rxSeries[2];
    QValueAxis              *m_rxAxisX, *m_rxAxisY;

    QPointer<QXYSeries>     m_vswrSeries;
    QValueAxis              *m_vswrAxisX, *m_vswrAxisY;

    QPointer<QXYSeries>     m_s11Series;
    QValueAxis              *m_s11AxisX, *m_s11AxisY;

    QVariantList            m_hamRadioBands;

    AAPiPtr<AAPiCalibrator> m_calibrator;

    uint32_t                m_minFreq;
    uint32_t                m_bandSpanKHz; /* expressed in KHz*/

    QAAPiBaseStyle          *m_style;

    // Variables to cache the latest scanned point data
    uint32_t                m_lastFreq;
    QString                 m_lastRxStr;
    double                  m_lastVswr;

Q_SIGNALS:
    void legendChanged(); // Trigger this to tell QML to update the legend values
    void scanStarted(int numSteps);
    void scanComplete();
    void scanError(QString message);
    void scanNoSignal();
    void hamRadioBandsChanged();
    void minFreqChanged();
    void isMinFreqCenterChanged();
    void startFreqChanged();
    void bandSpanChanged();

public Q_SLOTS:
    void handleRXChartSetup(QXYSeries *r_series, QXYSeries *x_series,
                            QValueAxis *axisX, QValueAxis *axisY);
    void handleVSWRChartSetup(QXYSeries *series, QValueAxis *axisX, QValueAxis *axisY);
    void handleS11ChartSetup(QXYSeries *series, QValueAxis *axisX, QValueAxis *axisY);
    void handleSmithChartSetup(QObject *chart);
    void handleTabChange(int index);

    void handleRXScanFast();
    void handleRXScanSlow();

    void handleVSWRScanFast();
    void handleVSWRScanSlow();

    void handleS11ScanFast();
    void handleS11ScanSlow();

    void handleSmithScanFast();
    void handleSmithScanSlow();

    void handleRXSnapshot(QQuickItemGrabResult *result);
    void handleVSWRSnapshot(QQuickItemGrabResult *result);
    void handleS11Snapshot(QQuickItemGrabResult *result);
    void handleSmithSnapshot(QQuickItemGrabResult *result);

    void handleRXPlotArea(QRectF rect);
    void handleVSWRPlotArea(QRectF rect);
    void handleS11PlotArea(QRectF rect);
    void handleSmithPlotArea(QRectF rect);

    void handleTuneFrequency(TuneDirection direction);
    void handleDirectFreqInput(quint32 freq);

};

#endif // UI_AAPI_PAN_VSWR_VIEW_H
