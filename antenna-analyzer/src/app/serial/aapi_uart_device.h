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

#ifndef AAPI_UART_DEVICE_H
#define AAPI_UART_DEVICE_H

#include "aapi_error.h"
#include "aapi_object.h"
#include "utils/simple_array.h"
#include "utils/simple_string.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// UART enum declarations
///////////////////////////////////////////////////////////////////////////////

enum AAPIUartOpenMode {
	UART_OM_READ_ONLY,
	UART_OM_WRITE_ONLY,
	UART_OM_READ_WRITE
};

enum AAPIUartBaudrate {
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

enum AAPIUartCharSize {
	UART_CHR_5BIT = 5,
	UART_CHR_6BIT = 6,
	UART_CHR_7BIT = 7,
	UART_CHR_8BIT = 8
};

enum AAPIUartStopBits {
	UART_STOP_1BIT = 1,
	UART_STOP_2BIT = 2
};

enum AAPIUartFlowCtrl {
    UART_FCTRL_NONE     = 0,
    UART_FCTRL_RTSCTS   = 1
};

enum AAPIUartError {
	/* Unable to open underlying UART hardware*/
    AAPI_UART_E_OPEN_DEVICE_FAILED   = (AAPI_UART_ERROR_START - 0),
    AAPI_UART_E_READ_FAILED          = (AAPI_UART_ERROR_START - 1),
    AAPI_UART_E_WRITE_FAILED         = (AAPI_UART_ERROR_START - 2),
    AAPI_UART_E_CALLBACK_FAILED      = (AAPI_UART_ERROR_START - 3),
};

///////////////////////////////////////////////////////////////////////////////
// struct AAPiUartParams
///////////////////////////////////////////////////////////////////////////////

struct AAPiUartParams
{
    AAPiUartParams()
        : open_mode ( UART_OM_READ_WRITE )
        , baud_rate ( UART_BR_9600 )
        , char_size ( UART_CHR_8BIT )
        , stop_bits ( UART_STOP_1BIT )
        , flow_control ( UART_FCTRL_NONE )
    { }

    AAPIUartOpenMode open_mode;
    AAPIUartBaudrate baud_rate;
    AAPIUartCharSize char_size;
    AAPIUartStopBits stop_bits;
    AAPIUartFlowCtrl flow_control;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIUartCallback
///////////////////////////////////////////////////////////////////////////////

class AAPiUartCallback
{
public:
    virtual ~AAPiUartCallback() { }

    virtual void uart_rx_data(const uint8_t *buffer, uint32_t len) = 0;
    virtual void uart_error(int code) = 0;
};

class AAPiUartPrivate;

///////////////////////////////////////////////////////////////////////////////
// class AAPIUart
///////////////////////////////////////////////////////////////////////////////

class AAPiUartDevice : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AAPiUartDevice)

protected:
    AAPiUartDevice();
    ~AAPiUartDevice();

public:
    virtual int open(const char *device_name, struct AAPiUartParams *params = nullptr);
	virtual void close();
    virtual int start(AAPiUartCallback *callback);
    virtual void stop();
	virtual bool is_open() const;
    virtual bool is_listening() const;
    virtual int write(const uint8_t *buffer, uint32_t len);

public:
    static int enumerate(AAPiArray<AAPiString>& device_names);

protected:
    virtual int read(uint8_t *buffer, uint32_t len);

private:
    AAPiUartPrivate *m_priv;
};

} //namespace aapi

#endif // AAPI_UART_DEVICE_H
