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

#ifndef UI_AAPI_DSP_VIEW_H
#define UI_AAPI_DSP_VIEW_H

#include <QtGraphs/QLineSeries>
#include <QtGraphs/QValueAxis>
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

public:
    explicit QAAPiSignalProcessView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                    AAPiGenerator *gen, QObject *parent = Q_NULLPTR);
    ~QAAPiSignalProcessView();

private:
// QAAPiViewBackend
    AAPiError onViewLoad() override;
    AAPiError onViewActivate() override;
    void onViewDeactivate() override;
    void onViewDestroy() override;

// AAPiSignalProcessEvents
    void onSignalProcessRaw(AAPiReal **buffers, uint32_t num_buffers, uint32_t buf_size) override;
    void onSignalProcessFFT(AAPiReal **buffers, uint32_t num_buffers, uint32_t buf_size) override;

    bool isSpectrumTab() const;
    bool isWaveformTab() const;

private:
    //QRecursiveMutex m_mutex;

    volatile int    m_tabIndex;
    volatile bool   m_dataRequested;

    QVector<AAPiReal>   m_spectrumPointsV;
    QVector<AAPiReal>   m_spectrumPointsI;
    QVector<AAPiReal>   m_waveformPointsV;
    QVector<AAPiReal>   m_waveformPointsI;

    QLineSeries *m_waveformSeriesV;
    QLineSeries *m_waveformSeriesI;
    QLineSeries *m_spectrumSeriesV;
    QLineSeries *m_spectrumSeriesI;

Q_SIGNALS:
    void spectrumDataReady();
    void waveformDataReady();

public Q_SLOTS:
    void handleTabChange(int index);
    void handleSpectrumSetup(QLineSeries *v_series, QLineSeries *i_series,
                             QValueAxis *axisVX, QValueAxis *axisVY,
                             QValueAxis *axisIX, QValueAxis *axisIY);
    void handleWaveformSetup(QLineSeries *v_series, QLineSeries *i_series,
                             QValueAxis *axisVX, QValueAxis *axisVY,
                             QValueAxis *axisIX, QValueAxis *axisIY);
    void handleSpectrumUpdate();
    void handleWaveformUpdate();
    void handleSpectrumDataReady();
    void handleWaveformDataReady();

};

#endif // UI_AAPI_DSP_VIEW_H
