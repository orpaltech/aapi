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

#ifndef AAPI_ERROR_CODE_H
#define AAPI_ERROR_CODE_H

namespace aapi
{

enum AAPIError {
    AAPI_SUCCESS = 0,
    AAPI_E_FAILURE = -1,
    AAPI_E_OUT_OF_MEMORY = -2,
    AAPI_E_INVALID_ARG = -3,
    AAPI_E_BAD_POINTER = -4,
    AAPI_E_INVALID_STATE = -5,
    AAPI_E_CREATE_THREAD_FAILED = -6,
    AAPI_E_RESOURCE_LOCKED = -7,
    AAPI_E_CREATE_DIR_FAILED = -8,
};

} //namespace aapi

#define CONFIG_ERROR_START              -50
#define UART_ERROR_START                -100
#define SERIAL_MODEM_ERROR_START        -150
#define ANTSCOPE_ERROR_START            -200
#define AUDIO_ERROR_START               -250
#define AAPI_DSP_ERROR_START            -300
#define AAPI_GEN_ERROR_START            -350
#define AAPI_CALIB_ERROR_START          -400

#define AAPI_SUCCESS(rc)        ((rc) >= AAPI_SUCCESS)
#define AAPI_FAILED(rc)         ((rc) < AAPI_SUCCESS)

#endif // AAPI_ERROR_CODE_H
