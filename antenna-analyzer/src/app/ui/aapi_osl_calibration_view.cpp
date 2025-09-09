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

#include "aapi_osl_calibration_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiOSLCalibrationView
///////////////////////////////////////////////////////////////////////////////

QAAPiOSLCalibrationView::QAAPiOSLCalibrationView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                                AAPiGenerator *gen, AAPiCalibrator *cal,
                                                QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
    , m_calibrator(cal)
{
    AAPI_ADDREF(m_calibrator);

    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiOSLCalibrationView::~QAAPiOSLCalibrationView()
{
    AAPI_DISPOSE(m_calibrator);
}

int QAAPiOSLCalibrationView::getOslFile() const
{
    return m_config->get_osl_selected();
}

void QAAPiOSLCalibrationView::setOslFile(int osl_file)
{
    if ( getOslFile() != osl_file )
    {
        m_config->set_osl_selected( osl_file );
    }
}

uint32_t QAAPiOSLCalibrationView::getNumOslFiles() const
{
    const AAPiVariantArray& values = m_config->get_opt_values(AAPI_PARAM_OSL_SELECTED);
    quint32                 num_vals = m_config->get_num_opts(AAPI_PARAM_OSL_SELECTED);
    quint32                 count       = 0;

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

QIntList QAAPiOSLCalibrationView::getFileOptionValues() const
{
    const AAPiVariantArray& values      = m_config->get_opt_values(AAPI_PARAM_OSL_SELECTED);
    quint32                 num_vals    = m_config->get_num_opts(AAPI_PARAM_OSL_SELECTED);
    QIntList                res;

    for (uint i = 0; i < num_vals; i++)
    {
        int val = values[i].toInt();
        if( val >= 0 )
        {
            res.append( val );
        }
    }
    return res;
}

QStringList QAAPiOSLCalibrationView::getFileOptionLabels() const
{
    const AAPiVariantArray& values      = m_config->get_opt_values(AAPI_PARAM_OSL_SELECTED);
    const AAPiStringArray&  labels      = m_config->get_opt_labels(AAPI_PARAM_OSL_SELECTED);
    quint32                 num_vals    = m_config->get_num_opts(AAPI_PARAM_OSL_SELECTED);
    QStringList             res;

    for (uint i = 0; i < num_vals; i++)
    {
        int val = values[i].toInt();
        if ( val >= 0 )
        {
            res.append( labels[i].length() != 0 ? (const char *)labels[i] : (const char *)values[i].toString());
        }
    }
    return res;
}

bool QAAPiOSLCalibrationView::getOslFileExists() const
{
    return m_calibrator->osl_file_exists( getOslFile() );
}

int QAAPiOSLCalibrationView::load_view()
{
    if (! AAPiCalibrator::is_valid_file( getOslFile() ))
    {
        // Select 1st available OSL file 
        QIntList vals = getFileOptionValues();
        m_config->set_osl_selected( vals.at(0) );
    }

    return 0;
}

void QAAPiOSLCalibrationView::destroy_view()
{
}

int QAAPiOSLCalibrationView::onViewMeasureFinished(AAPiMeasureTask *measure)
{
    AAPiComplex     rx;
    int             ret;

    // We are being called from the MAIN/UI THREAD

    if (measure == nullptr)
    {
        // Finalize 
        m_calibrator->scan_osl_ended( m_scanType );

        // Calculate OSL calibration coefficients
        ret = m_calibrator->calc_osl_entries();
        if (ret == AAPI_CAL_E_SCAN_INCOMPLETE)
        {
            // Not fully scanned 
        }
        else if (AAPI_SUCCESS( ret ))
        {
            // Save results into file
            ret = m_calibrator->flush_osl_correction_file();
            // TODO: check error

            // Notify frontend 
            emit oslFileExistsChanged();
        }
    }
    else
    {
        if ( m_scanCancel ) /*User cancelled*/
        {
            return AAPI_E_CANCELLED;
        }

        if ( measure->is_low_signal( ))
        {
            // Hardware problem 
            emit scanNoSignal( m_scanType );

            return AAPI_E_FAILURE;
        }

        rx = measure->Rx;
        ret = m_calibrator->set_osl_z( m_scanType, m_scanIndex++, rx );
        // TODO: check error

        // Notify UI 
        emit scanProgress( m_scanType, m_scanIndex, m_measures.length(), rx.real(), rx.imag());
    }

    return AAPI_SUCCESS;
}

int QAAPiOSLCalibrationView::start_scan(ScanType type)
{
    AAPiMeasureTaskList     measure_steps;
    uint32_t                freq, num_scans;
    int                     ret;

    // Read number of scans
    num_scans = m_config->get_osl_n_scans();

    ret = m_calibrator->set_osl_file( getOslFile() );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    // Prepare measurement steps 
    for (uint i = 0; i < AAPI_CAL_NUM_ENTRIES; i++)
    {
        freq = AAPiCalibrator::freq_by_index( i );

        AAPiPtr<AAPiMeasureTask> measure(
            AAPiMeasureTask::create( m_config, m_calibrator, this, freq, true, false, num_scans, false )
        );
        measure_steps.push_back( measure );
    }

    /* reset scan index */
    m_scanIndex = 0;

    /* reset cancel flag */
    m_scanCancel = false;

    /* set scan type */
    m_scanType = type;

    ret = m_calibrator->scan_osl_begin( m_scanType );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    ret = startMeasure( measure_steps );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiOSLCalibrationView::cancel_scan()
{
    m_scanCancel = true;
}
