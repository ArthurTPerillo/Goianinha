#ifndef PRINT_AST_H
#define PRINT_AST_H

#include "ast.h"


/* Imprime a AST completa do programa */
void print_ast(program_t *program);


/* Imprime uma expressão */
void print_expr(expr_t *expr);

/* Imprime uma lista de expressões (argumentos de função) */
void print_expr_list(expr_list_t *list);

/* Imprime um comando */
void print_cmd(cmd_t *cmd);

/* Imprime uma lista de comandos */
void print_cmd_list(cmd_list_t *cmds);

/* Imprime um bloco (variáveis locais + comandos) */
void print_block(block_t *block);

/* Imprime declarações de variáveis locais */
void print_local_decl(local_decl_t *decl);

/* Imprime declarações globais (variáveis e funções) */
void print_global_decl(global_decl_t *decl, int indent);

/* Imprime lista de parâmetros de função */
void print_param_list(param_t *params);


/* Retorna o nome do tipo (int, car, void) */
const char *type_name(types_t type);

/* Retorna o símbolo do operador (+, -, *, /, etc) */
const char *get_op_symbol(expr_e kind);

/* Retorna o nome do tipo de expressão (ID, INT, ADD, etc) */
const char *expr_kind_name(expr_e kind);

/* Retorna o nome do tipo de comando (IF, WHILE, etc) */
const char *cmd_kind_name(cmd_e kind);

#endif /* PRINT_AST_H */
