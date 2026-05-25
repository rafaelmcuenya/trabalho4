#ifndef STRDUPI_H
#define STRDUPI_H

#include <stdlib.h>
#include <string.h>

static inline char* strdupi(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* new_str = malloc(len);
    if (new_str) {
        memcpy(new_str, s, len);
    }
    return new_str;
}

#endif
