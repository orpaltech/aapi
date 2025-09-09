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

#ifndef AAPI_SERIAL_MODEM_H
#define AAPI_SERIAL_MODEM_H

#include "aapi_uart_device.h"

namespace aapi
{

enum AAPISerialModemError
{
    AAPI_SMODEM_E_CALLBACK_FAILED = AAPI_SMODEM_ERROR_START,
};

///////////////////////////////////////////////////////////////////////////////
// class AAPISerialModemCallback
///////////////////////////////////////////////////////////////////////////////

class AAPISerialModemCallback
{
public:
    virtual ~AAPISerialModemCallback() {}

    virtual void onSerialModemCommand(const char *command) = 0;
    virtual void onSerialModemError(int error) = 0;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPISerialModem
///////////////////////////////////////////////////////////////////////////////

class AAPISerialModem : public AAPiObject,
                        public AAPiUartCallback
{
    DECLARE_AAPI_OBJECT(AAPISerialModem)

protected:
    AAPISerialModem();
    ~AAPISerialModem();

public:
    const char *get_rx_delimit_chars() const;
    const char *get_tx_delimit_chars() const;
    void set_rx_delimit_chars(const char *delimit_chars);
    void set_tx_delimit_chars(const char *delimit_chars);

    virtual int start(const char *device_name, struct AAPiUartParams *params, AAPISerialModemCallback *callback);
	virtual void stop();
    virtual int send(const char *command);

private:
// AAPIUartCallback
    virtual void uart_rx_data(const uint8_t *buffer, uint32_t len);
	virtual void uart_error(int error_code);

private:
    char        *rx_delimit;
    char        *tx_delimit;
    char        *cmd_buff;
    uint32_t    cmd_size;

    AAPiUartDevice          *m_uart;
    AAPISerialModemCallback *m_callback;
};

} // namespace aapi

#endif // AAPI_SERIAL_MODEM_H
