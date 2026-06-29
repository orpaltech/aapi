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
    // Subscribe for DSP events 
    dsp->addCallback(this);
}

QAAPiOSLCalibrationView::~QAAPiOSLCalibrationView()
{
}

int QAAPiOSLCalibrationView::getFile() const
{
    return m_config->get_osl_file_selected();
}

void QAAPiOSLCalibrationView::setFile(int osl_file)
{
    if ( getFile() != osl_file ) {
        m_config->set_osl_file_selected( osl_file );

        emit oslFileChanged( osl_file );
    }
}

uint32_t QAAPiOSLCalibrationView::getNumFiles() const
{
    quint32                 num_vals = m_config->get_num_opts(AAPiParameter::OSL_FILE_SELECTED);
    const AAPiVariantArray& values = m_config->get_opt_values(AAPiParameter::OSL_FILE_SELECTED);
    quint32                 count       = 0;

    for ( uint i = 0; i < num_vals; ++i ) {
        int val = values[i].toInt( );
        if (AAPiCalibrator::is_valid_file( val )) {
            count++;
        }
    }
    return count;
}

QIntList QAAPiOSLCalibrationView::getFileOptionValues() const
{
    const AAPiVariantArray& values      = m_config->get_opt_values(AAPiParameter::OSL_FILE_SELECTED);
    quint32                 num_vals    = m_config->get_num_opts(AAPiParameter::OSL_FILE_SELECTED);
    QIntList                result;

    for ( uint i = 0; i < num_vals; ++i ) {
        int val = values[i].toInt( );
        if (AAPiCalibrator::is_valid_file( val )) {
            result.append( val );
        }
    }
    return result;
}

QStringList QAAPiOSLCalibrationView::getFileOptionLabels() const
{
    const AAPiStringArray&  labels      = m_config->get_opt_labels(AAPiParameter::OSL_FILE_SELECTED);
    const AAPiVariantArray& values      = m_config->get_opt_values(AAPiParameter::OSL_FILE_SELECTED);
    quint32                 num_vals    = m_config->get_num_opts(AAPiParameter::OSL_FILE_SELECTED);
    QStringList             result;

    for ( uint i = 0; i < num_vals; ++i ) {
        int val = values[i].toInt( );
        if (AAPiCalibrator::is_valid_file( val )) {
            result.append( labels[i].length() != 0 ? (const char *)labels[i] : (const char *)values[i].toString());
        }
    }
    return result;
}

bool QAAPiOSLCalibrationView::getFileExists() const
{
    return m_calibrator->osl_file_exists( getFile() );
}

AAPiError QAAPiOSLCalibrationView::loadView()
{
    int currentFile = getFile();

    // Treat your placeholder index -1 ("None") as a completely legal configuration selection!
    if (! AAPiCalibrator::is_valid_file( currentFile )) {
        QIntList vals = getFileOptionValues();
        if (! vals.isEmpty()) {
            m_config->set_osl_file_selected( vals.at(0) );
        }
    }

    return AAPI_SUCCESS;
}

void QAAPiOSLCalibrationView::destroyView()
{
}

AAPiError QAAPiOSLCalibrationView::onViewMeasureFinished(AAPiPtr<AAPiMeasureTask> measure)
{
    // We are being called from the MAIN/UI THREAD

    if (measure == nullptr) {
        // Finalize this scan. The OSL calibration is still considered incomplete
        // until calc_osl_entries() is successfully executed.
        //
        m_calibrator->osl_scan_finalize( m_scanType );

        // Calculate OSL calibration coefficients
        int ret = m_calibrator->calc_osl_entries();
        if (AAPI_SUCCEEDED( ret )) {

            // Save results into a file
            ret = m_calibrator->flush_osl_correction_file();
            if (AAPI_FAILED( ret )) {
                // TODO: check error
            }

            // Notify frontend 
            emit oslFileExistsChanged();

        } else if (ret == AAPI_CAL_E_SCAN_INCOMPLETE) {

            // Not fully scanned, which is fine

        } else {
            // Critical error
        }

    } else {
        if ( m_scanCancelled ) { /*User cancelled*/
            return AAPI_E_CANCELLED;
        }

        uint32_t freq = measure->measure_freq;
        AAPiComplex rx = measure->Rx;

        // Notify UI
        int step = m_scanIndex + 1; // Make step 1-based
        int total = getMaxMeasures();
        emit scanProgress( m_scanType, step, total, freq, rx.real(), rx.imag());

        if ( measure->is_low_signal( )) {
            // Hardware problem 
            emit scanNoSignal( m_scanType );

            return AAPI_E_FAILURE;
        }

        int ret = m_calibrator->set_osl_z( m_scanType, m_scanIndex++, rx );
        if (AAPI_FAILED( ret )) {
            // TODO: check error
        }

    }

    return AAPI_SUCCESS;
}

AAPiError QAAPiOSLCalibrationView::handleStartScan(ScanType type)
{
    auto ret = m_calibrator->set_osl_file( getFile() );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    // Read number of scans
    uint32_t num_scans = qMin( m_config->get_calibr_num_scans(), AAPI_MAX_MEASURE_SCANS );

    AAPiMeasureTaskList steps;
    // Reserve memory upfront so the list never reallocates
    steps.reserve(AAPiCalibrator::NUM_ENTRIES);
    // Prepare measurement steps 
    for (uint i = 0; i < AAPiCalibrator::NUM_ENTRIES; i++) {
        uint32_t freq = AAPiCalibrator::get_freq_by_index( i );

        AAPiPtr<AAPiMeasureTask> measure(
            AAPiMeasureTask::create( m_config, m_calibrator, this, freq, true, false, num_scans, false )
        );
        steps.push_back( std::move(measure) );
    }

    m_scanIndex = 0;
    m_scanCancelled = false;
    m_scanType = type;

    ret = m_calibrator->osl_scan_begin( m_scanType );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    ret = startMeasures( std::move(steps) );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    return AAPI_SUCCESS;
}

void QAAPiOSLCalibrationView::handleCancelScan()
{
    m_scanCancelled = true;
}
