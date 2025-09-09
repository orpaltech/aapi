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

#include "aapi_signal_process_view.h"


#define FFT_BIN_MIN 50
#define FFT_BIN_MAX 150

///////////////////////////////////////////////////////////////////////////////
// class QAAPiSignalProcessView
///////////////////////////////////////////////////////////////////////////////

QAAPiSignalProcessView::QAAPiSignalProcessView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_frequency(config->get_generator_freq( ))
    , m_tabIndex(0)
    , m_vOscilloscSeries(nullptr)
    , m_iOscilloscSeries(nullptr)
    , m_vSpectrumSeries(nullptr)
    , m_iSpectrumSeries(nullptr)
{
    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiSignalProcessView::~QAAPiSignalProcessView()
{
}

int QAAPiSignalProcessView::load_view()
{
    // If generator is not in use then set our frequency
    if (! m_generator->is_locked( ))
    {
        m_generator->resume( );

        m_generator->set_measure_freq( m_frequency );
    }

    enableSignalProcessing( );

    return 0;
}

void QAAPiSignalProcessView::destroy_view()
{
    enableSignalProcessing( false );

    m_generator->suspend( );
}

void QAAPiSignalProcessView::tab_changed(int index)
{
    m_tabIndex = index;
}

double getSignalMax(const QVector<double>& values)
{
    double val, max = 0;

    for (int i = 0; i < values.size(); i++)
    {
        val = std::abs(values.at(i));
        if (val > max)
            max = val;
    }

    return max;
}

void QAAPiSignalProcessView::update_oscilloscope()
{
    if (! isOscilloscopeTab( ))
    {
        return;
    }

    if (m_vOscilloscSeries == nullptr ||
        m_iOscilloscSeries == nullptr)
    {
        return;
    }

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    unsigned i;

    if (m_vOscilloscPoints.size() > 0)
    {
        double max = getSignalMax(m_vOscilloscPoints);

        for (i = 0; i < m_vOscilloscPoints.size(); i++)
        {
            points.append( QPointF(
                                i,
                                1000. * m_vOscilloscPoints.at(i) / max
                            ));
        }
        m_vOscilloscSeries->replace( points );
        points.clear();
    }

    if (m_iOscilloscPoints.size() > 0)
    {
        double max = getSignalMax(m_iOscilloscPoints);

        for (i = 0; i < m_iOscilloscPoints.size(); i++)
        {
            points.append( QPointF(
                                i,
                                1000. * m_iOscilloscPoints.at(i) / max
                            ));
        }
        m_iOscilloscSeries->replace( points );
    }
}

// ignore a few bins because they are in the low frequency range
#define SKIP_LOW_FREQ_BINS   0

void QAAPiSignalProcessView::update_spectrum()
{
    if (! isSpectrumTab( ))
    {
        return;
    }

    if (m_vSpectrumSeries == nullptr ||
        m_iSpectrumSeries == nullptr)
    {
        return;
    }

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    qsizetype i;


    if( m_vSpectrumPoints.size() > 0 )
    {
        for ( i = 0; i < m_vSpectrumPoints.size(); i++)
        {
            points.append( QPointF(
                            m_config->get_dsp_fft_bin_freq( i ) * 2,
                            AAPiSignalProcessor::mag2db( m_vSpectrumPoints.at(i) )
                        ));
        }
        m_vSpectrumSeries->replace( points );
        points.clear();
    }

    if( m_iSpectrumPoints.size() > 0 )
    {
        for ( i = 0; i < m_iSpectrumPoints.size(); i++ )
        {
            points.append( QPointF(
                            m_config->get_dsp_fft_bin_freq( i ) * 2,
                            AAPiSignalProcessor::mag2db( m_iSpectrumPoints.at(i) )
                        ));
        }
        m_iSpectrumSeries->replace( points );
    }
}

void QAAPiSignalProcessView::setup_spectrum(QLineSeries *v_series, QLineSeries *i_series)
{
    m_vSpectrumSeries = v_series;
    m_iSpectrumSeries = i_series;

    m_vSpectrumSeries->clear();
    m_iSpectrumSeries->clear();

    QChart *v_chart = m_vSpectrumSeries->chart();
    QChart *i_chart = m_iSpectrumSeries->chart();

    QValueAxis *v_axisX = qobject_cast<QValueAxis*>(v_chart->axes(Qt::Horizontal).first());
    QValueAxis *v_axisY = qobject_cast<QValueAxis*>(v_chart->axes(Qt::Vertical).first());

    qreal min_freq = 0;//m_config->get_fft_bin_width() * 2 * SKIP_LOW_FREQ_BINS;
    // No need to display points higher than the Nyquist Rate
    // See https://en.wikipedia.org/wiki/Nyquist_frequency
    qreal max_freq = m_config->get_dsp_sample_rate() / 2;

    v_axisX->setRange(min_freq, max_freq);
    //v_axisX->applyNiceNumbers();

    v_axisY->setRange(0, 100);
    v_axisY->setTickCount(10);
    v_axisY->setMinorTickCount(5);

    QValueAxis *i_axisX = qobject_cast<QValueAxis*>(i_chart->axes(Qt::Horizontal).first());
    QValueAxis *i_axisY = qobject_cast<QValueAxis*>(i_chart->axes(Qt::Vertical).first());

    i_axisX->setRange(min_freq, max_freq);
    //i_axisX->applyNiceNumbers();

    i_axisY->setRange(0, 100);
    i_axisY->setTickCount(10);
    i_axisY->setMinorTickCount(5);
}

void QAAPiSignalProcessView::setupChart(QChart *chart)
{
}

void QAAPiSignalProcessView::setup_oscilloscope(QLineSeries *v_series, QLineSeries *i_series)
{
    m_vOscilloscSeries = v_series;
    m_iOscilloscSeries = i_series;

    m_vOscilloscSeries->clear();
    m_iOscilloscSeries->clear();

    QChart *v_chart = m_vOscilloscSeries->chart();
    QChart *i_chart = m_iOscilloscSeries->chart();

    QValueAxis *v_axisX = qobject_cast<QValueAxis*>(v_chart->axes(Qt::Horizontal).first());
    QValueAxis *v_axisY = qobject_cast<QValueAxis*>(v_chart->axes(Qt::Vertical).first());

    v_axisX->setRange(0, m_config->get_dsp_num_samples());

    v_axisY->setRange(-1000, 1000);
    v_axisY->applyNiceNumbers();

    QValueAxis *i_axisX = qobject_cast<QValueAxis*>(i_chart->axes(Qt::Horizontal).first());
    QValueAxis *i_axisY = qobject_cast<QValueAxis*>(i_chart->axes(Qt::Vertical).first());

    i_axisX->setRange(0, m_config->get_dsp_num_samples());

    i_axisY->setRange(-1000, 1000);
    i_axisY->applyNiceNumbers();
}

bool QAAPiSignalProcessView::isSpectrumTab() const
{
    return m_tabIndex == 0;
}

bool QAAPiSignalProcessView::isOscilloscopeTab() const
{
    return m_tabIndex == 1;
}

void QAAPiSignalProcessView::onSignalProcessRaw(double **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isOscilloscopeTab( ))
    {
        return;
    }

    // do not block audio/dsp thread 
    if (m_mutex.tryLock())
    {
        QT_TRY
        {
            m_vOscilloscPoints.resize( buff_size );
            m_iOscilloscPoints.resize( buff_size );

            memcpy( m_vOscilloscPoints.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(double) );
            memcpy( m_iOscilloscPoints.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(double) );
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}

void QAAPiSignalProcessView::onSignalProcessFFT(double **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (! isSpectrumTab( ))
    {
        return;
    }

    // do not block audio/dsp thread 
    if (m_mutex.tryLock())
    {
        QT_TRY
        {
            m_vSpectrumPoints.resize( buff_size );
            m_iSpectrumPoints.resize( buff_size );

            memcpy( m_vSpectrumPoints.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(double) );
            memcpy( m_iSpectrumPoints.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(double) );
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}
