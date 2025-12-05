#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ast.h"


static void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "Erro: falta de memória na construção da AST.\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    char *r = strdup(s);
    if (!r) {
        fprintf(stderr, "Erro: falta de memória ao copiar string.\n");
        exit(EXIT_FAILURE);
    }
    return r;
}

//programa

program_t *ast_program(int line, global_decl_t *globals, block_t *main_block) {
    program_t *p = (program_t *)xmalloc(sizeof(program_t));
    p->line       = line;
    p->globals    = globals;
    p->main_block = main_block;
    return p;
}

//globais

global_decl_t *ast_global_var(int line, types_t type, char *id,
                              global_decl_t *next) {
    global_decl_t *g = (global_decl_t *)xmalloc(sizeof(global_decl_t));
    g->line   = line;
    g->id     = xstrdup(id);
    g->type   = type;
    g->params = NULL;   /* variável não tem parâmetros */
    g->body   = NULL;   /* nem corpo de função */
    g->next   = next;
    return g;
}

global_decl_t *ast_global_func(int line, types_t ret_type, char *id,
                               param_t *params, block_t *body,
                               global_decl_t *next) {
    global_decl_t *g = (global_decl_t *)xmalloc(sizeof(global_decl_t));
    g->line   = line;
    g->id     = xstrdup(id);
    g->type   = ret_type;
    g->params = params;
    g->body   = body;
    g->next   = next;
    return g;
}

//parâmetros 

param_t *ast_param(int line, types_t type, char *id, param_t *next) {
    param_t *p = (param_t *)xmalloc(sizeof(param_t));
    p->line = line;
    p->type = type;
    p->id   = xstrdup(id);
    p->next = next;
    return p;
}

local_decl_t *ast_local_decl(int line, types_t type, char *id,
                             local_decl_t *next) {
    local_decl_t *d = (local_decl_t *)xmalloc(sizeof(local_decl_t));
    d->line = line;
    d->type = type;
    d->id   = xstrdup(id);
    d->next = next;
    return d;
}

//bloco

block_t *ast_block(int line, local_decl_t *locals, cmd_list_t *cmds) {
    block_t *b = (block_t *)xmalloc(sizeof(block_t));
    b->line   = line;
    b->locals = locals;
    b->cmds   = cmds;
    return b;
}

cmd_list_t *ast_cmd_list(cmd_t *cmd, cmd_list_t *next) {
    cmd_list_t *cl = (cmd_list_t *)xmalloc(sizeof(cmd_list_t));
    cl->line = cmd ? cmd->line : 0;
    cl->cmd  = cmd;
    cl->next = next;
    return cl;
}

//comandos

static cmd_t *ast_cmd_base(int line, cmd_e kind) {
    cmd_t *c = (cmd_t *)xmalloc(sizeof(cmd_t));
    c->line      = line;
    c->kind      = kind;
    c->expr      = NULL;
    c->id        = NULL;
    c->block     = NULL;
    c->body      = NULL;
    c->else_body = NULL;
    return c;
}

cmd_t *ast_cmd_expr(int line, expr_t *expr) {
    cmd_t *c = ast_cmd_base(line, CMD_EXPR);
    c->expr = expr;
    return c;
}

cmd_t *ast_cmd_block(int line, block_t *block) {
    cmd_t *c = ast_cmd_base(line, CMD_BLOCK);
    c->block = block;
    return c;
}

cmd_t *ast_cmd_if(int line, expr_t *cond, cmd_t *then_cmd) {
    cmd_t *c = ast_cmd_base(line, CMD_IF);
    c->expr = cond;
    c->body = then_cmd;
    return c;
}

cmd_t *ast_cmd_if_else(int line, expr_t *cond, cmd_t *then_cmd, cmd_t *else_cmd) {
    cmd_t *c = ast_cmd_base(line, CMD_IF_ELSE);
    c->expr      = cond;
    c->body      = then_cmd;
    c->else_body = else_cmd;
    return c;
}

cmd_t *ast_cmd_while(int line, expr_t *cond, cmd_t *body) {
    cmd_t *c = ast_cmd_base(line, CMD_WHILE);
    c->expr = cond;
    c->body = body;
    return c;
}

cmd_t *ast_cmd_leia(int line, char *id) {
    cmd_t *c = ast_cmd_base(line, CMD_LEIA);
    c->id = xstrdup(id);
    return c;
}

cmd_t *ast_cmd_escreva(int line, expr_t *expr) {
    cmd_t *c = ast_cmd_base(line, CMD_ESCREVA);
    c->expr = expr;
    return c;
}

cmd_t *ast_cmd_string(int line, char *str) {
    cmd_t *c = ast_cmd_base(line, CMD_STRING);
    c->id = xstrdup(str);
    return c;
}

cmd_t *ast_cmd_retorne(int line, expr_t *expr) {
    cmd_t *c = ast_cmd_base(line, CMD_RETORNE);
    c->expr = expr;
    return c;
}

//expressões

expr_list_t *ast_expr_list(int line, expr_t *expr, expr_list_t *next) {
    expr_list_t *el = (expr_list_t *)xmalloc(sizeof(expr_list_t));
    el->line = line;
    el->expr = expr;
    el->next = next;
    return el;
}

static expr_t *ast_expr_base(int line, expr_e kind) {
    expr_t *e = (expr_t *)xmalloc(sizeof(expr_t));
    e->line         = line;
    e->kind         = kind;
    e->int_value    = 0;
    e->char_value   = '\0';
    e->string_value = NULL;
    e->id           = NULL;
    e->left         = NULL;
    e->right        = NULL;
    e->args         = NULL;
    return e;
}

/* folhas */

expr_t *ast_expr_id(int line, char *id) {
    expr_t *e = ast_expr_base(line, EXPR_ID);
    e->id = xstrdup(id);
    return e;
}

expr_t *ast_expr_int(int line, int value) {
    expr_t *e = ast_expr_base(line, EXPR_INT);
    e->int_value = value;
    return e;
}

expr_t *ast_expr_char(int line, char value) {
    expr_t *e = ast_expr_base(line, EXPR_CHAR);
    e->char_value = value;
    return e;
}

expr_t *ast_expr_string(int line, char *value) {
    expr_t *e = ast_expr_base(line, EXPR_STRING);
    e->string_value = xstrdup(value);
    return e;
}

/* unária: !expr, -expr, etc. (kind: EXPR_UN_OP, EXPR_MINUS, EXPR_NOT, ...) */
expr_t *ast_expr_unary(int line, expr_e kind, expr_t *expr) {
    expr_t *e = ast_expr_base(line, kind);
    e->left = expr;
    return e;
}

/* binária: +, -, *, /, <, >, &&, ||, ==, etc. */
expr_t *ast_expr_binary(int line, expr_e kind, expr_t *left, expr_t *right) {
    expr_t *e = ast_expr_base(line, kind);
    e->left  = left;
    e->right = right;
    return e;
}

/* atribuição: id = expr */
expr_t *ast_expr_assign(int line, char *id, expr_t *expr) {
    expr_t *e = ast_expr_base(line, EXPR_ASSIGN);
    e->id   = xstrdup(id);
    e->right = expr;   /* escolha: usamos 'right' como expressão atribuída */
    return e;
}

/* chamada de função: id(args) */
expr_t *ast_expr_call(int line, char *id, expr_list_t *args) {
    expr_t *e = ast_expr_base(line, EXPR_CALL);
    e->id = xstrdup(id);
    e->args = args;
    return e;
}
