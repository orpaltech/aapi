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
    : AAPiVariantBase( AAPiString{val} )
{
}

AAPiVariant::AAPiVariant(const AAPiString& val) noexcept
    : AAPiVariantBase( val )
{
}

AAPiVariant::AAPiVariant(unsigned char val) noexcept
    : AAPiVariantBase( static_cast<unsigned int>(val) ) // QVariant lacks native uint8_t support; safely promote to uint
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
#ifdef AAPI_VARIANT_BASE_QT
    // Map Qt's native metadata type IDs to your custom enum variants
    switch (AAPiVariantBase::userType())
    {
    case QMetaType::UChar:
    case QMetaType::Char:
        return AAPiVariantType::BYTE;
    case QMetaType::UInt:
        return AAPiVariantType::UINT;
    case QMetaType::Int:
        return AAPiVariantType::INT;
    case QMetaType::Float:
    case QMetaType::Double:
        return AAPiVariantType::FLOAT;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    case QMetaType::CharStar:
        return AAPiVariantType::TEXT;
    default:
        return AAPiVariantType::UNKNOWN;
    }
#else
    switch (AAPiVariantBase::index())
    {
    case 0: return AAPiVariantType::BYTE;
    case 1: return AAPiVariantType::UINT;
    case 2: return AAPiVariantType::INT;
    case 3: return AAPiVariantType::FLOAT;
    case 4: return AAPiVariantType::TEXT;
    default: return AAPiVariantType::UNKNOWN;
    }
#endif
}

AAPiVariant& AAPiVariant::operator=(const AAPiVariant& var)
{
    if (this != &var) {
        AAPiVariantBase::operator=( var );
    }
    return *this;
}

AAPiVariant& AAPiVariant::operator=(AAPiVariant&& var)
{
    if (this != &var) {
        AAPiVariantBase::operator=( std::move( var ));
    }
    return *this;
}

unsigned char AAPiVariant::toByte() const
{
#ifdef AAPI_VARIANT_BASE_QT
    return static_cast<unsigned char>(AAPiVariantBase::toUInt());
#else
    switch (AAPiVariantBase::index())
    {
    case 0: return std::get<0>(*this);
    case 1: return static_cast<unsigned char>(std::get<1>(*this));
    case 2: return static_cast<unsigned char>(std::get<2>(*this));
    case 3: return static_cast<unsigned char>(std::get<3>(*this));
    case 4:
    default:
        return static_cast<unsigned char>(std::stoul((const char *) std::get<4>(*this)));
    }
#endif
}

#ifdef AAPI_VARIANT_BASE_QT
#else
unsigned int AAPiVariant::toUInt() const
{
    switch (AAPiVariantBase::index())
    {
    case 0: return static_cast<unsigned int>(std::get<0>(*this));
    case 1: return std::get<1>(*this);
    case 2: return static_cast<unsigned int>(std::get<2>(*this));
    case 3: return static_cast<unsigned int>(std::get<3>(*this));
    case 4:
    default: return std::stoul((const char *) std::get<4>(*this));
    }
}

int AAPiVariant::toInt() const
{
    switch (AAPiVariantBase::index())
    {
    case 0: return static_cast<int>(std::get<0>(*this));
    case 1: return static_cast<int>(std::get<1>(*this));
    case 2: return std::get<2>(*this);
    case 3: return static_cast<int>(std::get<3>(*this));
    case 4:
    default: return std::stoi((const char *) std::get<4>(*this));
    }
}

float AAPiVariant::toFloat() const
{
    switch (AAPiVariantBase::index())
    {
    case 0: return static_cast<float>(std::get<0>(*this));
    case 1: return static_cast<float>(std::get<1>(*this));
    case 2: return static_cast<float>(std::get<2>(*this));
    case 3: return std::get<3>(*this);
    case 4:
    default: return std::stof((const char *) std::get<4>(*this));
    }
}
#endif


AAPiString AAPiVariant::toString() const
{
#ifdef AAPI_VARIANT_BASE_QT
    return AAPiString(AAPiVariantBase::toString().toUtf8().constData());
#else
    switch (AAPiVariantBase::index())
    {
    case 0: return AAPiString(std::to_string(std::get<0>(*this)).c_str());
    case 1: return AAPiString(std::to_string(std::get<1>(*this)).c_str());
    case 2: return AAPiString(std::to_string(std::get<2>(*this)).c_str());
    case 3: return AAPiString(std::to_string(std::get<3>(*this)).c_str());
    case 4:
    default: return std::get<4>(*this);
    }
#endif
}

AAPiVariant AAPiVariant::Unknown;

#ifdef AAPI_VARIANT_BASE_QT
#else
std::ostream& operator<<(std::ostream& os, const AAPiVariant& var)
{
    // Write out the current active type index first (0 to 4)
    os << var.index() << ' ';

    // Write the actual payload data
    std::visit([&os](const auto& arg) {
        os << arg;
    }, static_cast<const AAPiVariantBase&>(var));

    return os;
}

std::istream& operator>>(std::istream& is, AAPiVariant& var)
{
    size_t type = 0;
    if (is >> type) {
        is.ignore(); // Discard the space delimiter tracking the index tag

        switch (type) {
        case 0: { // unsigned char
            unsigned int temp = 0;
            if (is >> temp) {
                if (temp > 255) {
                    is.setstate(std::ios::failbit); // Mark stream as corrupt/invalid
                } else {
                    var = static_cast<unsigned char>(temp);
                }
            }
            break;
        }
        case 1: { // unsigned int
            unsigned int val = 0;
            if (is >> val)
                var = val;
            break;
        }
        case 2: { // int
            int val = 0;
            if (is >> val)
                var = val;
            break;
        }
        case 3: { // float
            float val = 0.0f;
            if (is >> val)
                var = val;
            break;
        }
        case 4: { // AAPiString
            AAPiString str;
            if (is >> str)
                var = std::move(str); // Cleanly assignments/moves string
            break;
        }
        default:
            is.setstate(std::ios::failbit); // Unknown type tag, corrupt stream
            break;
        }
    }
    return is;
}
#endif

};  //namespace aapi
