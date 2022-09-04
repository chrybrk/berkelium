#include "include/macro.h"

void log_proc(int level, char* msg)
{
    switch (level)
    {
        case 1: fprintf(stdout, "[INFO]: %s\n", msg); break;
        case 2: fprintf(stdout, "[WARN]: %s\n", msg); break;
        case 3: fprintf(stdout, "[ERR]: %s\n", msg); exit(1);
        default: fprintf(stdout, "[LOG]: %s\n", msg); break;
    }
}
