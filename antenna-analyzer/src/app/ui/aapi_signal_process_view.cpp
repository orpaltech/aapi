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
    , m_oscilloscSeriesV(nullptr), m_oscilloscSeriesI(nullptr)
    , m_spectrumSeriesV(nullptr), m_spectrumSeriesI(nullptr)
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

    if (m_oscilloscSeriesV == nullptr || m_oscilloscSeriesI == nullptr)
    {
        return;
    }

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    unsigned i;

    if (m_oscilloscPointsV.size() > 0)
    {
        double max = getSignalMax(m_oscilloscPointsV);

        points.reserve(m_oscilloscPointsV.size());

        for (i = 0; i < m_oscilloscPointsV.size(); i++)
        {
            points.append( QPointF(
                                i,
                                1000. * m_oscilloscPointsV.at(i) / max
                            ));
        }
        m_oscilloscSeriesV->replace( points );
        points.clear();
    }

    if (m_oscilloscPointsI.size() > 0)
    {
        double max = getSignalMax(m_oscilloscPointsI);

        points.reserve(m_oscilloscPointsI.size());

        for (i = 0; i < m_oscilloscPointsI.size(); i++)
        {
            points.append( QPointF(
                                i,
                                1000. * m_oscilloscPointsI.at(i) / max
                            ));
        }
        m_oscilloscSeriesI->replace( points );
    }
}


void QAAPiSignalProcessView::update_spectrum()
{
    if (! isSpectrumTab( ))
    {
        return;
    }

    if (m_spectrumSeriesV == nullptr || m_spectrumSeriesI == nullptr)
    {
        return;
    }

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    qsizetype i;


    if( m_spectrumPointsV.size() > 0 )
    {
        points.reserve(m_spectrumPointsV.size());

        for ( i = 0; i < m_spectrumPointsV.size(); i++)
        {
            points.append( QPointF(
                            m_config->get_dsp_fft_bin_freq( i ) * 2,
                            AAPiSignalProcessor::mag2db( m_spectrumPointsV.at(i) )
                        ));
        }
        m_spectrumSeriesV->replace( points );
        points.clear();
    }

    if( m_spectrumPointsI.size() > 0 )
    {
        points.reserve(m_spectrumPointsI.size());

        for ( i = 0; i < m_spectrumPointsI.size(); i++ )
        {
            points.append( QPointF(
                            m_config->get_dsp_fft_bin_freq( i ) * 2,
                            AAPiSignalProcessor::mag2db( m_spectrumPointsI.at(i) )
                        ));
        }
        m_spectrumSeriesI->replace( points );
    }
}

// ignore a few bins because they are in the low frequency range
#define SKIP_LOW_FREQ_BINS   0

void QAAPiSignalProcessView::setup_spectrum(QLineSeries *v_series, QLineSeries *i_series)
{
    m_spectrumSeriesV = v_series;
    m_spectrumSeriesI = i_series;

    m_spectrumSeriesV->clear();
    m_spectrumSeriesI->clear();

    QChart *chartV = m_spectrumSeriesV->chart();
    QChart *chartI = m_spectrumSeriesI->chart();

    QValueAxis *axisVX = qobject_cast<QValueAxis*>(chartV->axes(Qt::Horizontal).first());
    QValueAxis *axisVY = qobject_cast<QValueAxis*>(chartV->axes(Qt::Vertical).first());

    qreal minFreq = 0;//m_config->get_fft_bin_width() * 2 * SKIP_LOW_FREQ_BINS;

    // No need to display points higher than the Nyquist Rate
    // See https://en.wikipedia.org/wiki/Nyquist_frequency
    qreal maxFreq = m_config->get_dsp_sample_rate() / 2;

    axisVX->setRange(minFreq, maxFreq);
    //v_axisX->applyNiceNumbers();

    axisVY->setRange(0, 100);
    axisVY->setTickCount(10);
    axisVY->setMinorTickCount(5);

    QValueAxis *axisIX = qobject_cast<QValueAxis*>(chartI->axes(Qt::Horizontal).first());
    QValueAxis *axisIY = qobject_cast<QValueAxis*>(chartI->axes(Qt::Vertical).first());

    axisIX->setRange(minFreq, maxFreq);
    //i_axisX->applyNiceNumbers();

    axisIY->setRange(0, 100);
    axisIY->setTickCount(10);
    axisIY->setMinorTickCount(5);
}

void QAAPiSignalProcessView::setupChart(QChart *chart)
{
}

void QAAPiSignalProcessView::setup_oscilloscope(QLineSeries *v_series, QLineSeries *i_series)
{
    m_oscilloscSeriesV = v_series;
    m_oscilloscSeriesI = i_series;

    m_oscilloscSeriesV->clear();
    m_oscilloscSeriesI->clear();

    QChart *chartV = m_oscilloscSeriesV->chart();
    QValueAxis *axisVX = qobject_cast<QValueAxis*>(chartV->axes(Qt::Horizontal).first());
    QValueAxis *axisVY = qobject_cast<QValueAxis*>(chartV->axes(Qt::Vertical).first());

    axisVX->setRange(0, m_config->get_dsp_num_samples());

    axisVY->setRange(-1000, 1000);
    axisVY->applyNiceNumbers();

    QChart *chartI = m_oscilloscSeriesI->chart();
    QValueAxis *axisIX = qobject_cast<QValueAxis*>(chartI->axes(Qt::Horizontal).first());
    QValueAxis *axisIY = qobject_cast<QValueAxis*>(chartI->axes(Qt::Vertical).first());

    axisIX->setRange(0, m_config->get_dsp_num_samples());

    axisIY->setRange(-1000, 1000);
    axisIY->applyNiceNumbers();
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
            m_oscilloscPointsV.resize( buff_size );
            m_oscilloscPointsI.resize( buff_size );

            memcpy( m_oscilloscPointsV.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(double) );
            memcpy( m_oscilloscPointsI.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(double) );
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
            m_spectrumPointsV.resize( buff_size );
            m_spectrumPointsI.resize( buff_size );

            memcpy( m_spectrumPointsV.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(double) );
            memcpy( m_spectrumPointsI.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(double) );
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}
