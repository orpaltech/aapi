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
#include <unistd.h>
#include "aapi_view_backend.h"

using namespace aapi;

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DSP_PAUSE_MICROSEC  20000U /*20 millisec*/

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlView
///////////////////////////////////////////////////////////////////////////////

QAAPiViewBackend::QAAPiViewBackend(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                   AAPiGenerator *gen, QObject *parent)
    : QObject(parent)
    , m_active(false)
    , m_config(config)
    , m_dsp(dsp)
    , m_generator(gen)
{
    AAPI_ADDREF(m_dsp);
    AAPI_ADDREF(m_generator);
    AAPI_ADDREF(m_config);

    m_measureIter = m_measures.end();

    // Connect signal and slot 
    QObject::connect(this, SIGNAL(measureFinished(AAPiMeasureTask*)), this,
                     SLOT(measureFinishedHandler(AAPiMeasureTask*)),
                     Qt::QueuedConnection);

}

QAAPiViewBackend::~QAAPiViewBackend()
{
    AAPI_DISPOSE(m_config);
    AAPI_DISPOSE(m_dsp);
    AAPI_DISPOSE(m_generator);
}

quint32 QAAPiViewBackend::getBaseR0() const
{
    return static_cast<quint32> (m_config->get_base_r0( ));
}

void QAAPiViewBackend::setErrorMessage(const char *message)
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
}

AAPiMeasureTask *QAAPiViewBackend::getCurrentMeasure() const
{
    if (m_measureIter != m_measures.end())
        return *m_measureIter;

    return nullptr;
}

int QAAPiViewBackend::startMeasure(const AAPiMeasureTaskList& measure_steps)
{
    uint32_t    measure_freq;
    int         ret;

    if (measure_steps.isEmpty())
    {
        return AAPI_E_INVALID_ARG;
    }

    if (! m_measures.isEmpty())
    {
        return AAPI_E_INVALID_STATE;
    }

    // Lock generator to this owner
    ret = m_generator->lock( this );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    m_generator->resume();

    // Here is the starting frequency
    measure_freq = measure_steps.at( 0 )->measure_freq;

    enableSignalProcessing( false );

    // Setup generator to 1-st frequency value
    ret = m_generator->set_measure_freq( measure_freq, this );
    if (AAPI_FAILED( ret ))
    {
        m_generator->unlock( this );
        m_generator->suspend( );

        return ret;
    }

    // Let the filter stabilize after frequency switch
    skipFrames( );

    m_measures = measure_steps;
    m_measureIter = m_measures.begin();

    enableSignalProcessing( );

    return AAPI_SUCCESS;
}

void QAAPiViewBackend::skipFrames() const
{
    usleep( DSP_PAUSE_MICROSEC );
}

int QAAPiViewBackend::loaded()
{
    // Allow derived class load resources 
    int ret = load_view();
    if (AAPI_SUCCEEDED( ret ))
    {
        clearErrorMessage();
    }

    return ret;
}

int QAAPiViewBackend::activated()
{
    // Set active flag 
    m_active = true;

    // Allow derived class activate view
    int ret = activate_view();
    if (AAPI_SUCCEEDED( ret ))
    {
        clearErrorMessage();
    }

    return ret;
}

void QAAPiViewBackend::deactivated()
{
    // Allow derived class deactivate view 
    deactivate_view( );

    // Clear active flag 
    m_active = false;
}

void QAAPiViewBackend::destroyed()
{
    // Allow derived class destroy resources 
    destroy_view();
}

void QAAPiViewBackend::onSignalProcessMags(AAPiComplex *mags, uint32_t num_mags)
{
    // Process measurement
    if (getCurrentMeasure() != nullptr)
    {
        getCurrentMeasure()->process_mags( mags[DSP_V_CHANNEL], mags[DSP_I_CHANNEL] );
    }
}

void QAAPiViewBackend::onMeasureFinished(AAPiMeasureTask *measure)
{
    enableSignalProcessing( false );

    // Queue result to the main thread 
    emit measureFinished( measure );
}

void QAAPiViewBackend::measureFinishedHandler(AAPiMeasureTask *measure)
{
    int ret;

    // Allow derived class to handle measure 
    if (onViewMeasureFinished( measure ))
    {
        // An error occurred or derived class wants to cancel measure
        m_generator->unlock( this );
        m_generator->suspend( );

        // Cleanup measurement steps 
        m_measures.clear();
        return;
    }

    // Move to the next measure 
    if (++m_measureIter != m_measures.end())
    {
        // Setup generator 
        ret = m_generator->set_measure_freq( (*m_measureIter)->measure_freq, this );
        if (AAPI_FAILED( ret ))
        {
            // handle error 
            onViewMeasureError( ret );

            // need to finish measurement sequence 
            m_generator->unlock( this );
            m_generator->suspend( );
            return;
        }

        // Pause after switching operating frequency 
        skipFrames( );

        enableSignalProcessing( );
        return;
    }

    // Finished all measurements 
    m_generator->unlock( this );
    m_generator->suspend( );

    // Let derived class finalize 
    onViewMeasureFinished( nullptr );

    // Cleanup measurement steps 
    m_measures.clear();
}
