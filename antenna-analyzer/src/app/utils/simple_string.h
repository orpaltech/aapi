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

#ifndef AAPI_SIMPLE_STRING_H
#define AAPI_SIMPLE_STRING_H

#include <iostream>
#include "simple_array.h"

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPiString
///////////////////////////////////////////////////////////////////////////////
/// \brief The AAPiString class
///
class AAPiString
{
    char    *m_ss;
    int     m_len;

public:
    AAPiString();
    AAPiString(const char *str);
    AAPiString(const AAPiString& ss);
    AAPiString(AAPiString&& ss) noexcept;
    ~AAPiString();

    int length() const;

    operator const char* () const
    {
        return m_ss;
    }

    AAPiString& operator=(const char *str);
    AAPiString& operator=(const AAPiString& ss);
    AAPiString& operator=(AAPiString&& ss) noexcept;
    AAPiString& operator+=(const AAPiString& ss);
    AAPiString operator+(const AAPiString& ss);

    friend std::ostream& operator<<(std::ostream& os, const AAPiString& str);
    friend std::istream& operator>>(std::istream& is, AAPiString& str);

    int find(const AAPiString& str, int pos = 0) const;
    int rfind(const AAPiString& str, int pos = -1) const;

private:
    void copyFrom(const AAPiString& ss);
    void copyFrom(const char *str, int len = -1);
    void moveFrom(AAPiString& ss);
    void empty();
    void destroy();
    char* detach(int& len);
    void attach(char *&str, int len);
    void append(const char *str, int len = -1);
    static char *alloc(int len);
};

std::ostream& operator<<(std::ostream& os, const AAPiString& str);
std::istream& operator>>(std::istream& is, AAPiString& str);

using AAPiStringArray = AAPiArray<AAPiString>;

};  //namespace aapi

#endif // AAPI_SIMPLE_STRING_H
