/*
 * This file is part of the ORPALTECH AA-PI project
 *  (https://github.com/orpaltech/aapi).
 *
 * Copyright (c) 2013-2026 ORPAL Technology, Inc.
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
    pthread_t   tid;
    volatile bool keep_running;

    Private()
    {
        fd = -1;
        sock_fd[0] = -1;
        sock_fd[1] = -1;
        tid = 0;
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
        if (ret < 0) {
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
        pthread_kill(tid, SIGIO);

        /* Wait for thread */
        pthread_join(tid, nullptr);
        tid = 0;
    }

    static void *signal_thread(void *arg);
};

/// \brief AAPiDevice::AAPiDevice
///
AAPiDevice::AAPiDevice()
    : m_priv(std::make_unique<Private>())
{
}

AAPiDevice::~AAPiDevice() = default;

void *AAPiDevice::Private::signal_thread(void *arg)
{
    auto dev = reinterpret_cast<AAPiDevice::Private *>(arg);

    /* Setup signal set to wait for */
    sigset_t wset;
    sigemptyset (&wset);
    sigaddset (&wset, SIGIO);

    /* Block SIGIO in this thread (required for sigwait) */
    pthread_sigmask (SIG_BLOCK, &wset, nullptr);

    /* Setup fasync */
    if (fcntl (dev->fd, F_SETOWN, getpid()) < 0) {
        return nullptr;
    }

    int flags = fcntl (dev->fd, F_GETFL);
    if (fcntl (dev->fd, F_SETFL, flags | FASYNC) < 0) {
        return nullptr;
    }

    /* FASYNC configured, waiting for signals... */

    int sig, ret;

    while (dev->keep_running) {
        /* sigwait blocks until signal arrives */
        ret = sigwait (&wset, &sig);

        if (ret != 0) {
            // sigwait failed, exit thread
            break;
        }

        if (!dev->keep_running)
            break;

        /* SIGIO received; */
        ret = dev->write_socket();

        if (ret < 0) {
            // copy data failed, exit thread
            break;
        }
    }

    /* Exiting thread */
    return nullptr;
}

int AAPiDevice::open(const char *dev_path)
{
    if (!(m_priv->fd < 0)) {
        return AAPI_E_INVALID_STATE;
    }

    /* Block SIGIO in main thread (important!) */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    /* Open device */
    int ret = ::open (dev_path, O_RDONLY | O_NONBLOCK);
    if (ret < 0) {
        return AAPI_CDEV_E_OPEN_FAILED;
    }

    m_priv->fd = ret;

    /* Using socketpair is slightly more portable than pipe() */
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_priv->sock_fd)) {
        m_priv->close_file();
        return AAPI_E_CREATE_SOCKET;
    }

    m_priv->keep_running = true;

    /* Create signal thread */
    ret = pthread_create (&m_priv->tid, NULL, Private::signal_thread, m_priv.get());
    if (ret != 0) {
        m_priv->close_file();
        m_priv->close_socks();
        return AAPI_E_CREATE_THREAD_FAILED;
    }

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

} //namespace aapi
