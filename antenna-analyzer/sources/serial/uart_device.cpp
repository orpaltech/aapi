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
#include <map>
#include <mutex>
#include "uart_device.h"

namespace aapi
{
class uart_context;

typedef std::map<const uart_device*, uart_context*> uart_context_cache;
typedef std::recursive_mutex uart_context_mutex;

///////////////////////////////////////////////////////////////////////////////
// uart_context
///////////////////////////////////////////////////////////////////////////////

class uart_context
{
    friend class uart_device;
public:
    uart_context()
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
    uart_device_callback *callback;

    static uart_context *get(const uart_device *dev);
    static void set(const uart_device *dev, uart_context *ctx);
    static bool remove(const uart_device *dev);

private:
    void thread_worker();
    static void *thread_func (void *arg);
	static void io_sig_handler (int status);

    static uart_context_cache context_cache;
    static uart_context_mutex context_mtx;
};

uart_context_cache uart_context::context_cache;
uart_context_mutex uart_context::context_mtx;

///////////////////////////////////////////////////////////////////////////////
// uart_device
///////////////////////////////////////////////////////////////////////////////

uart_device::uart_device()
{
}

uart_device::~uart_device()
{
	close();
}

int uart_device::open(const char *device_name, struct uart_params* uart_params)
{
	int fd;
	int open_flag = O_RDONLY;
	struct sigaction saio;           /* definition of signal action */
    uart_context *ctx;
    struct uart_params params;
    uint32_t baudrate = B9600;
    uint32_t char_size = CS8;
    uint32_t stop_bits = 0;

    if( !device_name )
    {
        return AAPI_E_BAD_POINTER;
    }

    if( !uart_params )
    {
        uart_params = &params;
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

	/* open the device to be non-blocking (read will return immediately) */
    fd = ::open( device_name, open_flag | O_NOCTTY | O_NONBLOCK );
    if( fd < 0 )
    {
		// ERROR - CAN'T OPEN SERIAL PORT
		// Error - Unable to open UART.  Ensure it is not in use by another application
        return UART_E_OPEN_DEVICE;
	}

	/* install the signal handler before making the device asynchronous */
    memset(&saio, 0, sizeof(saio));
    saio.sa_handler = uart_context::io_sig_handler;
    sigaction( SIGIO, &saio, nullptr );

	/* allow the process to receive SIGIO */
    fcntl( fd, F_SETOWN, getpid() );

    /* make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
    fcntl( fd, F_SETFL, fcntl(fd, F_GETFL) | O_ASYNC );

    switch (uart_params->baud_rate)
    {
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

    switch (uart_params->char_size)
    {
    case UART_CHR_5BIT: baudrate = CS5; break;
    case UART_CHR_6BIT: baudrate = CS6; break;
    case UART_CHR_7BIT: baudrate = CS7; break;
    case UART_CHR_8BIT: baudrate = CS8; break;
	}

    switch (uart_params->stop_bits) {
    case UART_STOP_1BIT: break;
    case UART_STOP_2BIT: stop_bits = CSTOPB; break;
	}

    ctx = new uart_context();
    ctx->fd = fd;
    ctx->baudrate = baudrate;
    ctx->char_size = char_size;
    ctx->stop_bits = stop_bits;
    ctx->flow_control = ( uart_params->flow_control == UART_FCTRL_RTSCTS
                          ? CRTSCTS : 0 );

    uart_context::set(this, ctx);

	return 0;
}

void uart_device::close()
{
    uart_context *ctx = uart_context::get(this);

    //----- CLOSE -----
    if (ctx != nullptr)
	{
		// stop listener
		stop();

        // close the file descriptor
        if (ctx->fd != -1)
		{
            ::close(ctx->fd);
            ctx->fd = -1;
		}
	}

	// remove from the context cache
    uart_context::remove(this);
}

int uart_device::write(const uint8_t *buffer, uint32_t length)
{
    uart_context *ctx = uart_context::get(this);

	//----- TX BYTES -----
    if (ctx && ctx->fd != -1)
	{
        // Filestream, bytes to write, number of bytes to write
        ssize_t tx_len = ::write(ctx->fd, buffer, length);
        if (tx_len < 0)
		{
			// UART TX error
            return UART_E_WRITE_FAILED;
		}

        return tx_len;
	}

    return AAPI_E_INVALID_STATE;
}

int uart_device::read(uint8_t *buffer, uint32_t length)
{
    uart_context *ctx = uart_context::get(this);

	//----- CHECK FOR ANY RX BYTES -----
    if (ctx && ctx->fd != -1)
	{
		// Read characters from the port if they are there
        // Filestream, buffer to store in, number of bytes to read (max)
        ssize_t rx_len = ::read(ctx->fd, buffer, length);
        if (rx_len < 0)
		{
			// An error occurred (will occur if there are no bytes)
            return UART_E_READ_FAILED;
		}
        if (rx_len == 0)
		{
			// No data waiting
			return 0;
		}

        // Bytes received; set zero after the last charaster
        if (rx_len < static_cast<ssize_t> (length))
        {
            buffer[rx_len] = 0;
        }

        return rx_len;
	}

    return AAPI_E_INVALID_STATE;
}

void uart_context::thread_worker()
{
    ssize_t res;
    uint8_t* rx_buf = reinterpret_cast<uint8_t*> (alloca(rx_bufsize));
	struct termios oldtio, newtio;
    struct pollfd pfd;

    tcgetattr(this->fd, &oldtio); /* save current port settings */

    /* set new port settings for canonical input processing */
	/* The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html) */
    newtio.c_cflag = this->baudrate     /* desired baud rate */
                    | this->flow_control
                    | this->char_size   /* Character size (in bits) */
                    | this->stop_bits   /* Stop bits (1 or 2) */
                    | CLOCAL        /* Ignore modem status lines, i.e. CD */
                    | CREAD         /* Enable receiver */
					;
    newtio.c_iflag = IGNPAR         /*Ignore characters with parity errors*/
                    | ICRNL         /*Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for binary comms!)*/
					;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_line = 0;
	newtio.c_ispeed = 0;
	newtio.c_ospeed = 0;
    newtio.c_cc[VMIN] = 1; // http://www.unixwiz.net/techtips/termios-vmin-vtime.html
    newtio.c_cc[VTIME] = 1;
    tcflush(this->fd, TCIFLUSH);
    tcsetattr(this->fd, TCSANOW, &newtio);

    /* loop while waiting for input */
    while (!this->stopped)
    {
        usleep(100000); //100msec

        /* after receiving SIGIO, ready flag = true, input is available
          and can be read */
        if (this->io_ready)
        {
			/* poll parameters */
			pfd.events = POLLRDNORM|POLLRDBAND;
			pfd.revents = 0;
            pfd.fd = this->fd;

			/* poll the file descriptor */
			res = poll(&pfd, 1, 1);
            if (res > 0)
            {
				/* An event on the file descriptor has occurred. */
                while ((res = read(this->fd, rx_buf, this->rx_bufsize-1)) > 0)
                {
                    rx_buf[res] = 0; // set zero in the end
                    if (callback)
                        callback->uart_rx_data(rx_buf,
                                    static_cast<uint32_t> (res));
				}
                if (res < 0)
                {
                    // look at errno
                    if (callback)
                        callback->uart_error(errno);
				}
            }
            else if (res < 0)
            {
                // look at errno
                if (callback)
                    callback->uart_error(errno);
			}
            this->io_ready = false;		/* wait for new input */
		}
	}

	/* restore old port settings */
    tcsetattr(this->fd, TCSANOW, &oldtio);
}

void *uart_context::thread_func(void *arg)
{
    uart_context *ctx = reinterpret_cast<uart_context *>(arg);
    assert(ctx);
    ctx->thread_worker();
    return nullptr;
}

uart_context *uart_context::get(const uart_device *key)
{
    std::lock_guard<uart_context_mutex> lock(context_mtx);
	return context_cache[key];
}

void uart_context::set(const uart_device *key, uart_context *c)
{
    std::lock_guard<uart_context_mutex> lock(context_mtx);
	context_cache[key] = c;
}

bool uart_context::remove(const uart_device *key)
{
    std::lock_guard<uart_context_mutex> lock(context_mtx);
    uart_context_cache::iterator iter = context_cache.find(key);
    if (iter != context_cache.end())
    {
		delete iter->second;
		context_cache.erase(iter);
		return true;
	}
	return false;
}

/***************************************************************************
* Signal handler. Sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/

void uart_context::io_sig_handler(int status)
{
	(void)status;
    std::lock_guard<uart_context_mutex> lock(context_mtx);
    uart_context_cache::iterator iter = context_cache.begin();
    for (; iter != context_cache.end(); ++iter)
    {
		iter->second->io_ready = true;
	}
}

// http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

int uart_device::start(uart_device_callback *callback)
{
    uart_context *ctx = uart_context::get(this);
    int ret;

    if (ctx && ctx->pth == 0)
	{
        ctx->callback = callback;
        ret = pthread_create( &ctx->pth, nullptr, &uart_context::thread_func, ctx );
        if( ret )
        {
            return AAPI_E_CREATE_THREAD;
        }

		return 0;
	}

    return AAPI_E_INVALID_STATE;
}

void uart_device::stop()
{
    uart_context *ctx = uart_context::get(this);

    if (ctx && ctx->pth != 0)
	{
		// set stop flag
        ctx->stopped = true;
		// wait for worker thread to complete
        pthread_join(ctx->pth, nullptr);
		// reset thread handle
        ctx->pth = 0;
	}
}

bool uart_device::is_open() const
{
    uart_context *ctx = uart_context::get(this);

    return ( ctx != nullptr );
}

bool uart_device::is_listening() const
{
    uart_context *ctx = uart_context::get(this);

    return ( ctx && ctx->pth != 0 );
}

} // namespace aapi
