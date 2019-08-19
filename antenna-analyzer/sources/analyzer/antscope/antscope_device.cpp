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

#include <unistd.h>
#include <string.h>
#include <memory>
#include "analyzer/antscope/antscope_device.h"

///////////////////////////////////////////////////////////////////////////////
// AntScope Protocol Definitions
///////////////////////////////////////////////////////////////////////////////

#define RIGEXPERT_VER   "AA-170 201"

#define CMD_VER         "VER"
#define CMD_RF_ON       "ON"
#define CMD_RF_OFF      "OFF"
#define CMD_AM          "AM"	// set output amplitude
#define CMD_PH          "PH"	// set reference DDS phase
#define CMD_DE          "DE"	// set measurement delay, in relative units (1000 by default)
#define CMD_FQ          "FQ"	// set center frequency (Hz)
#define CMD_SW          "SW"	// set sweep range (Hz)
#define CMD_FRX         "FRX"	// return output frequency (MHz), R and X at n+1 steps

#define OK              "OK"
#define ERROR           "ERROR"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// Local utilities
///////////////////////////////////////////////////////////////////////////////

static int is_numeric(const char* text);
static int make_upper(char* text);

///////////////////////////////////////////////////////////////////////////////
// class AntScopeDevice
///////////////////////////////////////////////////////////////////////////////

AntScopeDevice *AntScopeDevice::create(AAPIConfig *config,
										AAPIGenerator* gen,
										bool add_ref)
{
    AntScopeDevice *obj = create(add_ref);
    if( obj ) {
        obj->m_config = config;
        obj->m_gen = gen;

		AAPI_ADDREF(config);
		AAPI_ADDREF(gen);
    }
    return obj;
}

AntScopeDevice::AntScopeDevice()
	: m_modem(nullptr)
	, m_config(nullptr)
	, m_gen(nullptr)
{
}

AntScopeDevice::~AntScopeDevice()
{
	stop();

	AAPI_DISPOSE(m_config);
	AAPI_DISPOSE(m_gen);
}

int AntScopeDevice::start()
{
    AAPtr<AAPISerialModem> modem( AAPISerialModem::create(false) );
    QString 				name	= m_config->get_uart_device();
	struct AAPIUartParams	params;
    int 					ret;

   	if( m_modem != nullptr )
    {
        return AAPI_E_INVALID_STATE;
    }

    /* Serial port is not specified */
    /* We DO NOT throw an error, just return */
    if( name.length() == 0 )
	{
        return 0;
	}

    // Set connection parameters 
	params.open_mode = UART_OM_READ_WRITE;
    params.baud_rate = static_cast<AAPIUartBaudrate>( m_config->get_uart_baudrate() );
	params.char_size = UART_CHR_8BIT;
	params.stop_bits = UART_STOP_1BIT;

    ret = modem->start( name.toLatin1(), &params, this );
    if( AAPI_FAILED( ret ) )
    {
        return ret;
    }

    modem->set_rx_delimit_chars("\r\n");
    modem->set_tx_delimit_chars("\r");

    m_sweep_freq = 0;
    m_center_freq = 0;
    m_modem = modem.detach();

	return AAPI_SUCCESS;
}

void AntScopeDevice::stop()
{
    if( !m_modem )
	{
		return;
	}

	m_modem->stop();
	AAPI_DISPOSE(m_modem);
}

///////////////////////////////////////////////////////////////////////////////
// AAPISerialModemCallback implementation
///////////////////////////////////////////////////////////////////////////////

void AntScopeDevice::serial_modem_command(const char *command)
{
	int res;
    //printf("-------COMMAND RECVD: '%s'\n", p_command);
    uint32_t len = strlen(command);
	char *cmd_text = (char *)alloca(len + 1);

    strcpy(cmd_text, command);
	make_upper(cmd_text);

    if (strcmp(CMD_VER, cmd_text) == 0)
	{
        res = m_modem->send(RIGEXPERT_VER);
		/* Success */
		//res = modem->send(OK);
	}
    else if( strcmp( CMD_RF_ON, cmd_text ) == 0)
	{
        m_gen->set_frequency(m_gen->get_last_frequency());

		/* Success */
		res = m_modem->send(OK);
	}
    else if( strcmp( CMD_RF_OFF, cmd_text ) == 0)
	{
        m_gen->set_frequency(0);

		/* Success */
		res = m_modem->send(OK);
	}
	else if( strcmp( CMD_AM, cmd_text ) == 0
			|| strcmp( CMD_PH, cmd_text ) == 0
			|| strcmp( CMD_DE, cmd_text ) == 0 )
	{
		/* Success */
		res = m_modem->send(OK);
	}
	else if( cmd_text == strstr( cmd_text, CMD_FQ ) )
	{
		/* Check parameter */
		char *param = &cmd_text[2];
		if (!is_numeric(param))
		{
			/* Error */
			res = m_modem->send(ERROR);
		}
		else
		{
			uint32_t fhz = (uint32_t)atoi(param);
            if (fhz < AA_BAND_FMIN)
	        {
	        	/* Error */
	        	res = m_modem->send(ERROR);
	        }
	        else
	        {
	            m_center_freq = fhz;
	            /* Success */
	        	res = m_modem->send(OK);
	        }
		}
	}
	else if( cmd_text == strstr( cmd_text, CMD_SW ) )
	{
		// Check parameter 
		char *param = &cmd_text[2];
		if( !is_numeric( param ) )
		{
			/* Error */
			res = m_modem->send(ERROR);
		}
		else
		{
			m_sweep_freq = (uint32_t)atoi( param );
			/* Success */
			res = m_modem->send(OK);
		}
	}
	else if( cmd_text == strstr( cmd_text, CMD_FRX ) )
	{
		uint32_t steps = 0;
		uint32_t fint;
		uint32_t fstep;
		// Check parameter 
		char *param = &cmd_text[3];
		if( is_numeric( param ) )
		{
			steps = (uint32_t)atoi( param );
		}
		if( steps == 0 )
		{
			/* Error */
			res = m_modem->send(ERROR);
		}
		else
		{
	        if( ( m_sweep_freq / 2 ) > m_center_freq )
	            fint = 10;
	        else
	            fint = m_center_freq - ( m_sweep_freq / 2 );

	        fstep = m_sweep_freq / steps;
	        steps ++;

	        char row[64];
	        while( steps-- )
	        {
	        	//TODO: measure RX

	        	float rx_re = 2.f,
	        			rx_im = 3.f;
	        	sprintf( row, "%.6f,%.2f,%.2f\r", ((float)fint) / 1000000.f, rx_re, rx_im );
	        	fint += fstep;
	        	res = m_modem->send( row );
	        }
	        /* Success */
	        res = m_modem->send(OK);
		}
	}
	else
	{
		/* Unsupported */
		res = m_modem->send(ERROR);
	}
	/*if (res != SUCCESS)
	{
		uart_modem_error(res);
	}*/

}

void AntScopeDevice::serial_modem_error(int error)
{
}

///////////////////////////////////////////////////////////////////////////////
// Local utilities
///////////////////////////////////////////////////////////////////////////////

int is_numeric(const char* text)
{
    size_t len = strlen(text);

    for (size_t i = 0; i < len; i++)
	{
		if (!isdigit(text[i]))
			return 0;
	}
	return 1;
}

int make_upper(char* text)
{
    size_t len = strlen(text);

	/* Copy in-place & uppercase */
    for (size_t i = 0; i < len; i++)
	{
        text[i] = static_cast<char> (toupper(text[i]));
	}
	return len;
}

} // namespace aapi
