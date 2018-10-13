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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <memory>
#include <iostream>
#include <string>
#include <algorithm>
#include "serial_modem.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define MAX_RX_DELIMIT_CHARS	10
#define MAX_TX_DELIMIT_CHARS	2
#define DEFAULT_DELIMIT_CHARS	"\r\n"

#ifndef min
	#define min(a,b) ((a)<(b)?(a):(b))
#endif

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// serial_modem
///////////////////////////////////////////////////////////////////////////////

serial_modem::serial_modem()
{
	this->callback	= nullptr;
	// allocate delimit char memory
    this->rx_delimit =
            reinterpret_cast<char *> (malloc (MAX_RX_DELIMIT_CHARS + 1));
    this->tx_delimit =
            reinterpret_cast<char *> (malloc (MAX_TX_DELIMIT_CHARS + 1));
	set_rx_delimit_chars(nullptr);
	set_tx_delimit_chars(nullptr);
	// allocate command buffer
	this->cmd_size = 260;
    this->cmd_buff = reinterpret_cast<char *>(malloc(260));
	this->cmd_buff[0] = '\0';
}

serial_modem::~serial_modem()
{
	stop();

	// free delimit char memory
    free (this->rx_delimit);
    free (this->tx_delimit);

    free (this->cmd_buff);
}

const char *serial_modem::get_rx_delimit_chars() const
{
	return this->rx_delimit;
}

void serial_modem::set_rx_delimit_chars(const char *delimit_chars)
{
    size_t len;

    if (!delimit_chars)
        delimit_chars = DEFAULT_DELIMIT_CHARS;

    len = min(MAX_RX_DELIMIT_CHARS, strlen (delimit_chars));
    strncpy (this->rx_delimit, delimit_chars, len);
    this->rx_delimit[len] = '\0';

}

const char *serial_modem::get_tx_delimit_chars() const
{
	return this->tx_delimit;
}

void serial_modem::set_tx_delimit_chars(const char *delimit_chars)
{
    size_t len;

    if (!delimit_chars)
        delimit_chars = DEFAULT_DELIMIT_CHARS;

    len = min(MAX_TX_DELIMIT_CHARS, strlen (delimit_chars));
    strncpy (this->tx_delimit, delimit_chars, len);
    this->tx_delimit[len] = '\0';
}

int serial_modem::start(const char *device_name, struct uart_params *params,
                        serial_modem_callback *callback)
{
    aapi_ptr<uart_device> uart;
    int ret;

    if( this->uart )
    {
        return AAPI_E_INVALID_STATE;
    }

    if( !device_name )
    {
        return AAPI_E_BAD_POINTER;
    }

	// allocate uart object
    uart.attach(uart_device::create());
    if( !uart )
    {
        return AAPI_E_OUT_OF_MEMORY;
    }

	// open uart
    ret = uart->open(device_name, params);
    if (AAPI_FAILED(ret))
    {
        return ret;
    }

	// start asynchronous input
    ret = uart->start(this);
    if (AAPI_FAILED(ret))
    {
        return ret;
    }

    this->callback = callback;
    this->uart  = uart;

	return 0;
}

void serial_modem::stop()
{
    if (this->uart)
	{
		this->uart->stop();
		this->uart->close();
	}

	this->callback	= nullptr;
	this->uart		= nullptr;
}

int serial_modem::send(const char *command)
{
    char *buffer;
    int ret;
    size_t len;

    if( !this->uart )
    {
        return AAPI_E_INVALID_STATE;
    }

    if (command == nullptr || (len = strlen(command)) == 0)
    {
        return AAPI_E_INVALID_ARG;
    }

    buffer = reinterpret_cast<char *>(
                alloca (len + MAX_TX_DELIMIT_CHARS + 1));
    strcpy (buffer, command);
	strcat (buffer, tx_delimit);

    ret = uart->write (reinterpret_cast<uint8_t*>(buffer),
                       strlen(buffer));
    if (AAPI_FAILED(ret))
    {
        return ret;
    }

    //printf("------- UART MODEM SENT: '%s'\n", p_command);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// uart_device_callback
///////////////////////////////////////////////////////////////////////////////

void serial_modem::uart_rx_data(const uint8_t *buffer, uint32_t len)
{
    if (!callback)
		return;

    if (len == 0)
		return;

	// remove any delimit symbols and route the command to modem callback
	std::string chars(this->rx_delimit);

    for (unsigned i = 0; i < len; i++)
	{
        char ch = static_cast<char>(buffer[i]);
		size_t len = strlen (cmd_buff);

        if (ch == 0 || chars.find(ch) != std::string::npos)
		{
			// delimiter character received
			if (len > 0) 
			{
				// there is an outstanding command, route the command to callback
				// and clear the command buffer
				try {
                    callback->serial_modem_command (cmd_buff);
				} catch (...) {
					try {
                        callback->serial_modem_error (MODEM_E_CALLBACK_FAILED);
					} catch (...) {
						// fatal error
					}
				}

				// reset command buffer
				cmd_buff[0]='\0';
			}
		} 
		else 
		{
			// this is a normal data character
			if (len + 1 > cmd_size -1) 
			{
				// need to reallocate the command buffer
                unsigned new_size = cmd_size + 260;
                char *new_buff = reinterpret_cast<char *>(realloc(cmd_buff, new_size));
                if (new_buff)
				{
					cmd_buff = new_buff;
					cmd_size = new_size;
                }
				else 
				{
					try {
                        callback->serial_modem_error( AAPI_E_OUT_OF_MEMORY );
					} catch (...) {
						// fatal error
					}
				}
			}
			// append character to the command buffer
            strncat (cmd_buff, &ch, 1);
		}
	}
}

void serial_modem::uart_error(int error)
{
    if (!callback)
		return ;

	try {
        callback->serial_modem_error (error);
	} catch (...) {
		// fatal error
	}
}

} //namespace aapi
