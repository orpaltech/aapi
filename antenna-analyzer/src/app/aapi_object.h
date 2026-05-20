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

#include <atomic>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define AAPI_ADDREF(ptr)    \
    if (ptr) {              \
        (ptr)->addRef();    \
    }

#define AAPI_DISPOSE(ptr)   \
    do { \
        if (ptr) { \
            (ptr)->release(); \
            (ptr) = nullptr; \
        } \
    } while (0)

#define DECLARE_AAPI_OBJECT(clazz)              \
public:                                         \
    static clazz *create(bool addRef = true);


#define DECLARE_AAPI_OBJECT_WITH_CONFIG(clazz)                      \
DECLARE_AAPI_OBJECT(clazz)                                          \
    static clazz *create(AAPiConfig *config, bool addRef = true);   \
private:                                                            \
    AAPiConfig *m_config;

#define IMPLEMENT_AAPI_OBJECT(clazz)            \
    clazz* clazz::create(bool addRef) {         \
        clazz* p = new(std::nothrow) clazz();   \
        if (p && addRef)                        \
            p->addRef();                        \
        return p;                               \
    }

#define IMPLEMENT_AAPI_OBJECT_WITH_CONFIG(clazz)            \
    IMPLEMENT_AAPI_OBJECT(clazz)                            \
    clazz* clazz::create(AAPiConfig *config, bool addRef) { \
        clazz *obj = create(addRef);                        \
        if( obj ) {                                         \
            obj->m_config = config;                         \
            AAPI_ADDREF(config);                            \
        }                                                   \
        return obj;                                         \
    }

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPiObject
///////////////////////////////////////////////////////////////////////////////

class AAPiObject
{
public:
    long addRef() {
        // fetch_add returns the OLD value; add 1 to return the current count.
        return m_ref.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    long release() {
        // Use release memory order so prior writes are visible to the final thread
        long old_ref = m_ref.fetch_sub(1, std::memory_order_release);
        if (old_ref == 1) {
            // Synchronize with all previous release operations
            std::atomic_thread_fence(std::memory_order_acquire);
            delete this;
            return 0;
        }
        return old_ref - 1;
    }

    AAPiObject() : m_ref(0) {}

protected:
    virtual ~AAPiObject() = default;

private:
    std::atomic_long m_ref;

    AAPiObject(const AAPiObject &) {}
    AAPiObject& operator=(const AAPiObject &) { return *this; }
};

///////////////////////////////////////////////////////////////////////////////
// class AAPtr
///////////////////////////////////////////////////////////////////////////////

template<class T> class AAPiPtr
{
public:
    AAPiPtr(T* ptr = nullptr, bool addRef = true) : m_ptr(ptr) {
        if (m_ptr && addRef)
            m_ptr->addRef();
    }

    // Copy Constructor
    AAPiPtr(const AAPiPtr<T>& other) : m_ptr(other.m_ptr) {
        if (m_ptr)
            m_ptr->addRef();
    }

    // Move Constructor (Zero overhead)
    AAPiPtr(AAPiPtr<T>&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }

    ~AAPiPtr()
    {
        if (m_ptr)
            m_ptr->release();
    }

    // Copy Assignment
    AAPiPtr<T>& operator=(const AAPiPtr<T>& other) {
        if (this != &other) {
            set(other.m_ptr);
        }
        return *this;
    }

    // Move Assignment
    AAPiPtr<T>& operator=(AAPiPtr<T>&& other) noexcept {
        if (this != &other) {
            if (m_ptr)
                m_ptr->release();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    AAPiPtr<T>& operator=(T* ptr) {
        set(ptr);
        return *this;
    }

    // Accessors
    T* get() const { return m_ptr; }
    explicit operator bool() const { return m_ptr != nullptr; }
    operator T* () const { return m_ptr; }
    T* operator->() const { return m_ptr; }
    T& operator*() { return *m_ptr; }
    const T& operator*() const { return *m_ptr; }

    void attach(T* ptr) {
        if (m_ptr)
            m_ptr->release();
        m_ptr = ptr;
    }

    T* detach() {
        T* tmp = m_ptr;
        m_ptr = nullptr;
        return tmp;
    }

    T *m_ptr;
protected:
    void set(T* ptr) {
        if (ptr)
            ptr->addRef();      // AddRef new first (Safety!)
        if (m_ptr)
            m_ptr->release();   // Release old second
        m_ptr = ptr;
    }
};

} //namespace aapi

#endif // AAPI_OBJECT_H
