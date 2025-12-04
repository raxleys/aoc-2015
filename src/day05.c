#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "rax_strv.h"

#define RAX_HSET_TYPE             char *
#define RAX_HSET_NAME             hsets
#define RAX_HSET_HASH(X)          (RAX_HSET_HASH_FUN((X), strlen((X))))
#define RAX_HSET_EQUAL(X, Y)      (strcmp((X), (Y)) == 0)
#define RAX_HSET_DESTROY(X)       free(X)
#include "rax_hset.h"

bool is_vowel(char c)
{
    switch (c) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return true;
    default:
        return false;
    }
}

bool is_string_nice(strv line)
{
    if (line.size < 3) return false;
    int n_vowels = 0;
    bool had_double = false;
    for (size_t i = 0; i < line.size; i++) {
        if (is_vowel(line.str[i])) n_vowels++;

        if (i == 0) continue;

        if (line.str[i] == line.str[i - 1]) had_double = true;
        if (line.str[i] == 'b' && line.str[i - 1] == 'a') return false;
        if (line.str[i] == 'd' && line.str[i - 1] == 'c') return false;
        if (line.str[i] == 'q' && line.str[i - 1] == 'p') return false;
        if (line.str[i] == 'y' && line.str[i - 1] == 'x') return false;
    }

    return n_vowels >= 3 && had_double;
}

bool is_string_nice_2(strv line)
{
    hsets pairs = {0};
    bool has_double_pair = false;
    bool has_sandwiched = false;
    for (size_t i = 0; i < line.size; i++) {
        if (i == 0) continue;

        // Pairs
        // TODO: @RAX_LIB Make dedicated strv map?
        strv pair = strv_slice(line, i - 1, i + 1);
        if (!has_double_pair && hsets_in(&pairs, strv_to_cstr_temp(pair))) {
            // Need to check that we are not overlapping
            //                 v       v
            // But consider: aaa vs aaaa
            // aaa will never satisfy, so we need to be at least 4 chars deep
            if (i > 2) {
                strv prev_pair = strv_slice(line, i - 2, i);
                strv prev_prev_pair = strv_slice(line, i - 3, i - 1);
                if (!strv_eq(pair, prev_pair)) {
                    has_double_pair = true;
                } else if (strv_eq(pair, prev_prev_pair)) {
                    has_double_pair = true;
                }
            }
        } else {
            hsets_set(&pairs, strv_to_cstr_owned(pair));
        }

        // Repeat
        if (!has_sandwiched && i < line.size - 1)
            has_sandwiched = line.str[i - 1] == line.str[i + 1];
    }

    hsets_destroy(&pairs);
    return has_double_pair && has_sandwiched;
}

int day05_nice_strings(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    int lines = 0;
    int nice_lines = 0;
    while (strv_next(&it)) {
        lines++;
        if (is_string_nice(it.sv)) nice_lines++;
    }

    return nice_lines;
}

int day05_nice_strings_2(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    int lines = 0;
    int nice_lines = 0;
    while (strv_next(&it)) {
        lines++;
        if (is_string_nice_2(it.sv)) nice_lines++;
    }

    return nice_lines;
}

void day05_tests()
{
    assert(is_string_nice(strv_from("ugknbfddgicrmopn")));
    assert(is_string_nice(strv_from("aaa")));
    assert(!is_string_nice(strv_from("jchzalrnumimnmhp")));
    assert(!is_string_nice(strv_from("haegwjzuvuyypxyu")));
    assert(!is_string_nice(strv_from("dvszwmarrgswjxmb")));

    assert(is_string_nice_2(strv_from("qjhvhtzxzqqjkmpb")));
    assert(is_string_nice_2(strv_from("xxyxx")));
    assert(!is_string_nice_2(strv_from("uurcxstgmygtbstg")));
    assert(!is_string_nice_2(strv_from("ieodomkazucvgmuy")));
}
