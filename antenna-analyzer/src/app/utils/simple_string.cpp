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

#include "simple_string.h"
#include <stdlib.h>
#include <string.h>
#include <string>
#include <utility>


namespace aapi
{

AAPiString::AAPiString()
    : m_len(0), m_ss(nullptr)
{
    empty();
}

AAPiString::AAPiString(const char *str)
    : m_len(0), m_ss(nullptr)
{
    copyFrom(str);
}

AAPiString::AAPiString(const AAPiString& ss)
    : m_len(0), m_ss(nullptr)
{
    copyFrom(ss);
}

AAPiString::AAPiString(AAPiString&& ss) noexcept
    : m_len(0), m_ss(nullptr)
{
    moveFrom(ss);
}

AAPiString::~AAPiString()
{
    destroy();
}

int AAPiString::length() const
{
    return m_len;
}

int AAPiString::find(const AAPiString& str, int pos) const
{
    std::string me(*this);
    return me.find(str, pos);
}

int AAPiString::rfind(const AAPiString& str, int pos) const
{
    std::string me(*this);
    return me.rfind(str, pos);
}


AAPiString& AAPiString::operator=(const char *str)
{
    copyFrom(str);

    return *this;
}

AAPiString& AAPiString::operator=(const AAPiString& ss)
{
    if (this != &ss)
    {
        copyFrom(ss);
    }
    return *this;
}

AAPiString& AAPiString::operator=(AAPiString&& ss) noexcept
{
    if (this != &ss)
    {
        moveFrom(ss);
    }
    return *this;
}

AAPiString& AAPiString::operator+=(const AAPiString& ss)
{
    if (ss.length())
    {
        AAPiString tmp(std::move(*this));
        tmp.append(ss, ss.length());
        *this = std::move(tmp);
    }
    return *this;
}

AAPiString AAPiString::operator+(const AAPiString& ss)
{
    if (ss.length())
    {
        AAPiString tmp(*this);
        tmp.append(ss, ss.length());
        return tmp;
    }
    return *this;
}

void AAPiString::copyFrom(const char *str, int len)
{
    destroy();

    if (!str) return;
    if (len < 0)
        len = strlen(str);

    m_ss = alloc(len + 1);
    if (len > 0)
        strncpy(m_ss, str, len);
    m_ss[len] = '\0';
    m_len = len;
}

void AAPiString::copyFrom(const AAPiString& ss)
{
    copyFrom(ss, ss.length());
}

void AAPiString::moveFrom(AAPiString& ss)
{
    destroy();

    m_ss = ss.detach(m_len);
}

void AAPiString::empty()
{
    if (!m_ss)
        m_ss = alloc(1);
    m_ss[0] = '\0';
    m_len = 0;
}

void AAPiString::destroy()
{
    if (m_len && m_ss)
    {
        free(m_ss);
        m_len = 0;
    }
}

void AAPiString::append(const char *str, int len)
{
    if (!str || len==0) return;
    if (len < 0)
        len = strlen(str);

    if (len > 0)
    {
        m_ss = (char *)realloc(m_ss, m_len + len + 1);
        strncpy(m_ss + m_len, str, len);
        m_ss[m_len + len] = '\0';
        m_len += len;
    }
}

char *AAPiString::detach(int& len)
{
    char *ss = m_ss;
    len = m_len;

    m_ss = nullptr;
    empty();

    return ss;
}

void AAPiString::attach(char *&ss, int len)
{
    destroy();

    m_ss = ss;
    m_len = len;
    ss = nullptr;
}

char *AAPiString::alloc(int len)
{
    return (char *)malloc(len);
}

std::ostream& operator<<(std::ostream& os, const AAPiString& str)
{
    os << str.length();
    os.write((const char *)str, str.length());
    return os;
}

std::istream& operator>>(std::istream& is, AAPiString& str)
{
    int length;
    is >> length;
    char *ss = AAPiString::alloc(length + 1);
    is.read(ss, length);
    ss[length] = '\0';
    str.attach(ss, length);
    return is;
}

};  //namespace aapi

