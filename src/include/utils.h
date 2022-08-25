#pragma once

char* read_file(const char* filename);
void write_file(const char* filename, char* buffer);
char* allocstr(char* dest, char* src);


#define exec_sys(temp, ...) {\
        char* src = calloc(strlen(temp) + 128, sizeof(char)); \
        sprintf(src, temp, __VA_ARGS__); \
        system(src); \
                                            }


void swap(char *s);
void collect(char *s, char c);
