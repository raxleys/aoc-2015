#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>

char *read_whole_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = malloc(fsize);
    if (!buf) {
        perror("OOM");
        goto fail;
    }

    if (fread(buf, 1, fsize, file) != (size_t)fsize) {
        perror("Failed to read file");
        goto fail;
    }

    fclose(file);
    return buf;

fail:
    free(buf);
    fclose(file);
    return NULL;
}


#endif // UTILS_H
