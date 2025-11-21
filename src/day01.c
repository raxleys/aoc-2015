#include <stddef.h>
#include <string.h>
#include <assert.h>

int day01_move_to_floor(const char *input)
{
    ptrdiff_t size = (ptrdiff_t)strlen(input);
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (input[i] == '(') {
            count++;
        } else if (input[i] == ')') {
            count--;
        } else {
            fprintf(stderr, "Invalid character at position %d: %c\n", i, input[i]);
            return -1;
        }
    }

    return count;
}

int day01_basement_position(const char *input)
{
    ptrdiff_t size = (ptrdiff_t)strlen(input);
    int floor = 0;
    int pos = 0;
    for (pos = 0; pos < size; pos++) {
        if (input[pos] == '(')
            floor++;
        else if (input[pos] == ')')
            floor--;

        if (floor < 0)
            return pos + 1;
    }

    return -1;
}

void day01_tests()
{
    // Tests
    assert(day01_move_to_floor("(())") == 0);
    assert(day01_move_to_floor("()()") == 0);
    assert(day01_move_to_floor("(((") == 3);
    assert(day01_move_to_floor("(()(()(") == 3);
    assert(day01_move_to_floor("))(") == -1);

    assert(day01_basement_position(")") == 1);
    assert(day01_basement_position("()())") == 5);
}
