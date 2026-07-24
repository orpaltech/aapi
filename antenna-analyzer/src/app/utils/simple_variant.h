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

#ifndef AAPI_SIMPLE_VARIANT_H
#define AAPI_SIMPLE_VARIANT_H

//#define AAPI_VARIANT_BASE_QT    1

#ifdef AAPI_VARIANT_BASE_QT
#include <QVariant>
#else
#include <variant>
#endif
#include "simple_string.h"


namespace aapi
{

enum class AAPiVariantType {
    UNKNOWN,
    BYTE,   //8-bit unsigned
    UINT,   //32-bit unsigned
    INT,    //32-bit signed
    FLOAT,  //32-bit float
    TEXT,   //char*
};

#ifdef AAPI_VARIANT_BASE_QT
using AAPiVariantBase = QVariant;
#else
using AAPiVariantBase = std::variant<unsigned char, unsigned int, int, float, AAPiString>;
#endif


///////////////////////////////////////////////////////////////////////////////
// class AAPiVariant
///////////////////////////////////////////////////////////////////////////////

class AAPiVariant : public AAPiVariantBase
{
public:
    AAPiVariant() noexcept;
    AAPiVariant(const AAPiVariant& var) noexcept;
    AAPiVariant(AAPiVariant&& var) noexcept;
    AAPiVariant(const AAPiString& str) noexcept;
    AAPiVariant(const char *str) noexcept;
    AAPiVariant(unsigned char val) noexcept;
    AAPiVariant(int val) noexcept;
    AAPiVariant(unsigned int val) noexcept;
    AAPiVariant(float val) noexcept;

    AAPiVariantType get_type() const;


    AAPiVariant& operator=(const AAPiVariant& var);
    AAPiVariant& operator=(AAPiVariant&& var);

#ifdef AAPI_VARIANT_BASE_QT
#else
    friend std::ostream& operator<<(std::ostream& os, const AAPiVariant& var);
    friend std::istream& operator>>(std::istream& is, AAPiVariant& var);
#endif

    static AAPiVariant Unknown;

#ifdef AAPI_VARIANT_BASE_QT
#else
    unsigned int toUInt() const;
    int toInt() const;
    float toFloat() const;
#endif
    unsigned char toByte() const;
    AAPiString toString() const;
};

#ifdef AAPI_VARIANT_BASE_QT
#else
std::ostream& operator<<(std::ostream& os, const AAPiVariant& var);
std::istream& operator>>(std::istream& is, AAPiVariant& var);
#endif

using AAPiVariantArray = AAPiArray<AAPiVariant>;

};  //namespace aapi

#endif // AAPI_SIMPLE_VARIANT_H
