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

#ifndef AAPI_SIMPLE_ARRAY_H
#define AAPI_SIMPLE_ARRAY_H

#include <stdlib.h>
#include <string.h>

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPiArray
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The AAPiArray class
///
template<class T>
class AAPiArray
{
public:
    AAPiArray();
    AAPiArray(const AAPiArray& sa);
    AAPiArray(AAPiArray&& sa) noexcept;
    ~AAPiArray();

    bool append(const T& element);
    bool remove(const T& element);
    bool remove_at(int i);
    void clear();

    const T& get_element(int i) const noexcept(false);
    T& get_element(int i) noexcept(false);
    int size() const;

    const T& operator [](int i) const;
    T& operator [](int i);

    AAPiArray& operator=(const AAPiArray& sa);
    AAPiArray& operator=(AAPiArray&& sa) noexcept;

private:
    void copyFrom(const AAPiArray& sa);
    void moveFrom(AAPiArray& sa);

private:
    T *m_elements;
    int m_numElements;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
AAPiArray<T>::AAPiArray()
    : m_elements(nullptr)
    , m_numElements(0)
{
}

template<class T>
AAPiArray<T>::AAPiArray(const AAPiArray& sa)
    : m_elements(nullptr)
    , m_numElements(0)
{
    copyFrom(sa);
}

template<class T>
AAPiArray<T>::AAPiArray(AAPiArray&& sa) noexcept
    : m_elements(nullptr)
    , m_numElements(0)
{
    moveFrom(sa);
}

template<class T>
AAPiArray<T>::~AAPiArray()
{
    clear();
}

template<class T>
bool AAPiArray<T>::append(const T& newElement)
{
    T *ptr = reinterpret_cast<T *>(realloc( m_elements, (m_numElements + 1) * sizeof (T) ));
    if (ptr != nullptr)
    {
        m_elements = ptr;
        new (&m_elements[m_numElements]) T ();
        m_elements[m_numElements++] = newElement;
        return true;
    }
    return false;
}

template<class T>
bool AAPiArray<T>::remove(const T& element)
{
    int index = -1;
    for (int i = 0; i < m_numElements; i++)
    {
        if (m_elements[i] == element) {
            index = i;
            break;
        }
    }
    return remove_at(index);
}

template<class T>
bool AAPiArray<T>::remove_at(int i)
{
    if (i < 0 || i >= m_numElements)
        return false;

    m_elements[i].~T();
    int right = m_numElements - i -1;
    if (right > 0)
        memmove(&m_elements[i], &m_elements[i+1], right * sizeof (T));

    if (--m_numElements == 0)
    {
        free(m_elements);
        m_elements = nullptr;
    }

    return true;
}

template<class T>
void AAPiArray<T>::clear()
{
    for (int i =0; i < m_numElements; i++)
        m_elements[i].~T();

    free(m_elements);
    m_numElements = 0;
}

template<class T>
const T& AAPiArray<T>::get_element(int index) const noexcept(false)
{
    for (int i = 0; i < m_numElements; i++)
    {
        if (i == index)
            return m_elements[i];
    }
    throw;
}

template<class T>
T& AAPiArray<T>::get_element(int index) noexcept(false)
{
    for (int i = 0; i < m_numElements; i++)
    {
        if (i == index)
            return m_elements[i];
    }
    throw;
}

template<class T>
const T& AAPiArray<T>::operator [](int i) const
{
    return get_element(i);
}

template<class T>
T& AAPiArray<T>::operator [](int i)
{
    return get_element(i);
}

template<class T>
int AAPiArray<T>::size() const
{
    return m_numElements;
}

template<class T>
AAPiArray<T>& AAPiArray<T>::operator=(const AAPiArray& sa)
{
    if (this != &sa)
    {
        copyFrom(sa);
    }
    return *this;
}

template<class T>
AAPiArray<T>& AAPiArray<T>::operator=(AAPiArray&& sa) noexcept
{
    if (this != &sa)
    {
        moveFrom(sa);
    }
    return *this;
}

template<class T>
void AAPiArray<T>::copyFrom(const AAPiArray& sa)
{
    clear();

    if (sa.m_numElements > 0)
    {
        m_elements = reinterpret_cast<T *>(malloc( sa.m_numElements * sizeof (T) ));
        if (m_elements != nullptr)
        {
            m_numElements = sa.m_numElements;
            for (int i = 0; i < m_numElements; i++)
            {
                new (&m_elements[i]) T ();
                m_elements[i] = sa.m_elements[i];
            }
        }
    }
}

template<class T>
void AAPiArray<T>::moveFrom(AAPiArray& sa)
{
    clear();
    // move
    m_elements = sa.m_elements;
    m_numElements = sa.m_numElements;
    sa.m_elements = nullptr;
    sa.m_numElements = 0;
}

} //namespace aapi

#endif // AAPI_SIMPLE_ARRAY_H
