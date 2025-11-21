////////////////////////////////////////////////////////////////////////////
// rax_strv.h - Single-header string view library.                        //
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
#ifndef RAX_STRV_H
#define RAX_STRV_H
#endif // RAX_STRV_H

#include <stdbool.h>
#include <stddef.h>

// TODO: Move?
#ifndef RAX_ASSERT
#include <assert.h>
#define RAX_ASSERT assert
#endif // RAX_ASSERT

typedef struct {
    size_t size;
    const char *str;
} rax_strv;

typedef struct {
    bool _did_start;
    bool _done;
    rax_strv _orig;
    rax_strv _delim;
    rax_strv sv;
} rax_strv_it;

// tsoding's nob.h
#define rax_strv_fmt "%.*s"
#define rax_strv_arg(sv) (int) (sv).size, (sv).str

// Create a string view from a range over a C-string
// [start, end)
rax_strv rax_strv_from_range(const char *s, size_t start, size_t end);

// Create a string view over an entire C-string
rax_strv rax_strv_from(const char *s);

// Duplicate a string view
rax_strv rax_strv_dup(rax_strv sv);

// Remove count characters from the left of a string view
rax_strv rax_strv_chop_left(rax_strv sv, size_t count);

// Remove count characters from the right of a string view
rax_strv rax_strv_chop_right(rax_strv sv, size_t count);

// Trim all whitespace from the left of a string view
rax_strv rax_strv_trim_left(rax_strv sv);

// Trim all whitespace from the right of a string view
rax_strv rax_strv_trim_right(rax_strv sv);

// Trim all whitespace from both sides of a string view
rax_strv rax_strv_trim(rax_strv sv);

// Return true if a string view starts with another string view
bool rax_strv_starts_with(rax_strv haystack, rax_strv needle);

// Create an iterator that "splits" a string into string view slices
// over a delimiter
void rax_strv_split(rax_strv_it *it, const char *s, const char *delim);

// rax_strv_split over the newline character
// TODO: DOS support
void rax_strv_lines(rax_strv_it *it, const char *s);

// Advance a strinv view iterator
bool rax_strv_next(rax_strv_it *it);

#ifdef RAX_STRV_IMPLEMENTATION

#include <string.h>
#include <ctype.h>

rax_strv rax_strv_from_range(const char *s, size_t start, size_t end)
{
    RAX_ASSERT(end >= start);
    return (rax_strv) {
        .size = end - start,
        .str = s + start,
    };
}

rax_strv rax_strv_from(const char *s)
{
    return rax_strv_from_range(s, 0, strlen(s));
}

rax_strv rax_strv_dup(rax_strv sv)
{
    return (rax_strv) {
        .size = sv.size,
        .str = sv.str,
    };
}

rax_strv rax_strv_chop_right(rax_strv sv, size_t count)
{
    RAX_ASSERT(sv.size >= count);
    return rax_strv_from_range(sv.str, 0, sv.size - count);
}

rax_strv rax_strv_chop_left(rax_strv sv, size_t count)
{
    RAX_ASSERT(sv.size >= count);
    return rax_strv_from_range(sv.str, count, sv.size);
}

rax_strv rax_strv_trim_left(rax_strv sv)
{
    size_t start = 0;
    for (; start < sv.size && isspace(sv.str[start]); start++);
    return rax_strv_chop_left(sv, start);
}

rax_strv rax_strv_trim_right(rax_strv sv)
{
    size_t end = sv.size - 1;
    for (; end > 0 && isspace(sv.str[end]); end--);

    // Edge case, whole string is empty
    if (end == 0 && isspace(sv.str[0]))
        return (rax_strv) { .size = 0, .str = sv.str, };

    return rax_strv_chop_right(sv, sv.size - end - 1);
}

rax_strv rax_strv_trim(rax_strv sv)
{
    return rax_strv_trim_left(rax_strv_trim_right(sv));
}

bool rax_strv_starts_with(rax_strv haystack, rax_strv needle)
{
    if (needle.size > haystack.size)
        return false;

    return strncmp(haystack.str, needle.str, needle.size) == 0;
}

void rax_strv_split(rax_strv_it *it, const char *s, const char *delim)
{
    memset(it, 0, sizeof(*it));

    it->_orig = rax_strv_from(s);
    it->_delim = rax_strv_from(delim);

    // Point at the string, but give it a length of 0
    it->sv = (rax_strv) {
        .size = 0,
        .str = s,
    };
}

void rax_strv_lines(rax_strv_it *it, const char *s)
{
    rax_strv_split(it, s, "\n");
}

bool rax_strv_next(rax_strv_it *it)
{
    if (it->_done)
        return false;

    // Temporary string view
    // Starts at the end position of strv (user-facing), and spans the
    // remainder of the string.
    rax_strv tmp = rax_strv_chop_left(it->_orig, (it->sv.str + it->sv.size) - it->_orig.str);

    // Check if we need to advance by a delimiter
    // Skip this in the edge case, we are at the start of the string
    if (it->_did_start && rax_strv_starts_with(tmp, it->_delim))
        tmp = rax_strv_chop_left(tmp, it->_delim.size);

    // From now on, we want to advance delimiters when encountered
    it->_did_start = true;

    // At the end of the string. We need to return a blank string to
    // the user, then stop futher iteration
    if (tmp.size == 0) it->_done = true;

    // Check for a delimiter
    const char *substr = strstr(tmp.str, it->_delim.str);

    // There is no delimeter remaining, so return the rest of the string.
    if (!substr) {
        it->sv = tmp;
        it->_done = true;
        return true;
    }

    // Chop right until start of substr, then return the slice.
    it->sv = rax_strv_chop_right(tmp, tmp.size - (substr - tmp.str));
    return true;
}

#endif // RAX_STRV_IMPLEMENTATION

#ifndef RAX_NO_STRIP_PREFIX

#define strv             rax_strv
#define strv_it          rax_strv_it

#define strv_fmt         rax_strv_fmt
#define strv_arg         rax_strv_arg

#define strv_from_range  rax_strv_from_range
#define strv_from        rax_strv_from
#define strv_dup         rax_strv_dup
#define strv_chop_left   rax_strv_chop_left
#define strv_chop_right  rax_strv_chop_right
#define strv_trim_left   rax_strv_trim_left
#define strv_trim_right  rax_strv_trim_right
#define strv_trim        rax_strv_trim

#define strv_starts_with rax_strv_starts_with
#define strv_split       rax_strv_split
#define strv_lines       rax_strv_lines
#define strv_next        rax_strv_next

#endif // RAX_NO_STRIP_PREFIX
