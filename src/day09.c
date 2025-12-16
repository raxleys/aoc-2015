#define RAX_HT_KEY_TYPE             char *
#define RAX_HT_VALUE_TYPE           int
#define RAX_HT_HASH(KEY)            (RAX_HT_HASH_FUN((KEY), strlen((KEY))))
#define RAX_HT_KEY_EQUAL(X, Y)      (strcmp((X), (Y)) == 0)
#define RAX_HT_DESTROY_KEY(KEY)     free(KEY)
#define RAX_HT_NAME htsi
#include "rax_ht.h"

/* #ifndef DEBUG */
/* #define DEBUG */
/* #endif */

void day09_swap(char **x, char **y)
{
    char *temp = *x;
    *x = *y;
    *y = temp;
}

int day09_shortest_route_helper(char **cities, const htsi *distances, size_t start, size_t end) {
    int smallest_dist = -1;
    if (start == end) {
        int total_dist = 0;
        char key[1024];
        for (size_t i = 0; i < end; i++) {
            snprintf(key, sizeof(key), "%s to %s", cities[i], cities[i + 1]);
            total_dist += *htsi_get(distances, key);
#ifdef DEBUG
            printf("%s -> %d\n", key, total_dist);
#endif
        }

        return total_dist;
    } else {
        for (size_t i = start; i <= end; i++) {
            day09_swap(&cities[start], &cities[i]);
            int tmp = day09_shortest_route_helper(cities, distances, start + 1, end);
            day09_swap(&cities[start], &cities[i]);

            if (smallest_dist < 0 || tmp < smallest_dist)
                smallest_dist = tmp;
        }
    }

    return smallest_dist;
}

int day09_find_shortest_route(char **cities_arr, const htsi *distances)
{
    char **arr = malloc(sizeof(*arr) * da_size(cities_arr));
    memcpy(arr, cities_arr, sizeof(*arr) * da_size(cities_arr));
    int res = day09_shortest_route_helper(arr, distances, 0, da_size(cities_arr) - 1);
    free(arr);
    return res;
}

int day09_solution(const char *input)
{
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

    // Convert set of cities to an array, to get all permutations
    char **cities_arr = NULL;
    hsets_it it = hsets_iter(&cities);
    while (hsets_next(&it))
        da_append(cities_arr, *it.value);

    int day09_shortest_path = day09_find_shortest_route(cities_arr, &distances);
    printf("Shortest path = %d\n", day09_shortest_path);

    da_free(cities_arr);
    hsets_destroy(&cities);
    htsi_destroy(&distances);

    return day09_shortest_path;
}

void day09_tests()
{
    const char *input = "London to Dublin = 464\n"
                        "London to Belfast = 518\n"
                        "Dublin to Belfast = 141";

    printf("Solution: %d\n", day09_solution(input));
}
