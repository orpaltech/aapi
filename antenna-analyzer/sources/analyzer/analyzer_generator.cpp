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

#define SYSFS_PATH "/sys/class/vna/aapi0/measure_freq"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPIGenerator
///////////////////////////////////////////////////////////////////////////////

AAPIGenerator *AAPIGenerator::create(AAPIConfig *config, bool add_ref)
{
    AAPIGenerator *gen = create(add_ref);
    if (gen)
        gen->m_config = config;
    return gen;
}

AAPIGenerator::AAPIGenerator()
    : m_lastFreq(0)
    , m_locker(nullptr)
{
}

int AAPIGenerator::open()
{
    FILE *file_ptr;

    file_ptr = fopen(SYSFS_PATH, "r");
    if (!file_ptr)
        return AAPIGEN_E_DEVICE_NOT_FOUND;

    fclose(file_ptr);

    m_lastFreq = m_config->get_measure_freq();
    m_locker = nullptr;

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

    if( m_locker && m_locker != owner )
    {
        return AAPI_E_RESOURCE_LOCKED;
    }

    m_locker = owner;
    return 0;
}

void AAPIGenerator::unlock(void *owner)
{
    if( m_locker == owner )
    {
        m_locker = nullptr;
    }
}

bool AAPIGenerator::is_locked() const
{
    return m_locker != nullptr;
}

unsigned int AAPIGenerator::get_last_frequency() const
{
    return m_lastFreq;
}

int AAPIGenerator::set_frequency(unsigned int freq, void *owner)
{
    if( m_locker && m_locker != owner )
    {
        return AAPI_E_RESOURCE_LOCKED;
    }

    if( freq > 0 )
    {
        FILE *file_ptr;

        file_ptr = fopen(SYSFS_PATH, "w");
        if (!file_ptr)
            return AAPIGEN_E_DEVICE_NOT_FOUND;

        fprintf(file_ptr, "%u", freq);
        fclose(file_ptr);

        m_lastFreq = freq;
    }

	return 0;
}

} //namespace aapi
