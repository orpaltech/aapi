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
#include <QtQmlIntegration/qqmlintegration.h>
using QIntList = QList<int>;
#include "analyzer/aapi_config.h"
#include "analyzer/aapi_generator.h"
#include "analyzer/aapi_signal_process.h"
#include "analyzer/aapi_measure_task.h"

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
    Q_PROPERTY(QString error_message READ getErrorMessage CONSTANT)
    Q_PROPERTY(quint32 freq_min READ getFreqMin CONSTANT)
    Q_PROPERTY(quint32 freq_max READ getFreqMax CONSTANT)
    Q_PROPERTY(quint32 base_r0 READ getBaseR0 CONSTANT)
    Q_PROPERTY(bool is_design_mode READ isDesignMode CONSTANT)

public:
    QAAPiViewBackend(AAPiConfig *config, AAPiSignalProcessor *dsp,
                     AAPiGenerator *gen, QObject *parent);
    ~QAAPiViewBackend();

    typedef QList<AAPiPtr<AAPiMeasureTask>> AAPiMeasureTaskList;

    enum ViewStatus {
        VS_IDLE = 0,
        VS_BUSY = 1
    };

    /* Enumerations */
    Q_ENUM(ViewStatus)

public:
    /* Property accessors */
    bool isActive() const { return m_active; }
    bool isDesignMode() const {
#ifdef AAPI_QML_DESIGN
        return true;
#else
        return false;
#endif
    }
    QString getErrorMessage() const { return m_errorMsg; }
    uint32_t getFreqMin() const { return AAPI_BAND_FMIN;}
    uint32_t getFreqMax() const { return AAPI_BAND_FMAX; }
    uint32_t getBaseR0() const;

protected:
    void setErrorMessage(const char *message);
    void clearErrorMessage();
    bool hasErrorMessage() const;

    /* Skip frames after frequency switch */
    void skipFrames() const;

    /* Initiates a new measurement sequence */
    int startMeasure(const AAPiMeasureTaskList& measure_steps);

    AAPiMeasureTask *getCurrentMeasure() const;

    /* Override to invoke view-specific logic*/
    virtual int load_view() { return 0; }
    virtual int activate_view() { return 0; }
    virtual void deactivate_view() {}
    virtual void destroy_view() {}

    /* Override to handle measure finished (called in main thread)*/
    virtual int onViewMeasureFinished(AAPiMeasureTask *measure) { return AAPI_SUCCESS; }
    virtual void onViewMeasureError(int error) {}

    bool isValidFreq(uint32_t f) const
    {
        return (f != 0 && f >= (getFreqMin() / 1000) && f <= (getFreqMax() / 1000) && (f % 100) == 0);
    }

private:
// AAPiSignalProcessEvents
    virtual void onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags);

// AAPiMeasureEvents
    virtual void onMeasureFinished(AAPiMeasureTask *measure);

protected:
    AAPiConfig              *m_config;
    AAPiGenerator           *m_generator;
    AAPiSignalProcessor     *m_dsp;
    AAPiMeasureTaskList     m_measures;

private:
    AAPiMeasureTaskList::const_iterator m_measureIter;

    QString     m_errorMsg;
    bool        m_active;   /* view is shown to user */

signals:
    void measureFinished(AAPiMeasureTask *measure);
    void snapshotTaken(QString file, QImage image);
    void quitApplication();
    void rebootApplication();

public slots:
    int loaded();
    int activated();
    void deactivated();
    void destroyed();
    void measureFinishedHandler(AAPiMeasureTask *measure);
};

#endif // UI_AAPI_VIEW_BACKEND_H
