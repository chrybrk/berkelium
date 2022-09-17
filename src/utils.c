#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/utils.h"
#include "include/macro.h"

char* read_file(const char* filename)
{
    FILE *fp;
    char *line = NULL;
    long len;

    fp = fopen(filename, "rb");
    if ( fp == NULL )
    {
        log(3, "Cannot read file from this filepath, `%s`\n", filename);
        exit(1);
    }

    /*
    char* buffer = (char*) calloc(1, sizeof(char));
    buffer[0] = '\0';

    while ((read = getline(&line, &len, fp)) != -1)
    {
        buffer = (char*) realloc(buffer, (strlen(buffer) + strlen(line) + 1) * sizeof(char));
        strcat(buffer, line);
    }
    */

    fseek(fp, 0L, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char *buffer = (char*)calloc(len, sizeof(char));
    if (buffer == NULL)
        return NULL;

    fread(buffer, sizeof(char), len, fp);

    fclose(fp);
    if (line)
        free(line);

    return buffer;
}

void write_file(const char* filename, char* buffer)
{
    FILE * fp;

    fp = fopen(filename, "w");
    if ( fp == NULL )
    {
        log(3, "Cannot read file from this filepath, `%s`\n", filename);
        exit(1);
    }

    fputs(buffer, fp);

    fclose(fp);
}

char* allocstr(char* dest, char* src)
{
    size_t s_dest = strlen(dest);
    size_t s_src = strlen(src);
    char* dst = calloc(s_dest + s_src + 8, sizeof(char));

    for (int i = 0; i < s_dest; i++)
        dst[i] = dest[i];

    for (int i = 0; i < s_src; i++)
        dst[s_dest + i] = src[i];

    return dst;
}

void swap(char *s)
{
    for (int i = (int)strlen(s) / 2 - 1; i >= 0; i--)
    {
        char a = s[i];
        s[i] = s[(int)strlen(s) - i - 1];
        s[(int)strlen(s) - i - 1] = a;
    }
}

void collect(char *s, char c)
{
    char *c_x = calloc(1, sizeof(char));
    c_x[0] = c;
    c_x[1] = '\0';
    s = realloc(s, (strlen(s) + strlen(c_x) + 8) * sizeof(char));
    strcat(s, c_x);
}
