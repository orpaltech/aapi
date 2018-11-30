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

#ifndef AAPI_GENERATOR_H
#define AAPI_GENERATOR_H

#include "analyzer/analyzer_config.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPIGeneratorError enum
///
enum AAPIGeneratorError
{
    AAPIGEN_E_DEVICE_NOT_FOUND  = AAPI_GEN_ERROR_START
};

///////////////////////////////////////////////////////////////////////////////
// class AAPIGenerator
///////////////////////////////////////////////////////////////////////////////

class AAPIGenerator : public AAPIObject
{
    DECLARE_AAPI_OBJECT(AAPIGenerator)

    static AAPIGenerator *create(AAPIConfig *config, bool add_ref = true);
protected:
    AAPIGenerator();

public:
    int open();
    void close();

    int lock(void *owner);
    void unlock(void *owner);
    bool is_locked() const;

    int set_frequency(uint32_t freq, void *owner = nullptr);
    uint32_t get_last_frequency() const;

private:
    aapi_ptr<AAPIConfig> config;
    uint32_t        last_frequency;
    volatile void   *current_owner;

};

} //namespace aapi

#endif // AAPI_GENERATOR_H
