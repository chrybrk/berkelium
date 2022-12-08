#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void print_byte(char x)
{
    printf("%c", x);
}

void print_i16(int16_t x)
{
    printf("%d", x);
}

void print_i32(int x)
{
    printf("%i", x);
}

void print_i64(long x)
{
    printf("%ld", x);
}

void print_addr(void *x)
{
    printf("%p", x);
}

void ln(int x)
{
    printf("\n");
}
