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
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_tabIndex(0)
    , m_waveformSeriesV(nullptr)
    , m_waveformSeriesI(nullptr)
    , m_spectrumSeriesV(nullptr)
    , m_spectrumSeriesI(nullptr)
{
    // Subscribe for DSP events 
    dsp->addCallback(this);

    // Connect signals and slots
    QObject::connect(this, &QAAPiSignalProcessView::spectrumDataReady,
                     this, &QAAPiSignalProcessView::handleSpectrumDataReady,
                     Qt::QueuedConnection);

    QObject::connect(this, &QAAPiSignalProcessView::waveformDataReady,
                     this, &QAAPiSignalProcessView::handleWaveformDataReady,
                     Qt::QueuedConnection);
}

QAAPiSignalProcessView::~QAAPiSignalProcessView()
{
}

AAPiError QAAPiSignalProcessView::loadView()
{
    return AAPI_SUCCESS;
}

AAPiError QAAPiSignalProcessView::activateView()
{
    uint32_t freq = m_config->get_generator_freq();

    // If generator is not in use then set our frequency
    if (! m_generator->is_locked( )) {
        m_generator->resume( );
        m_generator->set_measure_freq( freq );
    }

    //signal_process_enable( );

    m_spectrumPointsV.resize(m_dsp->getBufferSize());
    m_spectrumPointsI.resize(m_dsp->getBufferSize());
    m_waveformPointsV.resize(m_dsp->getBufferSize());
    m_waveformPointsI.resize(m_dsp->getBufferSize());

    return AAPI_SUCCESS;
}

void QAAPiSignalProcessView::deactivateView()
{
    //signal_process_enable( false );

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

void QAAPiSignalProcessView::destroyView()
{
}

void QAAPiSignalProcessView::handleTabChange(int index)
{
    m_tabIndex = index;
}

double getSignalMax(const QVector<double>* values)
{
    double max = 0;
    for (int i = 0; i < values->size(); i++) {
        double val = std::abs(values->at(i));
        if (val > max)
            max = val;
    }
    return max;
}

void QAAPiSignalProcessView::handleSpectrumDataReady()
{
    static QList<QPointF> pointsV, pointsI;
    qsizetype i;

    signal_process_enable( false);

    QVector<double> *spectrumPointsV = &m_spectrumPointsV;
    QVector<double> *spectrumPointsI = &m_spectrumPointsI;

    pointsV.clear();
    pointsI.clear();
    pointsV.reserve(spectrumPointsV->size());
    pointsI.reserve(spectrumPointsI->size());

    if (!spectrumPointsV->isEmpty()) {

        for (i = 0; i < spectrumPointsV->size(); i++) {
            double mag_db_val = AAPiSignalProcessor::mag2db(spectrumPointsV->at(i));
            double analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsV.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesV->replace(pointsV);
    }

    if (!spectrumPointsI->isEmpty()) {

        for (i = 0; i < spectrumPointsI->size(); i++) {
            double mag_db_val = AAPiSignalProcessor::mag2db(spectrumPointsI->at(i));
            double analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsI.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesI->replace(pointsI);
    }
}

void QAAPiSignalProcessView::handleWaveformDataReady()
{
    static QList<QPointF> pointsV, pointsI;
    qsizetype i;

    signal_process_enable( false);

    QVector<double> *waveformPointsV = &m_waveformPointsV;
    QVector<double> *waveformPointsI = &m_waveformPointsI;

    pointsV.clear();
    pointsI.clear();
    pointsV.reserve(waveformPointsV->size());
    pointsI.reserve(waveformPointsI->size());

    double sample_rate = static_cast<double>(m_config->get_dsp_sample_rate());
    double sample_step_ms = (1.0 / sample_rate) * 1000.0;

    // --- UPDATE V CHANNEL ---
    if (!waveformPointsV->isEmpty()) {
        double max_amplitude = getSignalMax(waveformPointsV);

        for (i = 0; i < waveformPointsV->size(); i++) {
            double current_time_ms = sample_step_ms * i;
            double raw_amplitude = waveformPointsV->at(i);

            // Normalized signal amplitude stream
            pointsV.append(QPointF(current_time_ms, raw_amplitude / max_amplitude));
        }

        m_waveformSeriesV->replace(pointsV);
    }

    // --- UPDATE I CHANNEL ---
    if (!waveformPointsI->isEmpty()) {
        double max_amplitude = getSignalMax(waveformPointsI);

        for (i = 0; i < waveformPointsI->size(); i++) {
            double current_time_ms = static_cast<double>(i) * sample_step_ms;
            double raw_amplitude = waveformPointsI->at(i);

            pointsI.append(QPointF(current_time_ms, raw_amplitude / max_amplitude));
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
    return;

    /*QMutexLocker lock(&m_mutex);
    QList<QPointF> points;
    qsizetype i;

    double sample_rate = static_cast<double>(m_config->get_dsp_sample_rate());
    double sample_step_ms = (1.0 / sample_rate) * 1000.0;

    // --- UPDATE V CHANNEL ---
    if (!m_waveformPointsV.isEmpty()) {
        points.reserve(m_waveformPointsV.size());
        double max_amplitude = getSignalMax(m_waveformPointsV);

        for (i = 0; i < m_waveformPointsV.size(); i++) {
            double current_time_ms = sample_step_ms * i;
            double raw_amplitude = m_waveformPointsV.at(i);

            // Normalized signal amplitude stream
            points.append(QPointF(current_time_ms, raw_amplitude / max_amplitude));
        }

        m_waveformSeriesV->replace(points);
        points.clear();
    }

    // --- UPDATE I CHANNEL ---
    if (!m_waveformPointsI.isEmpty()) {
        points.reserve(m_waveformPointsI.size());
        double max_amplitude = getSignalMax(m_waveformPointsI);

        for (i = 0; i < m_waveformPointsI.size(); i++) {
            double current_time_ms = static_cast<double>(i) * sample_step_ms;
            double raw_amplitude = m_waveformPointsI.at(i);

            points.append(QPointF(current_time_ms, raw_amplitude / max_amplitude));
        }

        m_waveformSeriesI->replace(points);
    }*/
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
    return;

    /*QMutexLocker lock(&m_mutex);
    QList<QPointF> pointsV, pointsI;
    qsizetype i;

    pointsV.reserve(m_spectrumPointsV.size());
    pointsI.reserve(m_spectrumPointsI.size());

    if (!m_spectrumPointsV.isEmpty()) {

        for (i = 0; i < m_spectrumPointsV.size(); i++) {
            double mag_db_val = AAPiSignalProcessor::mag2db(m_spectrumPointsV.at(i));
            double analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsV.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesV->replace(pointsV);
    }

    if (!m_spectrumPointsI.isEmpty()) {

        for (i = 0; i < m_spectrumPointsI.size(); i++) {
            double mag_db_val = AAPiSignalProcessor::mag2db(m_spectrumPointsI.at(i));
            double analytic_freq = m_config->get_dsp_fft_bin_freq(i);
            pointsI.append(QPointF(analytic_freq, mag_db_val));
        }

        m_spectrumSeriesI->replace(pointsI);
    }*/
}

// ignore a few bins because they are in the low frequency range
#define SKIP_LOW_FREQ_BINS   0

void QAAPiSignalProcessView::handleSpectrumSetup(QLineSeries *v_series, QLineSeries *i_series,
                                                 QValueAxis *axisVX, QValueAxis *axisVY,
                                                 QValueAxis *axisIX, QValueAxis *axisIY)
{
    m_spectrumSeriesV = v_series;
    m_spectrumSeriesI = i_series;

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
    // Legacy span: from -120 to 0. Total width = 120.
    // Intended steps = 12 (which equals 13 ticks). 120 / 12 = exactly 10 dB per step.
    axisIY->setRange(-120, 0);
    axisIY->setTickAnchor(0);       // Grid calculations start neatly at 0 dB
    axisIY->setTickInterval(10);    // Clean 10 dB interval steps
    axisIY->setSubTickCount(4);
}

void QAAPiSignalProcessView::handleWaveformSetup(QLineSeries *v_series, QLineSeries *i_series,
                                                 QValueAxis *axisVX, QValueAxis *axisVY,
                                                 QValueAxis *axisIX, QValueAxis *axisIY)
{
    m_waveformSeriesV = v_series;
    m_waveformSeriesI = i_series;

    m_waveformSeriesV->clear();
    m_waveformSeriesI->clear();

    // CALCULATE MAX TIME WINDOW (in milliseconds):
    double sample_rate = static_cast<double>(m_config->get_dsp_sample_rate());
    double num_samples = static_cast<double>(m_config->get_dsp_num_samples());
    double actual_max_ms = (num_samples / sample_rate) * 1000.0;

    double max_time_ms = qFloor(actual_max_ms);

    // --- V CHANNEL CONFIGURATION ---
    axisVX->setRange(0.0, max_time_ms);
    axisVX->setTickAnchor(0.0);
    axisVX->setTickInterval(1.0); // Guarantees explicit lines every 1 ms
    axisVX->setLabelFormat(QStringLiteral("%.0f ms"));

    axisVY->setRange(-1.0, 1.0);

    // --- I CHANNEL CONFIGURATION ---
    axisIX->setRange(0.0, max_time_ms);
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

void QAAPiSignalProcessView::onSignalProcessRaw(double **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isWaveformTab( )) {
        return;
    }

    if (!m_dataRequested)
        return;

    QT_TRY
    {
        QMutexLocker lock(&m_mutex);

        std::copy_n( buffers[DSP_V_CHANNEL], buff_size, m_waveformPointsV.data() );
        std::copy_n( buffers[DSP_I_CHANNEL], buff_size, m_waveformPointsI.data() );

    }
    QT_CATCH(...)
    {
    }

    emit waveformDataReady();
    m_dataRequested = false;
}

void QAAPiSignalProcessView::onSignalProcessFFT(double **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isSpectrumTab( )) {
        return;
    }

    if (!m_dataRequested)
        return;

    QT_TRY
    {
        QMutexLocker lock(&m_mutex);

        std::copy_n( buffers[DSP_V_CHANNEL], buff_size, m_spectrumPointsV.data() );
        std::copy_n( buffers[DSP_I_CHANNEL], buff_size, m_spectrumPointsI.data() );

    }
    QT_CATCH(...)
    {
    }

    emit spectrumDataReady();
    m_dataRequested = false;
}
