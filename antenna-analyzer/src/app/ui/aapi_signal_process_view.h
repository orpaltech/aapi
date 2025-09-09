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

#ifndef UI_AAPI_DSP_VIEW_H
#define UI_AAPI_DSP_VIEW_H

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "aapi_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// class QAAPiSignalProcessView
///////////////////////////////////////////////////////////////////////////////
/// \brief The QAAPiSignalProcessView class
///
class QAAPiSignalProcessView : public QAAPiViewBackend
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(SignalProcessViewBackend)

    /* Properties */
    Q_PROPERTY(uint frequency READ getFrequency CONSTANT)

public:
    explicit QAAPiSignalProcessView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                    AAPiGenerator *gen, QObject *parent = Q_NULLPTR);
    ~QAAPiSignalProcessView();

    /* Property accessors */
    uint32_t getFrequency() const { return m_frequency; }

private:
    virtual int load_view();
    virtual void destroy_view();

    bool isSpectrumTab() const;
    bool isOscilloscopeTab() const;

    // AAPiSignalProcessEvents
    virtual void onSignalProcessRaw(double **buffers, uint32_t num_buffers, uint32_t buf_size);
    virtual void onSignalProcessFFT(double **buffers, uint32_t num_buffers, uint32_t buf_size);

private:
    static void setupChart(QChart *chart);

private:
    QRecursiveMutex     m_mutex;

    uint32_t            m_frequency;
    volatile int        m_tabIndex;

    QVector<double>     m_vSpectrumPoints;
    QVector<double>     m_iSpectrumPoints;
    QVector<double>     m_vOscilloscPoints;
    QVector<double>     m_iOscilloscPoints;

    QLineSeries         *m_vOscilloscSeries;
    QLineSeries         *m_iOscilloscSeries;
    QLineSeries         *m_vSpectrumSeries;
    QLineSeries         *m_iSpectrumSeries;

signals:

public slots:
    void tab_changed(int index);
    void setup_spectrum(QLineSeries *v_series, QLineSeries *i_series);
    void setup_oscilloscope(QLineSeries *v_series, QLineSeries *i_series);
    void update_spectrum();
    void update_oscilloscope();

};

#endif // UI_AAPI_DSP_VIEW_H
