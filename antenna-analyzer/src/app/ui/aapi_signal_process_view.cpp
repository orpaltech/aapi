/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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

#include "aapi_signal_process_view.h"


#define FFT_BIN_MIN 50
#define FFT_BIN_MAX 150

///////////////////////////////////////////////////////////////////////////////
// class QAAPiSignalProcessView
///////////////////////////////////////////////////////////////////////////////

QAAPiSignalProcessView::QAAPiSignalProcessView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, nullptr, parent)
    , m_tabIndex(0)
    , m_waveformSeriesV(nullptr)
    , m_waveformSeriesI(nullptr)
    , m_spectrumSeriesV(nullptr)
    , m_spectrumSeriesI(nullptr)
{
    // Subscribe for DSP events 
    dsp->addCallback(this);

    // Connect signals and slots
    connect(this, &QAAPiSignalProcessView::spectrumDataReady,
            this, &QAAPiSignalProcessView::handleSpectrumDataReady,
            Qt::QueuedConnection);

    connect(this, &QAAPiSignalProcessView::waveformDataReady,
            this, &QAAPiSignalProcessView::handleWaveformDataReady,
            Qt::QueuedConnection);
}

QAAPiSignalProcessView::~QAAPiSignalProcessView()
{
}

AAPiError QAAPiSignalProcessView::onViewLoad()
{
    return AAPI_SUCCESS;
}

AAPiError QAAPiSignalProcessView::onViewActivate()
{
    uint32_t freq = m_config->get_generator_freq();

    // If generator is not in use then set our frequency
    if (! m_generator->is_locked( )) {
        m_generator->resume( );
        m_generator->set_measure_freq( freq );
    }


    m_spectrumPointsV.resize(m_dsp->getBufferSize());
    m_spectrumPointsI.resize(m_dsp->getBufferSize());
    m_waveformPointsV.resize(m_dsp->getBufferSize());
    m_waveformPointsI.resize(m_dsp->getBufferSize());

    return AAPI_SUCCESS;
}

void QAAPiSignalProcessView::onViewDeactivate()
{
    m_generator->suspend( );

    m_spectrumPointsV.resize(0);
    m_spectrumPointsI.resize(0);
    m_waveformPointsV.resize(0);
    m_waveformPointsI.resize(0);

    m_spectrumPointsV.squeeze();
    m_spectrumPointsI.squeeze();
    m_waveformPointsV.squeeze();
    m_waveformPointsI.squeeze();
}

void QAAPiSignalProcessView::onViewDestroy()
{
}

void QAAPiSignalProcessView::handleTabChange(int index)
{
    m_tabIndex = index;
}

AAPiReal getSignalMax(const QVector<AAPiReal>* values)
{
    AAPiReal max = 0;
    for (int i = 0; i < values->size(); i++) {
        AAPiReal val = std::abs(values->at(i));
        if (val > max)
            max = val;
    }
    return max;
}

void QAAPiSignalProcessView::handleSpectrumDataReady()
{
    static QList<QPointF> pointsV, pointsI;
    qsizetype i;

    signal_process_enable( false );

    QVector<AAPiReal> *spectrumPointsV = &m_spectrumPointsV;
    QVector<AAPiReal> *spectrumPointsI = &m_spectrumPointsI;

    pointsV.clear();
    pointsI.clear();
    pointsV.reserve(spectrumPointsV->size());
    pointsI.reserve(spectrumPointsI->size());

    if (!spectrumPointsV->isEmpty()) {

        for (i = 0; i < spectrumPointsV->size(); i++) {
            AAPiReal mag_db_val = AAPiSignalProcessor::mag2db(spectrumPointsV->at(i));
            AAPiReal analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsV.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesV->replace(pointsV);
    }

    if (!spectrumPointsI->isEmpty()) {

        for (i = 0; i < spectrumPointsI->size(); i++) {
            AAPiReal mag_db_val = AAPiSignalProcessor::mag2db(spectrumPointsI->at(i));
            AAPiReal analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsI.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesI->replace(pointsI);
    }
}

void QAAPiSignalProcessView::handleWaveformDataReady()
{
    static QList<QPointF> pointsV, pointsI;
    qsizetype i;

    signal_process_enable( false );

    QVector<AAPiReal> *waveformPointsV = &m_waveformPointsV;
    QVector<AAPiReal> *waveformPointsI = &m_waveformPointsI;

    pointsV.clear();
    pointsI.clear();
    pointsV.reserve(waveformPointsV->size());
    pointsI.reserve(waveformPointsI->size());

    double sampleRate = static_cast<double>(m_config->get_dsp_sample_rate());
    double sampleStepMs = (1.0 / sampleRate) * 1000.0;

    // --- UPDATE V CHANNEL ---
    if (!waveformPointsV->isEmpty()) {
        double amplitudeMax = getSignalMax(waveformPointsV);

        for (i = 0; i < waveformPointsV->size(); i++) {
            double currentTimeMs = sampleStepMs * i;
            double amplitudeRaw = waveformPointsV->at(i);

            // Normalized signal amplitude stream
            pointsV.append(QPointF(currentTimeMs, amplitudeRaw / amplitudeMax));
        }

        m_waveformSeriesV->replace(pointsV);
    }

    // --- UPDATE I CHANNEL ---
    if (!waveformPointsI->isEmpty()) {
        double amplitudeMax = getSignalMax(waveformPointsI);

        for (i = 0; i < waveformPointsI->size(); i++) {
            double currentTimeMs = static_cast<double>(i) * sampleStepMs;
            double amplitudeRaw = waveformPointsI->at(i);

            pointsI.append(QPointF(currentTimeMs, amplitudeRaw / amplitudeMax));
        }

        m_waveformSeriesI->replace(pointsI);
    }
}

void QAAPiSignalProcessView::handleWaveformUpdate()
{
    if (!isWaveformTab()) {
        return;
    }

    if (m_waveformSeriesV == nullptr || m_waveformSeriesI == nullptr) {
        return;
    }

    signal_process_enable( );
    m_dataRequested = true;
}

void QAAPiSignalProcessView::handleSpectrumUpdate()
{
    if (!isSpectrumTab()) {
        return;
    }

    if (m_spectrumSeriesV == nullptr || m_spectrumSeriesI == nullptr) {
        return;
    }

    signal_process_enable( );
    m_dataRequested = true;
}

// ignore a few bins because they are in the low frequency range
#define SKIP_LOW_FREQ_BINS   0

void QAAPiSignalProcessView::handleSpectrumSetup(QLineSeries *seriesV, QLineSeries *seriesI,
                                                 QValueAxis *axisVX, QValueAxis *axisVY,
                                                 QValueAxis *axisIX, QValueAxis *axisIY)
{
    m_spectrumSeriesV = seriesV;
    m_spectrumSeriesI = seriesI;

    m_spectrumSeriesV->clear();
    m_spectrumSeriesI->clear();

    qreal minFreq = 0;
    qreal maxFreq = static_cast<qreal>(m_config->get_dsp_sample_rate()) / 2.0;

    axisVX->setRange(minFreq, maxFreq);
    axisIX->setRange(minFreq, maxFreq);

    // --- V CHANNEL Y-AXIS ---
    // Legacy span: from -110 to 0. Total width = 110.
    // Intended steps = 11 (which equals 12 ticks). 110 / 11 = exactly 10 dB per step.
    axisVY->setRange(-110, 0);
    axisVY->setTickAnchor(0);       // Grid calculations start neatly at 0 dB
    axisVY->setTickInterval(10);    // Clean 10 dB interval steps
    axisVY->setSubTickCount(4);

    // --- I CHANNEL Y-AXIS ---
    // Legacy span: from -110 to 0. Total width = 110.
    // Intended steps = 11 (which equals 12 ticks). 110 / 11 = exactly 10 dB per step.
    axisIY->setRange(-110, 0);
    axisIY->setTickAnchor(0);       // Grid calculations start neatly at 0 dB
    axisIY->setTickInterval(10);    // Clean 10 dB interval steps
    axisIY->setSubTickCount(4);
}

void QAAPiSignalProcessView::handleWaveformSetup(QLineSeries *seriesV, QLineSeries *seriesI,
                                                 QValueAxis *axisVX, QValueAxis *axisVY,
                                                 QValueAxis *axisIX, QValueAxis *axisIY)
{
    m_waveformSeriesV = seriesV;
    m_waveformSeriesI = seriesI;

    m_waveformSeriesV->clear();
    m_waveformSeriesI->clear();

    // CALCULATE MAX TIME WINDOW (in milliseconds):
    double sampleRate = static_cast<double>(m_config->get_dsp_sample_rate());
    double numSamples = static_cast<double>(m_config->get_dsp_num_samples());
    double actualMaxMs = (numSamples / sampleRate) * 1000.0;

    double maxTimeMs = qFloor(actualMaxMs);

    // --- V CHANNEL CONFIGURATION ---
    axisVX->setRange(0.0, maxTimeMs);
    axisVX->setTickAnchor(0.0);
    axisVX->setTickInterval(1.0); // Guarantees explicit lines every 1 ms
    axisVX->setLabelFormat(QStringLiteral("%.0f ms"));

    axisVY->setRange(-1.0, 1.0);

    // --- I CHANNEL CONFIGURATION ---
    axisIX->setRange(0.0, maxTimeMs);
    axisIX->setTickAnchor(0.0);
    axisIX->setTickInterval(1.0); // Explicit 1 ms tracking steps
    axisIX->setLabelFormat(QStringLiteral("%.0f ms"));

    axisIY->setRange(-1.0, 1.0);
}

bool QAAPiSignalProcessView::isSpectrumTab() const
{
    return m_tabIndex == 0;
}

bool QAAPiSignalProcessView::isWaveformTab() const
{
    return m_tabIndex == 1;
}

void QAAPiSignalProcessView::onSignalProcessRaw(AAPiReal **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isWaveformTab( )) {
        return;
    }

    if (!m_dataRequested)
        return;

    std::copy_n( buffers[DSP_V_CHANNEL], buff_size, m_waveformPointsV.data() );
    std::copy_n( buffers[DSP_I_CHANNEL], buff_size, m_waveformPointsI.data() );

    emit waveformDataReady();
    m_dataRequested = false;
}

void QAAPiSignalProcessView::onSignalProcessFFT(AAPiReal **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isSpectrumTab( )) {
        return;
    }

    if (!m_dataRequested)
        return;

    std::copy_n( buffers[DSP_V_CHANNEL], buff_size, m_spectrumPointsV.data() );
    std::copy_n( buffers[DSP_I_CHANNEL], buff_size, m_spectrumPointsI.data() );

    emit spectrumDataReady();
    m_dataRequested = false;
}
