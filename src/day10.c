
/* #define DEBUG */
char *day10_get_nth(strv input_sv, size_t n)
{
    char *answer = NULL;

    for (size_t j = 0; j < n; j++) {
#ifdef DEBUG
        printf("Input: "strv_fmt"\n", strv_arg(input_sv));
#endif
        // dynamic array
        char *output = NULL;
        for (size_t i = 0; i < input_sv.size;) {
            char target = input_sv.str[i];
            size_t count = 1;
            for (; count < input_sv.size && input_sv.str[i + count] == target; count++);
            i += count;

            // Has to be a digit from 0 - 9, so just use atoi.
            char buf[2];
            buf[0] = target;
            buf[1] = '\0';
            int num = atoi(buf);
#ifdef DEBUG
            printf("parsed num = %d, count = %zu -> %zu%d\n", num, count, count, num);
#endif
            da_append(output, count + '0');
            da_append(output, num + '0');
        }
        da_append(output, '\0');

        if (answer != NULL && j < n - 1) {
            da_free(answer);
        }

        answer = output;
        input_sv = strv_from(answer);
    }

    return answer;
}

int day10_length(const char *input)
{
    strv input_sv = strv_trim(strv_from(input));
    char *answer = day10_get_nth(input_sv, 40);
    int ans = (int)strlen(answer);
    da_free(answer);
    return ans;
}

int day10_length_2(const char *input)
{
    strv input_sv = strv_trim(strv_from(input));
    char *answer = day10_get_nth(input_sv, 50);
    int ans = (int)strlen(answer);
    da_free(answer);
    return ans;
}

void day10_tests()
{
    const char *input = "1\n";
    strv input_sv = strv_trim(strv_from(input));
    char *answer = day10_get_nth(input_sv, 1);
    assert(strcmp(answer, "11") == 0);
    da_free(answer);

    answer = day10_get_nth(input_sv, 2);
    assert(strcmp(answer, "21") == 0);
    da_free(answer);

    answer = day10_get_nth(input_sv, 3);
    printf("%s\n", answer);
    assert(strcmp(answer, "1211") == 0);
    da_free(answer);

    answer = day10_get_nth(input_sv, 4);
    printf("%s\n", answer);
    assert(strcmp(answer, "111221") == 0);
    da_free(answer);

    answer = day10_get_nth(input_sv, 5);
    printf("%s\n", answer);
    assert(strcmp(answer, "312211") == 0);
    da_free(answer);
}
