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

#include "aapi_view_backend.h"
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QDebug>

using namespace aapi;


///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlView
///////////////////////////////////////////////////////////////////////////////

QAAPiViewBackend::QAAPiViewBackend(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                   AAPiGenerator *gen, QAAPiMessages *msgs,
                                   QObject *parent)
    : QObject(parent)
    , m_activated(false)
    , m_config(config)
    , m_dsp(dsp)
    , m_generator(gen)
    , m_msgs(msgs)
{
    m_timerThread = new QThread(this);
    m_timerThread->start();

    m_settlingTimer = new QTimer(nullptr); // No parent yet
    m_settlingTimer->setSingleShot(true);
    m_settlingTimer->moveToThread(m_timerThread); // Run strictly inside your timer loop thread

    // Connect the timer wake event cleanly to your signal parsing loop
    QObject::connect(m_settlingTimer, &QTimer::timeout, this, [this]() {
                        signal_process_enable();
                    },
                    Qt::DirectConnection);


    // Connect signals and slots
    QObject::connect(this, &QAAPiViewBackend::measureTaskFinished, this,
                     &QAAPiViewBackend::handleMeasureTaskFinished,
                     Qt::QueuedConnection);
    QObject::connect(this, &QAAPiViewBackend::measureTaskError, this,
                     &QAAPiViewBackend::handleMeasureTaskError,
                     Qt::QueuedConnection);
}

QAAPiViewBackend::~QAAPiViewBackend()
{
    if (m_timerThread) {
        m_timerThread->quit();    // Stop the background event loop
        m_timerThread->wait();    // Wait for the OS thread to close completely
    }
}

/*void QAAPiViewBackend::setErrorMessage(const char *message)
{
    m_errorMsg = message;
}

void QAAPiViewBackend::clearErrorMessage()
{
    m_errorMsg.clear();
}

bool QAAPiViewBackend::hasErrorMessage() const
{
    return m_errorMsg.length() > 0;
}*/

void QAAPiViewBackend::cleanupMeasures()
{
    m_measureSteps.clear();
    m_maxMeasures = 0;
    m_currentMeasure = nullptr;
}

AAPiError QAAPiViewBackend::prepareGenerator()
{
    // Lock generator to this owner
    AAPiError ret = m_generator->lock( this );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    ret = m_generator->resume();
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiViewBackend::releaseGenerator()
{
    if (m_generator->is_locked() ) {
        // Set default generator frequency
        m_generator->set_measure_freq(
            m_config->get_generator_freq(), this );
        m_generator->unlock( this );
        m_generator->suspend( );
    }
}

void QAAPiViewBackend::settleMeasure()
{
    // Fetch the dynamic hardware settling gate
    uint32_t settlingTime = m_config->get_dsp_settling_delay_ms();

    // Start our dedicated tracking instance cleanly
    QMetaObject::invokeMethod(m_settlingTimer, "start",
                              Qt::QueuedConnection,
                              Q_ARG(int, settlingTime));
}

AAPiError QAAPiViewBackend::startNextMeasure()
{
    // Setup generator to the first frequency value
    AAPiError ret = m_generator->set_measure_freq(
                    m_measureSteps.first()->measure_freq, this );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    // Grab the first measure from the queue
    m_currentMeasure = m_measureSteps.takeFirst();

    settleMeasure();

    return AAPI_SUCCESS;
}

AAPiError QAAPiViewBackend::startMeasures(AAPiMeasureTaskList&& measures)
{
    if (measures.isEmpty()) {
        return AAPI_E_INVALID_ARG;
    }

    if (! m_measureSteps.isEmpty()) {
        return AAPI_E_INVALID_STATE;
    }

    signal_process_enable( false );

    // Lock generator to this owner
    AAPiError ret = prepareGenerator();
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    m_measureSteps = std::move(measures);
    m_maxMeasures = m_measureSteps.length();

    ret = startNextMeasure();
    if (AAPI_FAILED( ret )) {
        releaseGenerator();
        cleanupMeasures();
        return ret;
    }

    return AAPI_SUCCESS;
}

AAPiError QAAPiViewBackend::cancelMeasures()
{
    cleanupMeasures();
    releaseGenerator();
    return AAPI_SUCCESS;
}

int QAAPiViewBackend::handleLoaded()
{
    // Allow derived class load resources
    AAPiError ret = onViewLoad();
    if (AAPI_SUCCEEDED( ret )) {
        //clearErrorMessage();
    }

    return ret;
}

int QAAPiViewBackend::handleActivated()
{
    // Set active flag 
    m_activated = true;

    // Allow derived class activate view
    AAPiError ret = onViewActivate();
    if (AAPI_SUCCEEDED( ret )) {
        //clearErrorMessage();
    }

    return ret;
}

void QAAPiViewBackend::handleDeactivated()
{
    // Allow derived class deactivate view 
    onViewDeactivate( );

    // Clear active flag 
    m_activated = false;
}

void QAAPiViewBackend::handleDestroyed()
{
    // Allow derived class destroy resources
    onViewDestroy();
}

void QAAPiViewBackend::onSignalProcessError(AAPiError error)
{
    if (error == AAPI_DSP_E_ADC_BUFFER_OVERRUN) {
        signal_process_enable( false );

        qWarning().noquote()
            << "[WARNING]"
            << QDateTime::currentDateTime().toString("hh:mm:ss.zzz")
            << "| Software ring buffer overrun detected!"
            << "Consumer thread fell behind. Current Thread ID:" << QThread::currentThreadId();

        // Restart current measure step
        settleMeasure();
        return;
    }

    emit measureTaskError( error );
}

void QAAPiViewBackend::onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags)
{
    // Process measurement
    if (m_currentMeasure != nullptr) {
        m_currentMeasure->process_mags( mags[DSP_V_CHANNEL], mags[DSP_I_CHANNEL] );
    }
}

void QAAPiViewBackend::onMeasureTaskFinished(AAPiMeasureTask *measure)
{
    signal_process_enable( false );

    if (m_currentMeasure != nullptr) {
        emit measureTaskFinished( std::move(m_currentMeasure));
    }
}

void QAAPiViewBackend::handleMeasureTaskFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    // Let derived class handle the measure
    AAPiError ret = onViewMeasureFinished( measure );
    if (AAPI_FAILED( ret )) {
        // An error occurred or derived class wants to cancel sweep
        releaseGenerator();
        cleanupMeasures();
        return;
    }

    if (! m_measureSteps.isEmpty()) {

        // Move to the next measure
        ret = startNextMeasure();
        if (AAPI_FAILED( ret )) {
            releaseGenerator();
            cleanupMeasures();
            // Notify derived class of error occurred
            onViewMeasureError( ret );
        }
        return;
    }

    // Let derived class finalize
    onViewMeasureFinished( AAPiPtr<AAPiMeasureTask>() );

    // Finished all measurements
    releaseGenerator();
    cleanupMeasures();
}

void QAAPiViewBackend::handleMeasureTaskError(AAPiError error)
{
    onViewMeasureError( error );
}
