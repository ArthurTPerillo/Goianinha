%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char *yytext;
extern FILE *yyin;

void yyerror(const char *s);

program_t *ast_root = NULL;
%}

%union {
    char* str;
    int   num;
    char  c;
    
    /* Tipos da AST */
    program_t *program;
    global_decl_t *global_decl;
    param_t *param;
    local_decl_t *local_decl;
    block_t *block;
    cmd_t *cmd;
    cmd_list_t *cmd_list;
    expr_t *expr;
    expr_list_t *expr_list;
    types_t type;
}

%token <str> IDENTIFIER STRING
%token <num> INTCONST
%token <c>   CARCONST

%token PLUS MINUS STAR SLASH COMMA SEMICOLON 
%token LEFT_PAREN RIGHT_PAREN LEFT_BRACE RIGHT_BRACE
%token PROGRAMA RETORNE LEIA ESCREVA ENQUANTO SENAO ENTAO EXECUTE 
%token E OU SE EQUAL EQUAL_EQUAL BANG BANG_EQUAL
%token LESS GREATER LESS_EQUAL GREATER_EQUAL
%token INT CAR NOVALINHA

%type <program> Programa
%type <global_decl> DeclFuncVar DeclVar
%type <block> DeclProg Bloco
%type <param> ListaParametros ListaParametrosCont
%type <local_decl> ListaDeclVar
%type <type> Tipo
%type <cmd_list> ListaComando
%type <cmd> Comando
%type <expr> Expr OrExpr AndExpr EqExpr DesigExpr AddExpr MulExpr UnExpr PrimExpr
%type <expr_list> ListExpr

%start Programa

%%

Programa:
        DeclFuncVar DeclProg
        {
            $$ = ast_program(yylineno, $1, $2);
            ast_root = $$;
        }
        ;

DeclFuncVar:
           Tipo IDENTIFIER DeclVar SEMICOLON DeclFuncVar
           {
               /* Declaração de variáveis globais: tipo id, id, id; */
               global_decl_t *vars = ast_global_var(yylineno, $1, $2, $3);
               /* Encadear com próximas declarações */
               if (vars) {
                   global_decl_t *last = vars;
                   while (last->next) last = last->next;
                   last->next = $5;
               }
               $$ = vars;
               free($2);
           }
           | Tipo IDENTIFIER LEFT_PAREN ListaParametros RIGHT_PAREN Bloco DeclFuncVar
           {
               /* Declaração de função */
               $$ = ast_global_func(yylineno, $1, $2, $4, $6, $7);
               free($2);
           }
           | /* vazio */
           {
               $$ = NULL;
           }
           ;

DeclVar:
       COMMA IDENTIFIER DeclVar
       {
           /* Mais variáveis na mesma declaração */
           $$ = ast_global_var(yylineno, TYPE_INT, $2, $3); 
           /* o tipo será ajustado pela regra pai */
           free($2);
       }
       | /* vazio */
       {
           $$ = NULL;
       }
       ;

DeclProg:
        PROGRAMA Bloco
        {
            $$ = $2;
        }
        ;

ListaParametros:
               /* vazio */
               {
                   $$ = NULL;
               }
               | ListaParametrosCont
               {
                   $$ = $1;
               }
               ;

ListaParametrosCont:
                   Tipo IDENTIFIER
                   {
                       $$ = ast_param(yylineno, $1, $2, NULL);
                       free($2);
                   }
                   | Tipo IDENTIFIER COMMA ListaParametrosCont
                   {
                       $$ = ast_param(yylineno, $1, $2, $4);
                       free($2);
                   }
                   ;

Bloco:
     LEFT_BRACE ListaDeclVar ListaComando RIGHT_BRACE
     {
         $$ = ast_block(yylineno, $2, $3);
     }
     ;

ListaDeclVar:
            /* vazio */
            {
                $$ = NULL;
            }
            | Tipo IDENTIFIER DeclVar SEMICOLON ListaDeclVar
            {
                /* Declarações locais no bloco */
                local_decl_t *var = ast_local_decl(yylineno, $1, $2, NULL);
                
                /* Se há mais variáveis na mesma linha (DeclVar) */
                if ($3) {
                    /* Converter global_decl para local_decl */
                    local_decl_t *last = var;
                    global_decl_t *gd = $3;
                    while (gd) {
                        last->next = ast_local_decl(gd->line, $1, gd->id, NULL);
                        last = last->next;
                        gd = gd->next;
                    }
                }
                
                /* Encadear com próximas declarações */
                if (var) {
                    local_decl_t *last = var;
                    while (last->next) last = last->next;
                    last->next = $5;
                }
                
                $$ = var;
                free($2);
            }
            ;

Tipo:
    INT
    {
        $$ = TYPE_INT;
    }
    | CAR
    {
        $$ = TYPE_CAR;
    }
    ;

ListaComando:
            Comando
            {
                $$ = ast_cmd_list($1, NULL);
            }
            | Comando ListaComando
            {
                $$ = ast_cmd_list($1, $2);
            }
            ;

Comando:
       SEMICOLON
       {
           $$ = NULL; /* Comando vazio */
       }
       | Expr SEMICOLON
       {
           $$ = ast_cmd_expr(yylineno, $1);
       }
       | RETORNE Expr SEMICOLON
       {
           $$ = ast_cmd_retorne(yylineno, $2);
       }
       | LEIA IDENTIFIER SEMICOLON
       {
           $$ = ast_cmd_leia(yylineno, $2);
           free($2);
       }
       | ESCREVA Expr SEMICOLON
       {
           $$ = ast_cmd_escreva(yylineno, $2);
       }
       | ESCREVA STRING SEMICOLON
       {
           $$ = ast_cmd_string(yylineno, $2);
           free($2);
       }
       | NOVALINHA SEMICOLON
       {
           /* NOVALINHA é tratado como escrever "\n" */
           $$ = ast_cmd_string(yylineno, "\"\\n\"");
       }
       | SE LEFT_PAREN Expr RIGHT_PAREN ENTAO Comando
       {
           $$ = ast_cmd_if(yylineno, $3, $6);
       }
       | SE LEFT_PAREN Expr RIGHT_PAREN ENTAO Comando SENAO Comando
       {
           $$ = ast_cmd_if_else(yylineno, $3, $6, $8);
       }
       | ENQUANTO LEFT_PAREN Expr RIGHT_PAREN EXECUTE Comando
       {
           $$ = ast_cmd_while(yylineno, $3, $6);
       }
       | Bloco
       {
           $$ = ast_cmd_block(yylineno, $1);
       }
       ;

Expr:
    OrExpr
    {
        $$ = $1;
    }
    | IDENTIFIER EQUAL Expr
    {
        $$ = ast_expr_assign(yylineno, $1, $3);
        free($1);
    }
    ;

OrExpr:
      OrExpr OU AndExpr
      {
          $$ = ast_expr_binary(yylineno, EXPR_OR, $1, $3);
      }
      | AndExpr
      {
          $$ = $1;
      }
      ;

AndExpr:
       AndExpr E EqExpr
       {
           $$ = ast_expr_binary(yylineno, EXPR_AND, $1, $3);
       }
       | EqExpr
       {
           $$ = $1;
       }
       ;

EqExpr:
      EqExpr EQUAL_EQUAL DesigExpr
      {
          $$ = ast_expr_binary(yylineno, EXPR_EQ, $1, $3);
      }
      | EqExpr BANG_EQUAL DesigExpr
      {
          $$ = ast_expr_binary(yylineno, EXPR_NE, $1, $3);
      }
      | DesigExpr
      {
          $$ = $1;
      }
      ;

DesigExpr: 
         DesigExpr LESS AddExpr
         {
             $$ = ast_expr_binary(yylineno, EXPR_LT, $1, $3);
         }
         | DesigExpr GREATER AddExpr
         {
             $$ = ast_expr_binary(yylineno, EXPR_GT, $1, $3);
         }
         | DesigExpr GREATER_EQUAL AddExpr
         {
             $$ = ast_expr_binary(yylineno, EXPR_GE, $1, $3);
         }
         | DesigExpr LESS_EQUAL AddExpr
         {
             $$ = ast_expr_binary(yylineno, EXPR_LE, $1, $3);
         }
         | AddExpr
         {
             $$ = $1;
         }
         ;

AddExpr:
       AddExpr PLUS MulExpr
       {
           $$ = ast_expr_binary(yylineno, EXPR_ADD, $1, $3);
       }
       | AddExpr MINUS MulExpr
       {
           $$ = ast_expr_binary(yylineno, EXPR_SUB, $1, $3);
       }
       | MulExpr
       {
           $$ = $1;
       }
       ;

MulExpr:
       MulExpr STAR UnExpr
       {
           $$ = ast_expr_binary(yylineno, EXPR_MUL, $1, $3);
       }
       | MulExpr SLASH UnExpr
       {
           $$ = ast_expr_binary(yylineno, EXPR_DIV, $1, $3);
       }
       | UnExpr
       {
           $$ = $1;
       }
       ;

UnExpr:
      MINUS PrimExpr
      {
          $$ = ast_expr_unary(yylineno, EXPR_MINUS, $2);
      }
      | BANG PrimExpr
      {
          $$ = ast_expr_unary(yylineno, EXPR_NOT, $2);
      }
      | PrimExpr
      {
          $$ = $1;
      }
      ;

PrimExpr:
        IDENTIFIER LEFT_PAREN ListExpr RIGHT_PAREN
        {
            /* Chamada de função com argumentos */
            $$ = ast_expr_call(yylineno, $1, $3);
            free($1);
        }
        | IDENTIFIER LEFT_PAREN RIGHT_PAREN
        {
            /* Chamada de função sem argumentos */
            $$ = ast_expr_call(yylineno, $1, NULL);
            free($1);
        }
        | IDENTIFIER
        {
            $$ = ast_expr_id(yylineno, $1);
            free($1);
        }
        | CARCONST
        {
            $$ = ast_expr_char(yylineno, $1);
        }
        | INTCONST
        {
            $$ = ast_expr_int(yylineno, $1);
        }
        | LEFT_PAREN Expr RIGHT_PAREN
        {
            $$ = $2;
        }
        ;

ListExpr:
        Expr
        {
            $$ = ast_expr_list(yylineno, $1, NULL);
        }
        | ListExpr COMMA Expr
        {
            /* Inverter a ordem para manter a ordem correta */
            expr_list_t *new_node = ast_expr_list(yylineno, $3, NULL);
            if ($1) {
                expr_list_t *last = $1;
                while (last->next) last = last->next;
                last->next = new_node;
            }
            $$ = $1 ? $1 : new_node;
        }
        ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO: %s na linha %d, próximo de '%s'\n", s, yylineno, yytext);
}

