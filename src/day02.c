#include "utils.h"

#define RAX_STRV_IMPLEMENTATION
#include "rax_strv.h"

void day02_parse_dims(strv view, int *dims)
{
    // Get a string to use strtol.
    char line[512];
    snprintf(line, sizeof(line), strv_fmt, strv_arg(view));

    char *end = (char *)line;
    for (size_t i = 0; i < 3; i++, end++)
        dims[i] = strtol(end, &end, 10);
}

int day02_single_total(strv view)
{
    int dims[3];
    day02_parse_dims(view, dims);

    int faces[3] = {
        dims[0] * dims[1],
        dims[1] * dims[2],
        dims[2] * dims[0],
    };

    int smallest = faces[0];
    for (size_t i = 1; i < SIZE(faces); i++)
        if (faces[i] < smallest) smallest = faces[i];

    int tot = smallest;
    for (size_t i = 0; i < SIZE(faces); i++)
        tot += 2 * faces[i];

    return tot;
}

int day02_single_ribbon(strv view)
{
    int dims[3];
    day02_parse_dims(view, dims);

    int perims[] = {
        2 * (dims[0] + dims[1]),
        2 * (dims[0] + dims[2]),
        2 * (dims[1] + dims[2]),
    };

    int smallest = perims[0];
    for (size_t i = 1; i < SIZE(perims); i++)
        if (perims[i] < smallest) smallest = perims[i];

    int vol = dims[0] * dims[1] * dims[2];
    return smallest + vol;
}

int day02_wrapping_paper(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    int tot = 0;
    while (strv_next(&it))
        tot += day02_single_total(it.sv);

    return tot;
}

int day02_ribbon(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    int tot = 0;
    while (strv_next(&it))
        tot += day02_single_ribbon(it.sv);

    return tot;
}

void day02_tests()
{
    assert(day02_wrapping_paper("2x3x4") == 58);
    assert(day02_wrapping_paper("1x1x10") == 43);

    assert(day02_ribbon("2x3x4") == 34);
    assert(day02_ribbon("1x1x10") == 14);
}
