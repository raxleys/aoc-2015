////////////////////////////////////////////////////////////////////////////
// rax_da.h - Single-header dynamic array implementation                  //
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
#ifndef RAX_DA_H
#define RAX_DA_H

#include <stdint.h>

#ifndef RAX_DA_INIT_CAP
#define RAX_DA_INIT_CAP 32
#endif

#ifndef RAX_DA_REALLOC_FACTOR
#define RAX_DA_REALLOC_FACTOR 2
#endif

#ifndef RAX_DA_REALLOC
#include <stdlib.h>
#define RAX_DA_REALLOC realloc
#endif

#ifndef RAX_DA_FREE
#include <stdlib.h>
#define RAX_DA_FREE free
#endif

// Internal
typedef struct {
    size_t size;
    size_t cap;
} rax__da_head;

#define rax__da_get_head(DA) ((rax__da_head *)(DA) - 1)

// Macros
#define rax_da_size(DA) ((DA) ? (rax__da_get_head(DA)->size) : 0)
#define rax_da_sizei(DA) ((DA) ? (ptrdiff_t)(rax__da_get_head(DA)->size) : 0)
#define rax_da_reserve(DA, COUNT) ((DA) = rax__da_grow_if_needed(DA, COUNT, sizeof(*(DA))))

#define rax_da_append(DA, EL) (((DA) = rax__da_grow_if_needed(DA, 1, sizeof(*(DA)))) ? ((DA)[rax__da_get_head(DA)->size++] = (EL), (DA)) : (NULL))
#define rax_da_pop(DA) ((DA)[--rax__da_get_head(DA)->size])

// Drop/remove an element
#define rax_da_drop(DA, IDX) (rax__da_drop(DA, IDX, sizeof(*(DA))))

// Remove an element and place it into PTR
#define rax_da_remove(DA, IDX, PTR) (rax__da_remove(DA, PTR, IDX, sizeof(*(DA))))

// When we do not want to directly append. Consider this:
// update_some_struct(my_arr[i]);
// We may want to pass a pointer directly.
//
// ptr is not guaranteed to be zero-initialized.
#define rax_da_append_ptr(DA) (((DA) = rax__da_grow_if_needed(DA, 1, sizeof(*(DA)))) ? (&(DA)[rax__da_get_head(DA)->size++]) : (NULL))

// Functions
void rax_da_free(void *da);

// Internal
void *rax__da_grow_if_needed(void *da, size_t count, size_t el_size);

// Arena support
#ifdef RAX_DA_ARENA_SUPPORT
/* #include "rax_arena.h" */
#endif

#endif // RAX_DA_H

#ifdef RAX_DA_IMPLEMENTATION
#define RAX_DA_MAX(X, Y) ((X) > (Y) ? (X) : (Y))

void *rax__da_grow_if_needed(void *da, size_t count, size_t el_size)
{
    rax__da_head *head = da ? rax__da_get_head(da) : NULL;
    size_t size = head ? head->size : 0;
    size_t cap = head ? head->cap : 0;

    if (size + count <= cap) return da;
    size_t new_cap = RAX_DA_MAX(cap * RAX_DA_REALLOC_FACTOR, RAX_DA_MAX(count, RAX_DA_INIT_CAP));

    rax__da_head *new_head = RAX_DA_REALLOC(head, sizeof(*new_head) + new_cap * el_size);
    if (!new_head) {
#ifndef RAX_DA_LEAK_ON_REALLOC_FAIL
#else
        RAX_DA_FREE(head);
#endif
        return NULL;
    }

    new_head->size = size;
    new_head->cap = new_cap;
    return new_head + 1;
}

void rax_da_free(void *da)
{
    if (da) {
        rax__da_head *head = rax__da_get_head(da);
        RAX_DA_FREE(head);
    }
}

void rax__da_drop(void *da, size_t idx, size_t el_size)
{
    rax__da_head *head = rax__da_get_head(da);
    if (idx >= head->size) return;

    if (idx == head->size - 1) {
        head->size--;
        return;
    }

    unsigned char *daptr = da;
    memmove(daptr + idx * el_size, daptr + (idx + 1) * el_size, (head->size - (idx + 1)) * el_size);
    head->size--;
}

void rax__da_remove(void *da, void *buf, size_t idx, size_t el_size)
{
    rax__da_head *head = rax__da_get_head(da);
    if (idx >= head->size) return;

    // Extract value
    unsigned char *daptr = da;
    memcpy(buf, daptr + idx * el_size, el_size);

    // Call da_drop
    rax__da_drop(da, idx, el_size);
}

#endif // RAX_DA_IMPLEMENTATION

#ifndef RAX_DA_NO_STRIP_PREFIX

#define da_size         rax_da_size
#define da_sizei        rax_da_sizei
#define da_reserve      rax_da_reserve
#define da_free         rax_da_free
#define da_append       rax_da_append
#define da_append_ptr   rax_da_append_ptr
#define da_pop          rax_da_pop
#define da_drop         rax_da_drop
#define da_remove       rax_da_remove

#endif // RAX_DA_NO_STRIP_PREFIX
