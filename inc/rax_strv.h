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

#include <stdbool.h>
#include <stddef.h>

// TODO: Move?
#ifndef RAX_ASSERT
#include <assert.h>
#define RAX_ASSERT assert
#endif // RAX_ASSERT

#ifndef RAX_STRV_MALLOC
#include <stdlib.h>
#define RAX_STRV_MALLOC malloc
#endif // RAX_STRV_MALLOC

// Size of statically allocated buffer for creating temporary strings
#ifndef RAX_STRV_TEMP_BUF_SIZE
#define RAX_STRV_TEMP_BUF_SIZE 4096
#endif // RAX_STRV_TEMP_BUF_SIZE

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

// Return a slice of a string view
// [start, end)
rax_strv rax_strv_slice(rax_strv sv, size_t start, size_t end);

// Trim all whitespace from the left of a string view
rax_strv rax_strv_trim_left(rax_strv sv);

// Trim all whitespace from the right of a string view
rax_strv rax_strv_trim_right(rax_strv sv);

// Trim all whitespace from both sides of a string view
rax_strv rax_strv_trim(rax_strv sv);

// Calls strncmp on the underlying owned string slice
int rax_strv_cmp_n(rax_strv first, rax_strv second, size_t n);

// Behave like strcmp, but works on string view.
int rax_strv_cmp(rax_strv first, rax_strv second);

// Checks if the underlying string slices are equal
bool rax_strv_eq(rax_strv first, rax_strv second);

// Compare a strv with a c-string
bool rax_strv_eq_cstr(rax_strv sv, const char *s);

// Return true if a string view starts with another string view
bool rax_strv_starts_with(rax_strv haystack, rax_strv needle);

// Return true if the view points to an empty string
bool rax_strv_is_empty(rax_strv sv);

// Return a string allocated with RAX_STRV_MALLOC
char *rax_strv_to_cstr_owned(rax_strv sv);

// Return a string allocated in the internal static buffer
// String is transient and will be overwritten on another call to
// rax_strv_*_temp
char *rax_strv_to_cstr_temp(rax_strv sv);

// Create an iterator that "splits" a string into string view slices
// over a delimiter
void rax_strv_split(rax_strv_it *it, const char *s, const char *delim);

// rax_strv_split over the newline character
// TODO: DOS support
void rax_strv_lines(rax_strv_it *it, const char *s);

// Return a string view over the next word in a string view
// A word is a string surrounded by spaces
// This advances the past string view to point to the beginning of the
// next word.
rax_strv rax_strv_pop_word(rax_strv *sv);

// Non destructive version of rax_strv_pop_word.
// Creates a strv over the next word only. Does not alter the original view.
rax_strv rax_strv_peek_word(rax_strv sv);

// Advance a string view iterator
bool rax_strv_next(rax_strv_it *it);

// strtol wrapper
bool rax_strv_parse_long(rax_strv word, long *n);

#endif // RAX_STRV_H
#ifdef RAX_STRV_IMPLEMENTATION

#include <string.h>
#include <ctype.h>

static char rax__strv_temp_buf[RAX_STRV_TEMP_BUF_SIZE];

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

rax_strv rax_strv_slice(rax_strv sv, size_t start, size_t end)
{
    RAX_ASSERT(end > start && start < sv.size && end <= sv.size);
    return rax_strv_from_range(sv.str, start, end);
}

rax_strv rax_strv_trim_left(rax_strv sv)
{
    if (rax_strv_is_empty(sv)) return sv;

    size_t start = 0;
    for (; start < sv.size && isspace(sv.str[start]); start++);
    return rax_strv_chop_left(sv, start);
}

rax_strv rax_strv_trim_right(rax_strv sv)
{
    if (rax_strv_is_empty(sv)) return sv;

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

// TODO: Move?
#define RAX_STRV_MIN(X, Y) ((X) < (Y) ? (X) : (Y))

int rax_strv_cmp_n(rax_strv first, rax_strv second, size_t n)
{
    return strncmp(first.str, second.str, n);
}

int rax_strv_cmp(rax_strv first, rax_strv second)
{
    // First compare first N bytes
    size_t n = RAX_STRV_MIN(first.size, second.size);
    size_t n_cmp = rax_strv_cmp_n(first, second, n);
    if (n_cmp != 0 || (n_cmp == 0 && first.size == second.size))
        return n_cmp;

    // First n chars are equal, but lengths differ.
    if (first.size > second.size)
        return 1;
    else
        return -1;
}

bool rax_strv_eq(rax_strv first, rax_strv second)
{
    return rax_strv_cmp(first, second) == 0;
}

bool rax_strv_eq_cstr(rax_strv sv, const char *s)
{
    return rax_strv_eq(sv, rax_strv_from(s));
}

bool rax_strv_starts_with(rax_strv haystack, rax_strv needle)
{
    if (needle.size > haystack.size)
        return false;

    return rax_strv_cmp_n(haystack, needle, needle.size) == 0;
}

bool rax_strv_is_empty(rax_strv sv)
{
    return sv.size == 0;
}

char *rax_strv_to_cstr_owned(rax_strv sv)
{
    // TODO: Just use strndup?
    char *buf = RAX_STRV_MALLOC(sv.size + 1);
    if (!buf) return NULL;

    memcpy(buf, sv.str, sv.size);
    buf[sv.size] = '\0';
    return buf;
}

char *rax_strv_to_cstr_temp(rax_strv sv)
{
    // TODO: Or truncate?
    RAX_ASSERT(sv.size < RAX_STRV_TEMP_BUF_SIZE);

    memcpy(rax__strv_temp_buf, sv.str, sv.size);
    rax__strv_temp_buf[sv.size] = '\0';
    return rax__strv_temp_buf;
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

rax_strv rax_strv_pop_word(rax_strv *sv)
{
    size_t i = 0;
    for (; i < sv->size && !isspace(sv->str[i]); i++);

    rax_strv word = rax_strv_chop_right(*sv, sv->size - i);
    rax_strv rest = rax_strv_trim_left(rax_strv_chop_left(*sv, i));
    sv->str = rest.str;
    sv->size = rest.size;

    return word;
}

rax_strv rax_strv_peek_word(rax_strv sv)
{
    size_t i = 0;
    for (; i < sv.size && !isspace(sv.str[i]); i++);

    rax_strv word = rax_strv_chop_right(sv, sv.size - i);
    return word;
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

bool rax_strv_parse_long(rax_strv word, long *n)
{
    if (word.size <= 0)
        return false;

    // Should fit into a long, so just use a buffer of appropriate size
    char buf[512];
    if (word.size + 1 > sizeof(buf))
        return false;

    memcpy(buf, word.str, word.size);
    buf[word.size] = '\0';

    char *end = NULL;
    long val;
    val = strtol(buf, &end, 10);
    if (*end != '\0')
        return false;

    *n = val;
    return true;
}

#endif // RAX_STRV_IMPLEMENTATION
#undef RAX_STRV_IMPLEMENTATION

#ifndef RAX_NO_STRIP_PREFIX

#define strv               rax_strv
#define strv_it            rax_strv_it
#define strv_fmt           rax_strv_fmt
#define strv_arg           rax_strv_arg
#define strv_from_range    rax_strv_from_range
#define strv_from          rax_strv_from
#define strv_dup           rax_strv_dup
#define strv_chop_left     rax_strv_chop_left
#define strv_chop_right    rax_strv_chop_right
#define strv_slice         rax_strv_slice
#define strv_trim_left     rax_strv_trim_left
#define strv_trim_right    rax_strv_trim_right
#define strv_trim          rax_strv_trim
#define strv_cmp_n         rax_strv_cmp_n
#define strv_cmp           rax_strv_cmp
#define strv_eq            rax_strv_eq
#define strv_eq_cstr       rax_strv_eq_cstr
#define strv_starts_with   rax_strv_starts_with
#define strv_is_empty      rax_strv_is_empty
#define strv_to_cstr_owned rax_strv_to_cstr_owned
#define strv_to_cstr_temp  rax_strv_to_cstr_temp
#define strv_split         rax_strv_split
#define strv_pop_word      rax_strv_pop_word
#define strv_peek_word     rax_strv_peek_word
#define strv_lines         rax_strv_lines
#define strv_next          rax_strv_next
#define strv_parse_long    rax_strv_parse_long

#endif // RAX_NO_STRIP_PREFIX
