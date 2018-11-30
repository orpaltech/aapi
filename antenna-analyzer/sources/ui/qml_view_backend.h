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

#ifndef AAPI_QML_VIEW_H
#define AAPI_QML_VIEW_H

#include <QObject>
#include <QString>
#include <QImage>
#include "analyzer/analyzer_config.h"
#include "analyzer/analyzer_generator.h"
#include "analyzer/analyzer_dsp.h"
#include "analyzer/analyzer_measure.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// UI definitions
///////////////////////////////////////////////////////////////////////////////

#define COLOR_CHART_PLOT_AREA   0xcdd2ff//0xc4caff
#define COLOR_CHART_AXIS_LINE   0x898e93
#define COLOR_CHART_GRID_LINE   0x898e93
#define COLOR_CHART_MINOR_LINE  0x898e93

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlView
///////////////////////////////////////////////////////////////////////////////

class QAAPIQmlView : public QObject,
          public AAPISignalProcessorEvents,
          public AAPIMeasurementEvents
{
    Q_OBJECT

    /* Properties */
    Q_PROPERTY(QString error_message READ get_error_message CONSTANT)
    Q_PROPERTY(quint32 freq_min READ get_freq_min CONSTANT)
    Q_PROPERTY(quint32 freq_max READ get_freq_max CONSTANT)
    Q_PROPERTY(quint32 base_r0 READ get_base_r0 CONSTANT)

public:
    QAAPIQmlView(AAPIConfig *config, AAPISignalProcessor *dsp,
                AAPIGenerator *gen, QObject *parent);
    ~QAAPIQmlView();

    enum ViewStatus {
        VS_IDLE = 0,
        VS_BUSY = 1
    };

    /* Enumerations */
    Q_ENUM(ViewStatus)

public:
    /* Property accessors */
    bool is_active() const { return m_active; }
    QString get_error_message() const { return m_errorMsg; }
    uint32_t get_freq_min() const { return AA_BAND_FMIN;}
    uint32_t get_freq_max() const { return AA_BAND_FMAX; }
    uint32_t get_base_r0() const;

    typedef QList<aapi_ptr<AAPIMeasure>> AAPIMeasureList;

protected:
    void set_error_message(const char* message);
    void clear_error_message();
    bool has_error_message() const;

    /* Skip frames after frequency switch */
    void skip_frames() const;

    /* Initiates a new measurement sequence */
    int start_measure(AAPIMeasureList& measures);

    /* AAPISignalProcessorEvents */
    virtual void dsp_magnitudes(std::complex<float> *mags, uint32_t num_mags);

    /* Override to add any view-specific logic*/
    virtual int load_view() { return 0; }
    virtual int activate_view() { return 0; }
    virtual void deactivate_view() {}
    virtual void destroy_view() {}

    /* Override to handle measure finished (called in main thread)*/
    virtual int on_measure_finished(AAPIMeasure *measure) { return 0; }
    virtual void on_measure_error(int error) {}

private:
    /* AnalyzerMeasureCallback */
    virtual void measure_finished(AAPIMeasure *measure);

protected:
    aapi_ptr<AAPIConfig>                m_config;
    aapi_ptr<AAPIGenerator>             m_generator;
    aapi_ptr<AAPISignalProcessor>       m_processor;

    AAPIMeasureList                     m_allMeasures;
    AAPIMeasureList::const_iterator     m_measureIter;
    AAPIMeasure                        *m_currentMeasure;

    QString                             m_errorMsg;
    /* true means the view is now shown to user */
    bool                                m_active;

signals:
    void measureFinished(AAPIMeasure *measure);
    void snapshotTaken(QString file, QImage image);
    void quitApplication();
    void rebootApplication();

public slots:
    int loaded();
    int activated();
    void deactivated();
    void destroyed();
    void measureFinishedHandler(AAPIMeasure *measure);
};

#endif // AAPI_QML_VIEW_H
