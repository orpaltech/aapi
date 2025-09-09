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

#include "serial/aapi_serial_modem.h"
#include "analyzer/aapi_generator.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// class AntScopeDevice
///////////////////////////////////////////////////////////////////////////////

class AntScopeDevice : public AAPiObject,
                       public AAPISerialModemCallback
{
    DECLARE_AAPI_OBJECT(AntScopeDevice)

    static AntScopeDevice *create(AAPiConfig *config, AAPiGenerator *gen,
                                  bool addRef = true);
protected:
    AntScopeDevice();
    ~AntScopeDevice();

public:
    int start();
	void stop();

private:
// AAPISerialModemCallback
    virtual void onSerialModemCommand(const char *command);
    virtual void onSerialModemError(int error);

private:
    uint32_t    m_centerFreq;
    uint32_t    m_sweepFreq;

    AAPiConfig      *m_config;
    AAPiGenerator   *m_gen;
    AAPISerialModem *m_modem;
};

} // namespace aapi
