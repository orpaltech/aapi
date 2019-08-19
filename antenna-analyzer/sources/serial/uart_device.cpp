/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc.
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
#include <unistd.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <fcntl.h>
#include <poll.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <memory.h>
#include <pthread.h>
#include <assert.h>
#include <list>
#include <mutex>
#include "uart_device.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPIUartContext
///////////////////////////////////////////////////////////////////////////////

class AAPIUartContext
{
    friend class AAPIUart;

    typedef std::list<AAPIUartContext*> TCache;
    typedef std::recursive_mutex TMutex;

public:
    AAPIUartContext()
        : fd ( -1 )
        , baudrate ( 0 )
        , char_size ( 0 )
        , rx_bufsize ( 260 )
        , pth ( 0 )
        , stopped ( false )
        , io_ready ( false )
        , callback ( nullptr )
    { }

    int	fd;
    uint32_t baudrate;
    uint32_t char_size;
    uint32_t stop_bits;
    uint32_t flow_control;
    size_t rx_bufsize;
	pthread_t pth;
	volatile bool stopped;
	volatile bool io_ready;
    AAPIUartCallback *callback;

    static void set(AAPIUartContext *ctx);
    static bool remove(AAPIUartContext *ctx);

private:
    void thread_worker();
    static void *thread_func (void *arg);
	static void io_sig_handler (int status);

    static TCache m_cache;
    static TMutex m_mtx;
};

AAPIUartContext::TCache AAPIUartContext::m_cache;
AAPIUartContext::TMutex AAPIUartContext::m_mtx;

///////////////////////////////////////////////////////////////////////////////
// class AAPIUart
///////////////////////////////////////////////////////////////////////////////

AAPIUart::AAPIUart()
{
    m_priv = nullptr;
}

AAPIUart::~AAPIUart()
{
	close();
}

int AAPIUart::open(const char *device_name, struct AAPIUartParams* uart_params)
{
    int                     fd;
	int                     open_flag = O_RDONLY;
	struct sigaction        saio;           /* definition of signal action */
    uint32_t                baudrate = B9600;
    uint32_t                char_size = CS8;
    uint32_t                stop_bits = 0;
    struct AAPIUartParams   loc_params;
    AAPIUartContext         *ctx;

    if( m_priv )
    {
        return AAPI_E_INVALID_STATE;
    }

    if( !device_name )
    {
        return AAPI_E_BAD_POINTER;
    }

    if( !uart_params )
    {
        uart_params = &loc_params;
    }

    switch( uart_params->open_mode )
    {
    case UART_OM_READ_ONLY:
        break;
    case UART_OM_WRITE_ONLY:
        open_flag = O_WRONLY;
        break;
    case UART_OM_READ_WRITE:
        open_flag = O_RDWR;
        break;
	}

	// open the device to be non-blocking (read will return immediately) 
    if( ( fd = ::open( device_name, open_flag | O_NOCTTY | O_NONBLOCK ) ) < 0 )
    {
		// Unable to open UART. Ensure it is not in use by another application
        return AAPI_UART_E_OPEN_DEVICE_FAILED;
	}

	/* install the signal handler before making the device asynchronous */
    memset( &saio, 0, sizeof( saio ) );
    saio.sa_handler = AAPIUartContext::io_sig_handler;
    sigaction( SIGIO, &saio, nullptr );

	/* allow the process to receive SIGIO */
    fcntl( fd, F_SETOWN, getpid() );

    /* make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    fcntl( fd, F_SETFL, fcntl(fd, F_GETFL) | O_ASYNC );

    switch (uart_params->baud_rate) {
    case UART_BR_1200: baudrate = B1200; break;
    case UART_BR_1800: baudrate = B1800; break;
    case UART_BR_2400: baudrate = B2400; break;
    case UART_BR_4800: baudrate = B4800; break;
    case UART_BR_9600: baudrate = B9600; break;
    case UART_BR_19200: baudrate = B19200; break;
    case UART_BR_38400: baudrate = B38400; break;
    case UART_BR_57600: baudrate = B57600; break;
    case UART_BR_115200: baudrate = B115200; break;
	}

    switch (uart_params->char_size) {
    case UART_CHR_5BIT: baudrate = CS5; break;
    case UART_CHR_6BIT: baudrate = CS6; break;
    case UART_CHR_7BIT: baudrate = CS7; break;
    case UART_CHR_8BIT: baudrate = CS8; break;
	}

    switch (uart_params->stop_bits) {
    case UART_STOP_1BIT: break;
    case UART_STOP_2BIT: stop_bits = CSTOPB; break;
	}

    ctx = new AAPIUartContext();
    ctx->fd = fd;
    ctx->baudrate = baudrate;
    ctx->char_size = char_size;
    ctx->stop_bits = stop_bits;
    ctx->flow_control = ( uart_params->flow_control == UART_FCTRL_RTSCTS
                          ? CRTSCTS : 0 );

    m_priv = ctx;
    // store private context to global cache
    AAPIUartContext::set( ctx );

	return AAPI_SUCCESS;
}

void AAPIUart::close()
{
    //----- CLOSE -----
    if( !is_open() )
    {
        return;
    }

    // stop listener
    stop();

    // close the file descriptor
    if( m_priv->fd != -1 )
    {
        ::close( m_priv->fd );
        m_priv->fd = -1;
    }

    // remove the context from cache
    AAPIUartContext::remove( m_priv );

    // destroy the context
    delete m_priv;
    m_priv = nullptr;
}

int AAPIUart::write(const uint8_t *buffer, uint32_t length)
{
    //----- TX BYTES -----
    if( !is_open() )
    {
        return AAPI_E_INVALID_STATE;
    }

    // Filestream, bytes to write, number of bytes to write
    ssize_t tx_len = ::write( m_priv->fd, buffer, length );
    if( tx_len < 0 )
    {
        // UART TX error
        return AAPI_UART_E_WRITE_FAILED;
    }

    return tx_len;
}

int AAPIUart::read(uint8_t *buffer, uint32_t length)
{
    //----- CHECK FOR ANY RX BYTES -----
    if( !is_open() )
    {
        return AAPI_E_INVALID_STATE;
    }

    // Read characters from the port if they are there
    // Filestream, buffer to store in, number of bytes to read (max)
    ssize_t rx_len = ::read( m_priv->fd, buffer, length );
    if( rx_len < 0 )
    {
        // An error occurred (will occur if there are no bytes)
        return AAPI_UART_E_READ_FAILED;
    }
    if( rx_len == 0 )
    {
        // No data pending
        return 0;
    }

    // Bytes received; set zero after the last charaster
    if( rx_len < static_cast<ssize_t> (length) )
    {
        buffer[ rx_len ] = 0;
    }

    return rx_len;
}

void AAPIUartContext::thread_worker()
{
    ssize_t res;
    uint8_t* rx_buf = reinterpret_cast< uint8_t *> ( alloca( rx_bufsize ) );
	struct termios oldtio, newtio;
    struct pollfd pfd;

    tcgetattr( this->fd, &oldtio ); // save current port settings 

    /* set new port settings for canonical input processing */
	/* The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html) */
    newtio.c_cflag = this->baudrate     /* desired baud rate */
                    | this->flow_control
                    | this->char_size   /* Character size (in bits) */
                    | this->stop_bits   /* Stop bits (1 or 2) */
                    | CLOCAL            /* Ignore modem status lines, i.e. CD */
                    | CREAD             /* Enable receiver */
					;
    newtio.c_iflag = IGNPAR             /*Ignore characters with parity errors*/
                    | ICRNL             /*Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for binary comms!)*/
					;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_line = 0;
	newtio.c_ispeed = 0;
	newtio.c_ospeed = 0;
    newtio.c_cc[VMIN] = 1;  // see http://www.unixwiz.net/techtips/termios-vmin-vtime.html
    newtio.c_cc[VTIME] = 1;
    tcflush(this->fd, TCIFLUSH);
    tcsetattr(this->fd, TCSANOW, &newtio);

    // loop while waiting for input 
    while (!this->stopped)
    {
        usleep(100000); //wait 100msec

        // after receiving SIGIO, ready flag = true, input is available
        // and can be read 
        if (this->io_ready)
        {
			// poll parameters 
			pfd.events = POLLRDNORM|POLLRDBAND;
			pfd.revents = 0;
            pfd.fd = this->fd;

			// poll the file descriptor 
			res = poll( &pfd, 1, 1 );
            if (res > 0)
            {
				// An event on the file descriptor has occurred. 
                while( ( res = read( this->fd, rx_buf, this->rx_bufsize-1 ) ) > 0 )
                {
                    rx_buf[res] = 0; // set zero in the end
                    if( callback )
                        callback->uart_rx_data( rx_buf,
                                    static_cast<uint32_t> (res));
				}
                if( res < 0 )
                {
                    // look at errno
                    if( callback )
                        callback->uart_error( errno );
				}
            }
            else if( res < 0 )
            {
                // look at errno
                if( callback )
                    callback->uart_error( errno );
			}
            this->io_ready = false;		/* wait for new input */
		}
	}

	// restore old port settings 
    tcsetattr( this->fd, TCSANOW, &oldtio );
}

void *AAPIUartContext::thread_func(void *arg)
{
    AAPIUartContext *priv = reinterpret_cast< AAPIUartContext *>( arg );
    assert( priv );
    priv->thread_worker();
    return nullptr;
}

void AAPIUartContext::set( AAPIUartContext *ctx )
{
    std::lock_guard<AAPIUartContext::TMutex> lock( m_mtx );
	m_cache.push_back( ctx );
}

bool AAPIUartContext::remove( AAPIUartContext *ctx )
{
    std::lock_guard<AAPIUartContext::TMutex> lock( m_mtx );
    AAPIUartContext::TCache::const_iterator iter = m_cache.begin();
    for ( ; iter != m_cache.end(); ++iter )
    {
        if( *iter == ctx )
        {
            m_cache.erase( iter );
            return true;
        }
	}
	return false;
}

/***************************************************************************
* Signal handler. Sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/

void AAPIUartContext::io_sig_handler (int status)
{
	(void)status;
    std::lock_guard<AAPIUartContext::TMutex> lock( m_mtx );
    AAPIUartContext::TCache::iterator iter = m_cache.begin();
    for (; iter != m_cache.end(); ++iter)
    {
		(*iter)->io_ready = true;
	}
}

// http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

int AAPIUart::start(AAPIUartCallback *callback)
{
    if( !is_open() )
    {
        return AAPI_E_INVALID_STATE;
    }

    m_priv->callback = callback;
    if( pthread_create( &m_priv->pth, nullptr,
                        &AAPIUartContext::thread_func,
                        m_priv ) )
    {
        return AAPI_E_CREATE_THREAD_FAILED;
    }

    return AAPI_SUCCESS;
}

void AAPIUart::stop()
{
    if( !is_open() )
    {
        return;
    }

    // set stop flag
    m_priv->stopped = true;
    // wait for worker thread to complete
    pthread_join( m_priv->pth, nullptr );
    // reset thread handle
    m_priv->pth = 0;
}

bool AAPIUart::is_open() const
{
    return ( m_priv != nullptr );
}

bool AAPIUart::is_listening() const
{
    return is_open() && ( m_priv->pth != 0 );
}

} // namespace aapi
