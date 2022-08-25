#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    s = realloc(s, (strlen(s) + 8) * sizeof(char));
    strcat(s, c_x);
}

int main(void)
{
    char *filename = "input008.bk";
    char *ext = NULL;
    char *name = NULL;
    char *path = NULL;

    for (int i = strlen(filename); i > 0; i--)
    {
        if (filename[i] == '.') break;

        if (ext == NULL) ext = calloc(1, sizeof(char));
        collect(ext, filename[i]);
    }


    for (int i = (strlen(filename) - strlen(ext) - 2); i > -1; i--)
    {
        if (filename[i] == '/') break;

        if (name == NULL) name = calloc(1, sizeof(char));
        collect(name, filename[i]);
    }

    int size = strlen(filename) - (strlen(ext) + strlen(name) + 1);
    if (size != 0)
    {
        for (int i = 0; i < size; i++)
        {
            if (path == NULL) path = calloc(1, sizeof(char));
            collect(path, filename[i]);
        }
    }
    else path = "./";

    swap(ext);
    swap(name);

    printf("%s\n", ext);
    printf("%s\n", name);
    printf("%s\n", path);
}
