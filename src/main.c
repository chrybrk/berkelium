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
    if ( argc < 2 ) log(3, "%s", "bk: no input file.");

    char *output_file = NULL;
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
            if (argc == i + 1) log(3, "%s", "bk: cannot assign null to output path.");
            output_file = calloc(strlen(argv[i + 1]) + 8, sizeof(char));
            strcpy(output_file, argv[i + 1]);
        }

        int val = !(strcmp(argv[i], "-r"));

#if !(val)
    #define run
#endif
    }

    char *path = argv[1];
    char *output_src = calloc(1, sizeof(char));
    char *filename = NULL;
    char *src = read_file(path);

    if (output_file == NULL)
    {
        int output_file_location = 0;
        int flag = 1;
        for (unsigned int i = strlen(path); i > 0; i--)
        {
            if ( path[i] == '/' )
            {
                output_file_location = i + 1; break;
            }

            if (flag == 1)
            {
                if (path[i] == '.') flag = 0;
            }
            else
            {
                if (filename == NULL) filename = calloc(1, sizeof(char));
                char *c = calloc(1, sizeof(char));
                c[0] = path[i]; c[1] = '\0';
                filename = realloc(filename, (strlen(filename) + 8)  *sizeof(char));
                strcat(filename, c);
            }
        }
        for (unsigned int i = 0; i < output_file_location; i++)
        {
            char *c = calloc(1, sizeof(char));
            c[0] = path[i]; c[1] = '\0';
            output_src = realloc(output_src, (strlen(output_src) + 8)  *sizeof(char));
            strcat(output_src, c);
        }
    } else strcpy(output_src, output_file);

    if (filename != NULL)
    {
        for (unsigned int i = 0; i < strlen(filename) / 2; i++)
        {
            char a = filename[i];
            filename[i] = filename[strlen(filename) - 1 - i];
            filename[strlen(filename) - 1 - i] = a;
        }

        output_src = realloc(output_src, (strlen(output_src) + strlen(filename) + 8)  *sizeof(char));
        strcat(output_src, filename);
    }

    lexer_T *lexer = init_lexer(src);
    parser_T *parser = init_parser(lexer);
    // struct ASTnode *root = parser_parse(parser, 0);
    // printf("interpreter output: %i\n", interpret(root));
    char *output_asm = calloc(strlen(output_src) + strlen(".s") + 8, sizeof(char));
    strcat(output_asm, output_src); strcat(output_asm, ".s");
    parser_parse_statements(parser, output_asm);
    // codegen_T *codegen = init_codegen(output_asm);
    // codegen_code(codegen, root);
    exec_sys("gcc -c ./%s -o ./%s.o", output_asm, output_src);
    exec_sys("gcc -no-pie ./%s.o -o ./%s", output_src, output_src);
    exec_sys("rm ./%s.o", output_src);
#ifdef run
    exec_sys("echo -e \"compiler output: $(./%s)\"", output_src);
#endif

    return 0;
}
