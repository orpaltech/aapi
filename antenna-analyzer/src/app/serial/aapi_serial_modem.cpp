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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include "aapi_serial_modem.h"

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
// AAPISerialModem
///////////////////////////////////////////////////////////////////////////////

AAPISerialModem::AAPISerialModem()
{
    m_uart      = nullptr;
	m_callback  = nullptr;
	// allocate delimit char memory
    rx_delimit  = reinterpret_cast<char *> (malloc (MAX_RX_DELIMIT_CHARS + 1));
    tx_delimit  = reinterpret_cast<char *> (malloc (MAX_TX_DELIMIT_CHARS + 1));
	set_rx_delimit_chars(nullptr);
	set_tx_delimit_chars(nullptr);
	// allocate command buffer
	cmd_size    = 260;
    cmd_buff    = reinterpret_cast<char *>(malloc(260));
	cmd_buff[0] = '\0';
}

AAPISerialModem::~AAPISerialModem()
{
	stop();

	// free delimit char memory
    free( this->rx_delimit );
    free( this->tx_delimit );

    free( this->cmd_buff );

    AAPI_DISPOSE( m_uart );
}

const char *AAPISerialModem::get_rx_delimit_chars() const
{
	return rx_delimit;
}

void AAPISerialModem::set_rx_delimit_chars(const char *delimit_chars)
{
    size_t len;

    if (! delimit_chars)
    {
        delimit_chars = DEFAULT_DELIMIT_CHARS;
    }

    len = min(MAX_RX_DELIMIT_CHARS, strlen (delimit_chars));
    strncpy (this->rx_delimit, delimit_chars, len);
    this->rx_delimit[len] = '\0';
}

const char *AAPISerialModem::get_tx_delimit_chars() const
{
	return tx_delimit;
}

void AAPISerialModem::set_tx_delimit_chars(const char *delimit_chars)
{
    size_t len;

    if (! delimit_chars)
    {
        delimit_chars = DEFAULT_DELIMIT_CHARS;
    }

    len = min(MAX_TX_DELIMIT_CHARS, strlen (delimit_chars));
    strncpy (this->tx_delimit, delimit_chars, len);
    this->tx_delimit[len] = '\0';
}

int AAPISerialModem::start(const char *device_name, struct AAPiUartParams *params,
                            AAPISerialModemCallback *callback)
{
    if (m_uart != nullptr)
    {
        return AAPI_E_INVALID_STATE;
    }

    if (device_name == nullptr)
    {
        return AAPI_E_BAD_POINTER;
    }

	// allocate uart object
    AAPiPtr<AAPiUartDevice> uart(AAPiUartDevice::create(false));
    if (uart == nullptr)
    {
        return AAPI_E_OUT_OF_MEMORY;
    }

	// open uart
    int ret = uart->open( device_name, params );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

	// start asynchronous input
    ret = uart->start( this );
    if (AAPI_FAILED( ret ))
    {
        return ret;
    }

    m_callback = callback;
    m_uart  = uart.detach();

	return 0;
}

void AAPISerialModem::stop()
{
    if (m_uart != nullptr)
	{
		m_uart->stop();
		m_uart->close();
        m_uart->release();
	}

	m_callback	= nullptr;
	m_uart		= nullptr;
}

int AAPISerialModem::send(const char *command)
{
    char    *buffer;
    int     ret;
    size_t  len;

    if (m_uart == nullptr)
    {
        return AAPI_E_INVALID_STATE;
    }

    if (command == nullptr || (len = strlen( command )) == 0)
    {
        return AAPI_E_INVALID_ARG;
    }

    buffer = reinterpret_cast< char *>(alloca( len + MAX_TX_DELIMIT_CHARS + 1 ));
    strcpy( buffer, command );
	strcat( buffer, tx_delimit );

    ret = m_uart->write( reinterpret_cast< uint8_t *>( buffer ), strlen( buffer ));
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    //printf("------- UART MODEM SENT: '%s'\n", p_command);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// AAPIUartCallback implementation
///////////////////////////////////////////////////////////////////////////////

void AAPISerialModem::uart_rx_data(const uint8_t *buffer, uint32_t len)
{
    if (! m_callback)
		return;

    if (len == 0)
		return;

	// remove any delimit symbols and route the command to modem callback
	std::string chars( this->rx_delimit );

    for (unsigned i = 0; i < len; i++)
	{
        char ch = static_cast< char >( buffer[i] );
		size_t len = strlen( cmd_buff );

        if (ch == 0 || chars.find(ch) != std::string::npos)
		{
			// delimiter character received
			if( len > 0 )
			{
				// there is an outstanding command, route the command to callback
				// and clear the command buffer
                try
                {
                    m_callback->onSerialModemCommand( cmd_buff );
                }
                catch (...)
                {
                    // ignore this error
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
                char *new_buff = reinterpret_cast< char *>(realloc( cmd_buff, new_size ));
                if (new_buff)
				{
					cmd_buff = new_buff;
					cmd_size = new_size;
                }
				else
				{
                    try
                    {
                        m_callback->onSerialModemError( AAPI_E_OUT_OF_MEMORY );
                    }
                    catch (...)
                    {
						// ignore error
					}
				}
			}
			// append character to the command buffer
            strncat( cmd_buff, &ch, 1 );
		}
	}
}

void AAPISerialModem::uart_error(int error)
{
    if( !m_callback )
		return;

	try {
        m_callback->onSerialModemError( error );
	} catch (...) {
		// fatal error
	}
}

} //namespace aapi
