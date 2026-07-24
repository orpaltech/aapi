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

#ifndef UI_AAPI_VIEW_BACKEND_H
#define UI_AAPI_VIEW_BACKEND_H

#include <QObject>
#include <QString>
#include <QImage>
//===> The following headers MUST be included before <complex.h>
#include <qvectornd.h>
#include <qnativeinterface.h>
#include <qtmochelpers.h>
//<====
#include <QtQmlIntegration/QtQmlIntegration>
using QIntList = QList<int>;
#include "analyzer/aapi_config.h"
#include "analyzer/aapi_generator.h"
#include "analyzer/aapi_signal_process.h"
#include "analyzer/aapi_measure_task.h"
#include "aapi_messages.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// UI definitions
///////////////////////////////////////////////////////////////////////////////

//#define AAPI_DESIGN_MODE


///////////////////////////////////////////////////////////////////////////////
// class QAAPiViewBackend
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The QAAPiViewBackend class
///
class QAAPiViewBackend : public QObject,
                         public AAPiSignalProcessEvents,
                         public AAPiMeasureEvents
{
    Q_OBJECT
    QML_UNCREATABLE(AAPI_QML_UNCREATABLE_REASON)
    QML_NAMED_ELEMENT(ViewBackend)

    /* Properties */
    Q_PROPERTY(quint32 freq_min READ getFrequencyMinKHz CONSTANT)
    Q_PROPERTY(quint32 freq_max READ getFrequencyMaxKHz CONSTANT)
    Q_PROPERTY(quint32 base_r0 READ getBaseR0 CONSTANT)
    Q_PROPERTY(bool is_design_mode READ isDesignMode CONSTANT)

public:
    QAAPiViewBackend(AAPiConfig *config, AAPiSignalProcessor *dsp,
                     AAPiGenerator *gen, QAAPiMessages *msgs,
                     QObject *parent);
    ~QAAPiViewBackend();

    using AAPiMeasureTaskList = QList<AAPiPtr<AAPiMeasureTask>>;

    enum ViewStatus {
        VS_IDLE = 0,
        VS_BUSY = 1
    };

    /* Enumerations */
    Q_ENUM(ViewStatus)

public:
    /* Property accessors */
    bool isActivated() const { return m_activated; }
    constexpr bool isDesignMode() const {
#ifdef AAPI_QML_DESIGN
        return true;
#else
        return false;
#endif
    }

    uint32_t getBaseR0() const { return m_config->get_base_r0(); }

    constexpr uint32_t getFrequencyMin() const { return AAPI_BAND_FREQ_MIN; }
    constexpr uint32_t getFrequencyMax() const { return AAPI_BAND_FREQ_MAX; }

    constexpr uint32_t getFrequencyMinKHz() const { return getFrequencyMin() / 1000; }
    constexpr uint32_t getFrequencyMaxKHz() const { return getFrequencyMax() / 1000; }

    constexpr bool isFrequencyValid(uint32_t freq) const {
        return (freq > 0 && freq >= getFrequencyMin() && freq <= getFrequencyMax() && (freq % 100'000) == 0);
    }

protected:
    // Initiates a new measurement sequence (will steal input data)
    AAPiError startMeasures(AAPiMeasureTaskList&& measures);
    AAPiError cancelMeasures();

    //
    // Override to invoke view-specific logic
    //
    virtual AAPiError onViewLoad() { return AAPI_SUCCESS; }
    virtual AAPiError onViewActivate() { return AAPI_SUCCESS; }
    virtual void onViewDeactivate() {}
    virtual void onViewDestroy() {}

    //
    // Override to handle measure finished (called in main thread)
    //
    virtual AAPiError onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure) {
        return AAPI_SUCCESS;
    }
    virtual void onViewMeasureError(AAPiError error) {}

    uint32_t getMaxMeasures() const { return m_maxMeasures; }

private:
// AAPiSignalProcessEvents
    void onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags) override;
    void onSignalProcessError(AAPiError error) override;

// AAPiMeasureEvents
    void onMeasureTaskFinished(AAPiMeasureTask *measure) override;

    AAPiError startNextMeasure();
    void cleanupMeasures();
    AAPiError prepareGenerator();
    void releaseGenerator();
    void settleMeasure();

protected:
    AAPiPtr<AAPiConfig>             m_config;
    AAPiPtr<AAPiGenerator>          m_generator;
    AAPiPtr<AAPiSignalProcessor>    m_dsp;
    QAAPiMessages                   *m_msgs;

private:
    uint                        m_maxMeasures;
    AAPiMeasureTaskList         m_measureSteps;
    AAPiPtr<AAPiMeasureTask>    m_currentMeasure;

    QThread     *m_timerThread;
    QTimer      *m_settlingTimer;   // Tracks our active settling delays
    bool        m_activated;        // view is shown to user

Q_SIGNALS:
    void measureTaskFinished(AAPiPtr<AAPiMeasureTask> measure);
    void measureTaskError(AAPiError error);
    void snapshotTaken(QString file, QImage image);
    void quitApplication();
    void rebootApplication();

public Q_SLOTS:
    int handleLoaded();
    int handleActivated();
    void handleDeactivated();
    void handleDestroyed();
    void handleMeasureTaskFinished(AAPiPtr<AAPiMeasureTask> measure);
    void handleMeasureTaskError(AAPiError error);
};

#endif // UI_AAPI_VIEW_BACKEND_H
