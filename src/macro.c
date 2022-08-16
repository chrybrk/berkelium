#include "include/macro.h"

void log_proc(int level, char* msg)
{
    switch (level)
    {
        case 1: printf("[INFO]: %s\n", msg); break;
        case 2: printf("[WARN]: %s\n", msg); break;
        case 3: printf("[ERR]: %s\n", msg); exit(1);
        default: printf("[LOG]: %s\n", msg); break;
    }
}
