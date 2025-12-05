#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "print_ast.h"

/* Declarações externas do parser */
extern int yyparse(void);
extern FILE *yyin;
extern program_t *ast_root;

int main(int argc, char **argv) {
    /* Verifica se foi passado um arquivo como argumento */
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Erro ao abrir arquivo: %s\n", argv[1]);
            return 1;
        }
    } else {
        fprintf(stderr, "Uso: %s <arquivo.goi>\n", argv[0]);
        return 1;
    }
    
    /* Executa o parser */
    int result = yyparse();
    
    /* Fecha o arquivo */
    if (yyin && yyin != stdin) {
        fclose(yyin);
    }
    
    /* Verifica se o parsing foi bem-sucedido */
    if (result == 0 && ast_root != NULL) {
        printf("Análise léxica: OK\n");
        printf("Análise sintática: OK\n");
        printf("AST construída com sucesso!\n");
        print_ast(ast_root); 
        
        /* chamar as próximas fases:
         * - print_ast(ast_root);           // Imprimir a AST
         * - semantic_analysis(ast_root);   // Análise semântica
         * - code_generation(ast_root);     // Geração de código MIPS
         */
    } else {
        fprintf(stderr, "X Erro durante a compilação.\n");
        return 1;
    }
    
    return 0;
}
