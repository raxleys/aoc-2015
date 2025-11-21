typedef struct {
    int x;
    int y;
} vec2;

#define RAX_HSET_TYPE vec2
#define RAX_HSET_NAME hsetv2
#define RAX_HSET_EQUAL(X, Y) (X.x == Y.x && X.y == Y.y)
#include "rax_hset.h"

int day03_visit(const char *input)
{
    strv sv = strv_from(input);

    int x = 0;
    int y = 0;
    hsetv2 visited = {0};
    hsetv2_set(&visited, (vec2) {.x = x, .y = y});
    for (size_t i = 0; i < sv.size; i++) {
        // TODO: @RAX_LIB change to buf!
        switch(sv.str[i]) {
        case '^':
            y++;
            break;
        case '>':
            x++;
            break;
        case 'v':
            y--;
            break;
        case '<':
            x--;
            break;
        default:
            fprintf(stderr, "Unexpected character: %c\n", sv.str[i]);
            abort();
        }

        hsetv2_set(&visited, (vec2) {.x = x, .y = y});
    }

    return visited.size;
}

int day03_robo(const char *input)
{
    strv sv = strv_from(input);

    int s_x = 0;
    int s_y = 0;
    int r_x = 0;
    int r_y = 0;
    hsetv2 santa = {0};
    hsetv2 robo = {0};

    hsetv2_set(&santa, (vec2) {.x = s_x, .y = s_y});
    hsetv2_set(&robo, (vec2) {.x = r_x, .y = r_y});
    for (size_t i = 0; i < sv.size; i++) {
        bool is_santa = i % 2 == 0;
        switch(sv.str[i]) {
        case '^':
            if (is_santa)
                s_y++;
            else
                r_y++;
            break;
        case '>':
            if (is_santa)
                s_x++;
            else
                r_x++;
            break;
        case 'v':
            if (is_santa)
                s_y--;
            else
                r_y--;
            break;
        case '<':
            if (is_santa)
                s_x--;
            else
                r_x--;
            break;
        default:
            fprintf(stderr, "Unexpected character: %c\n", sv.str[i]);
            abort();
        }

        if (is_santa)
            hsetv2_set(&santa, (vec2) {.x = s_x, .y = s_y});
        else
            hsetv2_set(&robo, (vec2) {.x = r_x, .y = r_y});
    }

    // Join sets
    // TODO: @RAX_LIB add method to library
    hsetv2_it iter = hsetv2_iter(&santa);
    while (hsetv2_next(&iter))
        hsetv2_set(&robo, *iter.value);

    return robo.size;
}

void day03_tests()
{
    assert(day03_visit(">") == 2);
    assert(day03_visit("^>v<") == 4);
    assert(day03_visit("^v^v^v^v^v") == 2);

    assert(day03_robo("^v") == 3);
    assert(day03_robo("^>v<") == 3);
    assert(day03_robo("^v^v^v^v^v") == 11);
}
