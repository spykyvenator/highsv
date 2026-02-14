#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

void die(const char *fmt, ...);
void* h_malloc(size_t s);

typedef struct {
    void *data;
    size_t len, size;
} highsv_vector;
#endif
