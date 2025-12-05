#ifndef TYPES_H
#define TYPES_H

/* Tipos de dados da linguagem Goianinha */
typedef enum {
    TYPE_INT,
    TYPE_CAR,
    TYPE_VOID   /* para funções que não retornam valor */
} types_t;

/* Tipos de comandos */
typedef enum {
    CMD_EXPR,        /* comando de expressão */
    CMD_BLOCK,       /* bloco { ... } */
    CMD_IF,          /* se ... entao ... */
    CMD_IF_ELSE,     /* se ... entao ... senao ... */
    CMD_WHILE,       /* enquanto ... execute ... */
    CMD_LEIA,        /* leia identificador; */
    CMD_ESCREVA,     /* escreva expr; */
    CMD_STRING,      /* escreva "string"; */
    CMD_RETORNE      /* retorne expr; */
} cmd_e;

/* Tipos de expressões */
typedef enum {
    /* Literais */
    EXPR_ID,         /* identificador */
    EXPR_INT,        /* constante inteira */
    EXPR_CHAR,       /* constante de caractere */
    EXPR_STRING,     /* literal de string */
    
    /* Operadores unários */
    EXPR_MINUS,      /* -expr (negação aritmética) */
    EXPR_NOT,        /* !expr (negação lógica) */
    
    /* Operadores binários aritméticos */
    EXPR_ADD,        /* expr + expr */
    EXPR_SUB,        /* expr - expr */
    EXPR_MUL,        /* expr * expr */
    EXPR_DIV,        /* expr / expr */
    
    /* Operadores relacionais */
    EXPR_LT,         /* expr < expr */
    EXPR_GT,         /* expr > expr */
    EXPR_LE,         /* expr <= expr */
    EXPR_GE,         /* expr >= expr */
    EXPR_EQ,         /* expr == expr */
    EXPR_NE,         /* expr != expr */
    
    /* Operadores lógicos */
    EXPR_AND,        /* expr E expr */
    EXPR_OR,         /* expr OU expr */
    
    /* Atribuição */
    EXPR_ASSIGN,     /* id = expr */
    
    /* Chamada de função */
    EXPR_CALL        /* id(args) */
} expr_e;

#endif /* TYPES_H */
