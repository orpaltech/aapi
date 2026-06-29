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

#include "aapi_panoramic_scan_view.h"
#include <cmath>
#include <QQuickItem>

///////////////////////////////////////////////////////////////////////////////
// class QAAPiPanoramicScanView
///////////////////////////////////////////////////////////////////////////////

QAAPiPanoramicScanView::QAAPiPanoramicScanView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, AAPiCalibrator *cal,
                                               QAAPiBaseStyle *style, QAAPiMessages *msgs,
                                               QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_style(style)
    , m_msgs(msgs)
    , m_calibrator(cal)
    , m_rxAxisX(nullptr), m_rxAxisY(nullptr)
    , m_vswrAxisX(nullptr), m_vswrAxisY(nullptr)
{
    m_minFreq = 0;
    m_bandSpanKHz = 0;

    // Variables to cache the latest scanned point data
    m_lastFreq = 0;
    m_lastRxStr = "0.0 + j0.0";
    m_lastVswr = 1.0;

    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiPanoramicScanView::~QAAPiPanoramicScanView()
{
}

void QAAPiPanoramicScanView::handleSmithChartSetup(QObject *chart)
{
    m_smithChart = chart;
}

void QAAPiPanoramicScanView::handleRXChartSetup(QXYSeries *r_series, QXYSeries *x_series,
                                                QValueAxis *axisX, QValueAxis *axisY)
{
    // Save our modern series tracker pointers into your member array
    m_rxSeries[0] = r_series;
    m_rxSeries[1] = x_series;
    m_rxAxisX = axisX;
    m_rxAxisY = axisY;

    // Clear old sample cache traces instantly across the hardware paths
    if (m_rxSeries[0])
        m_rxSeries[0]->clear();
    if (m_rxSeries[1])
        m_rxSeries[1]->clear();
}

void QAAPiPanoramicScanView::handleVSWRChartSetup(QXYSeries *series, QValueAxis *axisX, QValueAxis *axisY)
{
    m_vswrSeries = series;
    m_vswrAxisX = axisX;
    m_vswrAxisY = axisY;

    if (m_vswrSeries) {
        m_vswrSeries->clear();
    }
}

void QAAPiPanoramicScanView::handleS11ChartSetup(QXYSeries *series, QValueAxis *axisX, QValueAxis *axisY)
{
    m_s11Series = series;
    m_s11AxisX = axisX;
    m_s11AxisY = axisY;

    if (m_s11Series) {
        m_s11Series->clear();
    }
}

void QAAPiPanoramicScanView::updatePlotArea(ChartType chart_type, const QRectF& rect)
{
}

void QAAPiPanoramicScanView::setBandSpanKHz(quint32 band_span)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<BandSpan>();
    const char* keyName = metaEnum.valueToKey(static_cast<int>(band_span));

    if (keyName == nullptr) {
        return;
    }

    // Input is fully verified and matches your defined C++ values!
    if (m_bandSpanKHz != band_span) {
        m_bandSpanKHz = band_span;

        emit bandSpanChanged();
        emit startFreqChanged();
    }
}

void QAAPiPanoramicScanView::setMinFreq(quint32 min_freq)
{
    const int64_t absoluteMin = getFrequencyMin();
    const int64_t absoluteMax = getFrequencyMax();
    const int64_t bandSpan = static_cast<int64_t>(getBandSpanKHz()) * 1000LL;

    // Initialize adaptive safety limit boundaries
    int64_t allowedMin = absoluteMin;
    int64_t allowedMax = absoluteMax;

    // Dynamically restrict allowed inputs depending on Center vs Start tuning modes
    if (m_config->get_pan_is_center_freq()) {
        // Mode A: Center Frequency Tuning
        // Center cannot drop so low that Center - Span/2 underflows hardware minimums.
        allowedMin = absoluteMin + (bandSpan / 2);

        // Center cannot rise so high that Center + Span/2 overflows hardware maximums.
        allowedMax = absoluteMax - (bandSpan / 2);
    } else {
        // Mode B: Standard Base Start Frequency Tuning.
        // Start position cannot be so high that Start + Span exceeds hardware limits.
        allowedMax = absoluteMax - bandSpan;
    }

    //  Prevent limits from crossing if span is wider than hardware range
    if (allowedMin > allowedMax) {
        allowedMin = absoluteMin;
        allowedMax = absoluteMax;
    }

    // Clamp the incoming requested value within our calculated safe margins
    quint32 minFreq = min_freq;

    if (minFreq < allowedMin)
        minFreq = static_cast<quint32>(allowedMin);

    if (minFreq > allowedMax)
        minFreq = static_cast<quint32>(allowedMax);

    if (m_minFreq != minFreq) {
        m_minFreq = minFreq;

        emit minFreqChanged();
        emit startFreqChanged();
    }
}

void QAAPiPanoramicScanView::setIsMinFreqCenter(bool new_val)
{
    bool current_val = m_config->get_pan_is_center_freq() != 0;

    if (current_val != new_val) {
        m_config->set_pan_is_center_freq(new_val);

        emit isMinFreqCenterChanged();

        setMinFreq(m_minFreq);
    }
}

quint32 QAAPiPanoramicScanView::getStartFreqKHz() const
{
    quint32 freq_start;

    if (m_config->get_pan_is_center_freq()) {
        freq_start = m_minFreq - (getBandSpanKHz() * 1000) / 2;

    } else {
        freq_start = m_minFreq;
    }

    return freq_start / 1000;
}

int QAAPiPanoramicScanView::startSweep(SweepPoints numPoints)
{
    uint32_t num_scans = qMin( m_config->get_pan_num_scans(), AAPI_MAX_MEASURE_SCANS );

    uint32_t freq_start_hz = getStartFreqKHz() * 1000;
    uint32_t band_span_hz = getBandSpanKHz() * 1000;  // Convert to Hz

    // Find how many points to scan
    uint32_t points = (uint32_t) numPoints;
    uint32_t step_hz = band_span_hz / points;

    AAPiMeasureTaskList steps;
    // Reserve memory upfront so the list never reallocates
    int num_steps = points + 1;
    steps.reserve(num_steps);

    // Prepare scan steps
    uint32_t freq = freq_start_hz;
    for (uint i = 0; i < num_steps; ++i) {
        AAPiPtr<AAPiMeasureTask> measure(
            AAPiMeasureTask::create( m_config, m_calibrator, this, freq, true, true, num_scans, false )
        );
        steps.push_back( std::move(measure) );
        freq += step_hz;
    }

    switch (m_chartType) {
    case ChartType::VSWR:
        m_vswrSeries->clear();      /* Clear chart series */
        m_vswrAxisY->setRange(1.0, 11.0);
        m_vswrAxisY->setTickInterval(1.0);
        m_vswrAxisY->setTickAnchor(1.0);
        break;

    case ChartType::RX:
        m_rxSeries[0]->clear();     /* Clear chart series */
        m_rxSeries[1]->clear();
        m_rxAxisY->setRange(-500.0, 500.0);
        m_rxAxisY->setTickAnchor(0.0);
        break;

    case ChartType::S11:
        m_s11Series->clear();       /* Clear chart series */
        m_s11AxisY->setRange(-40.0, 0.0);
        m_s11AxisY->setTickInterval(10.0);
        m_s11AxisY->setTickAnchor(0.0);
        break;

    case ChartType::SMITH:
        QMetaObject::invokeMethod( m_smithChart, "clear" );    /* clear plot area */
        break;

    default:
        break;
    }

    // Start measurement sequence
    int ret = startMeasures( std::move(steps) );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    // Notify UI that scan has started
    emit scanStarted( num_steps );

    return 0;
}

int QAAPiPanoramicScanView::validateConfig()
{
    // Obtain frequency and span from configuration
    m_minFreq = m_config->get_pan_min_freq();
    m_bandSpanKHz = m_config->get_pan_freq_span() / 1000;

    // Defaults to VSWR chart
    m_chartType = ChartType::VSWR;

    // Flag shows if we need to update configuration
    bool updateConfig = false;

    // Validate frequency
    if (! isFrequencyValid( m_minFreq )) {
        // Reset frequency to default
        m_minFreq = 14'000'000;
        updateConfig = true;
    }

    // Now validate span value
    if (! isBandSpanValid( m_bandSpanKHz )) {
        // Reset span to default
        m_bandSpanKHz = static_cast<uint32_t>(BandSpan::_800K);
        updateConfig = true;
    }

    if ( updateConfig ) {
        m_config->set_pan_min_freq( m_minFreq );
        m_config->set_pan_freq_span( m_bandSpanKHz * 1000 );

        // Update configuration
        AAPiError ret = m_config->flush();
        if (AAPI_FAILED( ret )) {
            setErrorMessage("Failed to update configuration");
            return ret;
        }
    }

    return 0;
}

AAPiError QAAPiPanoramicScanView::loadView()
{
    AAPiError ret = validateConfig();
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    emit minFreqChanged();
    emit bandSpanChanged();

    updateAxisXRange();

    setDefaultYRange(ChartType::RX);
    setDefaultYRange(ChartType::VSWR);
    setDefaultYRange(ChartType::S11);

    return AAPI_SUCCESS;
}

void QAAPiPanoramicScanView::destroyView()
{
    m_rxSeries[0] = nullptr;
    m_rxSeries[1] = nullptr;
    m_vswrSeries = nullptr;
    m_s11Series = nullptr;
    m_smithChart = nullptr;
}

void QAAPiPanoramicScanView::updateAxisXRange()
{
    uint32_t freq_start = getStartFreqKHz();
    uint32_t freq_end = freq_start + getBandSpanKHz();

    // --- FREQUENCY X-AXIS CONFIGURATION ---
    if (m_vswrAxisX) {
        m_vswrAxisX->setRange(freq_start, freq_end);
    }
    if (m_rxAxisX) {
        m_rxAxisX->setRange(freq_start, freq_end);
    }
    if (m_s11AxisX) {
        m_s11AxisX->setRange(freq_start, freq_end);
    }

    // --- PACK BANDS FOR THE QML RENDERING OVERLAY ---
    m_hamRadioBands.clear();
    uint32_t num_bands = AAPiConfig::get_num_ham_bands();

    for (uint i = 0; i < num_bands; i++) {

        AAPiRadioBand &band = AAPiConfig::get_ham_bands()[i];

        if (band.hi > freq_start && band.lo < freq_end) {
            // Build matching map structures that QML objects can digest instantly
            QVariantMap bandMap;
            bandMap[QStringLiteral("lo")] = qMax(freq_start, band.lo);
            bandMap[QStringLiteral("hi")] = qMin(band.hi, freq_end);

            m_hamRadioBands.append(bandMap);
        }
    }

    emit hamRadioBandsChanged();
}

qreal calculateCleanStep(qreal rangeSpan)
{
    if (rangeSpan <= 0.0) return 1.0;

    // Determine the rough order of magnitude
    qreal rawStep = rangeSpan / 5.0; // Aiming for roughly 5 nice grid subdivisions
    qreal log10_step = std::log10(rawStep);
    qreal power = std::pow(10.0, std::floor(log10_step));
    qreal normalized = rawStep / power;

    // Force step size to standard human-readable steps (1, 2, 5, 10, etc.)
    if (normalized < 1.5)      return 1.0 * power;
    else if (normalized < 3.5) return 2.0 * power;
    else if (normalized < 7.5) return 5.0 * power;
    else                       return 10.0 * power;
}

void QAAPiPanoramicScanView::setDefaultYRange(ChartType chart_type)
{
    switch (chart_type) {
    case ChartType::RX:
        m_rxAxisY->setRange(-500.0, 500.0);
        m_rxAxisY->setTickAnchor(0.0);
        break;
    case ChartType::VSWR:
        m_vswrAxisY->setRange(1.0, 11.0); // Clean, industry-standard starting view for VSWR
        m_vswrAxisY->setTickInterval(1.0);
        m_vswrAxisY->setTickAnchor(1.0);
        break;
    case ChartType::S11:
        m_s11AxisY->setRange(-40.0, 0.0);
        m_s11AxisY->setTickInterval(10.0);
        m_s11AxisY->setTickAnchor(0.0);
        break;
    default:
        break;
    }
}

void QAAPiPanoramicScanView::updateAxisYRange()
{
    switch (m_chartType) {
    case ChartType::RX: {
        if (!m_rxAxisY) return;

        qreal minVal = std::numeric_limits<qreal>::max();
        qreal maxVal = std::numeric_limits<qreal>::lowest();
        bool hasPoints = false;

        for (int n = 0; n < 2; n++) {
            if (!m_rxSeries[n])
                continue;

            const auto currentPoints = m_rxSeries[n]->points();
            if (currentPoints.isEmpty())
                continue;

            hasPoints = true;
            for (const QPointF& pt : currentPoints) {
                qreal yVal = pt.y();
                if (yVal > maxVal) maxVal = yVal;
                if (yVal < minVal) minVal = yVal;
            }
        }

        if (!hasPoints) {
            setDefaultYRange(ChartType::RX);
            break;
        }

        // Add 15% visual breathing room padding
        qreal rangeSpan = std::abs(maxVal - minVal);
        if (rangeSpan < 1e-3)
            rangeSpan = 10.0;
        maxVal += rangeSpan * 0.15;
        minVal -= rangeSpan * 0.15;

        // Calculate a clean grid step interval (e.g. 10, 50, 100)
        qreal stepInterval = calculateCleanStep(maxVal - minVal);

        // Snap the floor boundary to an integer step milestone first
        minVal = std::floor(minVal / stepInterval) * stepInterval;

        // Calculate the required whole-number of grid cells to fully include maxVal
        qreal requiredTicks = std::ceil((maxVal - minVal) / stepInterval);

        // Derive maxVal exactly. Total height is now a perfect multiple of stepInterval.
        maxVal = minVal + (requiredTicks * stepInterval);

        m_rxAxisY->setRange(minVal, maxVal);
        m_rxAxisY->setTickInterval(stepInterval);
        m_rxAxisY->setTickAnchor(0.0); // Keep centered grid elements aligned to 0.0 Ohm baseline
        break;
    }

    case ChartType::VSWR: {
        if (!m_vswrAxisY) return;

        qreal minVal = 1.0; // Hard clamp baseline floor (VSWR can't drop below 1.0)
        qreal maxVal = 1.0;
        bool hasPoints = false;

        if (m_vswrSeries) {
            const auto currentPoints = m_vswrSeries->points();
            for (const QPointF& pt : currentPoints) {
                if (pt.x() < 0) continue;

                hasPoints = true;
                if (pt.y() > maxVal) {
                    maxVal = pt.y();
                }
            }
        }

        if (!hasPoints) {
            setDefaultYRange(ChartType::VSWR);
            break;
        }

        // Apply a 15% padding cushion to the top
        qreal rangeSpan = maxVal - minVal;
        if (rangeSpan < 1e-3)
            rangeSpan = 1.0;
        maxVal += rangeSpan * 0.15;

        qreal stepInterval = calculateCleanStep(maxVal - minVal);

        // Anchor minVal at 1.0, and force maxVal to match a whole step multiple
        qreal requiredTicks = std::ceil((maxVal - minVal) / stepInterval);
        maxVal = minVal + (requiredTicks * stepInterval);

        m_vswrAxisY->setRange(minVal, maxVal);
        m_vswrAxisY->setTickInterval(stepInterval);
        m_vswrAxisY->setTickAnchor(1.0);
        break;
    }

    case ChartType::S11: {
        if (!m_s11AxisY) return;

        qreal minVal = std::numeric_limits<qreal>::max();
        qreal maxVal = -std::numeric_limits<qreal>::max();
        bool hasPoints = false;

        if (m_s11Series) {
            const QList<QPointF> currentPoints = m_s11Series->points();
            if (!currentPoints.isEmpty()) {
                hasPoints = true;
                for (const QPointF& pt : currentPoints) {
                    if (pt.x() < 0)
                        continue;

                    qreal yVal = pt.y();
                    if (yVal > maxVal) maxVal = yVal;
                    if (yVal < minVal) minVal = yVal;
                }
            }
        }

        if (!hasPoints) {
            setDefaultYRange(ChartType::S11);
            break;
        }

        if (maxVal > 0.0)
            maxVal = 0.0;

        // Add 10% padding to the bottom to keep the tuning dips clean
        qreal rangeSpan = std::abs(maxVal - minVal);
        if (rangeSpan < 1e-3)
            rangeSpan = 10.0;
        minVal -= rangeSpan * 0.10;

        if (minVal < -60.0)
            minVal = -60.0;

        qreal stepInterval = calculateCleanStep(maxVal - minVal);

        // S11 grids look best anchored to the top 0.0 dB reference line.
        // We snap the ceiling (maxVal) to 0.0 dB or an exact interval milestone.
        maxVal = std::ceil(maxVal / stepInterval) * stepInterval;

        // Step downward toward minVal by a whole number of intervals
        qreal requiredTicks = std::ceil((maxVal - minVal) / stepInterval);
        minVal = maxVal - (requiredTicks * stepInterval);

        if (qFuzzyCompare(minVal, maxVal)) {
            minVal = -40.0;
            maxVal = 0.0;
            stepInterval = 10.0;
        }

        m_s11AxisY->setRange(minVal, maxVal);
        m_s11AxisY->setTickInterval(stepInterval);
        m_s11AxisY->setTickAnchor(0.0);
        break;
    }

    default:
        break;
    }
}

void QAAPiPanoramicScanView::onViewMeasureError(AAPiError error)
{
    emit scanError(m_msgs->error(error));
}

AAPiError QAAPiPanoramicScanView::onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    if (measure == nullptr) {
        switch (m_chartType) {
        case ChartType::SMITH:
            QMetaObject::invokeMethod(m_smithChart, "updateSweepTrace",
                                      Qt::QueuedConnection,
                                      Q_ARG(QVariant, QVariant::fromValue(m_smithTracePoints)));
            m_smithTracePoints.clear();
            break;
        default:
            break;
        }

        updateAxisYRange();
        emit scanComplete();
        return AAPI_SUCCESS;
    }

    if (measure->is_low_signal()) {
        emit scanNoSignal();
        return AAPI_E_FAILURE;
    }

    const AAPiComplex rx = measure->Rx;
    const uint32_t r0 = m_config->get_base_r0();
    const double vswr = measure->vswr;
    const double freq = measure->measure_freq / 1000.0; // convert to KHz
    AAPiComplex gamma;

    switch (m_chartType) {
    case ChartType::RX:
        if (m_rxSeries[0] && m_rxSeries[1]) {
            m_rxSeries[0]->append(freq, rx.real());
            m_rxSeries[1]->append(freq, rx.imag());
        }
        break;

    case ChartType::VSWR:
        if (m_vswrSeries) {
            m_vswrSeries->append(freq, vswr);
        }
        break;

    case ChartType::S11:
        if (m_s11Series) {
            gamma = AAPiCalibrator::gamma_from_z(rx, r0);

            // Compute the absolute magnitude (length) of the complex gamma vector
            // std::abs automatically calculates sqrt(real^2 + imag^2) for complex numbers
            double gamma_mag = std::abs(gamma);
            double s11_db = -100.0; // Clean baseline floor for a perfect match (gamma_mag == 0)

            // Prevent log10(0) domain crashes using a tiny epsilon guard
            if (gamma_mag > 1e-5) {
                s11_db = 20.0 * std::log10(gamma_mag);
            }

            // Clamp the visual baseline to keep the graph window looking clean
            if (s11_db < -50.0)
                s11_db = -50.0;

            m_s11Series->append(freq, s11_db);
        }
        break;

    case ChartType::SMITH:
        m_lastFreq = measure->measure_freq;
        m_lastVswr = vswr;
        m_lastRxStr = QString("%1 %2 j%3")
                          .arg(rx.real(), 0, 'f', 1)
                          .arg(rx.imag() >= 0 ? "+" : "-")
                          .arg(std::abs(rx.imag()), 0, 'f', 1);

        emit legendChanged();

        gamma = AAPiCalibrator::gamma_from_z(rx, r0);
        m_smithTracePoints.append(QPointF(gamma.real(), gamma.imag()));

        if (m_smithTracePoints.length() % 20 == 0) {
            QMetaObject::invokeMethod(m_smithChart, "updateSweepTrace",
                                      Qt::QueuedConnection,
                                      Q_ARG(QVariant, QVariant::fromValue(m_smithTracePoints)));
        }
        break;

    default:
        break;
    }

    return AAPI_SUCCESS;
}

void QAAPiPanoramicScanView::handleTabChange(int index)
{
    if (index > 0) {
        updateAxisXRange();
    }
}

void QAAPiPanoramicScanView::handleRXPlotArea(QRectF rect)
{
    updatePlotArea(ChartType::RX, rect);
}

void QAAPiPanoramicScanView::handleVSWRPlotArea(QRectF rect)
{
    updatePlotArea(ChartType::VSWR, rect);
}

void QAAPiPanoramicScanView::handleS11PlotArea(QRectF rect)
{
    updatePlotArea(ChartType::S11, rect);
}

void QAAPiPanoramicScanView::handleSmithPlotArea(QRectF rect)
{
    updatePlotArea(ChartType::SMITH, rect);
}

void QAAPiPanoramicScanView::handleRXScanFast()
{
    m_chartType = ChartType::RX;

    startSweep(SweepPoints::Fast);
}

void QAAPiPanoramicScanView::handleRXScanSlow()
{
    m_chartType = ChartType::RX;

    startSweep(SweepPoints::MaxVisual);
}

void QAAPiPanoramicScanView::handleVSWRScanFast()
{
    m_chartType = ChartType::VSWR;

    startSweep(SweepPoints::Fast);
}

void QAAPiPanoramicScanView::handleVSWRScanSlow()
{
    m_chartType = ChartType::VSWR;

    startSweep(SweepPoints::MaxVisual);
}

void QAAPiPanoramicScanView::handleS11ScanFast()
{
    m_chartType = ChartType::S11;

    startSweep(SweepPoints::Fast);
}

void QAAPiPanoramicScanView::handleS11ScanSlow()
{
    m_chartType = ChartType::S11;

    startSweep(SweepPoints::MaxVisual);
}

void QAAPiPanoramicScanView::handleSmithScanFast()
{
    m_chartType = ChartType::SMITH;

    startSweep(SweepPoints::Fast);
}

void QAAPiPanoramicScanView::handleSmithScanSlow()
{
    m_chartType = ChartType::SMITH;

    startSweep(SweepPoints::MaxVisual);
}

void QAAPiPanoramicScanView::handleRXSnapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("pan_rx", result->image());
}

void QAAPiPanoramicScanView::handleVSWRSnapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("pan_vswr", result->image());
}

void QAAPiPanoramicScanView::handleS11Snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("pan_s11", result->image());
}

void QAAPiPanoramicScanView::handleSmithSnapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("pan_smith", result->image());
}

void QAAPiPanoramicScanView::handleTuneFrequency(TuneDirection dir)
{
    uint32_t stepHz = 0;

    switch (dir) {
    case TUNE_DOWN_SMALL:
    case TUNE_UP_SMALL:
        stepHz = 1000;   // 1 kHz
        break;
    case TUNE_DOWN_MEDIUM:
    case TUNE_UP_MEDIUM:
        stepHz = 10'000;  // 10 kHz
        break;
    case TUNE_DOWN_LARGE:
    case TUNE_UP_LARGE:
        stepHz = 100'000; // 100 kHz
        break;
    }

    // Determine addition or subtraction
    int64_t minFreq = m_minFreq;
    if (dir < 0) {
        minFreq -= stepHz;
    } else {
        minFreq += stepHz;
    }

    // Reuse setter logic to cleanly dispatch all notify & range events
    setMinFreq(static_cast<quint32>(minFreq));
}

void QAAPiPanoramicScanView::handleDirectFreqInput(quint32 freqKHz)
{
    // Convert incoming safe text input from user-facing kHz straight back to physical Hz
    int64_t minFreq = static_cast<int64_t>(freqKHz) * 1000ULL;

    // Reuse setter logic to cleanly dispatch all notify & range events
    setMinFreq(static_cast<quint32>(minFreq));
}

