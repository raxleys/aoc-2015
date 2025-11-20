#include <stdio.h>
#include "utils.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("Hello, world!\n");

    char *input = read_whole_file("./inputs/day01.txt");
    printf("%s\n", input);
    if (input)
        free(input);

    return 0;
}
