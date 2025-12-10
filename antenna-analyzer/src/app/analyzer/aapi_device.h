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

#ifndef AAPI_DEVICE_H
#define AAPI_DEVICE_H

#include "aapi_object.h"
#include "aapi_error.h"


namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiGeneratorError enum
///
enum AAPiDeviceError {
    AAPI_CDEV_E_UNSPECIFIED     = (AAPI_DEVICE_ERROR_START - 0),
    AAPI_CDEV_E_OPEN_FAILED     = (AAPI_DEVICE_ERROR_START - 1),
    AAPI_CDEV_E_CNTL_FAILED     = (AAPI_DEVICE_ERROR_START - 2),
    AAPI_CDEV_E_READ_FAILED     = (AAPI_DEVICE_ERROR_START - 3),
    AAPI_CDEV_E_WRITE_FAILED    = (AAPI_DEVICE_ERROR_START - 4),
};

struct AAPiDeviceStatus
{
    uint8_t is_shutdown;
    int64_t timestamp;
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiDevice
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiDevice class
///
class AAPiDevice : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AAPiDevice)

protected:
    AAPiDevice();
    ~AAPiDevice();

public:
    int open(const char *dev_path);
    void close();

    int get_event_handle() const;
    int read_status(struct AAPiDeviceStatus *status);

private:
    struct Private;

    Private *m_priv;
};

};

#endif // AAPI_DEVICE_H
