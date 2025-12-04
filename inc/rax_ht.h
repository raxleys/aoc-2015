////////////////////////////////////////////////////////////////////////////
// rax_ht.h - Single-header templatable hash table implementation,        //
//            in C.                                                       //
// Copyright (C) 2025 Alexander Reyes <raxleys@gmail.com>                 //
//                                                                        //
// This program is free software: you can redistribute it and/or modify   //
// it under the terms of the GNU General Public License as published by   //
// the Free Software Foundation, either version 3 of the License, or      //
// (at your option) any later version.                                    //
//                                                                        //
// This program is distributed in the hope that it will be useful,        //
// but WITHOUT ANY WARRANTY; without even the implied warranty of         //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
// GNU General Public License for more details.                           //
//                                                                        //
// You should have received a copy of the GNU General Public License      //
// along with this program.  If not, see <https://www.gnu.org/licenses/>. //
////////////////////////////////////////////////////////////////////////////
#ifndef RAX_HT_H
#define RAX_HT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// Macro magic for generating type names correctly
#define RAX_HT_IMPL(word)         RAX_HT_JOIN(RAX_HT_PREFIX, word)
#define RAX_HT_JOIN(pre, word)    RAX_HT_JOIN2(pre, word)
#define RAX_HT_JOIN2(pre, word)   pre##word

#define RAX_HT_STRINGIFY(X)       #X
#define RAX_HT_STRINGIFY_MACRO(X) RAX_HT_STRINGIFY(X)

#ifndef RAX_HT_MALLOC
#define RAX_HT_MALLOC  malloc
#endif

#ifndef RAX_HT_CALLOC
#define RAX_HT_CALLOC  calloc
#endif

#ifndef RAX_HT_REALLOC
#define RAX_HT_REALLOC realloc
#endif

#ifndef RAX_HT_FREE
#define RAX_HT_FREE    free
#endif

// NOTE: This needs to be a power of 2.
#ifndef RAX_HT_INIT_CAPACITY
#define RAX_HT_INIT_CAPACITY 64
#endif

#ifndef RAX_HT_HASH_FUN
#define RAX_HT_HASH_FUN rax_ht_hash
#endif

#define RAX_HT_FNV_OFFSET 14695981039346656037ULL
#define RAX_HT_FNV_PRIME  1099511628211ULL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static inline uint64_t rax_ht_hash(const void *key, size_t size)
{
    uint64_t hash = RAX_HT_FNV_OFFSET;
    const unsigned char *bytes = key;
    for (size_t i = 0; i < size; i++) {
        hash ^= (uint64_t)bytes[i];
        hash *= RAX_HT_FNV_PRIME;
    }

    return hash;
}

#endif // RAX_HT_H

#ifndef RAX_HT_KEY_TYPE
#error "RAX_HT_KEY_TYPE must be defined"
#endif

#ifndef RAX_HT_VALUE_TYPE
#error "RAX_HT_VALUE_TYPE must be defined"
#endif

// This function is called to make an actual hash of a key.
// This can & should be defined by the user for keys that are complex types.
// I.e. for char *: #define RAX_HT_HASH(KEY) (RAX_HT_HASH_FUN((KEY), strlen((KEY))))
#ifndef RAX_HT_HASH
#define RAX_HT_HASH(X) (RAX_HT_HASH_FUN((&X), sizeof(X)))
#endif

// This function is called to check for equality of keys.
// This can & should be defined by the user for keys that are complex types.
// I.e. for char *: #define RAX_HT_KEY_EQUAL(X, Y) (strcmp((X), (Y)) == 0)
#ifndef RAX_HT_KEY_EQUAL
#define RAX_HT_KEY_EQUAL(X, Y) (X == Y)
#endif

// This function is called to destroy keys.
// This can & should be defined by the user for keys that are complex types.
// I.e. for char *: #define RAX_HT_DESTROY_KEY(KEY) free(KEY)
#ifndef RAX_HT_DESTROY_KEY
#define RAX_HT_DESTROY_KEY(X)
#endif

// This function is called to destroy keys.
// This can & should be defined by the user for keys that are complex types.
// I.e. for char *: #define RAX_HT_DESTROY_VALUE(VALUE) free(VALUE)
#ifndef RAX_HT_DESTROY_VALUE
#define RAX_HT_DESTROY_VALUE(X)
#endif

// I.e. if not supplied, (int, int) -> rax_ht_int_int
#ifndef RAX_HT_NAME
#define RAX_HT_NAME RAX_HT_JOIN(RAX_HT_JOIN(RAX_HT_JOIN(RAX_HT_JOIN(rax_ht, _), RAX_HT_KEY_TYPE), _), RAX_HT_VALUE_TYPE)
#endif

// Prefix for generated functions
#ifndef RAX_HT_PREFIX
#define RAX_HT_PREFIX RAX_HT_JOIN(RAX_HT_NAME, _)
#endif

// Entry struct
#define RAX_HT_ENTRY_NAME RAX_HT_JOIN(RAX_HT_PREFIX, entry)

// Iterator
#define RAX_HT_IT_NAME RAX_HT_JOIN(RAX_HT_PREFIX, it)

// Customize linkage
#ifndef RAX_HT_LINKAGE
#define RAX_HT_LINKAGE static inline
#endif

// Struct definition
typedef struct RAX_HT_ENTRY_NAME RAX_HT_ENTRY_NAME;
struct RAX_HT_ENTRY_NAME {
    bool is_occupied;
    RAX_HT_KEY_TYPE key;
    RAX_HT_VALUE_TYPE value;
};

typedef struct RAX_HT_NAME RAX_HT_NAME;
struct RAX_HT_NAME {
    size_t size;
    size_t capacity;
    RAX_HT_ENTRY_NAME *entries;
};

typedef struct RAX_HT_IT_NAME RAX_HT_IT_NAME;
struct RAX_HT_IT_NAME {
    size_t i;
    RAX_HT_NAME *ht;
    RAX_HT_KEY_TYPE *key;
    RAX_HT_VALUE_TYPE *value;
};

// Function names (public)
#define RAX_HT_hash        RAX_HT_IMPL(hash)
#define RAX_HT_destroy     RAX_HT_IMPL(destroy)
#define RAX_HT_clear       RAX_HT_IMPL(clear)
#define RAX_HT_iter        RAX_HT_IMPL(iter)
#define RAX_HT_next        RAX_HT_IMPL(next)
#define RAX_HT_set         RAX_HT_IMPL(set)
#define RAX_HT_get         RAX_HT_IMPL(get)
#define RAX_HT_key_in      RAX_HT_IMPL(key_in)
#define RAX_HT_remove      RAX_HT_IMPL(remove)
#define RAX_HT_drop        RAX_HT_IMPL(drop)
#define RAX_HT_extractkv   RAX_HT_IMPL(extractkv)

// Function names (internal)
#define RAX_HT_grow         RAX_HT_IMPL(grow)
#define RAX_HT_internal_set RAX_HT_IMPL(internal_set)
#define RAX_HT_get_index    RAX_HT_IMPL(get_index)

////////// Function implementations //////////

// Return the hash of a given key.
RAX_HT_LINKAGE uint64_t RAX_HT_hash(const RAX_HT_KEY_TYPE key)
{
    return RAX_HT_HASH(key);
}

// Create a new iterator for the hash table
RAX_HT_LINKAGE RAX_HT_IT_NAME RAX_HT_iter(RAX_HT_NAME *ht)
{
    RAX_HT_IT_NAME it = {0};
    it.i = 0;
    it.ht = ht;
    return it;
}

// Advance the given iterator to point to the next value in the table.
RAX_HT_LINKAGE bool RAX_HT_next(RAX_HT_IT_NAME *it)
{
    while (it->i < it->ht->capacity) {
        RAX_HT_ENTRY_NAME *entry = &it->ht->entries[it->i++];
        if (entry->is_occupied) {
            it->key = &entry->key;
            it->value = &entry->value;
            return true;
        }
    }

    return false;
}

// Attempt to get the index a key resides at in a given table.
// The index is returned through the pointer index, which must not be NULL.
// Returns true if the key was found in the hash table, else false.
RAX_HT_LINKAGE bool RAX_HT_get_index(RAX_HT_ENTRY_NAME *entries, size_t capacity, const RAX_HT_KEY_TYPE key, size_t *index)
{
    // If nothing has ever been inserted into the table, the capacity could be 0.
    // mod 0 is undefined, and capacity - 1 could be an integer overflow
    if (capacity == 0)
        return false;
    else if (capacity == 1)
        return RAX_HT_KEY_EQUAL(key, entries[0].key);

    uint64_t hash = RAX_HT_hash(key);
    size_t i = (size_t)(hash & (uint64_t)(capacity - 1));
    while (entries[i].is_occupied) {
        if (RAX_HT_KEY_EQUAL(key, entries[i].key)) {
            *index = i;
            return true;
        }

        // Increment & wrap if necessary
        i++;
        if (i >= capacity)
            i = 0;
    }

    *index = i;
    return false;
}

// INTERNAL: Set helper
// Should not be called externally. It does not check for size of the table.
// If key does not exist in the hash table, take ownership of key and value, and insert them into the table.
// If key does exist in the hash table, call the destructor on the new key, and old value, and put the new value in the table.
// Increment size if it is non-NULL.
// returns a pointer to the newly inserted value.
RAX_HT_LINKAGE RAX_HT_VALUE_TYPE *RAX_HT_internal_set(RAX_HT_ENTRY_NAME *entries, size_t capacity, size_t *size,
                                                      RAX_HT_KEY_TYPE key, RAX_HT_VALUE_TYPE val)
{
    size_t index = 0;
    if (RAX_HT_get_index(entries, capacity, key, &index)) {
        // Key exists. Drop the new key & old value, and update internally.
        // Do not increment the size of the table.
        RAX_HT_DESTROY_KEY(key);
        RAX_HT_DESTROY_VALUE(entries[index].value);
        entries[index].value = val;
        return &entries[index].value;
    }

    // Key was not found in table.
    // Take ownership of the key
    entries[index].key = key;
    entries[index].value = val;
    entries[index].is_occupied = true;

    // Increment the size if non-NULL
    // (Only NULL when resizing the table)
    if (size != NULL)
        (*size)++;

    return &entries[index].value;
}

// Grow the hashtable.
// Return false if memory allocation fails, otherwise true.
RAX_HT_LINKAGE bool RAX_HT_grow(RAX_HT_NAME *ht)
{
    size_t new_cap = (ht->capacity == 0) ? RAX_HT_INIT_CAPACITY : ht->capacity * 2;
    RAX_HT_ENTRY_NAME *new_entries = RAX_HT_CALLOC(new_cap, sizeof(*new_entries));
    if (!new_entries) {
        perror(RAX_HT_STRINGIFY_MACRO(RAX_HT_grow) " - calloc failed!");
        return false;
    }

    for (size_t i = 0; i < ht->capacity; i++) {
        RAX_HT_ENTRY_NAME entry = ht->entries[i];
        if (entry.is_occupied)
            RAX_HT_internal_set(new_entries, new_cap, NULL, entry.key, entry.value);
    }

    if (ht->entries)
        free(ht->entries);
    ht->entries = new_entries;
    ht->capacity = new_cap;
    return true;
}

// Attempt to set the value at key key to value.
// The table takes ownership of any keys and values passed to it.
// If a key exists, the value will be replaced, and the destructor of the new key & old value will be called.
// If a key doesn't exist, ownership will still be taken and the key, value pair inserted into the map.
// Returns a pointer to the new value on success, otherwise NULL.
RAX_HT_LINKAGE RAX_HT_VALUE_TYPE *RAX_HT_set(RAX_HT_NAME *ht, RAX_HT_KEY_TYPE key, RAX_HT_VALUE_TYPE val)
{
    if (ht->size >= ht->capacity / 2) {
        if (!RAX_HT_grow(ht))
            return NULL;
    }

    return RAX_HT_internal_set(ht->entries, ht->capacity, &ht->size, key, val);
}

// Attempt to retrieve the value pointed to by the given key.
// If the value is found, a pointer to the value is returned.
// If the value is not found, NULL is returned.
RAX_HT_LINKAGE RAX_HT_VALUE_TYPE *RAX_HT_get(const RAX_HT_NAME *ht, const RAX_HT_KEY_TYPE key)
{
    size_t index = 0;
    if (RAX_HT_get_index(ht->entries, ht->capacity, key, &index))
        return &ht->entries[index].value;

    return NULL;
}

// Check whether a key exists in the hashtable.
// Return true if found, otherwise false.
RAX_HT_LINKAGE bool RAX_HT_key_in(const RAX_HT_NAME *ht, const RAX_HT_KEY_TYPE key)
{
    size_t dummy;
    return RAX_HT_get_index(ht->entries, ht->capacity, key, &dummy);
}

// Drop the key/value pair referenced by key in the hash table.
// Destructors for both will be called.
// true is returned if the key was present & destroyed. Otherwise false.
RAX_HT_LINKAGE bool RAX_HT_drop(RAX_HT_NAME *ht, const RAX_HT_KEY_TYPE key)
{
    size_t index = 0;
    if (RAX_HT_get_index(ht->entries, ht->capacity, key, &index)) {
        RAX_HT_DESTROY_KEY(ht->entries[index].key);
        RAX_HT_DESTROY_VALUE(ht->entries[index].value);
        memset(&ht->entries[index], 0, sizeof(*ht->entries));
        ht->size--;
        return true;
    }

    return false;
}

// Attempt to remove & return a value from the hash table.
// If the key exists, the destructor is called on the key and the value is removed from the table.
// The value is returned through the pointer value.
// Return true if the value was removed from the table, otherwise false.
RAX_HT_LINKAGE bool RAX_HT_remove(RAX_HT_NAME *ht, const RAX_HT_KEY_TYPE key, RAX_HT_VALUE_TYPE *value)
{
    size_t index = 0;
    if (!RAX_HT_get_index(ht->entries, ht->capacity, key, &index))
        return false;

    assert(value != NULL && "NULL value passed as value pointer!");

    *value = ht->entries[index].value;
    RAX_HT_DESTROY_KEY(ht->entries[index].key);
    memset(&ht->entries[index], 0, sizeof(*ht->entries));
    ht->size--;
    return true;
}

// Attempt to extract a key/value pair from the hash table.
// If the value does not exist, nothing is altered and false is returned.
// If the value does exist, the owned key and value are returned through the corresponding parameters.
// owned_key and owned_value should not be NULL.
// Return true if the key/value pair is successfully extractd from the table.
RAX_HT_LINKAGE bool RAX_HT_extractkv(RAX_HT_NAME *ht, const RAX_HT_KEY_TYPE key, RAX_HT_KEY_TYPE *owned_key, RAX_HT_VALUE_TYPE *owned_val)
{
    size_t index = 0;
    if (!RAX_HT_get_index(ht->entries, ht->capacity, key, &index))
        return false;

    assert(owned_key != NULL && owned_val != NULL && "NULL values passed as owned_key and/or owned_val!");

    *owned_key = ht->entries[index].key;
    *owned_val = ht->entries[index].value;

    ht->entries[index].is_occupied = false;
    ht->size--;
    return true;
}

// Free memory used by the hashtable, and reset its state
// This function will call the destructors of all keys and values.
RAX_HT_LINKAGE void RAX_HT_destroy(RAX_HT_NAME *ht)
{
    RAX_HT_IT_NAME it = RAX_HT_iter(ht);
    while (RAX_HT_next(&it)) {
        RAX_HT_DESTROY_KEY(*it.key);
        RAX_HT_DESTROY_VALUE(*it.value);
    }

    if (ht->entries != NULL)
        RAX_HT_FREE(ht->entries);
    ht->entries = NULL;
    ht->size = 0;
    ht->capacity = 0;
}

// Call destructors on all keys and values, and zero out the table.
RAX_HT_LINKAGE void RAX_HT_clear(RAX_HT_NAME *ht)
{
    RAX_HT_IT_NAME it = RAX_HT_iter(ht);
    while (RAX_HT_next(&it)) {
        RAX_HT_DESTROY_KEY(*it.key);
        RAX_HT_DESTROY_VALUE(*it.value);
    }

    memset(ht->entries, 0, sizeof(*ht->entries) * ht->capacity);
    ht->size = 0;
}

// undef so that they can be redefined and new types generated
#undef RAX_HT_KEY_TYPE
#undef RAX_HT_VALUE_TYPE
#undef RAX_HT_HASH
#undef RAX_HT_KEY_EQUAL
#undef RAX_HT_DESTROY_KEY
#undef RAX_HT_DESTROY_VALUE
#undef RAX_HT_PREFIX
#undef RAX_HT_NAME
#undef RAX_HT_ENTRY_NAME
#undef RAX_HT_IT_NAME
#undef RAX_HT_LINKAGE
