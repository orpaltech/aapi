/*  Copyright (C) 2013-2018 ORPAL Technologies, Inc. All rights reserved.
 * 	This file is part of ORPAL-AA-Pi software.
 *
 * 	ORPAL-AA-Pi is free software: you can redistribute it and/or modify
 * 	it under the terms of the GNU General Public License as published by
 * 	the Free Software Foundation, either version 3 of the License, or
 * 	(at your option) any later version.
 *
 * 	ORPAL-AA-Pi is distributed in the hope that it will be useful,
 * 	but WITHOUT ANY WARRANTY; without even the implied warranty of
 * 	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 	GNU General Public License for more details.
 *
 * 	You should have received a copy of the GNU General Public License
 * 	along with ORPAL-AA-Pi. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AAPI_SIMPLE_ARRAY_H
#define AAPI_SIMPLE_ARRAY_H

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class SimpleArray
///////////////////////////////////////////////////////////////////////////////

template<class T>
class SimpleArray
{
public:
    SimpleArray();
    ~SimpleArray();

    bool add_element(T *element);
    bool remove_element(T *element);
    bool remove_index(int i);

    T *get_element(int i) const;
    int get_num_elements() const;

private:
    T **m_elements;
    int m_numElements;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
SimpleArray<T>::SimpleArray()
    : m_elements(nullptr)
    , m_numElements(0)
{
}

template<class T>
SimpleArray<T>::~SimpleArray()
{
    free(m_elements);
}

template<class T>
bool SimpleArray<T>::add_element(T *newElement)
{
    T **ptr = reinterpret_cast<T **>(realloc(m_elements, m_numElements + 1));
    if (ptr != nullptr)
    {
        m_elements = ptr;
        m_elements[m_numElements++] = newElement;
        return true;
    }
    return false;
}

template<class T>
bool SimpleArray<T>::remove_element(T *element)
{
    int index = -1;
    for (int i = 0; i < m_numElements; i++)
    {
        if (m_elements[i] == element) {
            index = i;
            break;
        }
    }
    return remove_index(index);
}

template<class T>
bool SimpleArray<T>::remove_index(int i)
{
    if (i < 0 || i >= m_numElements)
        return false;

    int right = m_numElements - i -1;
    if (right > 0)
        memmove(&m_elements[i], &m_elements[i+1], right * sizeof (T*));

    if (--m_numElements == 0)
    {
        free(m_elements);
        m_elements = nullptr;
    }

    return true;
}

template<class T>
T *SimpleArray<T>::get_element(int index) const
{
    for (int i = 0; i < m_numElements; i++)
    {
        if (i == index)
            return m_elements[i];
    }
    return nullptr;
}

template<class T>
int SimpleArray<T>::get_num_elements() const
{
    return m_numElements;
}

} //namespace aapi

#endif // AAPI_SIMPLE_ARRAY_H
