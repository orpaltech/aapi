/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
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

#include <memory>
#include "error_codes.h"
#include "serial/serial_modem.h"
#include "analyzer/analyzer_generator.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// class AntScopeDevice
///////////////////////////////////////////////////////////////////////////////

class AntScopeDevice : public AAPIObject, public AAPISerialModemCallback
{
    DECLARE_AAPI_OBJECT(AntScopeDevice)

    static AntScopeDevice *create(AAPIConfig *config,
                                  AAPIGenerator *gen,
                                  bool add_ref = true);
protected:
    AntScopeDevice();
    ~AntScopeDevice();

public:
    int start();
	void stop();

private:
// AAPISerialModemCallback
    virtual void serial_modem_command(const char *command);
    virtual void serial_modem_error(int error);

private:
    uint32_t    m_center_freq;
    uint32_t    m_sweep_freq;

    AAPIConfig      *m_config;
    AAPIGenerator   *m_gen;
    AAPISerialModem *m_modem;
};

} // namespace aapi
