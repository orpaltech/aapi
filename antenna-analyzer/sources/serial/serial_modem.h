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

#ifndef AAPI_SERIAL_MODEM_H
#define AAPI_SERIAL_MODEM_H

#include "uart_device.h"

namespace aapi
{

enum serial_modem_error
{
    MODEM_E_CALLBACK_FAILED = SERIAL_MODEM_ERROR_START,
};

///////////////////////////////////////////////////////////////////////////////
// serial_modem_callback
///////////////////////////////////////////////////////////////////////////////

class serial_modem_callback
{
public:
    virtual ~serial_modem_callback() {}

    virtual void serial_modem_command(const char *command) = 0;
    virtual void serial_modem_error(int code) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// serial_modem
///////////////////////////////////////////////////////////////////////////////

class serial_modem
        : public AAPIObject
        , public uart_device_callback
{
    DECLARE_AAPI_OBJECT(serial_modem)

protected:
    serial_modem();
public:
    ~serial_modem();

    const char *get_rx_delimit_chars() const;
    const char *get_tx_delimit_chars() const;
    void set_rx_delimit_chars(const char *delimit_chars);
    void set_tx_delimit_chars(const char *delimit_chars);

    virtual int start(const char *device_name, struct uart_params *params, serial_modem_callback *callback);
	virtual void stop();
    virtual int send(const char *command);

protected:
    virtual void uart_rx_data(const uint8_t *buffer, uint32_t len);
	virtual void uart_error(int error_code);

protected:
    char        *rx_delimit;
    char        *tx_delimit;
    char        *cmd_buff;
    uint32_t    cmd_size;
    aapi_ptr<uart_device>   uart;
    serial_modem_callback   *callback;
};

} // namespace aapi

#endif // AAPI_SERIAL_MODEM_H
