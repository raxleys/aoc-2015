#define RAX_HT_KEY_TYPE             char *
#define RAX_HT_VALUE_TYPE           int
#define RAX_HT_HASH(KEY)            (RAX_HT_HASH_FUN((KEY), strlen((KEY))))
#define RAX_HT_KEY_EQUAL(X, Y)      (strcmp((X), (Y)) == 0)
#define RAX_HT_DESTROY_KEY(KEY)     free(KEY)
#define RAX_HT_NAME htsi
#include "rax_ht.h"

void day09_swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

#ifndef DEBUG
#define DEBUG
#endif

void day09_tests()
{
    const char *input = "London to Dublin = 464\n"
                        "London to Belfast = 518\n"
                        "Dublin to Belfast = 141";

    // Parse distances
    hsets cities = {0};
    htsi distances = {0};

    strv_it lines = {0};
    strv_lines(&lines, input);
    while (strv_next(&lines) && !strv_is_empty(lines.sv)) {
        strv line = strv_dup(lines.sv);
        strv city1 = strv_pop_word(&line);
        strv_pop_word(&line);  // to
        strv city2 = strv_pop_word(&line);
        strv_pop_word(&line);  // =
        long distance;
        assert(strv_parse_long(line, &distance));

#ifdef DEBUG
        printf("city1 = "strv_fmt", city2 = "strv_fmt", distance = %d\n", strv_arg(city1), strv_arg(city2), (int)distance);
#endif

        // Set of all cities
        hsets_set(&cities, strv_to_cstr_owned(city1));
        hsets_set(&cities, strv_to_cstr_owned(city2));

        // twice the memory usage, but its fine...
        size_t s_size = city1.size + city2.size + strlen(" to ") + 1;
        char *s = malloc(s_size);
        assert(s);
        snprintf(s, s_size, strv_fmt" to "strv_fmt, strv_arg(city1), strv_arg(city2));
        htsi_set(&distances, s, (int)distance);
        s = malloc(s_size);
        assert(s);
        snprintf(s, s_size, strv_fmt" to "strv_fmt, strv_arg(city2), strv_arg(city1));
        htsi_set(&distances, s, (int)distance);
    }

    // Convert set of cities to an array.
    /* char **cities_arr = malloc(sizeof(char *) * cities.size); */
    /* assert(cities_arr); */

    /* size_t count = 0; */
    /* hsets_iter it = {0}; */
    /* while (hsets_next(&it)) { */
        /* assert(count < cities.size); */
        /* cities_arr[count++] = *it.value; */
    /* } */

    /* size_t n_cities = cities.size; */

    char **cities_arr = NULL;
    hsets_iter it = {0};
    while (hsets_next(&it))
        da_append(cities_arr, *it.value);

    da_free(cities_arr);
    hsets_destroy(&cities);
    hsets_destroy(&distances);
}
