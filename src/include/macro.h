#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define assert(x, info) { if(!x) { printf("Assertion Failed: %s\n%s:%i", info, __FILE__, __LINE__); exit(1); }}

#define log(level, ctx, ...){\
        char* src = calloc(strlen(ctx) + 128, sizeof(char)); \
        sprintf(src, ctx, __VA_ARGS__); \
        log_proc(level, src); \
    }

void log_proc(int level, char* msg);
