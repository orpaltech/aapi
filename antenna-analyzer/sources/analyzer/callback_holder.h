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

#ifndef AAPI_CALLBACK_HOLDER_H
#define AAPI_CALLBACK_HOLDER_H

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// callback_holder
///////////////////////////////////////////////////////////////////////////////

template<class T>
class callback_holder
{
public:
    callback_holder();
    ~callback_holder();

    void add_callback(T *callback);
    void remove_callback(T *callback);
    T *get_callback(int i) const;
    int get_num_callbacks() const;

private:
    T **callbacks;
    int num_callbacks;
};

///////////////////////////////////////////////////////////////////////////////

template<class T>
callback_holder<T>::callback_holder()
    : callbacks(nullptr)
    , num_callbacks(0)
{
}

template<class T>
callback_holder<T>::~callback_holder()
{
    free(callbacks);
}

template<class T>
void callback_holder<T>::add_callback(T *callback)
{
    T **ptr = reinterpret_cast<T **>(realloc(callbacks, num_callbacks + 1));
    if (ptr != nullptr)
    {
        callbacks = ptr;
        callbacks[num_callbacks++] = callback;
    }
}

template<class T>
void callback_holder<T>::remove_callback(T *callback)
{
    for (int i = 0; i < num_callbacks; i++)
    {
        if (callbacks[i] == callback)
        {
            int right = num_callbacks - i -1;
            if (right > 0)
            {
                memmove(&callbacks[i], &callbacks[i+1], right * sizeof (T*));
            }
            num_callbacks --;
            break;
        }
    }

    if (num_callbacks == 0)
    {
        /* Release array of callback pointers */
        free( callbacks );
        callbacks = nullptr;
    }
}

template<class T>
T *callback_holder<T>::get_callback(int index) const
{
    for (int i = 0; i < num_callbacks; i++)
    {
        if (i == index)
            return callbacks[i];
    }
    return nullptr;
}

template<class T>
int callback_holder<T>::get_num_callbacks() const
{
    return num_callbacks;
}

} //namespace aapi

#endif // AAPI_CALLBACK_HOLDER_H
