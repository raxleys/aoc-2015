#include <stdio.h>
#include "utils.h"
#include "day01.c"
#include "day02.c"

int (*solutions[][2])(const char *) = {
    {day01_move_to_floor, day01_basement_position},
    {day02_wrapping_paper, day02_ribbon},
};

int main(int argc, char *argv[])
{
#ifdef TEST
    (void)argc;
    (void)argv;
    day01_tests();
    day02_tests();
#else
    if (argc != 3) {
        fprintf(stderr, "Usage: %s DAY PART\n", argv[0]);
        return 1;
    }

    int day = atoi(argv[1]);
    int part = atoi(argv[2]);

    if (day < 1 || (size_t)day > SIZE(solutions)) {
        fprintf(stderr, "Highest day is %zu!\n", SIZE(solutions));
        return 1;
    }

    if (part < 1 || part > 2) {
        fprintf(stderr, "Each solution only has 2 parts\n");
        return 1;
    }

    char fname[512];
    snprintf(fname, sizeof(fname), "./inputs/day%02d.txt", day);

    char *input = read_whole_file(fname);
    if (!input) {
        fprintf(stderr, "Failed to read input file.\n");
        return 1;
    }

    int (*f)(const char *) = solutions[day - 1][part - 1];
    int ans = f(input);
    printf("Solution: %d\n", ans);

    free(input);
#endif

    return 0;
}
