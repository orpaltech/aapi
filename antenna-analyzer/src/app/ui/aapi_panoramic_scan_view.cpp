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

///////////////////////////////////////////////////////////////////////////////
// class QAAPiPanoramicScanView
///////////////////////////////////////////////////////////////////////////////

QAAPiPanoramicScanView::QAAPiPanoramicScanView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                               AAPiGenerator *gen, AAPiCalibrator *cal,
                                               QAAPiBaseStyle *style, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_style(style)
    , m_calibrator(cal)
{
    AAPI_ADDREF(m_calibrator);

    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiPanoramicScanView::~QAAPiPanoramicScanView()
{
    AAPI_DISPOSE(m_calibrator);
}

void QAAPiPanoramicScanView::rx_setup(QLineSeries *r_series, QLineSeries *x_series)
{
    m_rxSeries[0] = r_series;
    m_rxSeries[1] = x_series;

    QChart *chart = r_series->chart();
    /*chart->setBackgroundVisible();
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundVisible();*/

    QAbstractAxis *axisX = chart->axes(Qt::Horizontal).first();
    QAbstractAxis *axisY = chart->axes(Qt::Vertical).first();

    /*QRgb axisColor(COLOR_CHART_AXIS_LINE);
    axisX->setLinePenColor(axisColor);
    axisY->setLinePenColor(axisColor);

    QRgb gridColor(COLOR_CHART_GRID_LINE);
    axisX->setGridLineColor(gridColor);
    axisY->setGridLineColor(gridColor);

    QRgb minGridColor(COLOR_CHART_MINOR_LINE);
    axisX->setMinorGridLineColor(minGridColor);
    axisY->setMinorGridLineColor(minGridColor);
*/

    unsigned int freq_min = getFreqStart();
    axisX->setMin(freq_min);
    axisX->setMax(freq_min + m_span);
    axisY->setMin(-500);
    axisY->setMax(500);

    setPlotBkgnd( chart );
}

void QAAPiPanoramicScanView::vswr_setup(QLineSeries *series)
{
    m_vswrSeries = series;

    QChart *chart = series->chart();
    /*chart->setBackgroundVisible();
    chart->setBackgroundRoundness(0);
    chart->setPlotAreaBackgroundVisible();*/

    QAbstractAxis *axisX = chart->axes(Qt::Horizontal).first();
    QAbstractAxis *axisY = chart->axes(Qt::Vertical).first();

    /*QRgb axisColor(COLOR_CHART_AXIS_LINE);
    axisX->setLinePenColor(axisColor);
    axisY->setLinePenColor(axisColor);

    QRgb gridColor(COLOR_CHART_GRID_LINE);
    axisX->setGridLineColor(gridColor);
    axisY->setGridLineColor(gridColor);

    QRgb minGridColor(COLOR_CHART_MINOR_LINE);
    axisX->setMinorGridLineColor(minGridColor);
    axisY->setMinorGridLineColor(minGridColor);
*/

    uint32_t freq_min = getFreqStart();
    axisX->setMin(freq_min);
    axisX->setMax(freq_min + m_span);
    axisY->setMin(0);
    axisY->setMax(20);

    setPlotBkgnd( chart );
}

void QAAPiPanoramicScanView::smith_setup(QObject *chart)
{
    m_smithChart = chart;
}

void QAAPiPanoramicScanView::setPlotBkgnd(QChart *chart)
{
    qDebug() << "setting plot aread background";

    QRectF plotArea = chart->plotArea();
    QColor color = m_style->getChart()->m_plotAreaColor;

    /* setup background */
    QImage texture( plotArea.width(), plotArea.height(), QImage::Format_ARGB32_Premultiplied );
    texture.fill( Qt::transparent );

    QPainter painter( &texture );
    painter.fillRect( 0,0, plotArea.width(),plotArea.height(), color );

    uint32_t freq_min = getFreqStart();
    uint32_t freq_max = freq_min + m_span;
    uint32_t num_bands = AAPiConfig::get_num_ham_bands();

    for ( uint i = 0; i < num_bands; i++ )
    {
        AAPiRadioBand &band = AAPiConfig::get_ham_bands()[i];
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

void QAAPiPanoramicScanView::updatePlotArea(ChartType chart_type, QRectF rect)
{
    m_plotArea[chart_type] = QSize( rect.width(), rect.height() );

    switch ( chart_type )
    {
    case CHART_RX:
        if ( m_rxSeries[0] != nullptr )
        {
            setPlotBkgnd( m_rxSeries[0]->chart() );
        }
        break;

    case CHART_VSWR:
        if (m_vswrSeries != nullptr)
        {
            setPlotBkgnd( m_vswrSeries->chart() );
        }
        break;

    case CHART_SMITH:
        break;

    default:
        break;
    }
}

uint32_t QAAPiPanoramicScanView::getFreqStart() const
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

int QAAPiPanoramicScanView::startScan(bool fast)
{
    AAPiMeasureTaskList measure_steps;
    uint32_t            freq, i, freq_step, points, num_scans;
    uint32_t            freq_min    = getFreqStart();
    uint32_t            band_span   = this->m_span;
    QAbstractAxis       *axis;
    int                 ret;

    num_scans = qMin( m_config->get_pan_n_scans(), AAPI_MEASURE_MAX_SCANS );

    freq_min *= 1000;
    band_span *= 1000;

    /* Find how many points to scan */
    points = qMin( ( fast ? 100 : 800 ), m_plotArea[m_chartType].width() );

    /* Prepare scan steps */
    for (i = 0, freq_step = band_span/points; i <= points; i++)
    {
        freq = freq_min + i * freq_step;

        AAPiPtr<AAPiMeasureTask> measure(
            AAPiMeasureTask::create( m_config, m_calibrator, this, freq, true, true, num_scans, false )
        );
        measure_steps.push_back( measure );
    }

    switch (m_chartType)
    {
    case CHART_VSWR:
        m_vswrSeries->clear();   /* Clear chart series */
        axis = m_vswrSeries->chart()->axes(Qt::Vertical).first();
        axis->setMin(0);
        axis->setMax(20);
        break;

    case CHART_RX:
        m_rxSeries[0]->clear();      /* Clear chart series */
        m_rxSeries[1]->clear();
        axis = m_rxSeries[0]->chart()->axes(Qt::Vertical).first();
        axis->setMin(-500);
        axis->setMax(500);
        break;

    case CHART_SMITH:
        QMetaObject::invokeMethod( m_smithChart, "clear" );    /* clear plot area */
        break;

    case CHART_S11:
        // TODO: clear plot area
        break;
    }


    /* Start measurement sequence */
    ret = startMeasure( measure_steps );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    /* Notify UI that scan has started */
    emit scanStarted( measure_steps.length() );

    return 0;
}

int QAAPiPanoramicScanView::getConfigParams()
{
    /* Obtain frequency and span from configuration */
    m_freq1 = m_config->get_pan_freq1();
    m_span = m_config->get_pan_span();

    /* Default to VSWR chart */
    m_chartType = CHART_VSWR;

    /* Flag shows if we need to update configuration */
    bool updateConfig = false;

    /* Validate frequency*/
    if (isValidFreq( m_freq1 ))
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
            updateConfig = true;
            m_config->set_pan_span(m_span);
        }
    }
    else
    {
        // Reset frequency and span to defaults 
        m_freq1 = 14000;
        m_span = SPAN_800;
        updateConfig = true;
        m_config->set_pan_freq1( m_freq1 );
        m_config->set_pan_span( m_span);
    }

    if (updateConfig)
    {
        /* Update configuration */
        int ret = m_config->flush();
        if (AAPI_FAILED( ret ))
        {
            setErrorMessage("Failed to update configuration");
            return ret;
        }
    }

    return 0;
}

int QAAPiPanoramicScanView::load_view()
{
    return getConfigParams();
}

void QAAPiPanoramicScanView::destroy_view()
{
    m_rxSeries[0] = nullptr;
    m_rxSeries[1] = nullptr;
    m_vswrSeries = nullptr;
    m_smithChart = nullptr;
}

void QAAPiPanoramicScanView::updateAxisRange()
{
    qreal min, max;
    QAbstractAxis *axis;
    int i, n;

    switch (m_chartType)
    {
    case CHART_RX:
        min = -1; max = 1;
        for (n = 0; n < 2; n++)
        {
            for (i = 0; i < m_rxSeries[n]->points().size(); i++)
            {
                const QPointF& pt = m_rxSeries[n]->points().at(i);
                if (pt.y() > max)
                    max = pt.y();
                else if (pt.y() < min)
                    min = pt.y();
            }
        }
        max = std::ceil(max + std::abs(max)*0.1);
        min = std::floor(min - std::abs(min)*0.1);

        axis = m_rxSeries[0]->chart()->axes(Qt::Vertical).first();
        axis->setMin(min);
        axis->setMax(max);
        break;

    case CHART_VSWR:
        min = 0; max = 1;
        for (i = 0; i < m_vswrSeries->points().size(); i++)
        {
            const QPointF& pt = m_vswrSeries->points().at(i);
            if (pt.y() > max)
                max = pt.y();
        }
        max = std::ceil(max + max*0.1);

        axis = m_vswrSeries->chart()->axes(Qt::Vertical).first();
        axis->setMin(min);
        axis->setMax(max);
        break;

    default:
        break;
    }

}

int QAAPiPanoramicScanView::onViewMeasureFinished(AAPiMeasureTask *measure)
{
    double          freq, vswr;
    AAPiComplex     rx, gamma;

    if (measure == nullptr)
    {
        emit scanComplete();

        updateAxisRange();
    }
    else
    {
        if (measure->is_low_signal())
        {
            // Hardware problem 
            emit scanNoSignal();

            return AAPI_E_FAILURE;
        }

        // update chart series
        rx = measure->Rx;
        vswr = measure->vswr;
        freq = measure->measure_freq / 1000;

        switch (m_chartType)
        {
        case CHART_RX:
            m_rxSeries[0]->append( freq, rx.real() );
            m_rxSeries[1]->append( freq, rx.imag() );
            break;

        case CHART_VSWR:
            m_vswrSeries->append( freq, vswr );
            break;

        case CHART_SMITH:
            gamma = AAPiCalibrator::gamma_from_z( rx, m_config->get_base_r0() );

            QMetaObject::invokeMethod( m_smithChart, "append",
                                      Q_ARG(QVariant, QPointF(gamma.real(), gamma.imag())));
            break;

        case CHART_S11:
            break;
        }
    }

    return AAPI_SUCCESS;
}

void QAAPiPanoramicScanView::tab_changed(int index)
{
}

void QAAPiPanoramicScanView::rx_plot_area(QRectF rect)
{
    updatePlotArea(CHART_RX, rect);
}

void QAAPiPanoramicScanView::vswr_plot_area(QRectF rect)
{
    updatePlotArea(CHART_VSWR, rect);
}

void QAAPiPanoramicScanView::smith_plot_area(QRectF rect)
{
    updatePlotArea(CHART_SMITH, rect);
}

void QAAPiPanoramicScanView::rx_fast_scan()
{
    m_chartType = CHART_RX;

    startScan(true);
}

void QAAPiPanoramicScanView::rx_slow_scan()
{
    m_chartType = CHART_RX;

    startScan(false);
}

void QAAPiPanoramicScanView::vswr_fast_scan()
{
    m_chartType = CHART_VSWR;

    startScan(true);
}

void QAAPiPanoramicScanView::vswr_slow_scan()
{
    m_chartType = CHART_VSWR;

    startScan(false);
}

void QAAPiPanoramicScanView::smith_fast_scan()
{
    m_chartType = CHART_SMITH;

    startScan(true);
}

void QAAPiPanoramicScanView::smith_slow_scan()
{
    m_chartType = CHART_SMITH;

    startScan(false);
}

void QAAPiPanoramicScanView::rx_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_RX", result->image());
}

void QAAPiPanoramicScanView::vswr_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_VSWR", result->image());
}

void QAAPiPanoramicScanView::smith_snapshot(QQuickItemGrabResult *result)
{
    emit snapshotTaken("PAN_SMITH", result->image());
}
