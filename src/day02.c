#include "utils.h"

int day02_wrapping_paper(const char *input)
{
    int dims[3];
    char *end = (char *)input;
    for (size_t i = 0; i < SIZE(dims); i++, end++)
        dims[i] = strtol(end, &end, 10);

    int faces[3] = {
        dims[0] * dims[1],
        dims[1] * dims[2],
        dims[2] * dims[0],
    };

    int smallest = faces[0];
    for (size_t i = 1; i < SIZE(faces); i++)
        if (faces[i] < smallest) smallest = faces[i];

    int tot = smallest;
    for (size_t i = 0; i < SIZE(faces); i++, end++)
        tot += 2 * faces[i];

    return tot;
}

void day02_tests()
{
    assert(day02_wrapping_paper("2x3x4") == 58);
    assert(day02_wrapping_paper("1x1x10") == 43);
}
