

// return a pointer to buf on success
char *day11_inc(char *buf, strv s)
{
    bool has_swapped = false;
    for (ptrdiff_t i = (ptrdiff_t)s.size - 1; i >= 0; i--) {
        if (!has_swapped) {
            char next = s.str[i] + 1;
            if (next <= 'z') {
                // Done
                has_swapped = true;
                buf[i] = next;
                continue;
            } else {
                buf[i] = 'a';
                continue;
            }
        }

        buf[i] = s.str[i];
    }

    buf[s.size] = '\0';
    return buf;
}

bool day11_no_iol(strv sv)
{
    for (size_t i = 0; i < sv.size; i++) {
        switch(sv.str[i]) {
        case 'i':
        case 'o':
        case 'l':
            return false;
        }
    }

    return true;
}

bool day11_double_pair(strv sv)
{
    bool has_double_pair = false;
    char target = sv.str[0];
    hsets pairs = {0};

    // Unlike in day5, the 2 pairs must be unique, so we don't have to
    // worry about overlap. I.e, both aaa and aaaa fail the
    // requirements.
    for (size_t i = 1; i < sv.size; i++) {
        strv pair = strv_slice(sv, i - 1, i + 1);
        if (pair.str[0] != pair.str[1])
            continue;

        hsets_set(&pairs, strv_to_cstr_owned(pair));
    }

    has_double_pair = pairs.size > 1;
    hsets_destroy(&pairs);
    return has_double_pair;
}

bool day11_has_straight(strv sv)
{
    char pprev = sv.str[0];
    char prev = sv.str[1];
    for (size_t i = 2; i < sv.size; i++) {
        if (sv.str[i] == prev + 1 && sv.str[i] == pprev + 2)
            return true;

        pprev = sv.str[i - 1];
        prev = sv.str[i];
    }

    return false;
}

int day11_next_password(const char *input)
{
    strv input_sv = strv_trim(strv_from(input));
    char *next = malloc(input_sv.size + 1);
    strv next_sv = strv_dup(input_sv);

    while (true) {
        day11_inc(next, next_sv);
        next_sv = strv_from(next);

        if (!day11_has_straight(next_sv))
            continue;
        if (!day11_no_iol(next_sv))
            continue;
        if (!day11_double_pair(next_sv))
            continue;

        break;
    }

    printf("The next password should be: %s\n", next);
    free(next);
    return 0;
}

void day11_tests()
{
    const char *input = "aaaaaaaa";
    char *next = malloc(strlen(input) + 1);
    char *tmp = input;

    for (size_t i = 0; i < 1000; i++) {
        day11_inc(next, strv_from(tmp));
        printf("%s -> %s\n", tmp, next);
        tmp = next;
    }

    // These are all O(n), so let's just make separate functions to
    // keep things simpler...
    input = "hijklmmn";
    bool has_straight = day11_has_straight(strv_from(input));
    bool iol = day11_no_iol(strv_from(input));
    bool double_pair = day11_double_pair(strv_from(input));
    printf("has_straight = %s\n", BOOL_ARG(has_straight));
    printf("iol = %s\n", BOOL_ARG(iol));
    printf("double_pair = %s\n", BOOL_ARG(double_pair));

    free(next);
}
