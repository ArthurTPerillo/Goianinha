#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "print_ast.h"
#include "semantic.h"

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
        printf("✓ Análise léxica: OK\n");
        printf("✓ Análise sintática: OK\n");
        printf("✓ AST construída com sucesso!\n");
        
        /* Imprime a AST */
        print_ast(ast_root);
        
        /* Análise Semântica */
        int semantic_result = semantic_analysis(ast_root);
        
        if (semantic_result == 0) {
            printf("✓ Programa compilado com sucesso!\n");
            printf("\n");
            printf("Próximas etapas:\n");
            printf("  - Geração de código MIPS\n");
            printf("\n");
        } else {
            fprintf(stderr, "\n✗ Compilação falhou devido a erros semânticos.\n\n");
            return 1;
        }
    } else {
        fprintf(stderr, "✗ Erro durante a compilação.\n");
        return 1;
    }
    
    return 0;
}
