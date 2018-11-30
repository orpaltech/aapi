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

#include "qml_measure_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPIQmlMeasureView
///////////////////////////////////////////////////////////////////////////////

QAAPIQmlMeasureView::QAAPIQmlMeasureView(AAPIConfig *config, AAPISignalProcessor *processor,
                                         AAPIGenerator *gen, QObject *parent)
    : QAAPIQmlView(config, processor, gen, parent)
{
}

QAAPIQmlMeasureView::~QAAPIQmlMeasureView()
{
}

int QAAPIQmlMeasureView::load_view()
{
    unsigned int freq;
    int osl_sel;

    if (has_error_message()) {
        // TODO: wut should be here ?
    }

    freq = m_config->get_measure_freq();

    /* Validate measurement frequency*/
    if (freq < get_freq_min() ||
        freq > get_freq_max() || (freq % 1000) != 0)
    {
        freq = 14000000ul;
    }

    osl_sel = m_config->get_osl_selected();
    if (osl_sel < 0)
    {
        set_error_message("No OSL-calibration file selected");
        return -1;
    }

    /* Clear error message */
    clear_error_message();
    return 0;
}

void QAAPIQmlMeasureView::destroy_view()
{
}

int QAAPIQmlMeasureView::activate_view()
{
    return 0;
}

void QAAPIQmlMeasureView::deactivate_view()
{
}
