

int count_literal_chars(strv sv)
{
    size_t count = 0;
    for (size_t i = 0; i < sv.size; i++)
        count++;

    /* printf("literal count: %zu\n", count); */
    return (int)count; // == sv.size ?
}

int count_in_mem_chars(strv sv)
{
    size_t count = 0;
    for (size_t i = 0; i < sv.size; i++) {
        if (sv.str[i] == '"') continue;

        if (sv.str[i] == '\\') {
            // Assume that we will not read garbage (I.e not a single
            // \ at end of string.)
            if (sv.str[i + 1] == '\\' || sv.str[i + 1] == '\"') {
                i++;
            } else {
                // Must be a \x00 type.
                i += 3;
            }
        }

        count++;
    }

    /* printf("memory count: %zu\n", count); */
    return (int)count;
}

int day08_count_chars(const char *input)
{
    strv_it lines = {0};
    strv_lines(&lines, input);

    int lit_count = 0;
    int in_mem_count = 0;
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
        lit_count += count_literal_chars(lines.sv);
        in_mem_count += count_in_mem_chars(lines.sv);
    }

    return lit_count - in_mem_count;
}

char *day08_encode(strv sv)
{
    // everything gets surrounded by some additonal "".
    char *da = NULL;
    da_append(da, '"');
    for (size_t i = 0; i < sv.size; i++) {
        if (sv.str[i] == '"' || sv.str[i] == '\\')
            da_append(da, '\\');

        da_append(da, sv.str[i]);
    }

    da_append(da, '"');
    da_append(da, '\0');
    return da;
}

int day08_count_chars_2(const char *input)
{
    strv_it lines = {0};
    strv_lines(&lines, input);

    int lit_count = 0;
    int encoded_count = 0;
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
        lit_count += count_literal_chars(lines.sv);
        char *encoded = day08_encode(lines.sv);
        encoded_count += count_literal_chars(strv_from(encoded));
        da_free(encoded);
    }

    return encoded_count - lit_count;
}

void day08_tests()
{
    strv empty_string = strv_from("\"\"");
    assert(count_literal_chars(empty_string) == 2);
    assert(count_in_mem_chars(empty_string) == 0);

    strv abc = strv_from("\"abc\"");
    assert(count_literal_chars(abc) == 5);
    assert(count_in_mem_chars(abc) == 3);

    strv aaa = strv_from("\"aaa\\\"aaa\"");
    printf(strv_fmt"\n", strv_arg(aaa));
    assert(count_literal_chars(aaa) == 10);
    assert(count_in_mem_chars(aaa) == 7);

    strv hex = strv_from("\"\\x27\"");
    printf(strv_fmt"\n", strv_arg(hex));
    printf("Length = %zu\n", hex.size);
    assert(count_literal_chars(hex) == 6);
    assert(count_in_mem_chars(hex) == 1);

    size_t lit_total = count_literal_chars(empty_string)
                     + count_literal_chars(abc)
                     + count_literal_chars(aaa)
                     + count_literal_chars(hex);

    size_t in_mem_total = count_in_mem_chars(empty_string)
                        + count_in_mem_chars(abc)
                        + count_in_mem_chars(aaa)
                        + count_in_mem_chars(hex);

    printf("Solution = %d\n", (int)(lit_total - in_mem_total));

    // Part 2 - encoding
    size_t encoded_lit_total = 0;
    char *encoded_s = day08_encode(empty_string);
    printf("%s\n", encoded_s);
    assert(count_literal_chars(strv_from(encoded_s)) == 6);
    encoded_lit_total += count_literal_chars(strv_from(encoded_s));
    da_free(encoded_s);

    encoded_s = day08_encode(abc);
    printf("%s\n", encoded_s);
    assert(count_literal_chars(strv_from(encoded_s)) == 9);
    encoded_lit_total += count_literal_chars(strv_from(encoded_s));
    da_free(encoded_s);

    encoded_s = day08_encode(aaa);
    printf("%s\n", encoded_s);
    assert(count_literal_chars(strv_from(encoded_s)) == 16);
    encoded_lit_total += count_literal_chars(strv_from(encoded_s));
    da_free(encoded_s);

    encoded_s = day08_encode(hex);
    printf("%s\n", encoded_s);
    assert(count_literal_chars(strv_from(encoded_s)) == 11);
    encoded_lit_total += count_literal_chars(strv_from(encoded_s));
    da_free(encoded_s);

    assert(encoded_lit_total == 42);
    assert(encoded_lit_total - lit_total == 19);
}
