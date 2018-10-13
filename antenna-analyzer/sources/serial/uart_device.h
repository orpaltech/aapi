/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef AAPI_SERIAL_PORT_H
#define AAPI_SERIAL_PORT_H

#include "error_codes.h"
#include "analyzer_object.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// UART enum declarations
///////////////////////////////////////////////////////////////////////////////

enum uart_open_mode {
	UART_OM_READ_ONLY,
	UART_OM_WRITE_ONLY,
	UART_OM_READ_WRITE
};

enum uart_baudrate {
	UART_BR_1200 = 1200u,
	UART_BR_1800 = 1800u,
	UART_BR_2400 = 2400u,
	UART_BR_4800 = 4800u,
	UART_BR_9600 = 9600u,
	UART_BR_19200 = 19200u,
	UART_BR_38400 = 38400u,
	UART_BR_57600 = 57600u,
	UART_BR_115200 = 115200u
};

enum uart_char_size {
	UART_CHR_5BIT = 5,
	UART_CHR_6BIT,
	UART_CHR_7BIT,
	UART_CHR_8BIT
};

enum uart_stop_bits {
	UART_STOP_1BIT = 1,
	UART_STOP_2BIT
};

enum uart_flow_ctrl {
    UART_FCTRL_NONE = 0,
    UART_FCTRL_RTSCTS   = 1
};

enum aapi_uart_device_error {
	/* Unable to open underlying UART hardware*/
    UART_E_OPEN_DEVICE = UART_ERROR_START,
    UART_E_READ_FAILED = UART_ERROR_START - 1,
    UART_E_WRITE_FAILED = UART_ERROR_START - 2,
    UART_E_CALLBACK_FAILED = UART_ERROR_START - 3
};

///////////////////////////////////////////////////////////////////////////////
// uart_params
///////////////////////////////////////////////////////////////////////////////

struct uart_params
{
    uart_params()
        : open_mode ( UART_OM_READ_WRITE )
        , baud_rate ( UART_BR_9600 )
        , char_size ( UART_CHR_8BIT )
        , stop_bits ( UART_STOP_1BIT )
        , flow_control ( UART_FCTRL_NONE )
    { }

    uart_open_mode open_mode;
    uart_baudrate baud_rate;
    uart_char_size char_size;
    uart_stop_bits stop_bits;
    uart_flow_ctrl flow_control;
};

///////////////////////////////////////////////////////////////////////////////
// uart_device_callback
///////////////////////////////////////////////////////////////////////////////

class uart_device_callback
{
public:
    virtual ~uart_device_callback() { }

    virtual void uart_rx_data(const uint8_t *buffer, uint32_t len) = 0;
    virtual void uart_error(int code) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// uart_device
///////////////////////////////////////////////////////////////////////////////

class uart_device
        : public AAPIObject
{
    DECLARE_AAPI_OBJECT(uart_device)

protected:
    uart_device();
public:
    ~uart_device();

    virtual int open(const char *device_name, struct uart_params *params = nullptr);
	virtual void close();
    virtual int start(uart_device_callback *callback);
    virtual void stop();
	virtual bool is_open() const;
    virtual bool is_listening() const;
    virtual int write(const uint8_t *buffer, uint32_t len);

protected:
    virtual int read(uint8_t *buffer, uint32_t len);
};

} //namespace aapi

#endif // AAPI_SERIAL_PORT_H
