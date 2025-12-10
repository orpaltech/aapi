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

#ifndef AAPI_SIMPLE_MAP_H
#define AAPI_SIMPLE_MAP_H


#include <list>
#include <stdexcept>
#include <vector>

namespace aapi
{

///////////////////////////////////////////////////////////////////////////////
// class AAPiSimpleMap
///////////////////////////////////////////////////////////////////////////////
///
/// \brief The AAPiSimpleMap class
///
template<typename K, typename V>
class AAPiSimpleMap
{
private:
    struct KeyValuePair
    {
        K key;
        V value;

        KeyValuePair(const K& k, const V& v) : key(k), value(v) {}
    };

    std::vector<std::list<KeyValuePair>> buckets;
    size_t elementCount;
    static constexpr size_t DEFAULT_BUCKET_COUNT = 16;
    static constexpr float MAX_LOAD_FACTOR = 0.75f;

    // Hash function
    size_t hash(const K& key) const
    {
        return std::hash<K>{}(key) % buckets.size();
    }

    // Rehash when load factor exceeds threshold
    void rehash();

public:
    AAPiSimpleMap() : buckets(DEFAULT_BUCKET_COUNT), elementCount(0) {}

    // Insert or update a key-value pair
    void insert(const K& key, const V& value);

    // Get value by key (throws exception if not found)
    V& get(const K& key);

    // Check if key exists
    bool contains(const K& key) const;

    // Remove a key-value pair
    bool remove(const K& key);

    // Get the number of elements
    size_t size() const { return elementCount; }

    // Check if map is empty
    bool is_empty() const { return elementCount == 0; }

    // Operator[] for convenient access
    V& operator[](const K& key);

    // Get load factor for debugging
    float load_factor() const
    {
        return static_cast<float>(elementCount) / buckets.size();
    }
};

///////////////////////////////////////////////////////////////////////////////


template<typename K, typename V>
void AAPiSimpleMap<K, V>::insert(const K& key, const V& value)
{
    size_t idx = hash(key);

    // Update if key exists
    for (auto& pair : buckets[idx])
    {
        if (pair.key == key)
        {
            pair.value = value;
            return;
        }
    }

    // Insert new key-value pair
    buckets[idx].emplace_back(key, value);
    elementCount++;

    // Rehash if load factor is too high
    if (static_cast<float>(elementCount) / buckets.size() > MAX_LOAD_FACTOR)
    {
        rehash();
    }
}

template<typename K, typename V>
V& AAPiSimpleMap<K, V>::get(const K& key)
{
    size_t idx = hash(key);

    for (auto& pair : buckets[idx])
    {
        if (pair.key == key)
        {
            return pair.value;
        }
    }

    throw std::out_of_range("Key not found");
}

template<typename K, typename V>
bool AAPiSimpleMap<K, V>::contains(const K& key) const
{
    size_t idx = std::hash<K>{}(key) % buckets.size();

    for (const auto& pair : buckets[idx])
    {
        if (pair.key == key)
        {
            return true;
        }
    }

    return false;
}

template<typename K, typename V>
bool AAPiSimpleMap<K, V>::remove(const K& key)
{
    size_t idx = hash(key);

    for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it)
    {
        if (it->key == key)
        {
            buckets[idx].erase(it);
            elementCount--;
            return true;
        }
    }

    return false;
}

template<typename K, typename V>
V& AAPiSimpleMap<K, V>::operator[](const K& key)
{
    size_t idx = hash(key);

    for (auto& pair : buckets[idx])
    {
        if (pair.key == key)
        {
            return pair.value;
        }
    }

    // Insert with default value if not found
    buckets[idx].emplace_back(key, V());
    elementCount++;

    if (static_cast<float>(elementCount) / buckets.size() > MAX_LOAD_FACTOR)
    {
        rehash();
        // Need to find the element again after rehash
        idx = hash(key);
        for (auto& pair : buckets[idx])
        {
            if (pair.key == key)
            {
                return pair.value;
            }
        }
    }

    return buckets[idx].back().value;
}

template<typename K, typename V>
void AAPiSimpleMap<K, V>::rehash()
{
    size_t newSize = buckets.size() * 2;
    std::vector<std::list<KeyValuePair>> newBuckets(newSize);

    for (auto& bucket : buckets)
    {
        for (auto& pair : bucket)
        {
            size_t idx = std::hash<K>{}(pair.key) % newSize;
            newBuckets[idx].emplace_back(pair.key, pair.value);
        }
    }

    buckets = std::move(newBuckets);
}

};  //namespace aapi

#endif // AAPI_SIMPLE_MAP_H
