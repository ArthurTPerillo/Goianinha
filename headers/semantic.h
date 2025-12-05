#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "table.h"


/* Converte types_t (da AST) para Tipo (da tabela de símbolos) */
Tipo ast_type_to_table_type(types_t ast_type);

/* Converte Tipo (da tabela) para types_t (da AST) */
types_t table_type_to_ast_type(Tipo table_type);


/* Analisa semanticamente o programa completo
 * Retorna 0 se não houver erros, -1 caso contrário */
int semantic_analysis(program_t *program);



/* Processa declarações globais (variáveis e funções) */
int analyze_global_declarations(global_decl_t *globals, TableNode **scope);

/* Processa uma função (cria escopo, analisa parâmetros e corpo) */
int analyze_function(global_decl_t *func_decl, TableNode **scope);

/* Processa declarações locais dentro de um bloco */
int analyze_local_declarations(local_decl_t *locals, TableNode *scope);


/* Analisa um bloco (cria novo escopo, processa locais e comandos) */
int analyze_block(block_t *block, TableNode **scope);

/* Analisa uma lista de comandos */
int analyze_cmd_list(cmd_list_t *cmds, TableNode *scope, types_t func_return_type);

/* Analisa um comando individual */
int analyze_cmd(cmd_t *cmd, TableNode *scope, types_t func_return_type);


/* Analisa uma expressão e retorna seu tipo
 * Retorna TYPE_VOID em caso de erro */
types_t analyze_expr(expr_t *expr, TableNode *scope);

/* Analisa lista de argumentos de chamada de função */
int analyze_call_args(expr_list_t *args, TableNode *scope, 
                      TableEntry *func_entry, int line);



/* Verifica se dois tipos são compatíveis */
int types_compatible(types_t t1, types_t t2);

/* Verifica se uma variável já foi declarada no escopo local */
int check_redeclaration(TableNode *scope, char *name, int line);


/* Reporta erro semântico e incrementa contador */
void semantic_error(int line, const char *format, ...);

/* Retorna o número de erros semânticos encontrados */
int get_error_count(void);

/* Reseta o contador de erros */
void reset_error_count(void);

#endif /* SEMANTIC_H */
