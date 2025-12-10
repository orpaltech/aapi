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

#include "aapi_measure_view.h"

///////////////////////////////////////////////////////////////////////////////
// class QAAPiMeasurementView
///////////////////////////////////////////////////////////////////////////////

QAAPiMeasurementView::QAAPiMeasurementView(AAPiConfig *config, AAPiSignalProcessor *dsp,
                                           AAPiGenerator *gen, QObject *parent)
    : QAAPiViewBackend(config, dsp, gen, parent)
{
}

QAAPiMeasurementView::~QAAPiMeasurementView()
{
}

int QAAPiMeasurementView::load_view()
{
    unsigned int freq;
    int osl_sel;

    if (hasErrorMessage()) {
        // TODO: wut should be here ?
    }

    freq = m_config->get_measure_freq();

    /* Validate measurement frequency*/
    if (! isValidFreq( freq ))
    {
        freq = 14000000ul;
    }

    osl_sel = m_config->get_osl_selected();
    if (osl_sel < 0)
    {
        setErrorMessage("No OSL-calibration file selected");
        return -1;
    }

    clearErrorMessage();

    return 0;
}

void QAAPiMeasurementView::destroy_view()
{
}

int QAAPiMeasurementView::activate_view()
{
    return 0;
}

void QAAPiMeasurementView::deactivate_view()
{
}
