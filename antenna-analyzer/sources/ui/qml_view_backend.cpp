/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#include <unistd.h>
#include "qml_view_backend.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DSP_PAUSE_MICROSEC  20000 /*20 millisec*/

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlView::QAAPIQmlView(AAPIConfig *config, AAPISignalProcessor *dsp,
                           AAPIGenerator *gen, QObject *parent)
    : QObject(parent)
    , m_current_measure(nullptr)
    , m_active(false)
{
    m_dsp = dsp;
    m_generator = gen;
    m_config = config;

    AAPI_ADDREF(m_dsp);
    AAPI_ADDREF(m_generator);
    AAPI_ADDREF(m_config);

    // Connect signal and slot 
    QObject::connect(this, SIGNAL(measureFinished(AAPIMeasure*)), this,
                     SLOT(measureFinishedHandler(AAPIMeasure*)),
                     Qt::QueuedConnection);

}

QAAPIQmlView::~QAAPIQmlView()
{
    AAPI_DISPOSE(m_config);
    AAPI_DISPOSE(m_dsp);
    AAPI_DISPOSE(m_generator);
}

quint32 QAAPIQmlView::get_base_r0() const
{
    return static_cast< quint32 > ( m_config->get_base_r0() );
}

void QAAPIQmlView::set_error_message(const char *message)
{
    m_error_msg = message;
}

void QAAPIQmlView::clear_error_message()
{
    m_error_msg.clear();
}

bool QAAPIQmlView::has_error_message() const
{
    return m_error_msg.length() > 0;
}

int QAAPIQmlView::start_measure(const AAPIMeasureList& measure_steps)
{
    int     ret;
    uint32_t measure_freq;

    if( measure_steps.length() == 0 )
    {
        return AAPI_E_INVALID_ARG;
    }

    if( m_measures.length() > 0 )
    {
        return AAPI_E_INVALID_STATE;
    }

    // Lock generator to this owner
    ret = m_generator->lock( this );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    // Here is the starting frequency 
    measure_freq = measure_steps.at( 0 )->Frequency;

    enable_signal_processing(false);

    // Setup generator to 1-st frequency value
    ret = m_generator->set_frequency( measure_freq, this );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    // Let the filter stabilize after frequency switch 
    skip_frames();

    m_measures = measure_steps;
    m_measure_iter = m_measures.begin();
    m_current_measure = *m_measure_iter;

    enable_signal_processing();

    return AAPI_SUCCESS;
}

void QAAPIQmlView::skip_frames() const
{
    usleep( DSP_PAUSE_MICROSEC );
}

int QAAPIQmlView::loaded()
{
    // Allow derived class load resources 
    int ret = load_view();
    if( AAPI_SUCCESS( ret ) )
    {
        clear_error_message();
    }

    return ret;
}

int QAAPIQmlView::activated()
{
    // Set active flag 
    m_active = true;

    // Allow derived class activate view
    int ret = activate_view();
    if( AAPI_SUCCESS( ret ) )
    {
        clear_error_message();
    }

    return ret;
}

void QAAPIQmlView::deactivated()
{
    // Allow derived class deactivate view 
    deactivate_view();

    // Clear active flag 
    m_active = false;
}

void QAAPIQmlView::destroyed()
{
    // Allow derived class destroy resources 
    destroy_view();
}

void QAAPIQmlView::dsp_magnitudes(std::complex<float> *mags, uint32_t num_mags)
{
    // Process measurement 
    if( m_current_measure )
    {
        m_current_measure->process_mags( mags[DSP_V_CHANNEL], mags[DSP_I_CHANNEL] );
    }
}

void QAAPIQmlView::measure_finished(AAPIMeasure *measure)
{
    m_current_measure = nullptr;

    // Queue result to the main thread 
    emit measureFinished( measure );
}

void QAAPIQmlView::measureFinishedHandler(AAPIMeasure *measure)
{
    enable_signal_processing( false );

    int ret;

    // Allow derived class to handle measure 
    ret = on_measure_finished( measure );
    if( ret )
    {
        // Derived class want to cancel measure 
        m_generator->set_frequency( 0, this );
        m_generator->unlock( this );

        // Cleanup measurement steps 
        m_measures.clear();
        return;
    }

    // Move to the next measure 
    if( ++m_measure_iter != m_measures.end() )
    {
        // Setup generator 
        ret = m_generator->set_frequency( (*m_measure_iter)->Frequency, this );
        if( AAPI_FAILED( ret ) )
        {
            // handle error 
            on_measure_error( ret );

            // need to finish measurement sequence 
            m_generator->set_frequency( 0, this );
            m_generator->unlock( this );
            return;
        }

        // Pause after switching operating frequency 
        skip_frames();

        m_current_measure = *m_measure_iter;

        enable_signal_processing();
        return;
    }

    // Finished all measurements 
    m_generator->set_frequency( 0, this );
    m_generator->unlock( this );

    // Let derived class finalize 
    on_measure_finished( nullptr );

    // Cleanup measurement steps 
    m_measures.clear();
}
