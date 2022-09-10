#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/macro.h"
#include "include/utils.h"

#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpret.h"
#include "include/codegen.h"

#define version "0.0.2"
#define debug


const char *help = "Usage: bk <file> <options>\n"
             "Options:\n"
             " -o Output path\n"
             " -a Generate Assembly\n"
             " -r Run\n"
             " -v Version\n";

int main(int argc, char *argv[])
{
    int arg_r = 0;
    int arg_a = 0;

    if ( argc < 2 ) log(3, "%s", "bk: no input file.");

    char *fp = NULL;
    char *file_extention = NULL;
    char *filename = NULL;
    char *output_src = NULL;

    while (argc != 1)
    {
        argv++; argc--;

        switch (*argv[0])
        {
            case '-':
            {
                if ( argv[0][1] == 'v' ) fprintf(stdout, "version: %s\n", version);
                else if ( argv[0][1] == 'h' ) fprintf(stdout, "%s\n", help);
                else if ( argv[0][1] == 'a' ) arg_a = 1;
                else if ( argv[0][1] == 'r' ) arg_r = 1;
                else if ( argv[0][1] == 'o' )
                {
                    if (argc > 1)
                    {
                        argv++; argc--;
                        if ( argv[0][0] == '-' ) log(3, "%s", "bk: seg fault, expected file after `-o`");
                        output_src = argv[0];
                    }
                    else log(3, "%s", "bk: seg fault, expected file after `-o`");
                }
                break;
            }
            default: fp = argv[0];
        }
    }

    if (fp == NULL) return 0;

    for (int i = strlen(fp) - 1; i >= 0; i--)
    {
        if (fp[i] == '.') break;
        if (file_extention == NULL) file_extention = calloc(1, sizeof(char));
        collect(file_extention, fp[i]);
    }

    for (int i = (strlen(fp) - strlen(file_extention) - 2); i >= 0; i--)
    {
        if (fp[i] == '/') break;
        if (filename == NULL) filename = calloc(1, sizeof(char));
        collect(filename, fp[i]);
    }

    swap(file_extention);
    swap(filename);

    int size = strlen(fp) - (strlen(file_extention) + strlen(filename) + 1);

    if (output_src == NULL)
    {
        if (size == 0)
        {
            output_src = calloc(strlen(filename) + 8, sizeof(char));
            strcat(output_src, "./");
            strcat(output_src, filename);
        }
        else
        {
            for (int i = 0; i < size; i++)
            {
                if (output_src == NULL) output_src = calloc(1, sizeof(char));
                collect(output_src, fp[i]);
            }
            output_src = realloc(output_src, (strlen(output_src) + strlen(filename)) * sizeof(char));
            strcat(output_src, filename);
        }
    }

    lexer_T *lexer = init_lexer(read_file(fp));
    parser_T *parser = init_parser(lexer);

    char *output_asm = calloc(strlen(output_src) + strlen(".s") + 8, sizeof(char));
    strcat(output_asm, output_src); strcat(output_asm, ".s");
    struct ASTnode *node = parser_parse(parser);
    codegen_T *codegen = init_codegen(output_asm);

#define if_execute_debug

#ifdef if_execute_debug
    codegen_code(codegen, node);
    if (!arg_a)
    {
        exec_sys("gcc -c ./%s -o ./%s.o", output_asm, output_src);
        exec_sys("gcc -no-pie ./%s.o -o ./%s", output_src, output_src);
        exec_sys("rm ./%s.o", output_src);
    }
    if (arg_r) exec_sys("echo -e \"$(./%s)\"", output_src);
#endif

#ifndef debug
        exec_sys("rm ./%s.s", output_src);
#endif

    return 0;
}
