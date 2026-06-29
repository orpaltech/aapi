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

#ifndef AAPI_OBJECT_WITH_CONFIG_H
#define AAPI_OBJECT_WITH_CONFIG_H

#include "aapi_config.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_AAPI_OBJECT_WITH_CONFIG(clazz)                      \
DECLARE_AAPI_OBJECT(clazz)                                          \
    static clazz *create(AAPiConfig *config, bool addRef = true);   \
    private:                                                        \
    AAPiPtr<AAPiConfig> m_config;


#define IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(clazz)            \
IMPLEMENT_AAPI_OBJECT(clazz)                                \
    clazz* clazz::create(AAPiConfig *config, bool addRef) { \
        clazz *obj = create(addRef);                        \
        if( obj ) {                                         \
            obj->m_config = config;                         \
    }                                                       \
    return obj;                                             \
}

#endif // AAPI_OBJECT_WITH_CONFIG_H
