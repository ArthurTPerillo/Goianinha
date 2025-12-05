#ifndef AST_H
#define AST_H

#include "types.h"

typedef struct program program_t;
typedef struct global_decl global_decl_t;
typedef struct param param_t;
typedef struct block block_t;
typedef struct local_decl local_decl_t;
typedef struct cmd cmd_t;
typedef struct cmd_list cmd_list_t;
typedef struct expr expr_t;
typedef struct expr_list expr_list_t;

struct program{
    int line;
    global_decl_t *globals;
    block_t *main_block;
};

struct param{
    int line;
    char *id;
    types_t type;
    param_t *next;
};

struct global_decl{
    int line;
    char *id;
    types_t type;       /* tipo da var global ou retorno da função */
    param_t *params;    /* NULL para var global, lista de params para função */
    global_decl_t *next;
    block_t *body;      /* NULL se for var global, != NULL se for função */
};

struct local_decl{
    int line;
    char *id;
    types_t type;
    local_decl_t *next;
};

struct cmd_list{
    int line;
    cmd_t *cmd;
    cmd_list_t *next;
};

struct block{
    int line;
    local_decl_t *locals;
    cmd_list_t *cmds;
};

struct cmd{
    int line;
    cmd_e kind;

    expr_t *expr;       /* para CMD_EXPR, CMD_ESCREVA, CMD_RETORNE, CMD_IF, CMD_WHILE */
    char *id;           /* para CMD_LEIA, CMD_STRING */
    block_t *block;     /* para CMD_BLOCK */
    cmd_t *body;        /* para CMD_IF, CMD_IF_ELSE, CMD_WHILE */
    cmd_t *else_body;   /* para CMD_IF_ELSE */
};

struct expr_list{
    int line;
    expr_t *expr;
    expr_list_t *next;
};

struct expr{
    int line;
    expr_e kind;

    int int_value;      /* EXPR_INT */
    char char_value;    /* EXPR_CHAR */
    char *string_value; /* EXPR_STRING */
    char *id;           /* EXPR_ID, EXPR_ASSIGN, EXPR_CALL */

    expr_t *left;       /* subexpressão esquerda (binários e unários) */
    expr_t *right;      /* subexpressão direita (binários e EXPR_ASSIGN) */
    expr_list_t *args;  /* lista de argumentos (EXPR_CALL) */
};

/* ===== Funções construtoras da AST ===== */

/* programa */
program_t *ast_program(int line, global_decl_t *globals, block_t *main_block);

/* globais */
global_decl_t *ast_global_var(int line, types_t type, char *id, global_decl_t *next);
global_decl_t *ast_global_func(int line, types_t ret_type, char *id,
                               param_t *params, block_t *body,
                               global_decl_t *next);

/* parâmetros e locais */
param_t *ast_param(int line, types_t type, char *id, param_t *next);
local_decl_t *ast_local_decl(int line, types_t type, char *id, local_decl_t *next);

/* bloco e lista de comandos */
block_t *ast_block(int line, local_decl_t *locals, cmd_list_t *cmds);
cmd_list_t *ast_cmd_list(cmd_t *cmd, cmd_list_t *next);

/* comandos */
cmd_t *ast_cmd_expr(int line, expr_t *expr);
cmd_t *ast_cmd_block(int line, block_t *block);
cmd_t *ast_cmd_if(int line, expr_t *cond, cmd_t *then_cmd);
cmd_t *ast_cmd_if_else(int line, expr_t *cond, cmd_t *then_cmd, cmd_t *else_cmd);
cmd_t *ast_cmd_while(int line, expr_t *cond, cmd_t *body);
cmd_t *ast_cmd_leia(int line, char *id);
cmd_t *ast_cmd_escreva(int line, expr_t *expr);
cmd_t *ast_cmd_string(int line, char *str);
cmd_t *ast_cmd_retorne(int line, expr_t *expr);

/* expressões */
expr_list_t *ast_expr_list(int line, expr_t *expr, expr_list_t *next);

expr_t *ast_expr_id(int line, char *id);
expr_t *ast_expr_int(int line, int value);
expr_t *ast_expr_char(int line, char value);
expr_t *ast_expr_string(int line, char *value);

expr_t *ast_expr_unary(int line, expr_e kind, expr_t *expr);
expr_t *ast_expr_binary(int line, expr_e kind, expr_t *left, expr_t *right);
expr_t *ast_expr_assign(int line, char *id, expr_t *expr);
expr_t *ast_expr_call(int line, char *id, expr_list_t *args);

#endif /* AST_H */
