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

#ifndef AAPI_ERROR_CODE_H
#define AAPI_ERROR_CODE_H

namespace aapi
{

/*
 * Basic error codes
 */
enum AAPiError {
    AAPI_SUCCESS                = 0,
    AAPI_E_FAILURE              = -1,
    AAPI_E_CANCELLED            = -2,
    AAPI_E_OUT_OF_MEMORY        = -3,
    AAPI_E_INVALID_ARG          = -4,
    AAPI_E_BAD_POINTER          = -5,
    AAPI_E_NOT_IMPLEMENTED      = -6,
    AAPI_E_INVALID_STATE        = -7,
    AAPI_E_CREATE_THREAD_FAILED = -8,
    AAPI_E_RESOURCE_LOCKED      = -9,
    AAPI_E_CREATE_DIR_FAILED    = -10,
    AAPI_E_OPEN_DIR_FAILED      = -11,
};

}; //namespace aapi

/*
 * Error categories
 */
#define AAPI_CONF_ERROR_START           -100
#define AAPI_UART_ERROR_START           -200
#define AAPI_SMODEM_ERROR_START         -300
#define AAPI_ANTSCOPE_ERROR_START       -400
#define AAPI_AUDIO_ERROR_START          -500
#define AAPI_DSP_ERROR_START            -600
#define AAPI_GENERATOR_ERROR_START      -700
#define AAPI_CALIBRATOR_ERROR_START     -800

#define AAPI_SUCCESS(rc)        ((rc) >= AAPI_SUCCESS)
#define AAPI_FAILED(rc)         ((rc) < AAPI_SUCCESS)

#endif // AAPI_ERROR_CODE_H
