#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/macro.h"
#include "include/utils.h"

#include "include/lexer.h"
#include "include/parser.h"
#include "include/codegen.h"

#define version "0.0.6"
#define debug


const char *help = "Usage: bk <file> <options>\n"
             "Options:\n"
             " -o Output path\n"
             " -a Generate Assembly\n"
             " -b bind c file\n"
             " -r Run\n"
             " -S Generate Assembly [without compiling]\n"
             " -v Version\n";

int main(int argc, char *argv[])
{
    int arg_r = 0;
    int arg_a = 0;
    int arg_S = 0;

    if ( argc < 2 ) log(3, "%s", "bk: no input file.");

    char *fp = NULL;
    char *file_extention = NULL;
    char *filename = NULL;
    char *output_src = NULL;
    char *bind_src = NULL;

    while (argc != 1)
    {
        argv++; argc--;

        switch (*argv[0])
        {
            case '-':
            {
                switch ( argv[0][1] )
                {
                    case 'v': fprintf(stdout, "version: %s\n", version); break;
                    case 'h': fprintf(stdout, "%s\n", help); break;
                    case 'a': arg_a = 1; break;
                    case 'r': arg_r = 1; break;
                    case 'S': arg_S = 1; break;
                    case 'b':
                            {
                                if (argc > 1)
                                {
                                    argv++; argc--;
                                    if ( argv[0][0] == '-' ) log(3, "%s", "bk: seg fault, expected file after `-b`");
                                    bind_src = argv[0];
                                }
                                else log(3, "%s", "bk: seg fault, expected file after `-b`");
                            } break;
                    case 'o':
                            {
                                if (argc > 1)
                                {
                                    argv++; argc--;
                                    if ( argv[0][0] == '-' ) log(3, "%s", "bk: seg fault, expected file after `-o`");
                                    output_src = argv[0];
                                }
                                else log(3, "%s", "bk: seg fault, expected file after `-o`");
                            } break;
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
    swap(file_extention);

    if (strcmp(file_extention, "bk")) log(3, "%s", "file format not supported.");

    for (int i = (strlen(fp) - strlen(file_extention) - 2); i >= 0; i--)
    {
        if (fp[i] == '/') break;
        if (filename == NULL) filename = calloc(1, sizeof(char));
        collect(filename, fp[i]);
    }
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
    codegen_code(codegen, node);

    if (!arg_S && bind_src == NULL)
    {
        exec_sys("gcc -c ./%s -o ./%s.o", output_asm, output_src);
        exec_sys("gcc -no-pie ./%s.o -o ./%s", output_src, output_src);
        exec_sys("rm ./%s.o", output_src);

        if (arg_r) exec_sys("echo -e \"$(./%s)\"", output_src);
        if (!arg_a) exec_sys("rm ./%s.s", output_src);
    }
    else if (bind_src != NULL)
    {
        exec_sys("gcc %s ./%s -o ./%s", bind_src, output_asm, output_src);
        if (arg_r) exec_sys("echo -e \"$(./%s)\"", output_src);
        if (arg_S) exec_sys("rm ./%s.s", output_src);
    }

    return 0;
}
