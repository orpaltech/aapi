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

QAAPIQmlDSPView::QAAPIQmlDSPView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                 AAPIGenerator *gen, QObject *parent)
    : QAAPIQmlView(config, dsp, gen, parent)
    , m_mutex(QMutex::Recursive)
    , m_frequency(3500000)
    , m_tab_index(0)
    , m_v_oscillosc_series(nullptr)
    , m_i_oscillosc_series(nullptr)
    , m_v_spectrum_series(nullptr)
    , m_i_spectrum_series(nullptr)
{
    // Subscribe for DSP events 
    dsp->add_callback(this);
}

QAAPIQmlDSPView::~QAAPIQmlDSPView()
{
}

int QAAPIQmlDSPView::load_view()
{
    // If generator is not in use then set our frequency 
    if( m_generator->is_locked() == false )
    {
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
    m_tab_index = index;
}

void QAAPIQmlDSPView::update_oscilloscope()
{
    if( is_oscilloscope_tab() == false )
        return;

    if( m_v_oscillosc_series == nullptr || 
        m_i_oscillosc_series == nullptr )
        return;

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    int i;

    if( m_v_oscillosc_points.size() > 0 )
    {
        for( i = 0; i < m_v_oscillosc_points.size(); i++ )
        {
            points.append( QPointF( i, m_v_oscillosc_points.at(i) ) );
        }
        m_v_oscillosc_series->replace( points );
        points.clear();
    }

    if( m_i_oscillosc_points.size() > 0 )
    {
        for( i = 0; i < m_i_oscillosc_points.size(); i++ )
        {
            points.append( QPointF( i, m_i_oscillosc_points.at(i) ) );
        }
        m_i_oscillosc_series->replace( points );
    }
}

void QAAPIQmlDSPView::update_spectrum()
{
    if( !is_spectrum_tab() )
        return;

    if( m_v_spectrum_series == nullptr ||
        m_i_spectrum_series == nullptr )
        return;

    QMutexLocker lock( &m_mutex );
    QList<QPointF> points;
    int i, b;

    /* Strip ~20% from left and right */
    uint32_t num_samples = m_config->get_dsp_nsamples();
    uint32_t bin_min = 0,/*num_fft*0.2,*/
            bin_max = num_samples/2-1;/*num_fft*0.8;*/

    if( m_v_spectrum_points.size() > 0 )
    {
        for( i = 0, b = bin_min; b <= bin_max; i++, b++ )
        {
            points.append( QPointF(
                            m_config->get_bin_freq(b),
                            AAPISignalProcessor::mag2db( m_v_spectrum_points.at(i) )
                        ));
        }
        m_v_spectrum_series->replace( points );
        points.clear();
    }

    if( m_i_spectrum_points.size() > 0 )
    {
        for( i = 0, b = bin_min; b <= bin_max; i++, b++ )
        {
            points.append( QPointF(
                            m_config->get_bin_freq(b),
                            AAPISignalProcessor::mag2db( m_i_spectrum_points.at(i) )
                        ));
        }
        m_i_spectrum_series->replace( points );
    }
}

void QAAPIQmlDSPView::setup_spectrum(QLineSeries *v_series, QLineSeries *i_series)
{
    m_v_spectrum_series = v_series;
    m_i_spectrum_series = i_series;

    m_v_spectrum_series->clear();
    m_i_spectrum_series->clear();

    QChart *v_chart = m_v_spectrum_series->chart();
    QChart *i_chart = m_i_spectrum_series->chart();

    QValueAxis *vAxisX = dynamic_cast<QValueAxis*>(v_chart->axisX());
    QValueAxis *vAxisY = dynamic_cast<QValueAxis*>(v_chart->axisY());
    QValueAxis *iAxisX = dynamic_cast<QValueAxis*>(i_chart->axisX());
    QValueAxis *iAxisY = dynamic_cast<QValueAxis*>(i_chart->axisY());

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

    // Setup chart backgrounds etc 
    setup_chart( v_chart );
    setup_chart( i_chart );
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
    m_v_oscillosc_series = v_series;
    m_i_oscillosc_series = i_series;

    m_v_oscillosc_series->clear();
    m_i_oscillosc_series->clear();

    QChart *v_chart = m_v_oscillosc_series->chart();
    QChart *i_chart = m_i_oscillosc_series->chart();

    QAbstractAxis *vAxisX = v_chart->axes(Qt::Horizontal).first();
    QAbstractAxis *vAxisY = v_chart->axes(Qt::Vertical).first();

    vAxisX->setMin(0);
    vAxisX->setMax(m_config->get_dsp_nsamples());
    vAxisY->setMin(-1000);
    vAxisY->setMax(1000);

    QAbstractAxis *iAxisX = i_chart->axes(Qt::Horizontal).first();
    QAbstractAxis *iAxisY = i_chart->axes(Qt::Vertical).first();

    iAxisX->setMin(0);
    iAxisX->setMax(m_config->get_dsp_nsamples());
    iAxisY->setMin(-1000);
    iAxisY->setMax(1000);

    // Setup chart backgrounds etc 
    setup_chart( v_chart );
    setup_chart( i_chart );
}

bool QAAPIQmlDSPView::is_spectrum_tab() const
{
    return m_tab_index == 0;
}

bool QAAPIQmlDSPView::is_oscilloscope_tab() const
{
    return m_tab_index == 1;
}

void QAAPIQmlDSPView::dsp_raw_frame(float **buffers, uint32_t num_buffers, uint32_t buff_size)
{
    if( is_oscilloscope_tab() == false )
        return;

    // do not block audio/dsp thread 
    if( m_mutex.tryLock() )
    {
        QT_TRY
        {
            m_i_oscillosc_points.resize( buff_size );
            m_v_oscillosc_points.resize( buff_size );

            memcpy( m_i_oscillosc_points.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(float) );

            memcpy( m_v_oscillosc_points.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(float) );
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

    // do not block audio/dsp thread 
    if (m_mutex.tryLock())
    {
        QT_TRY
        {
            m_i_spectrum_points.resize( buff_size );
            m_v_spectrum_points.resize( buff_size );

            memcpy( m_i_spectrum_points.data(), buffers[DSP_I_CHANNEL], buff_size * sizeof(float));

            memcpy( m_v_spectrum_points.data(), buffers[DSP_V_CHANNEL], buff_size * sizeof(float));
        }
        QT_CATCH(...)
        {
        }

        m_mutex.unlock();
    }
}
