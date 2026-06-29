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

#include "simple_string.h"
#include <cstring>
#include <string>
#include <charconv>
#include <cstdlib>


namespace aapi
{

AAPiString::AAPiString()
    : m_ss(nullptr), m_len(0)
{
    empty();
}
AAPiString::AAPiString(const char *str)
    : m_ss(nullptr), m_len(0)
{
    copyFrom(str);
}
AAPiString::AAPiString(const AAPiString& ss)
    : m_ss(nullptr), m_len(0)
{
    copyFrom(ss.m_ss, ss.m_len);
}
AAPiString::AAPiString(AAPiString&& ss) noexcept
    : m_ss(nullptr), m_len(0)
{
    moveFrom(std::move(ss));
}
AAPiString::~AAPiString()
{
    destroy();
}

void AAPiString::append(unsigned int num)
{
    // Fast path for 0
    if (num == 0) {
        append("0", 1);
        return;
    }

    char buf[11]; // Holds up to 10 digits + null terminator
    int i = 10;
    buf[i] = '\0';

    // Extract digits backward
    while (num > 0) {
        buf[--i] = '0' + (num % 10);
        num /= 10;
    }

    // Call your existing memory-safe append method
    append(buf + i, 10 - i);
}

int AAPiString::find(const AAPiString& str, int pos) const
{
    // Fix: Safely pass the explicit raw operator pointer fallback
    return std::string((const char *) *this).find((const char*) str, pos);
}

int AAPiString::rfind(const AAPiString& str, int pos) const
{
    return std::string((const char *) *this).rfind((const char *) str, pos);
}


bool AAPiString::toUInt(unsigned int& outValue, int base) const
{
    if (!m_ss || m_len == 0)
        return false;

    const char *startPtr = m_ss;
    const char *endPtr = m_ss + m_len;

    // Handle Hex prefix '0x' or '0X' if parsing base 16
    if (base == 16 && m_len >= 2 && startPtr[0] == '0' && (startPtr[1] == 'x' || startPtr[1] == 'X')) {
        startPtr += 2;
    }

    auto [ptr, ec] = std::from_chars(startPtr, endPtr, outValue, base);
    return (ec == std::errc());
}

bool AAPiString::toInt(int& outValue, int base) const
{
    if (!m_ss || m_len == 0)
        return false;

    auto [ptr, ec] = std::from_chars(m_ss, m_ss + m_len, outValue, base);
    return (ec == std::errc());
}

bool AAPiString::toFloat(float& outValue) const
{
    if (!m_ss || m_len == 0)
        return false;

    char *endCheckPtr = nullptr;
    // std::strtof is exception-free and matches standard library behaviors
    outValue = std::strtof(m_ss, &endCheckPtr);

    // Returns true if it successfully consumed characters from the string buffer
    return (endCheckPtr != m_ss);
}

AAPiString AAPiString::trim(const char *chars) const
{
    if (!m_ss || m_len == 0)
        return AAPiString();

    int start = 0;
    while (start < m_len && std::strchr(chars, m_ss[start])) {
        start++;
    }

    if (start == m_len)
        return AAPiString();

    int end = m_len - 1;
    while (end >= start && std::strchr(chars, m_ss[end])) {
        end--;
    }

    int newLen = end - start + 1;

    // Allocate newLen + 1 bytes so the null-terminator has dedicated room
    char *tempBuffer = alloc(newLen + 1);
    std::memcpy(tempBuffer, m_ss + start, newLen);
    tempBuffer[newLen] = '\0'; // Manually enforce early null termination safety

    AAPiString result;
    result.attach(tempBuffer, newLen);
    return result;
}

AAPiString& AAPiString::operator=(const char *str)
{
    copyFrom(str);
    return *this;
}

AAPiString& AAPiString::operator=(const AAPiString& ss)
{
    if (this != &ss) {
        copyFrom(ss.m_ss, ss.m_len);
    }
    return *this;
}

AAPiString& AAPiString::operator=(AAPiString&& ss) noexcept
{
    if (this != &ss) {
        moveFrom(std::move(ss));
    }
    return *this;
}

AAPiString& AAPiString::operator+=(const AAPiString& ss)
{
    append(ss.m_ss, ss.m_len);
    return *this;
}

AAPiString& AAPiString::operator+=(unsigned int num)
{
    append(num);
    return *this;
}

AAPiString operator+(const AAPiString& lhs, const AAPiString& rhs)
{
    AAPiString tmp(lhs);
    tmp.append(rhs.m_ss, rhs.m_len);
    return tmp;
}

AAPiString operator+(const AAPiString& lhs, const char* rhs)
{
    AAPiString tmp(lhs);
    if (rhs) {
        tmp.append(rhs, static_cast<int>(std::strlen(rhs)));
    }
    return tmp;
}

AAPiString operator+(const char* lhs, const AAPiString& rhs)
{
    AAPiString tmp;
    if (lhs) {
        int lhsLen = static_cast<int>(std::strlen(lhs));
        tmp.append(lhs, lhsLen);
    }
    tmp.append(rhs.m_ss, rhs.m_len);
    return tmp;
}

bool operator==(const AAPiString& lhs, const AAPiString& rhs)
{
    if (lhs.m_len != rhs.m_len)
        return false;

    if (lhs.m_ss == rhs.m_ss)
        return true;

    // The one has an allocated empty string "" and the other is nullptr.
    // They are semantically equal.
    if (!lhs.m_ss || !rhs.m_ss)
        return true;

    // Safe to compare because lengths match and neither is null
    return std::memcmp(lhs.m_ss, rhs.m_ss, lhs.m_len) == 0;
}

bool operator==(const AAPiString& lhs, const char* rhs)
{
    if (!rhs)
        return lhs.m_ss == nullptr || lhs.m_len == 0;
    if (!lhs.m_ss)
        return *rhs == '\0';

    return std::strcmp(lhs.m_ss, rhs) == 0;
}

bool operator==(const char* lhs, const AAPiString& rhs)
{
    return rhs == lhs;
}

char& AAPiString::operator[](int index)
{
    // Optional: Add an assertion or throw an exception if index is out of bounds
    // if (index < 0 || index >= m_len) throw std::out_of_range("Index out of bounds");

    return m_ss[index];
}

const char& AAPiString::operator[](int index) const
{
    // Optional: Add an assertion or throw an exception if index is out of bounds
    // if (index < 0 || index >= m_len) throw std::out_of_range("Index out of bounds");

    return m_ss[index];
}

void AAPiString::copyFrom(const char *str, int len)
{
    destroy();
    if (!str) {
        empty();
        return;
    }
    if (len < 0)
        len = std::strlen(str);

    m_ss = alloc(len + 1);
    if (len > 0)
        std::memcpy(m_ss, str, len);
    m_ss[len] = '\0';
    m_len = len;
}

void AAPiString::moveFrom(AAPiString&& ss) noexcept
{
    destroy();
    m_ss = ss.detach(m_len);
}

void AAPiString::empty()
{
    destroy();
}

void AAPiString::destroy()
{
    if (m_ss) {
        std::free(m_ss);
        m_ss = nullptr;
    }
    m_len = 0;
}


void AAPiString::append(const char *str, int len)
{
    if (!str || len == 0)
        return;
    if (len < 0)
        len = std::strlen(str);

    // Identify if we are appending data from our own internal buffer
    bool self_append = (str >= m_ss && str < (m_ss + m_len));

    // Map the exact boundary offset before realloc touches anything
    size_t self_offset = self_append ? (str - m_ss) : 0;

    // Allocate using a separate temporary pointer to keep original reference safe
    char *new_ss = (char *)std::realloc(m_ss, m_len + len + 1);
    if (!new_ss)
        return; // Drop out safely if out of memory

    // If realloc moved the block, calculate the source's new location in RAM
    const char *source = self_append ? (new_ss + self_offset) : str;

    // Commit the pointer shift safely
    m_ss = new_ss;

    // Execute high-speed copy using the re-based source pointer
    std::memcpy(m_ss + m_len, source, len);

    // Establish deterministic termination boundaries
    m_len += len;
    m_ss[m_len] = '\0';
}



/*void AAPiString::append(const char *str, int len)
{
    if (!str || len==0) return;
    if (len < 0)
        len = strlen(str);

    if (len > 0)  {
        m_ss = (char *)realloc(m_ss, m_len + len + 1);
        strncpy(m_ss + m_len, str, len);
        m_ss[m_len + len] = '\0';
        m_len += len;
    }
}*/

/*
void AAPiString::append(const char *str, int len)
{
    if (!str || len == 0)
        return;
    if (len < 0)
        len = std::strlen(str);

    // If appending self, realloc might invalidate str.
    // Handled via temporary pointer offset logic
    bool self_append = (str >= m_ss && str < (m_ss + m_len));
    size_t offset = self_append ? (str - m_ss) : 0;

    char *new_ss = (char *)std::realloc(m_ss, m_len + len + 1);
    if (!new_ss)
        return; // Allocation failed
    m_ss = new_ss;

    const char *source = self_append ? (m_ss + offset) : str;
    std::memcpy(m_ss + m_len, source, len);
    m_len += len;
    m_ss[m_len] = '\0';
}
*/

char *AAPiString::detach(int& len)
{
    char *ss = m_ss;
    len = m_len;

    // Reset to base safe state without calling destroy()
    m_ss = nullptr;
    m_len = 0;

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
    return (char *)std::malloc(len);
}

std::ostream& operator<<(std::ostream& os, const AAPiString& str)
{
    os << str.length() << ' '; // Separate length field clear from payload
    if (str.length() > 0) {
        os.write(str.m_ss, str.length());
    }
    return os;
}

std::istream& operator>>(std::istream& is, AAPiString& str)
{
    int length = 0;
    if (is >> length) {
        is.ignore(); // Skips the single space/delimiter tracking length

        char *ss = AAPiString::alloc(length + 1);
        if (length > 0) {
            is.read(ss, length);
        }
        ss[length] = '\0';
        str.attach(ss, length);

        // Consume and discard trailing whitespace/newlines until the next valid token
        while (is.good() && std::isspace(is.peek())) {
            is.get();
        }
    }
    return is;
}


};  //namespace aapi

