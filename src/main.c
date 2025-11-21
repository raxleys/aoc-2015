#include <stdio.h>
#include "utils.h"
#include "day01.c"

#define SIZE(X) (sizeof(X) / sizeof(X[0]))

int (*solutions[][2])(const char *) = {
    {day01_move_to_floor, day01_basement_position},
};

int main(int argc, char *argv[])
{
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

    char *input = read_whole_file("./inputs/day01.txt");
    if (!input) {
        fprintf(stderr, "Failed to read input file.\n");
        return 1;
    }

    int (*f)(const char *) = solutions[day - 1][part - 1];
    int ans = f(input);
    printf("Solution: %d\n", ans);

    free(input);
    return 0;
}
