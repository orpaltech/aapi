/*  Copyright (C) 2013-2017 ORPAL Technologies, Inc. All rights reserved.
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

#ifndef AAPI_OBJECT_H
#define AAPI_OBJECT_H

#include <inttypes.h>

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define AAPI_ADDREF(d)  \
    if (d) {            \
        (d)->add_ref(); \
    }

#define AAPI_DISPOSE(d) \
    if (d) {            \
        (d)->release(); \
        (d) = nullptr;  \
    }

#define DECLARE_AAPI_OBJECT(clazz) \
public:                                         \
    static clazz* create(bool add_ref = true)   \
    { \
        clazz *p = new clazz(); \
        if (!p)                 \
            return nullptr;     \
        if (add_ref)            \
            p->add_ref();       \
        return p;               \
    }

///////////////////////////////////////////////////////////////////////////////
// class AAPIObject
///////////////////////////////////////////////////////////////////////////////

class AAPIObject
{
public:
    virtual ~AAPIObject() {}

    long add_ref()
    {
        return ++m_ref;
    }

    long release()
    {
        if (--m_ref == 0)
        {
            delete this;
            return 0;
        }
        return m_ref;
    }

protected:
    AAPIObject()
    {
        m_ref = 0;
    }

    volatile long m_ref;
private:
    AAPIObject(const AAPIObject &) {}
    AAPIObject& operator=(const AAPIObject &) { return *this; }
};

///////////////////////////////////////////////////////////////////////////////
// class aapi_ptr
///////////////////////////////////////////////////////////////////////////////

template<class T> class aapi_ptr
{
public:
    aapi_ptr(T *ptr = nullptr)
    {
        m_ptr = ptr;
        if (m_ptr)
            m_ptr->add_ref();
    }

    aapi_ptr(const aapi_ptr<T>& other)
    {
        m_ptr = other.m_ptr;
        if (m_ptr)
            m_ptr->add_ref();
    }

    ~aapi_ptr()
    {
        if (m_ptr)
            m_ptr->release();
    }

    aapi_ptr<T>& operator=(T* ptr)
    {
        set(ptr);
        return *this;
    }

    aapi_ptr<T>& operator=(const aapi_ptr<T>& other)
    {
        if (this != &other)
        {
            set(other.m_ptr);
        }
        return *this;
    }

    operator T* () const
    {
        return m_ptr;
    }

    T* operator->() const
    {
        return m_ptr;
    }

    const T& operator*() const
    {
        return *m_ptr;
    }

    T& operator*()
    {
        return *m_ptr;
    }

    void attach(T *ptr)
    {
        if (m_ptr)
            m_ptr->release();
        m_ptr = ptr;
    }

    T *detach()
    {
        T *tmp = m_ptr;
        m_ptr = nullptr;
        return tmp;
    }

    T *m_ptr;
protected:
    void set(T *ptr)
    {
        if (m_ptr)
            m_ptr->release();
        m_ptr = ptr;
        if (m_ptr)
            m_ptr->add_ref();
    }
};

} //namespace aapi

#endif // AAPI_OBJECT_H
