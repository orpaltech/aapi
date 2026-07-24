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

#include <stdlib.h>
#include <unistd.h>
#include "aapi_generator.h"
#include "utils/simple_string.h"


namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPIGenerator
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(AAPiGenerator)

AAPiGenerator::AAPiGenerator()
    : m_lastFreq(0)
    , m_owner(nullptr)
{
}

AAPiGenerator::~AAPiGenerator()
{
    close ();
}

AAPiError AAPiGenerator::open()
{
    AAPiError ret = resume( );
    if (AAPI_FAILED( ret )) {
        return ret;
    }

    suspend( );

    m_lastFreq = m_config->get_measure_freq();
    m_owner = nullptr;

    return AAPI_SUCCESS;
}

void AAPiGenerator::close()
{
    // force unlocked
    m_owner = nullptr;

    suspend( );
}

AAPiError AAPiGenerator::suspend()
{
    if ( m_owner ) {
        return AAPI_E_RESOURCE_LOCKED;
    }

    AAPiError ret = write_uint( "enable", 0 );
    return ret;
}

AAPiError AAPiGenerator::resume()
{
    AAPiError ret = write_uint( "enable", 1 );
    if (AAPI_SUCCEEDED( ret )) {
        usleep(20'000U);    // wait 20ms

        ret = set_intermediate_freq(m_config->get_intermediate_freq());
    }
    return ret;
}

int AAPiGenerator::lock(void *owner)
{
    if ( owner == nullptr ) {
        return AAPI_E_INVALID_ARG;
    }

    if ( m_owner && m_owner != owner ) {
        return AAPI_E_RESOURCE_LOCKED;
    }

    m_owner = owner;
    return AAPI_SUCCESS;
}

void AAPiGenerator::unlock(void *owner)
{
    if ( m_owner == owner ) {
        m_owner = nullptr;
    }
}

bool AAPiGenerator::is_locked() const
{
    return ( m_owner != nullptr );
}

AAPiError AAPiGenerator::read_uint(const char *prop_name, uint32_t& val) const
{
    AAPiString propPath = get_sysfs_property_path(prop_name);

    FILE *fp = fopen( propPath, "r" );
    if ( fp == nullptr ) {
        return AAPI_GEN_E_DEVICE_OPEN_FAILED;
    }

    // The maximum (unsigned) 32-bit integer is 4294967295
    // which is 10 digits long
    char *buf = (char *) malloc( 11 );
    int i = 0, c; // note: int required to handle EOF
    while (i < 10 && (c = fgetc( fp )) != EOF)
        buf[i++] = c;

    if (ferror( fp )) {
        // I/O error when reading
        return AAPI_GEN_E_DEVICE_READ_FAILED;
    }

    if (! feof( fp )) {
        // End of file hasn't reached
        return AAPI_GEN_E_DEVICE_BAD_VALUE;
    }

    buf[i] = '\0';
    char *endptr = nullptr;
    val = strtoul(buf, &endptr, 10);

    return AAPI_SUCCESS;
}

AAPiError AAPiGenerator::write_uint(const char *prop_name, uint32_t value)
{
    AAPiString propPath = get_sysfs_property_path(prop_name);

    FILE *fp = fopen( propPath, "w" );
    if ( fp == nullptr ) {
        return AAPI_GEN_E_DEVICE_OPEN_FAILED;
    }

    int ret = fprintf( fp, "%u\n", value );
    fclose( fp );
    if (!( ret > 0 )) {
        return AAPI_GEN_E_DEVICE_WRITE_FAILED;
    }

    return AAPI_SUCCESS;
}

AAPiError AAPiGenerator::get_intermediate_freq(uint32_t& freq) const
{
    return read_uint( "intermediate_freq", freq );
}

AAPiError AAPiGenerator::set_intermediate_freq(uint32_t new_freq)
{
    if ( new_freq == 0 ) {
        return AAPI_E_INVALID_ARG;
    }

    return write_uint( "intermediate_freq", new_freq );
}

uint32_t AAPiGenerator::get_last_freq() const
{
    return m_lastFreq;
}

AAPiError AAPiGenerator::get_measure_freq(uint32_t& freq) const
{
    return read_uint( "measure_freq", freq );
}

AAPiError AAPiGenerator::set_measure_freq(uint32_t new_freq, void *owner)
{
    if ( m_owner && m_owner != owner ) {
        return AAPI_E_RESOURCE_LOCKED;
    }

    if ( new_freq == 0 ) {
        return AAPI_E_INVALID_ARG;
    }

    int ret = write_uint( "measure_freq", new_freq );
    if (ret == AAPI_SUCCESS) {
        // save the last measure frequency set
        m_lastFreq = new_freq;
    }

    return ret;
}

} //namespace aapi
