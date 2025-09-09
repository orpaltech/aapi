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

#ifndef AAPI_GENERATOR_H
#define AAPI_GENERATOR_H

#include "analyzer/aapi_configuration.h"

namespace aapi
{
///////////////////////////////////////////////////////////////////////////////
// Enum definitions
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiGeneratorError enum
///
enum AAPiGeneratorError {
    AAPI_GEN_E_UNSPECIFIED          = (AAPI_GENERATOR_ERROR_START - 0),
    AAPI_GEN_E_DEVICE_OPEN_FAILED   = (AAPI_GENERATOR_ERROR_START - 1),
    AAPI_GEN_E_DEVICE_READ_FAILED   = (AAPI_GENERATOR_ERROR_START - 2),
    AAPI_GEN_E_DEVICE_BAD_VALUE     = (AAPI_GENERATOR_ERROR_START - 4),
    AAPI_GEN_E_DEVICE_WRITE_FAILED  = (AAPI_GENERATOR_ERROR_START - 5),
};

///////////////////////////////////////////////////////////////////////////////
// class AAPiGenerator
///////////////////////////////////////////////////////////////////////////////

class AAPiGenerator : public AAPiObject
{
    DECLARE_AAPI_OBJECT(AAPiGenerator)

    static AAPiGenerator *create(AAPiConfig *config, bool addRef = true);
protected:
    AAPiGenerator();
    ~AAPiGenerator();

public:
    int open();
    void close();

    int suspend();
    int resume();

    int lock(void *owner);
    void unlock(void *owner);
    bool is_locked() const;

    int set_intermediate_freq(uint32_t freq);
    int get_intermediate_freq(uint32_t& freq) const;

    int set_measure_freq(uint32_t freq, void *owner = nullptr);
    int get_measure_freq(uint32_t& freq) const;

    // the last measure frequency that was set
    uint32_t get_last_freq() const;

private:
    int write_uint(const char *prop_name, uint32_t val);
    int read_uint(const char *prop_name, uint32_t& val) const;

private:
    AAPiConfig      *m_config;
    uint32_t        m_lastFreq;
    volatile void   *m_owner;

};

} //namespace aapi

#endif // AAPI_GENERATOR_H
