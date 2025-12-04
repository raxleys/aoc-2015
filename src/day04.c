#include "rax_strv.h"

// This day involves md5 hashes which aren't trivial to implement. So
// I will "cheat" and use the external `md5sum` program on Linux. But,
// now these solutions only work on Linux.
bool day04_get_md5hash(char *sum, size_t sum_size, const char *s)
{
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "echo -n \"%s\" | md5sum", s);

    FILE *proc = popen(cmd, "r");
    if (!proc) {
        perror("Failed to open process");
        return false;
    }

    // Just assume it will fit in 1 line/call
    char buf[1024];
    fgets(buf, sizeof(buf), proc);
    pclose(proc);

    strv_it it = {0};
    strv_split(&it, buf, "  -");
    if (!strv_next(&it)) {
        fprintf(stderr, "Failed to parse md5 hash\n");
        return false;
    }

    snprintf(sum, sum_size, strv_fmt, strv_arg(it.sv));
    return true;
}

bool day04_check_hash(strv hash)
{
    /* assert(hash.size >= 5); */
    for (int i = 0; i < 5; i++)
        if (hash.str[i] != '0') return false;

    return true;
}

void day04_tests()
{
    /* char buf[1024]; */
    /* const char *s = "abcdef609043"; */
    /* assert(day04_get_md5hash(buf, sizeof(buf), s)); */
    /* printf("sum = %s\n", buf); */
    /* if (day04_check_hash(strv_from(buf))) */
        /* printf("It's a match!\n"); */

    const char *secret = "abcdef";
    size_t i = 1;
    char input[1024];
    char hash[1024];
    while (true) {
        snprintf(input, sizeof(input), "%s%zu", secret, i);
        printf("Try: %s\n", input);

        day04_get_md5hash(hash, sizeof(hash), input);
        if (day04_check_hash(strv_from(hash)))
            break;

        i++;
    }

    printf("Solution: %zu -> %s\n", i, hash);
}
