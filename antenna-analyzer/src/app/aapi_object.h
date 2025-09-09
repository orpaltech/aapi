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

#ifndef AAPI_OBJECT_H
#define AAPI_OBJECT_H

#include <cassert>
#include <atomic>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define AAPI_ADDREF(ptr)    \
    if (ptr) {              \
        (ptr)->addRef();    \
    }

#define AAPI_DISPOSE(ptr)   \
    if (ptr) {              \
        (ptr)->release();   \
        (ptr) = nullptr;    \
    }

#define DECLARE_AAPI_OBJECT(clazz)              \
public:                                         \
    static clazz* create(bool addRef = true)    \
    {                           \
        clazz *p = new clazz(); \
        if (!p)                 \
            return nullptr;     \
        if (addRef)             \
            p->addRef();        \
        return p;               \
    }

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPiObject
///////////////////////////////////////////////////////////////////////////////

class AAPiObject
{
public:
    long addRef()
    {
        return ++m_ref;
    }

    long release()
    {
        assert(m_ref > 0);

        if (--m_ref == 0)
        {
            delete this;
            return 0;
        }
        return m_ref;
    }

    AAPiObject()
    {
        m_ref = 0;
    }

protected:
    virtual ~AAPiObject()
    {
    }

    std::atomic_long m_ref;

private:
    AAPiObject(const AAPiObject &) {}
    AAPiObject& operator=(const AAPiObject &) { return *this; }
};

///////////////////////////////////////////////////////////////////////////////
// class AAPtr
///////////////////////////////////////////////////////////////////////////////

template<class T> class AAPiPtr
{
public:
    AAPiPtr(T *ptr = nullptr)
    {
        m_ptr = ptr;
        if (m_ptr)
            m_ptr->addRef();
    }

    AAPiPtr(const AAPiPtr<T>& other)
    {
        m_ptr = other.m_ptr;
        if (m_ptr)
            m_ptr->addRef();
    }

    ~AAPiPtr()
    {
        if (m_ptr)
            m_ptr->release();
    }

    AAPiPtr<T>& operator=(T* ptr)
    {
        set(ptr);
        return *this;
    }

    AAPiPtr<T>& operator=(const AAPiPtr<T>& other)
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
            m_ptr->addRef();
    }
};

} //namespace aapi

#endif // AAPI_OBJECT_H
