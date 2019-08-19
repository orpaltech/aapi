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

#include "analyzer_generator.h"
#include "analyzer/analyzer_dsp.h"

#define AAPI_SYSFS_PATH "/sys/class/vna/aapi0/measure_freq"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// AAPIGenerator implementation
///////////////////////////////////////////////////////////////////////////////

AAPIGenerator *AAPIGenerator::create(AAPIConfig *config, bool add_ref)
{
    AAPIGenerator *obj = create(add_ref);
    if( obj ) {
        obj->m_config = config;

        AAPI_ADDREF(config);
    }
    return obj;
}

AAPIGenerator::AAPIGenerator()
    : last_freq(0)
    , current_owner(nullptr)
{
}

AAPIGenerator::~AAPIGenerator()
{
    AAPI_DISPOSE(m_config);
}

int AAPIGenerator::open()
{
    FILE *file_ptr;

    file_ptr = fopen( AAPI_SYSFS_PATH, "r" );
    if( file_ptr == nullptr )
    {
        return AAPI_GEN_E_DEVICE_NOT_FOUND;
    }

    fclose(file_ptr);

    last_freq = m_config->get_measure_freq();
    current_owner = nullptr;

	return 0;
}

void AAPIGenerator::close()
{
}

int AAPIGenerator::lock(void *owner)
{
    if( owner == nullptr )
    {
        return AAPI_E_INVALID_ARG;
    }

    if( current_owner && current_owner != owner )
    {
        return AAPI_E_RESOURCE_LOCKED;
    }

    current_owner = owner;
    return 0;
}

void AAPIGenerator::unlock(void *owner)
{
    if( current_owner == owner )
    {
        current_owner = nullptr;
    }
}

bool AAPIGenerator::is_locked() const
{
    return ( current_owner != nullptr );
}

uint32_t AAPIGenerator::get_last_frequency() const
{
    return last_freq;
}

int AAPIGenerator::set_frequency(uint32_t freq, void *owner)
{
    FILE *file_ptr;

    if( current_owner && current_owner != owner )
    {
        return AAPI_E_RESOURCE_LOCKED;
    }

    if( freq > 0 )
    {
        file_ptr = fopen( AAPI_SYSFS_PATH, "w" );
        if (!file_ptr)
        {
            return AAPI_GEN_E_DEVICE_NOT_FOUND;
        }

        fprintf( file_ptr, "%u", freq );
        fclose( file_ptr );

        last_freq = freq;
    }

	return 0;
}

} //namespace aapi
