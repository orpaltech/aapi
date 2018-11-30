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

#ifndef QML_DSP_VIEW_H
#define QML_DSP_VIEW_H

#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "ui/qml_view_backend.h"

QT_CHARTS_USE_NAMESPACE

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// QAAPIQmlDSPView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPIQmlDSPView class
///
class QAAPIQmlDSPView : public QAAPIQmlView
{
    Q_OBJECT

    /* Properties */
    Q_PROPERTY(uint frequency READ get_frequency CONSTANT)

public:
    explicit QAAPIQmlDSPView(AAPIConfig *config, AAPISignalProcessor *processor,
                             AAPIGenerator *gen, QObject *parent = Q_NULLPTR);
    ~QAAPIQmlDSPView();

    /* Property accessors */
    uint32_t get_frequency() const { return m_frequency; }

private:
    virtual int load_view();
    virtual void destroy_view();

    /* AAPISignalProcessorEvents */
    virtual void dsp_raw_frame(float **buffers, uint32_t num_buffers, uint32_t buf_size);
    virtual void dsp_fft_frame(float **buffers, uint32_t num_buffers, uint32_t buf_size);

    bool is_spectrum_tab() const;
    bool is_oscilloscope_tab() const;

private:
    static void setup_chart(QChart *chart);

private:
    QMutex              m_mutex;

    uint32_t            m_frequency;
    volatile int        m_tabIndex;

    QVector<float>      m_VSpectrumPoints;
    QVector<float>      m_ISpectrumPoints;
    QVector<float>      m_VOscilloscPoints;
    QVector<float>      m_IOscilloscPoints;

    QLineSeries         *m_VSeriesOscillosc;
    QLineSeries         *m_ISeriesOscillosc;
    QLineSeries         *m_VSeriesSpectrum;
    QLineSeries         *m_ISeriesSpectrum;

signals:

public slots:
    void tab_changed(int index);
    void setup_spectrum(QLineSeries *v_series, QLineSeries *i_series);
    void setup_oscilloscope(QLineSeries *v_series, QLineSeries *i_series);
    void update_spectrum();
    void update_oscilloscope();

};

#endif // QML_DSP_VIEW_H
