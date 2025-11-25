bool day06_turn_on(bool cur)
{
    (void)cur;
    return true;
}

bool day06_turn_off(bool cur)
{
    (void)cur;
    return false;
}

bool day06_toggle(bool cur)
{
    return !cur;
}

strv day06_parse_coord(strv instruction, int *x, int *y)
{
    // Forgo error checking...
    char *end = NULL;
    *x = strtol(instruction.str, &end, 10);

    // +1 for the ,
    instruction = strv_chop_left(instruction, 1 + (end - instruction.str));
    *y = strtol(instruction.str, &end, 10);

    instruction = strv_chop_left(instruction, end - instruction.str);
    return strv_trim_left(instruction);
}

void day06_run_instruction(bool *grid, strv instruction)
{
    const strv TURN_ON = strv_from("turn on");
    const strv TURN_OFF = strv_from("turn off");
    const strv TOGGLE = strv_from("toggle");

    bool (*op)(bool) = NULL;
    if (strv_starts_with(instruction, TURN_ON)) {
        op = day06_turn_on;
        instruction = strv_trim_left(strv_chop_left(instruction, TURN_ON.size));
    } else if (strv_starts_with(instruction, TURN_OFF)) {
        op = day06_turn_off;
        instruction = strv_trim_left(strv_chop_left(instruction, TURN_OFF.size));
    } else if (strv_starts_with(instruction, TOGGLE)) {
        op = day06_toggle;
        instruction = strv_trim_left(strv_chop_left(instruction, TOGGLE.size));
    } else {
        fprintf(stderr, "Unexpected instruction: \""strv_fmt"\"\n", strv_arg(instruction));
        abort();
    }

    int x0, y0;
    instruction = day06_parse_coord(instruction, &x0, &y0);
    assert(x0 >= 0 && y0 >= 0);

    instruction = strv_chop_left(instruction, strlen("through"));
    instruction = strv_trim_left(instruction);

    int x1, y1;
    instruction = day06_parse_coord(instruction, &x1, &y1);
    assert(x1 >= 0 && y1 >= 0);

    for (size_t i = (size_t)x0; i <= (size_t)x1; i++)
        for (size_t j = (size_t)y0; j <= (size_t)y1; j++)
            grid[i * 1000 + j] = op(grid[i * 1000 + j]);
}

int day06_count_lights(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    bool grid[1000 * 1000] = {0};
    while (strv_next(&it))
        if (!strv_is_empty(strv_trim(it.sv)))
            day06_run_instruction(grid, it.sv);

    int count = 0;
    for (size_t i = 0; i < 1000; i++)
        for (size_t j = 0; j < 1000; j++)
            if (grid[i * 1000 + j]) count++;

    return count;
}

int day06_turn_on_2(int n)
{
    return n + 1;
}

int day06_turn_off_2(int n)
{
    return (--n < 0) ? 0 : n;
}

int day06_toggle_2(int n)
{
    return n + 2;
}

void day06_run_instruction_2(int *grid, strv instruction)
{
    const strv TURN_ON = strv_from("turn on");
    const strv TURN_OFF = strv_from("turn off");
    const strv TOGGLE = strv_from("toggle");

    int (*op)(int) = NULL;
    if (strv_starts_with(instruction, TURN_ON)) {
        op = day06_turn_on_2;
        instruction = strv_trim_left(strv_chop_left(instruction, TURN_ON.size));
    } else if (strv_starts_with(instruction, TURN_OFF)) {
        op = day06_turn_off_2;
        instruction = strv_trim_left(strv_chop_left(instruction, TURN_OFF.size));
    } else if (strv_starts_with(instruction, TOGGLE)) {
        op = day06_toggle_2;
        instruction = strv_trim_left(strv_chop_left(instruction, TOGGLE.size));
    } else {
        fprintf(stderr, "Unexpected instruction: \""strv_fmt"\"\n", strv_arg(instruction));
        abort();
    }

    int x0, y0;
    instruction = day06_parse_coord(instruction, &x0, &y0);
    assert(x0 >= 0 && y0 >= 0);

    instruction = strv_chop_left(instruction, strlen("through"));
    instruction = strv_trim_left(instruction);

    int x1, y1;
    instruction = day06_parse_coord(instruction, &x1, &y1);
    assert(x1 >= 0 && y1 >= 0);

    for (size_t i = (size_t)x0; i <= (size_t)x1; i++)
        for (size_t j = (size_t)y0; j <= (size_t)y1; j++)
            grid[i * 1000 + j] = op(grid[i * 1000 + j]);
}

int day06_count_lights_2(const char *input)
{
    strv_it it = {0};
    strv_lines(&it, input);

    int grid[1000 * 1000] = {0};
    while (strv_next(&it))
        if (!strv_is_empty(strv_trim(it.sv)))
            day06_run_instruction_2(grid, it.sv);

    int count = 0;
    for (size_t i = 0; i < 1000; i++)
        for (size_t j = 0; j < 1000; j++)
            count += grid[i * 1000 + j];

    return count;
}

void day06_tests()
{
    /* bool grid[1000 * 1000] = {0}; */
    /* day06_run_instruction(grid, strv_from("turn on 0,0 through 999,999")); */
    /* int count = day06_count_lights(grid, 1000); */
    /* printf("count = %d\n", count); */
    /* day06_run_instruction(grid, strv_from("toggle 0,0 through 999,0")); */
}
