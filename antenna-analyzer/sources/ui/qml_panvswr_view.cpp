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

#include <QChart>
#include "qml_panvswr_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlPanVSWRView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlPanVSWRView::QAAPIQmlPanVSWRView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                         AAPIGenerator *gen, AAPICalibrator *calibrator,
                                         QObject *parent)
    : QAAPIQmlView(config, dsp, gen, parent)
{
    m_calibrator = calibrator;
    AAPI_ADDREF(m_calibrator);

    // Subscribe for DSP events 
    dsp->add_callback(this);
}

QAAPIQmlPanVSWRView::~QAAPIQmlPanVSWRView()
{
    AAPI_DISPOSE(m_calibrator);
}

void QAAPIQmlPanVSWRView::rx_setup(QLineSeries *r_series, QLineSeries *x_series)
{
    m_rx_series[0] = r_series;
    m_rx_series[1] = x_series;

    QChart *chart = r_series->chart();
    chart->setBackgroundVisible();
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundVisible();

    QRgb axisColor(COLOR_CHART_AXIS_LINE);
    chart->axisX()->setLinePenColor(axisColor);
    chart->axisY()->setLinePenColor(axisColor);

    QRgb gridColor(COLOR_CHART_GRID_LINE);
    chart->axisX()->setGridLineColor(gridColor);
    chart->axisY()->setGridLineColor(gridColor);

    QRgb minGridColor(COLOR_CHART_MINOR_LINE);
    chart->axisX()->setMinorGridLineColor(minGridColor);
    chart->axisY()->setMinorGridLineColor(minGridColor);

    unsigned int freq_min = get_freq_start();
    chart->axisX()->setMin(freq_min);
    chart->axisX()->setMax(freq_min + m_span);
    chart->axisY()->setMin(-500);
    chart->axisY()->setMax(500);

    set_plot_bgnd(chart);
}

void QAAPIQmlPanVSWRView::vswr_setup(QLineSeries *series)
{
    m_vswr_series = series;

    QChart *chart = series->chart();
    chart->setBackgroundVisible();
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundVisible();

    QRgb axisColor(COLOR_CHART_AXIS_LINE);
    chart->axisX()->setLinePenColor(axisColor);
    chart->axisY()->setLinePenColor(axisColor);

    QRgb gridColor(COLOR_CHART_GRID_LINE);
    chart->axisX()->setGridLineColor(gridColor);
    chart->axisY()->setGridLineColor(gridColor);

    QRgb minGridColor(COLOR_CHART_MINOR_LINE);
    chart->axisX()->setMinorGridLineColor(minGridColor);
    chart->axisY()->setMinorGridLineColor(minGridColor);

    uint32_t freq_min = get_freq_start();
    chart->axisX()->setMin(freq_min);
    chart->axisX()->setMax(freq_min + m_span);
    chart->axisY()->setMin(0);
    chart->axisY()->setMax(20);

    set_plot_bgnd(chart);
}

void QAAPIQmlPanVSWRView::smith_setup(QObject *chart)
{
    m_smith_chart = chart;
}

void QAAPIQmlPanVSWRView::set_plot_bgnd(QChart *chart)
{
    QRectF plotArea = chart->plotArea();
    QColor color( QRgb( COLOR_CHART_PLOT_AREA ) );

    /* setup background */
    QImage texture( plotArea.width(), plotArea.height(), QImage::Format_ARGB32_Premultiplied );
    texture.fill( Qt::transparent );

    QPainter painter( &texture );
    painter.fillRect( 0,0, plotArea.width(),plotArea.height(), color );

    uint32_t freq_min = get_freq_start();
    uint32_t freq_max = freq_min + m_span;
    uint32_t num_bands = AAPIConfig::get_num_ham_bands();

    for ( uint i = 0; i < num_bands; i++ )
    {
        AAPIRadioBand &band = AAPIConfig::get_ham_bands()[i];
        if ( band.hi > freq_min && band.lo < freq_max )
        {
            uint32_t band_min = qMax(freq_min, band.lo);
            uint32_t band_max = qMin(band.hi, freq_max);
            uint32_t band_width = band_max - band_min;
            uint32_t plot_band_width = static_cast<uint32_t> (( plotArea.width() / m_span ) * band_width );

            if ( plot_band_width > 3 ) /* hide if band is too narrow*/
            {
                uint32_t plot_band_min = static_cast<uint32_t> (( plotArea.width() / m_span ) * ( band_min - freq_min ));
                QRectF rect( plot_band_min, 0, plot_band_width, plotArea.height() );
                painter.fillRect( rect, color.darker( 120 ) );
            }
        }
    }
    painter.end();

    QBrush areaBrush(texture);
    chart->setPlotAreaBackgroundBrush(areaBrush);
}

void QAAPIQmlPanVSWRView::update_plot_area(ChartType chart_type, QRectF rect)
{
    m_plot_area[chart_type] = QSize( rect.width(), rect.height() );

    switch ( chart_type )
    {
    case CHART_RX:
        if ( m_rx_series[0] != nullptr )
        {
            set_plot_bgnd( m_rx_series[0]->chart() );
        }
        break;

    case CHART_VSWR:
        if ( m_vswr_series != nullptr )
        {
            set_plot_bgnd( m_vswr_series->chart() );
        }
        break;

    case CHART_SMITH:
        break;

    default:
        break;
    }
}

uint32_t QAAPIQmlPanVSWRView::get_freq_start() const
{
    uint32_t    freq_min;

    if( m_config->get_pan_is_center_freq() )
    {
        freq_min = m_freq1 - m_span/2;
    }
    else
    {
        freq_min = m_freq1;
    }
    return freq_min;
}

int QAAPIQmlPanVSWRView::start_scan(bool fast)
{
    uint32_t        freq, i, freq_step, points, num_scans;
    uint32_t        freq_min    = get_freq_start();
    uint32_t        band_span   = this->m_span;
    QAbstractAxis   *axis;
    AAPIMeasureList measure_steps;
    int             ret;

    num_scans = qMin( m_config->get_pan_n_scans(), AA_MEASURE_MAX_SCANS );

    freq_min *= 1000;
    band_span *= 1000;

    /* Find how many points to scan */
    points = qMin( ( fast ? 100 : 800 ), m_plot_area[m_chart_type].width() );

    /* Prepare scan steps */
    for( i = 0, freq_step = band_span/points; i <= points; i++ )
    {
        freq = freq_min + i * freq_step;

        AAPtr<AAPIMeasure> measure( AAPIMeasure::create( m_config, m_calibrator, this, freq, true, true, num_scans, false) );
        measure_steps.push_back( measure );
    }

    switch (m_chart_type)
    {
    case CHART_VSWR:
        m_vswr_series->clear();   /* Clear chart series */
        axis = m_vswr_series->chart()->axisY();
        axis->setMin(0);
        axis->setMax(20);
        break;

    case CHART_RX:
        m_rx_series[0]->clear();      /* Clear chart series */
        m_rx_series[1]->clear();
        axis = m_rx_series[0]->chart()->axisY();
        axis->setMin(-500);
        axis->setMax(500);
        break;

    case CHART_SMITH:
        QMetaObject::invokeMethod( m_smith_chart, "clear" );    /* clear plot area */
        break;

    case CHART_S11:
        // TODO: clear plot area
        break;
    }


    /* Start measurement sequence */
    ret = start_measure( measure_steps );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    /* Notify UI that scan has started */
    emit scanStarted( measure_steps.length() );

    return 0;
}

int QAAPIQmlPanVSWRView::get_params()
{
    /* Obtain frequency and span from configuration */
    m_freq1 = m_config->get_pan_freq1();
    m_span = m_config->get_pan_span();

    /* Default to VSWR chart */
    m_chart_type = CHART_VSWR;

    /* Flag shows if we need to update configuration */
    bool update_config = false;

    /* Validate frequency*/
    if (m_freq1 != 0 &&
            m_freq1 >= get_freq_min()/1000 &&
            m_freq1 <= get_freq_max()/1000 &&
            (m_freq1 % 100) == 0)
    {
        /* Valid frequency */

        /* Now validate span value */
        if (m_span >= SPAN_200 && m_span <= SPAN_40M)
        {
            /* Valid span*/
        }
        else
        {
            /* Reset span to default */
            m_span = SPAN_800;
            update_config = true;
            m_config->set_pan_span(m_span);
        }
    }
    else
    {
        // Reset frequency and span to defaults 
        m_freq1 = 14000;
        m_span = SPAN_800;
        update_config = true;
        m_config->set_pan_freq1( m_freq1 );
        m_config->set_pan_span( m_span);
    }

    if (update_config)
    {
        /* Update configuration */
        int ret = m_config->flush();
        if (AAPI_FAILED(ret))
        {
            set_error_message("Failed to update configuration");
            return ret;
        }
    }
    return 0;
}

int QAAPIQmlPanVSWRView::load_view()
{
    return get_params();
}

void QAAPIQmlPanVSWRView::destroy_view()
{
    m_rx_series[0] = nullptr;
    m_rx_series[1] = nullptr;
    m_vswr_series = nullptr;
    m_smith_chart = nullptr;
}

void QAAPIQmlPanVSWRView::update_axis_range()
{
    qreal min, max;
    QAbstractAxis *axis;
    int i, n;

    switch (m_chart_type)
    {
    case CHART_RX:
        min = -1; max = 1;
        for (n = 0; n < 2; n++)
        {
            for (i = 0; i < m_rx_series[n]->points().size(); i++)
            {
                const QPointF& pt = m_rx_series[n]->points().at(i);
                if (pt.y() > max)
                    max = pt.y();
                else if (pt.y() < min)
                    min = pt.y();
            }
        }
        max = std::ceil(max + std::abs(max)*0.1);
        min = std::floor(min - std::abs(min)*0.1);

        axis = m_rx_series[0]->chart()->axisY();
        axis->setMin(min);
        axis->setMax(max);
        break;

    case CHART_VSWR:
        min = 0; max = 1;
        for (i = 0; i < m_vswr_series->points().size(); i++)
        {
            const QPointF& pt = m_vswr_series->points().at(i);
            if (pt.y() > max)
                max = pt.y();
        }
        max = std::ceil(max + max*0.1);

        axis = m_vswr_series->chart()->axisY();
        axis->setMin(min);
        axis->setMax(max);
        break;

    default:
        break;
    }

}

int QAAPIQmlPanVSWRView::on_measure_finished(AAPIMeasure *measure)
{
    float               freq, vswr;
    std::complex<float> rx, gamma;

    if ( measure == nullptr )
    {
        emit scanComplete();

        update_axis_range();
    }
    else
    {
        if( measure->is_signal_low() )
        {
            // Hardware problem 
            emit scanNoSignal();
            return 1;
        }

        // update chart series
        rx = measure->Rx;
        vswr = measure->vswr;
        freq = measure->Frequency / 1000;

        switch( m_chart_type )
        {
        case CHART_RX:
            m_rx_series[0]->append( freq, rx.real() );
            m_rx_series[1]->append( freq, rx.imag() );
            break;

        case CHART_VSWR:
            m_vswr_series->append( freq, vswr );
            break;

        case CHART_SMITH:
            gamma = AAPICalibrator::gamma_from_z( rx, m_config->get_base_r0() );

            QMetaObject::invokeMethod( m_smith_chart, "append",
                                      Q_ARG(QVariant, QPointF(gamma.real(), gamma.imag())));
            break;
        }
    }

    return 0;
}

void QAAPIQmlPanVSWRView::tab_changed(int index)
{
}

void QAAPIQmlPanVSWRView::rx_plot_area(QRectF rect)
{
    update_plot_area(CHART_RX, rect);
}

void QAAPIQmlPanVSWRView::vswr_plot_area(QRectF rect)
{
    update_plot_area(CHART_VSWR, rect);
}

void QAAPIQmlPanVSWRView::smith_plot_area(QRectF rect)
{
    update_plot_area(CHART_SMITH, rect);
}

void QAAPIQmlPanVSWRView::rx_fast_scan()
{
    m_chart_type = CHART_RX;

    start_scan(true);
}

void QAAPIQmlPanVSWRView::rx_slow_scan()
{
    m_chart_type = CHART_RX;

    start_scan(false);
}

void QAAPIQmlPanVSWRView::vswr_fast_scan()
{
    m_chart_type = CHART_VSWR;

    start_scan(true);
}

void QAAPIQmlPanVSWRView::vswr_slow_scan()
{
    m_chart_type = CHART_VSWR;

    start_scan(false);
}

void QAAPIQmlPanVSWRView::smith_fast_scan()
{
    m_chart_type = CHART_SMITH;

    start_scan(true);
}

void QAAPIQmlPanVSWRView::smith_slow_scan()
{
    m_chart_type = CHART_SMITH;

    start_scan(false);
}

void QAAPIQmlPanVSWRView::rx_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_RX", result->image());
}

void QAAPIQmlPanVSWRView::vswr_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_VSWR", result->image());
}

void QAAPIQmlPanVSWRView::smith_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_SMITH", result->image());
}
