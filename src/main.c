#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/macro.h"
#include "include/utils.h"

#include "include/lexer.h"
#include "include/parser.h"
#include "include/interpret.h"
#include "include/codegen.h"

int main(int argc, char *argv[])
{
    int arg_r = 0;

    if ( argc < 2 ) log(3, "%s", "bk: no input file.");

    char *fp = argv[1];
    char *file_extention = NULL;
    char *filename = NULL;
    char *output_src = NULL;

    for( unsigned int i = 0; i < argc; i++ )
    {
        if (!strcmp(argv[i], "-h"))
        {
            char *template = "Usage: bk <file> <options>\n"
                             "Options:\n"
                             " -o Output path\n"
                             " -l Generate Lexer\n"
                             " -a Generate Assembly\n";
            printf("%s", template);
        }

        if (!strcmp(argv[i], "-o"))
        {
            if (argc == i + 1 || *argv[i + 1] == '-') log(3, "%s", "bk: cannot assign null to output path.");
            output_src = calloc(strlen(argv[i + 1]) + 8, sizeof(char));
            strcpy(output_src, argv[i + 1]);
        }

        int val = !(strcmp(argv[i], "-r"));
        if (val) arg_r = 1;
    }

    for (int i = strlen(fp); i >= 0; i--)
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
    // parser_parse_statements(parser, output_asm);
    struct ASTnode *node = parser_parse(parser);
    codegen_T *codegen = init_codegen(output_asm);
    codegen_code(codegen, node);

    exec_sys("gcc -c ./%s -o ./%s.o", output_asm, output_src);
    exec_sys("gcc -no-pie ./%s.o -o ./%s", output_src, output_src);
    exec_sys("rm ./%s.o", output_src);
    if (arg_r) exec_sys("echo -e \"compiler output: $(./%s)\"", output_src);

    return 0;
}
