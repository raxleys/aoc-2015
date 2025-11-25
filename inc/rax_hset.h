////////////////////////////////////////////////////////////////////////////
// rax_ht.h - Single-header templatable hash set implementation, in C.    //
//                                                                        //
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
#ifndef RAX_HSET_H
#define RAX_HSET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

// Macro magic for generating type names correctly
#define RAX_HSET_IMPL(word)         RAX_HSET_JOIN(RAX_HSET_PREFIX, word)
#define RAX_HSET_JOIN(pre, word)    RAX_HSET_JOIN2(pre, word)
#define RAX_HSET_JOIN2(pre, word)   pre##word

#define RAX_HSET_STRINGIFY(X)       #X
#define RAX_HSET_STRINGIFY_MACRO(X) RAX_HSET_STRINGIFY(X)

#ifndef RAX_HSET_MALLOC
#define RAX_HSET_MALLOC  malloc
#endif

#ifndef RAX_HSET_CALLOC
#define RAX_HSET_CALLOC  calloc
#endif

#ifndef RAX_HSET_REALLOC
#define RAX_HSET_REALLOC realloc
#endif

#ifndef RAX_HSET_FREE
#define RAX_HSET_FREE    free
#endif

// NOTE: This needs to be a power of 2.
#ifndef RAX_HSET_INIT_CAPACITY
#define RAX_HSET_INIT_CAPACITY 64
#endif

#ifndef RAX_HSET_HASH_FUN
#define RAX_HSET_HASH_FUN rax_hset_hash
#endif

#define RAX_HSET_FNV_OFFSET 14695981039346656037ULL
#define RAX_HSET_FNV_PRIME  1099511628211ULL

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static inline uint64_t rax_hset_hash(const void *key, size_t size)
{
    uint64_t hash = RAX_HSET_FNV_OFFSET;
    const unsigned char *bytes = key;
    for (size_t i = 0; i < size; i++) {
        hash ^= (uint64_t)bytes[i];
        hash *= RAX_HSET_FNV_PRIME;
    }

    return hash;
}

#endif // RAX_HSET_H

#ifndef RAX_HSET_TYPE
#error "RAX_HSET_TYPE must be defined"
#endif

// This function is called to make an actual hash of a value.
// This can & should be defined by the user for values that are complex types.
// I.e. for char *: #define RAX_HSET_HASH(X) (RAX_HSET_HASH_FUN((X), strlen((X))))
#ifndef RAX_HSET_HASH
#define RAX_HSET_HASH(X) (RAX_HSET_HASH_FUN((&X), sizeof(X)))
#endif

// This function is called to check for equality of values.
// This can & should be defined by the user for values that are complex types.
// I.e. for char *: #define RAX_HSET_EQUAL(X, Y) (strcmp((X), (Y)) == 0)
#ifndef RAX_HSET_EQUAL
#define RAX_HSET_EQUAL(X, Y) (X == Y)
#endif

// This function is called to destroy values.
// This can & should be defined by the user for values that are complex types.
// I.e. for char *: #define RAX_HSET_DESTROY(X) free(X)
#ifndef RAX_HSET_DESTROY
#define RAX_HSET_DESTROY(X)
#endif

// I.e. if not supplied, (int, int) -> rax_ht_int_int
#ifndef RAX_HSET_NAME
#define RAX_HSET_NAME RAX_HSET_JOIN(RAX_HSET_JOIN(rax_hset, _), RAX_HSET_TYPE)
#endif

// Prefix for generated functions
#ifndef RAX_HSET_PREFIX
#define RAX_HSET_PREFIX RAX_HSET_JOIN(RAX_HSET_NAME, _)
#endif

// Entry struct
#define RAX_HSET_ENTRY_NAME RAX_HSET_JOIN(RAX_HSET_PREFIX, entry)

// Iterator
#define RAX_HSET_IT_NAME RAX_HSET_JOIN(RAX_HSET_PREFIX, it)

// Customize linkage
#ifndef RAX_HSET_LINKAGE
#define RAX_HSET_LINKAGE static inline
#endif

// Struct definition
typedef struct RAX_HSET_ENTRY_NAME RAX_HSET_ENTRY_NAME;
struct RAX_HSET_ENTRY_NAME {
    bool is_occupied;
    RAX_HSET_TYPE value;
};

typedef struct RAX_HSET_NAME RAX_HSET_NAME;
struct RAX_HSET_NAME {
    size_t size;
    size_t capacity;
    RAX_HSET_ENTRY_NAME *entries;
};

typedef struct RAX_HSET_IT_NAME RAX_HSET_IT_NAME;
struct RAX_HSET_IT_NAME {
    size_t i;
    RAX_HSET_NAME *set;
    RAX_HSET_TYPE *value;
};

// Function names (public)

#define RAX_HSET_hash        RAX_HSET_IMPL(hash)
#define RAX_HSET_iter        RAX_HSET_IMPL(iter)
#define RAX_HSET_next        RAX_HSET_IMPL(next)

#define RAX_HSET_set         RAX_HSET_IMPL(set)
#define RAX_HSET_in          RAX_HSET_IMPL(in)

#define RAX_HSET_drop        RAX_HSET_IMPL(drop)
#define RAX_HSET_remove      RAX_HSET_IMPL(remove)

#define RAX_HSET_clear       RAX_HSET_IMPL(clear)
#define RAX_HSET_destroy     RAX_HSET_IMPL(destroy)

// Function names (internal)
#define RAX_HSET_grow         RAX_HSET_IMPL(grow)
#define RAX_HSET_internal_set RAX_HSET_IMPL(internal_set)
#define RAX_HSET_get_index    RAX_HSET_IMPL(get_index)

////////// Function implementations //////////

// Return the hash of a given key.
RAX_HSET_LINKAGE uint64_t RAX_HSET_hash(const RAX_HSET_TYPE key)
{
    return RAX_HSET_HASH(key);
}

// Create a new iterator for the hash table
RAX_HSET_LINKAGE RAX_HSET_IT_NAME RAX_HSET_iter(RAX_HSET_NAME *hset)
{
    RAX_HSET_IT_NAME it = {0};
    it.i = 0;
    it.set = hset;
    return it;
}

// Advance the given iterator to point to the next value in the table.
RAX_HSET_LINKAGE bool RAX_HSET_next(RAX_HSET_IT_NAME *it)
{
    while (it->i < it->set->capacity) {
        RAX_HSET_ENTRY_NAME *entry = &it->set->entries[it->i++];
        if (entry->is_occupied) {
            it->value = &entry->value;
            return true;
        }
    }

    return false;
}

// Attempt to get the index a key resides at in a given table.
// The index is returned through the pointer index, which must not be NULL.
// Returns true if the key was found in the hash table, else false.
RAX_HSET_LINKAGE bool RAX_HSET_get_index(RAX_HSET_ENTRY_NAME *entries, size_t capacity, const RAX_HSET_TYPE value, size_t *index)
{
    // If nothing has ever been inserted into the table, the capacity could be 0.
    // mod 0 is undefined, and capacity - 1 could be an integer overflow
    if (capacity == 0)
        return false;
    else if (capacity == 1)
        return RAX_HSET_EQUAL(value, entries[0].value);

    uint64_t hash = RAX_HSET_hash(value);
    size_t i = (size_t)(hash & (uint64_t)(capacity - 1));
    while (entries[i].is_occupied) {
        if (RAX_HSET_EQUAL(value, entries[i].value)) {
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
RAX_HSET_LINKAGE RAX_HSET_TYPE *RAX_HSET_internal_set(RAX_HSET_ENTRY_NAME *entries, size_t capacity, size_t *size, RAX_HSET_TYPE value)
{
    size_t index = 0;
    if (RAX_HSET_get_index(entries, capacity, value, &index)) {
        // Value exists. Drop the new value.
        // Do not increment the size of the table.
        RAX_HSET_DESTROY(value);
        return &entries[index].value;
    }

    // Key was not found in table.
    // Take ownership of the value
    entries[index].value = value;
    entries[index].is_occupied = true;

    // Increment the size if non-NULL
    // (Only NULL when resizing the table)
    if (size != NULL)
        (*size)++;

    return &entries[index].value;
}

// Grow the hashset
// Return false if memory allocation fails, otherwise true.
RAX_HSET_LINKAGE bool RAX_HSET_grow(RAX_HSET_NAME *hset)
{
    size_t new_cap = (hset->capacity == 0) ? RAX_HSET_INIT_CAPACITY : hset->capacity * 2;
    RAX_HSET_ENTRY_NAME *new_entries = RAX_HSET_CALLOC(new_cap, sizeof(*new_entries));
    if (!new_entries) {
        perror(RAX_HSET_STRINGIFY_MACRO(RAX_HSET_grow) " - calloc failed!");
        return false;
    }

    for (size_t i = 0; i < hset->capacity; i++) {
        RAX_HSET_ENTRY_NAME entry = hset->entries[i];
        if (entry.is_occupied)
            RAX_HSET_internal_set(new_entries, new_cap, NULL, entry.value);
    }

    if (hset->entries)
        free(hset->entries);
    hset->entries = new_entries;
    hset->capacity = new_cap;
    return true;
}

// Attempt to add a value to the set.
// The table takes ownership of any values passed to it.
// If a value exists, the destructor of the new value will be called.
// If a value doesn't exist, ownership will still be taken, and the value inserted into the set.
// Returns a pointer to the new value on success, otherwise NULL.
RAX_HSET_LINKAGE RAX_HSET_TYPE *RAX_HSET_set(RAX_HSET_NAME *hset, RAX_HSET_TYPE value)
{
    if (hset->size >= hset->capacity / 2) {
        if (!RAX_HSET_grow(hset))
            return NULL;
    }

    return RAX_HSET_internal_set(hset->entries, hset->capacity, &hset->size, value);
}

// Check whether a value exists in the set.
// Return true if found, otherwise false.
RAX_HSET_LINKAGE bool RAX_HSET_in(const RAX_HSET_NAME *hset, const RAX_HSET_TYPE value)
{
    size_t dummy;
    return RAX_HSET_get_index(hset->entries, hset->capacity, value, &dummy);
}

// Drop a value from the table.
// The Destructor of the value will be called.
// true is returned if the key was present & destroyed. Otherwise false.
RAX_HSET_LINKAGE bool RAX_HSET_drop(RAX_HSET_NAME *hset, const RAX_HSET_TYPE value)
{
    size_t index = 0;
    if (RAX_HSET_get_index(hset->entries, hset->capacity, value, &index)) {
        RAX_HSET_DESTROY(hset->entries[index].value);
        memset(&hset->entries[index], 0, sizeof(*hset->entries));
        hset->size--;
        return true;
    }

    return false;
}

// Attempt to remove & return a value from the hash set.
// The value is returned through the pointer value.
// Return true if the value was removed from the table, otherwise false.
RAX_HSET_LINKAGE bool RAX_HSET_remove(RAX_HSET_NAME *hset, const RAX_HSET_TYPE value_ref, RAX_HSET_TYPE *value)
{
    size_t index = 0;
    if (!RAX_HSET_get_index(hset->entries, hset->capacity, value_ref, &index))
        return false;

    assert(value != NULL && "NULL value passed as value pointer!");

    *value = hset->entries[index].value;
    memset(&hset->entries[index], 0, sizeof(*hset->entries));
    hset->size--;
    return true;
}

// Call destructors on all values, and zero out the set.
RAX_HSET_LINKAGE void RAX_HSET_clear(RAX_HSET_NAME *hset)
{
    RAX_HSET_IT_NAME it = RAX_HSET_iter(hset);
    while (RAX_HSET_next(&it))
        RAX_HSET_DESTROY(*it.value);

    memset(hset->entries, 0, sizeof(*hset->entries) * hset->capacity);
    hset->size = 0;
}

// Free memory used by the hashset, and reset its state
// This function will call the destructors of all values.
RAX_HSET_LINKAGE void RAX_HSET_destroy(RAX_HSET_NAME *hset)
{
    RAX_HSET_IT_NAME it = RAX_HSET_iter(hset);
    while (RAX_HSET_next(&it))
        RAX_HSET_DESTROY(*it.value);

    if (hset->entries != NULL)
        RAX_HSET_FREE(hset->entries);

    hset->entries = NULL;
    hset->size = 0;
    hset->capacity = 0;
}

// undef so that they can be redefined and new types generated
#undef RAX_HSET_TYPE
#undef RAX_HSET_HASH
#undef RAX_HSET_EQUAL
#undef RAX_HSET_DESTROY
#undef RAX_HSET_PREFIX
#undef RAX_HSET_NAME
#undef RAX_HSET_ENTRY_NAME
#undef RAX_HSET_IT_NAME
#undef RAX_HSET_LINKAGE
