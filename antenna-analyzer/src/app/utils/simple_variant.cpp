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

#include "simple_variant.h"

namespace aapi
{

AAPiVariant::AAPiVariant() noexcept
{
}

AAPiVariant::AAPiVariant(const AAPiVariant& var) noexcept
    : AAPiVariantBase( var )
{
}

AAPiVariant::AAPiVariant(AAPiVariant&& var) noexcept
    : AAPiVariantBase( std::move( var ))
{

}

AAPiVariant::AAPiVariant(const char *val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(const AAPiString& val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(unsigned char val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(int val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(unsigned int val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(float val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariantType AAPiVariant::get_type() const
{
    // uint8_t, uint32_t, int, float, AAPiString

#ifdef AAPI_VARIANT_BASE_QT
    return AAPI_VT_UNKNOWN;
#else
    switch (AAPiVariantBase::index())
    {
    case 0:
        return AAPI_VT_BYTE;
    case 1:
        return AAPI_VT_UINT;
    case 2:
        return AAPI_VT_INT;
    case 3:
        return AAPI_VT_FLOAT;
    case 4:
        return AAPI_VT_TEXT;
    default:
        return AAPI_VT_UNKNOWN;
    }
#endif
}

AAPiVariant& AAPiVariant::operator=(const AAPiVariant& var)
{
    AAPiVariantBase::operator=( var );
    return *this;
}

AAPiVariant& AAPiVariant::operator=(AAPiVariant&& var)
{
    AAPiVariantBase::operator=( std::move( var ));
    return *this;
}

unsigned char AAPiVariant::toByte() const
{
#ifdef AAPI_VARIANT_BASE_QT
    return (unsigned char) AAPiVariantBase::toInt( res );
#else
    size_t index = AAPiVariantBase::index();
    switch (index)
    {
    case 0:
        return std::get<0>(*this);
    case 1:
        return (unsigned char)std::get<1>(*this);
    case 2:
        return (unsigned char)std::get<2>(*this);
    case 3:
        return (unsigned char)std::get<3>(*this);
    case 4:
    default:
        return std::stoul((const char *) std::get<4>(*this));
    }
#endif
}

#ifdef AAPI_VARIANT_BASE_QT
#else
unsigned int AAPiVariant::toUInt() const
{
    size_t index = AAPiVariantBase::index();
    switch (index)
    {
    case 0:
        return (unsigned int)std::get<0>(*this);
    case 1:
        return std::get<1>(*this);
    case 2:
        return (unsigned int)std::get<2>(*this);
    case 3:
        return (unsigned int)std::get<3>(*this);
    case 4:
    default:
        return std::stoul((const char *) std::get<4>(*this));
    }
}

int AAPiVariant::toInt() const
{
    size_t index = AAPiVariantBase::index();
    switch (index)
    {
    case 0:
        return (int)std::get<0>(*this);
    case 1:
        return (int)std::get<1>(*this);
    case 2:
        return std::get<2>(*this);
    case 3:
        return (int)std::get<3>(*this);
    case 4:
    default:
        return std::stoi((const char *) std::get<4>(*this));
    }
}

float AAPiVariant::toFloat() const
{
    size_t index = AAPiVariantBase::index();
    switch (index)
    {
    case 0:
        return (float)std::get<0>(*this);
    case 1:
        return (float)std::get<1>(*this);
    case 2:
        return (float)std::get<2>(*this);
    case 3:
        return std::get<3>(*this);
    case 4:
    default:
        return std::stof((const char *) std::get<4>(*this));
    }
}
#endif

AAPiString AAPiVariant::toString() const
{
#ifdef AAPI_VARIANT_BASE_QT
    return AAPiVariantBase::toString( ).toLatin1().constData();
#else
    size_t index = AAPiVariantBase::index();
    switch (index)
    {
    case 0:
        return std::to_string(std::get<0>(*this)).c_str();
    case 1:
        return std::to_string(std::get<1>(*this)).c_str();
    case 2:
        return std::to_string(std::get<2>(*this)).c_str();
    case 3:
        return std::to_string(std::get<3>(*this)).c_str();
    case 4:
    default:
        return std::get<4>(*this);
    }
#endif
}

AAPiVariant AAPiVariant::Unknown;

#ifdef AAPI_VARIANT_BASE_QT
#else
std::ostream& operator<<(std::ostream& os, const AAPiVariant& var)
{
    const AAPiVariantBase& base = var;
    std::visit([&os](auto&& arg) { os << arg; }, base );
    return os;
}

std::istream& operator>>(std::istream& is, AAPiVariant& var)
{
    AAPiVariantBase& base = var;
    std::visit([&is](auto&& arg) { is >> arg; }, base );
    return is;
}
#endif

};  //namespace aapi
