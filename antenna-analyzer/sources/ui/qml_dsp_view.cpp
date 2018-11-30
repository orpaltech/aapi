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

#include "qml_dsp_view.h"
#include <QChart>

#define FFT_BIN_MIN 50
#define FFT_BIN_MAX 150

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlDSPView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlDSPView::QAAPIQmlDSPView(AAPIConfig *config, AAPISignalProcessor *processor,
                                 AAPIGenerator *generator, QObject *parent)
    : QAAPIQmlView(config, processor, generator, parent)
    , m_mutex(QMutex::Recursive)
    , m_frequency(3500000)
    , m_tabIndex(0)
    , m_VSeriesOscillosc(nullptr)
    , m_ISeriesOscillosc(nullptr)
    , m_VSeriesSpectrum(nullptr)
    , m_ISeriesSpectrum(nullptr)
{
    /* Subscribe for DSP events */
    processor->add_callback(this);
}

QAAPIQmlDSPView::~QAAPIQmlDSPView()
{
}

int QAAPIQmlDSPView::load_view()
{
    if( !m_generator->is_locked() )
    {
        /* If generator is not in use then set our frequency */
        m_generator->set_frequency( m_frequency );
    }

    enable_signal_processing();

    return 0;
}

void QAAPIQmlDSPView::destroy_view()
{
    enable_signal_processing(false);
}

void QAAPIQmlDSPView::tab_changed(int index)
{
    m_tabIndex = index;
}

void QAAPIQmlDSPView::update_oscilloscope()
{
    if( !is_oscilloscope_tab() )
        return;

    if( !m_VSeriesOscillosc || !m_ISeriesOscillosc )
        return;

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    int i;

    if( m_VOscilloscPoints.size() > 0 )
    {
        for( i = 0; i < m_VOscilloscPoints.size(); i++ )
        {
            points.append(QPointF( i, m_VOscilloscPoints.at(i) ));
        }
        m_VSeriesOscillosc->replace(points);
        points.clear();
    }

    if( m_IOscilloscPoints.size() > 0 )
    {
        for( i = 0; i < m_IOscilloscPoints.size(); i++ )
        {
            points.append(QPointF( i, m_IOscilloscPoints.at(i) ));
        }
        m_ISeriesOscillosc->replace(points);
    }
}

void QAAPIQmlDSPView::update_spectrum()
{
    if( !is_spectrum_tab() )
        return;

    if( !m_VSeriesSpectrum || !m_ISeriesSpectrum)
        return;

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    int i, b;

    /* Strip ~20% from left and right */
    uint32_t num_samples = m_config->get_dsp_nsamples();
    uint32_t bin_min = 0,/*num_fft*0.2,*/
            bin_max = num_samples/2-1;/*num_fft*0.8;*/

    if( m_VSpectrumPoints.size() > 0 )
    {
        for( i = 0, b = bin_min; b <= bin_max; i++, b++ )
        {
            points.append( QPointF(
                            m_config->get_bin_freq(b),
                            AAPISignalProcessor::mag2db( m_VSpectrumPoints.at(i) )
                        ));
        }
        m_VSeriesSpectrum->replace(points);
        points.clear();
    }

    if( m_ISpectrumPoints.size() > 0 )
    {
        for( i = 0, b = bin_min; b <= bin_max; i++, b++ )
        {
            points.append( QPointF(
                            m_config->get_bin_freq(b),
                            AAPISignalProcessor::mag2db( m_ISpectrumPoints.at(i) )
                        ));
        }
        m_ISeriesSpectrum->replace(points);
    }
}

void QAAPIQmlDSPView::setup_spectrum(QLineSeries *vSeries, QLineSeries *iSeries)
{
    m_VSeriesSpectrum = vSeries;
    m_ISeriesSpectrum = iSeries;

    m_VSeriesSpectrum->clear();
    m_ISeriesSpectrum->clear();

    QChart *vChart = m_VSeriesSpectrum->chart();
    QChart *iChart = m_ISeriesSpectrum->chart();

    QValueAxis *vAxisX = dynamic_cast<QValueAxis*>(vChart->axisX());
    QValueAxis *vAxisY = dynamic_cast<QValueAxis*>(vChart->axisY());
    QValueAxis *iAxisX = dynamic_cast<QValueAxis*>(iChart->axisX());
    QValueAxis *iAxisY = dynamic_cast<QValueAxis*>(iChart->axisY());

    /* Strip ~20% from left and right */
    uint32_t num_samples = m_config->get_dsp_nsamples();
    uint32_t bin_min = 0/*num_fft*0.2*/,
            bin_max = num_samples/2-1/*num_fft*0.8*/;
    uint32_t min_freq = m_config->get_bin_freq(bin_min);
    uint32_t max_freq = m_config->get_bin_freq(bin_max);

    vAxisX->setRange(min_freq, max_freq);
    vAxisX->applyNiceNumbers();

    vAxisY->setRange(0, 80);
    vAxisY->setTickCount(8);
    vAxisY->setMinorTickCount(4);

    iAxisX->setRange(min_freq, max_freq);
    iAxisX->applyNiceNumbers();

    iAxisY->setRange(0, 80);
    iAxisY->setTickCount(8);
    iAxisY->setMinorTickCount(4);

    /* Setup chart backgrounds etc */
    setup_chart(vChart);
    setup_chart(iChart);
}

void QAAPIQmlDSPView::setup_chart(QChart *chart)
{
    chart->setBackgroundVisible();
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundVisible();

    QBrush areaBrush(QRgb(COLOR_CHART_PLOT_AREA));
    /*QLinearGradient areaBrush;
    areaBrush.setStart(QPointF(0,1));
    areaBrush.setFinalStop(QPointF(1,0));
    areaBrush.setColorAt(0, QRgb(0xc4caff));
    areaBrush.setColorAt(1, QRgb(0xc4caff));
    areaBrush.setCoordinateMode(QGradient::ObjectBoundingMode);*/
    chart->setPlotAreaBackgroundBrush(areaBrush);

    QRgb axisColor(COLOR_CHART_AXIS_LINE);
    chart->axisX()->setLinePenColor(axisColor);
    chart->axisY()->setLinePenColor(axisColor);

    QRgb gridColor(COLOR_CHART_GRID_LINE);
    chart->axisX()->setGridLineColor(gridColor);
    chart->axisY()->setGridLineColor(gridColor);

    QRgb minGridColor(COLOR_CHART_MINOR_LINE);
    chart->axisX()->setMinorGridLineColor(minGridColor);
    chart->axisY()->setMinorGridLineColor(minGridColor);
}

void QAAPIQmlDSPView::setup_oscilloscope(QLineSeries *v_series, QLineSeries *i_series)
{
    m_VSeriesOscillosc = v_series;
    m_ISeriesOscillosc = i_series;

    m_VSeriesOscillosc->clear();
    m_ISeriesOscillosc->clear();

    QChart *vChart = m_VSeriesOscillosc->chart();
    QChart *iChart = m_ISeriesOscillosc->chart();

    QAbstractAxis *vAxisX = vChart->axes(Qt::Horizontal).first();
    QAbstractAxis *vAxisY = vChart->axes(Qt::Vertical).first();

    vAxisX->setMin(0);
    vAxisX->setMax(m_config->get_dsp_nsamples());
    vAxisY->setMin(-1000);
    vAxisY->setMax(1000);

    QAbstractAxis *iAxisX = iChart->axes(Qt::Horizontal).first();
    QAbstractAxis *iAxisY = iChart->axes(Qt::Vertical).first();

    iAxisX->setMin(0);
    iAxisX->setMax(m_config->get_dsp_nsamples());
    iAxisY->setMin(-1000);
    iAxisY->setMax(1000);

    /* Setup chart backgrounds etc */
    setup_chart(vChart);
    setup_chart(iChart);
}

bool QAAPIQmlDSPView::is_spectrum_tab() const
{
    return m_tabIndex == 0;
}

bool QAAPIQmlDSPView::is_oscilloscope_tab() const
{
    return m_tabIndex == 1;
}

void QAAPIQmlDSPView::dsp_raw_frame(float **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if( !is_oscilloscope_tab() )
        return;

    /* do not block audio/dsp thread */
    if( m_mutex.tryLock() )
    {
        QT_TRY
        {
            m_IOscilloscPoints.resize( buff_size );
            m_VOscilloscPoints.resize( buff_size );

            memcpy( m_IOscilloscPoints.data(), buffers[DSP_I_CHANNEL],
                    buff_size * sizeof(float) );

            memcpy( m_VOscilloscPoints.data(), buffers[DSP_V_CHANNEL],
                    buff_size * sizeof(float) );
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}

void QAAPIQmlDSPView::dsp_fft_frame(float **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if (!is_spectrum_tab())
        return;

    /* do not block audio/dsp thread */
    if (m_mutex.tryLock())
    {
        QT_TRY
        {
            m_ISpectrumPoints.resize( buff_size );
            m_VSpectrumPoints.resize( buff_size );

            memcpy( m_ISpectrumPoints.data(), buffers[DSP_I_CHANNEL],
                    buff_size * sizeof(float));

            memcpy( m_VSpectrumPoints.data(), buffers[DSP_V_CHANNEL],
                    buff_size * sizeof(float));
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}
