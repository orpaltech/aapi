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

#include "aapi_device.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <QDebug>
//#include "utils/simple_map.h"


namespace aapi
{

IMPLEMENT_AAPI_OBJECT(AAPiDevice)

///////////////////////////////////////////////////////////////////////////////
// AAPiDevice implementation
///////////////////////////////////////////////////////////////////////////////

struct AAPiDevice::Private
{
    int         fd;
    int         sock_fd[2];
    pthread_t   thread_id;
    volatile bool keep_running;

    Private()
    {
        fd = -1;
        sock_fd[0] = -1;
        sock_fd[1] = -1;
        thread_id = 0;
        keep_running = true;
    }

    ~Private()
    {
        close();
    }

    void close()
    {
        if (fd < 0)
            return;

        /* Stop thread */
        stop_thread();

        /* Cleanup */
        close_socks();
        close_file();
    }

    int write_socket()
    {
        struct AAPiDeviceStatus status;

        int ret = (int) ::read (fd, &status, sizeof(status));
        if (ret < 0)
        {
            return ret;
        }

        ret = (int) ::write (sock_fd[1], &status, sizeof(status));
        return ret;
    }

    int read_socket(struct AAPiDeviceStatus *status)
    {
        // Clear the pipe to reset the notifier
        return (int) ::read (sock_fd[0], status, sizeof(*status));
    }

    void close_file()
    {
        ::close(fd);
        fd = -1;
    }

    void close_socks()
    {
        ::close(sock_fd[0]);
        ::close(sock_fd[1]);
        sock_fd[0] = -1;
        sock_fd[1] = -1;
    }

    void stop_thread()
    {
        /* Stop thread */
        keep_running = false;
        pthread_kill(thread_id, SIGIO);

        /* Wait for thread */
        pthread_join(thread_id, NULL);
        thread_id = 0;
    }

    static void *signal_thread(void *arg);
};

/// \brief AAPiDevice::AAPiDevice
///
AAPiDevice::AAPiDevice()
{
    m_priv = new Private();
}

AAPiDevice::~AAPiDevice()
{
    delete m_priv;
}

void *AAPiDevice::Private::signal_thread(void *arg)
{
    int sig, ret;
    auto d = reinterpret_cast<AAPiDevice::Private *>(arg);

    /* Setup signal set to wait for */
    sigset_t waitset;
    sigemptyset (&waitset);
    sigaddset (&waitset, SIGIO);

    /* Block SIGIO in this thread (required for sigwait) */
    pthread_sigmask (SIG_BLOCK, &waitset, NULL);

    /* Setup fasync */
    if (fcntl (d->fd, F_SETOWN, getpid()) < 0)
    {
        return NULL;
    }

    int flags = fcntl (d->fd, F_GETFL);
    if (fcntl (d->fd, F_SETFL, flags | FASYNC) < 0)
    {
        return NULL;
    }

    /* FASYNC configured, waiting for signals... */

    while (d->keep_running)
    {
        /* sigwait blocks until signal arrives */
        ret = sigwait (&waitset, &sig);

        if (ret != 0)
        {
            // sigwait failed, exit thread
            break;
        }

        if (!d->keep_running)
            break;

        /* SIGIO received; */
        ret = d->write_socket();

        if (ret < 0)
        {
            // copy data failed, exit thread
            break;
        }
    }

    /* Exiting thread */
    return NULL;
}

int AAPiDevice::open(const char *dev_path)
{
    int ret;
    sigset_t set;

    if (!(m_priv->fd < 0))
    {
        return AAPI_E_INVALID_STATE;
    }

    /* Block SIGIO in main thread (important!) */
    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    /* Open device */
    ret = ::open (dev_path, O_RDONLY | O_NONBLOCK);
    if (ret < 0)
    {
        qCritical() << "device open failed: " << strerror(ret);
        return AAPI_CDEV_E_OPEN_FAILED;
    }

    m_priv->fd = ret;

    qDebug() << "Device opened (fd=" << m_priv->fd << ")";

    /* Using socketpair is slightly more portable than pipe() */
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_priv->sock_fd))
    {
        qCritical() << "couldn't create socket pair";
        m_priv->close_file();
        return AAPI_E_CREATE_SOCKET;
    }

    m_priv->keep_running = true;

    /* Create signal thread */
    ret = pthread_create (&m_priv->thread_id, NULL, Private::signal_thread, m_priv);
    if (ret != 0)
    {
        qCritical() << "pthread_create() failed: " << strerror(ret);
        m_priv->close_file();
        m_priv->close_socks();
        return AAPI_E_CREATE_THREAD_FAILED;
    }

    qDebug() << "Signal thread created";

    return AAPI_SUCCESS;
}

void AAPiDevice::close()
{
    m_priv->close();
}

int AAPiDevice::get_event_handle() const
{
    if (m_priv->fd < 0)
        return -1;

    return m_priv->sock_fd[0];
}

int AAPiDevice::read_status(struct AAPiDeviceStatus *status)
{
    if (m_priv->fd < 0)
        return -1;

    return m_priv->read_socket(status);
}

};
