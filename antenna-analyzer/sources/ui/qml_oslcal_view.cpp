/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-PI software.
 *
 * 	ORPAL-AA-PI is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-PI is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-PI. If not, see <http://www.gnu.org/licenses/>.
 */

#include "qml_oslcal_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlOSLCalView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlOSLCalView::QAAPIQmlOSLCalView(AAPIConfig *config, AAPISignalProcessor *dsp,
                                       AAPIGenerator *gen, AAPICalibrator *calibrator,
                                       QObject *parent)
    : QAAPIQmlView(config, dsp, gen, parent)
{
    m_calibrator = calibrator;
    AAPI_ADDREF(m_calibrator);

    m_osl_file = -1;

    // Subscribe for DSP events 
    dsp->add_callback(this);
}

QAAPIQmlOSLCalView::~QAAPIQmlOSLCalView()
{
    AAPI_DISPOSE(m_calibrator);
}

void QAAPIQmlOSLCalView::set_osl_file(int osl_file)
{
    if( m_osl_file != osl_file )
    {
        m_osl_file = osl_file;
    }
}

uint32_t QAAPIQmlOSLCalView::get_file_num_options() const
{
    uint32_t        num_vals    = m_config->get_num_opt(AA_PARAM_OSL_SELECTED);
    const QVariant *values      = m_config->get_opt_values(AA_PARAM_OSL_SELECTED);
    uint32_t        count       = 0;

    for (uint i = 0; i < num_vals; i++)
    {
        int val = values[i].toInt();
        if ( val >= 0 )
        {
            count++;
        }
    }
    return count;
}

QList<int> QAAPIQmlOSLCalView::get_file_opt_values() const
{
    QList<int>      res;
    uint32_t        num_vals    = m_config->get_num_opt(AA_PARAM_OSL_SELECTED);
    const QVariant *values      = m_config->get_opt_values(AA_PARAM_OSL_SELECTED);

    for( uint i = 0; i < num_vals; i++ )
    {
        int val = values[i].toInt();
        if( val >= 0 )
        {
            res.append(val);
        }
    }
    return res;
}

QStringList QAAPIQmlOSLCalView::get_file_opt_labels() const
{
    QStringList res;
    uint32_t        num_vals    = m_config->get_num_opt(AA_PARAM_OSL_SELECTED);
    const QString   *labels     = m_config->get_opt_labels(AA_PARAM_OSL_SELECTED);
    const QVariant  *values     = m_config->get_opt_values(AA_PARAM_OSL_SELECTED);

    for( uint i = 0; i < num_vals; i++ )
    {
        int val = values[i].toInt();
        if ( val >= 0 )
        {
            res.append(labels ? labels[i] : values[i].toString());
        }
    }
    return res;
}

bool QAAPIQmlOSLCalView::get_file_exists() const
{
    return m_calibrator->osl_file_exists( m_osl_file );
}

int QAAPIQmlOSLCalView::load_view()
{
    m_osl_file = m_config->get_osl_selected();

    if (! AAPICalibrator::is_valid_file( m_osl_file ))
    {
        // Select 1st available OSL file 
        QList<int> vals = get_file_opt_values();
        m_osl_file = vals.at(0);
    }

    return 0;
}

void QAAPIQmlOSLCalView::destroy_view()
{
}

int QAAPIQmlOSLCalView::on_measure_finished(AAPIMeasure *measure)
{
    std::complex<float>     rx;
    int                     ret;

    // We are being called from the MAIN/UI THREAD 

    if( measure == nullptr )
    {
        // Finalize 
        m_calibrator->scan_osl_ended( m_scan_type );

        /* Calculate OSL calibration coefficients */
        ret = m_calibrator->calc_osl_entries();
        if( ret == AAPI_CAL_E_SCAN_INCOMPLETE )
        {
            // Not fully scanned 
        }
        else if( AAPI_SUCCESS( ret ) )
        {
            // Save results into file 
            ret = m_calibrator->save_osl_correction_file();

            // Notify frontend 
            emit oslFileExistsChanged();
        }
    }
    else
    {
        if( m_scan_cancelled ) /*User cancelled*/
        {
            return 1;
        }

        if( measure->is_signal_low() )
        {
            // Hardware problem 
            emit scanNoSignal( m_scan_type );
            return 1;
        }

        rx = measure->Rx;
        ret = m_calibrator->set_osl_z( m_scan_type, m_scan_index++, rx );

        // Notify UI 
        emit scanProgress( m_scan_type, m_scan_index, m_measures.length(), rx.real(), rx.imag());
    }

    return 0;
}

int QAAPIQmlOSLCalView::start_scan(ScanType type)
{
    uint32_t        freq, num_scans;
    AAPIMeasureList measure_steps;
    int             ret;

    // Read number of scans
    num_scans = m_config->get_osl_n_scans();

    ret = m_calibrator->set_osl_file( m_osl_file );
    if( AAPI_FAILED( ret ))
    {
        return ret;
    }

    // Prepare measurement steps 
    for( uint i = 0; i < CALIB_NUM_ENTRIES; i++ )
    {
        freq = AAPICalibrator::freq_by_index(i);

        AAPtr<AAPIMeasure> measure( AAPIMeasure::create(m_config, m_calibrator, this, freq, true, false, num_scans, false) );
        measure_steps.push_back( measure );
    }

    /* reset scan index */
    m_scan_index = 0;

    /* reset cancel flag */
    m_scan_cancelled = false;

    /* set scan type */
    m_scan_type = type;

    ret = m_calibrator->scan_osl_begin( m_scan_type );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    ret = start_measure( measure_steps );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    return 0;
}

void QAAPIQmlOSLCalView::cancel_scan()
{
    m_scan_cancelled = true;
}
