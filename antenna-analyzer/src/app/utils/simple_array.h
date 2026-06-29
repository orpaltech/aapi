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

#ifndef AAPI_SIMPLE_ARRAY_H
#define AAPI_SIMPLE_ARRAY_H

#include <utility>
#include <stdexcept>
#include <cstdlib>

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
    AAPiArray() : m_elements(nullptr), m_numElements(0), m_capacity(0) {}
    AAPiArray(const AAPiArray& sa) : m_elements(nullptr), m_numElements(0), m_capacity(0) { copyFrom(sa); }
    AAPiArray(AAPiArray&& sa) noexcept : m_elements(nullptr), m_numElements(0), m_capacity(0) { moveFrom(sa); }
    ~AAPiArray() { clear(); }

    bool append(const T& element);
    bool remove(const T& element);
    bool remove_at(int i);
    void clear();

    bool reserve(uint capacity);

    uint size() const       { return m_numElements; }
    uint capacity() const   { return m_capacity; }

    const T& get_element(int i) const;
    T& get_element(int i);

    const T& operator [](int i) const   { return get_element(i); }
    T& operator [](int i)               { return get_element(i); }

    AAPiArray& operator=(const AAPiArray& sa);
    AAPiArray& operator=(AAPiArray&& sa) noexcept;

private:
    void copyFrom(const AAPiArray& sa);
    void moveFrom(AAPiArray& sa) noexcept;

private:
    T *m_elements;
    uint m_capacity;
    uint m_numElements;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
bool AAPiArray<T>::reserve(uint newCapacity)
{
    // Never shrink memory allocations via reserve
    if (newCapacity <= m_capacity)
        return true;

    T* newBuffer = reinterpret_cast<T*>(std::malloc(newCapacity * sizeof(T)));
    if (!newBuffer)
        return false;

    // Shift active objects forward into the expanded layout
    for (uint i = 0; i < m_numElements; ++i) {
        new (&newBuffer[i]) T(std::move(m_elements[i]));
        m_elements[i].~T();
    }

    std::free(m_elements);
    m_elements = newBuffer;
    m_capacity = newCapacity;
    return true;
}

template<class T>
bool AAPiArray<T>::append(const T& newElement)
{
    // Auto-grow geometric strategy: Double capacity when boundary is breached
    if (m_numElements >= m_capacity) {
        uint nextCapacity = (m_capacity == 0) ? 4 : m_capacity * 2;
        if (!reserve(nextCapacity))
            return false;
    }

    // Direct O(1) assignment into pre-allocated memory slot
    new (&m_elements[m_numElements]) T(newElement);
    ++m_numElements;
    return true;
}

template<class T>
bool AAPiArray<T>::remove(const T& element)
{
    for (int i = 0; i < m_numElements; i++) {
        if (m_elements[i] == element) {
            return remove_at(i);
        }
    }
    return false;
}

template<class T>
bool AAPiArray<T>::remove_at(int i)
{
    if (i < 0 || static_cast<uint>(i) >= m_numElements)
        return false;

    m_elements[i].~T();

    // Shift elements left using clean placement move construction
    for (uint j = static_cast<uint>(i); j < m_numElements - 1; ++j) {
        new (&m_elements[j]) T(std::move(m_elements[j + 1]));
        m_elements[j + 1].~T();
    }

    --m_numElements;
    // To preserve reserve performance benefits, do not shrink on single removals.
    // Memory can be left open for future insertions.
    return true;
}

template<class T>
void AAPiArray<T>::clear()
{
    if (m_elements) {
        for (uint i = 0; i < m_numElements; i++) {
            m_elements[i].~T();
        }
        std::free(m_elements);
        m_elements = nullptr;
    }
    m_numElements = 0;
    m_capacity = 0; // Clear allocation boundary
}

template<class T>
const T& AAPiArray<T>::get_element(int index) const
{
    if (index < 0 || index >= m_numElements) {
        throw std::out_of_range("Index out of bounds");
    }
    return m_elements[index]; // O(1) Fast lookup optimization
}

template<class T>
T& AAPiArray<T>::get_element(int index)
{
    if (index < 0 || index >= m_numElements) {
        throw std::out_of_range("Index out of bounds");
    }
    return m_elements[index]; // O(1) Fast lookup optimization
}

template<class T>
AAPiArray<T>& AAPiArray<T>::operator=(const AAPiArray& sa)
{
    if (this != &sa) {
        copyFrom(sa);
    }
    return *this;
}

template<class T>
AAPiArray<T>& AAPiArray<T>::operator=(AAPiArray&& sa) noexcept
{
    if (this != &sa) {
        moveFrom(sa);
    }
    return *this;
}

template<class T>
void AAPiArray<T>::copyFrom(const AAPiArray& sa)
{
    clear();

    if (sa.m_numElements > 0) {
        m_elements = reinterpret_cast<T*>(std::malloc(sa.m_numElements * sizeof(T)));
        if (m_elements != nullptr) {
            for (uint i = 0; i < sa.m_numElements; i++) {
                new (&m_elements[i]) T(sa.m_elements[i]);
            }
            m_numElements = sa.m_numElements;
            m_capacity = sa.m_numElements;
        }
    }
}

template<class T>
void AAPiArray<T>::moveFrom(AAPiArray& sa) noexcept
{
    clear();

    m_elements = sa.m_elements;
    m_numElements = sa.m_numElements;
    m_capacity = sa.m_capacity;

    sa.m_elements = nullptr;
    sa.m_numElements = 0;
    sa.m_capacity = 0;
}

} //namespace aapi

#endif // AAPI_SIMPLE_ARRAY_H
