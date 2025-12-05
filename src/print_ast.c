#include <stdio.h>
#include <stdlib.h>
#include "print_ast.h"


const char *type_name(types_t type) {
    switch (type) {
        case TYPE_INT:  return "int";
        case TYPE_CAR:  return "car";
        case TYPE_VOID: return "void";
        default:        return "unknown";
    }
}

const char *get_op_symbol(expr_e kind) {
    switch (kind) {
        case EXPR_ADD: return "+";
        case EXPR_SUB: return "-";
        case EXPR_MUL: return "*";
        case EXPR_DIV: return "/";
        case EXPR_AND: return "&&";
        case EXPR_OR:  return "||";
        case EXPR_EQ:  return "==";
        case EXPR_NE:  return "!=";
        case EXPR_LT:  return "<";
        case EXPR_GT:  return ">";
        case EXPR_LE:  return "<=";
        case EXPR_GE:  return ">=";
        default: return "?";
    }
}



void print_expr(expr_t *expr) {
    if (!expr) return;

    switch (expr->kind) {
        case EXPR_INT:
            printf("%d", expr->int_value);
            break;

        case EXPR_CHAR:
            printf("'%c'", expr->char_value);
            break;

        case EXPR_STRING:
            printf("%s", expr->string_value);
            break;

        case EXPR_ID:
            printf("%s", expr->id);
            break;

        case EXPR_ASSIGN:
            printf("%s = ", expr->id);
            print_expr(expr->right);
            break;

        case EXPR_CALL:
            printf("%s(", expr->id);
            print_expr_list(expr->args);
            printf(")");
            break;

        /* Operações Unárias */
        case EXPR_MINUS:
            printf("-");
            print_expr(expr->left);
            break;

        case EXPR_NOT:
            printf("!");
            print_expr(expr->left);
            break;

        /* Operações Binárias */
        case EXPR_ADD: case EXPR_SUB: case EXPR_MUL: case EXPR_DIV:
        case EXPR_AND: case EXPR_OR:  case EXPR_EQ:  case EXPR_NE:
        case EXPR_LT:  case EXPR_GT:  case EXPR_LE:  case EXPR_GE:
            printf("(");
            print_expr(expr->left);
            printf(" %s ", get_op_symbol(expr->kind));
            print_expr(expr->right);
            printf(")");
            break;

        default:
            printf("Expr?");
    }
}

void print_expr_list(expr_list_t *list) {
    while (list) {
        print_expr(list->expr);
        if (list->next) printf(", ");
        list = list->next;
    }
}


void print_block(block_t *block);


void print_cmd(cmd_t *cmd) {
    if (!cmd) {
        printf("  /* comando vazio */\n");
        return;
    }

    printf("  Linha %d: ", cmd->line);

    switch (cmd->kind) {
        case CMD_EXPR:
            printf("Expr: ");
            print_expr(cmd->expr);
            printf("\n");
            break;

        case CMD_IF:
            printf("Se: ");
            print_expr(cmd->expr);
            printf("\n    Entao:\n");
            print_cmd(cmd->body);
            break;

        case CMD_IF_ELSE:
            printf("Se: ");
            print_expr(cmd->expr);
            printf("\n    Entao:\n");
            print_cmd(cmd->body);
            printf("    Senao:\n");
            print_cmd(cmd->else_body);
            break;

        case CMD_WHILE:
            printf("Enquanto: ");
            print_expr(cmd->expr);
            printf("\n    Execute:\n");
            print_cmd(cmd->body);
            break;

        case CMD_RETORNE:
            printf("Retorne: ");
            print_expr(cmd->expr);
            printf("\n");
            break;

        case CMD_LEIA:
            printf("Leia: %s\n", cmd->id);
            break;

        case CMD_ESCREVA:
            printf("Escreva: ");
            print_expr(cmd->expr);
            printf("\n");
            break;

        case CMD_STRING:
            printf("Escreva: %s\n", cmd->id);
            break;

        case CMD_BLOCK:
            printf("--- Inicio Bloco ---\n");
            print_block(cmd->block);
            printf("  --- Fim Bloco ---\n");
            break;

        default:
            printf("Comando desconhecido\n");
    }
}

void print_cmd_list(cmd_list_t *cmds) {
    while (cmds) {
        print_cmd(cmds->cmd);
        cmds = cmds->next;
    }
}


void print_local_decl(local_decl_t *decl) {
    while (decl) {
        printf("    [VAR] %s %s (Linha %d)\n",
               type_name(decl->type),
               decl->id,
               decl->line);
        decl = decl->next;
    }
}


void print_block(block_t *block) {
    if (!block) return;

    /* 1. Imprimir Variáveis Locais */
    if (block->locals) {
        printf("    Declaracoes Locais:\n");
        print_local_decl(block->locals);
    }

    /* 2. Imprimir Lista de Comandos */
    print_cmd_list(block->cmds);
}


void print_param_list(param_t *params) {
    printf("(");
    while (params) {
        printf("%s %s", type_name(params->type), params->id);
        if (params->next) printf(", ");
        params = params->next;
    }
    printf(")");
}


void print_global_decl(global_decl_t *decl, int indent) {
    while (decl) {
        if (decl->body == NULL) {
            /* Variável Global */
            printf("[GLOBAL VAR] %s %s (Linha %d)\n",
                   type_name(decl->type),
                   decl->id,
                   decl->line);
        } else {
            /* Função */
            printf("\n[FUNCAO] %s %s", type_name(decl->type), decl->id);
            print_param_list(decl->params);
            printf(" (Linha %d)\n", decl->line);
            print_block(decl->body);
        }
        decl = decl->next;
    }
}


void print_ast(program_t *program) {
    if (!program) {
        printf("Programa Vazio (NULL)\n");
        return;
    }

    printf("\n");
    printf("=== AST START ===\n\n");

    /*Declarações Globais */
    if (program->globals) {
        printf("=== DECLARACOES GLOBAIS ===\n");
        print_global_decl(program->globals, 0);
        printf("\n");
    }

    /* 2. Bloco Principal */
    if (program->main_block) {
        printf("=== MAIN BLOCK ===\n");
        print_block(program->main_block);
    }

    printf("\n=== AST END ===\n\n");
}


const char *expr_kind_name(expr_e kind) {
    switch (kind) {
        case EXPR_ID:     return "ID";
        case EXPR_INT:    return "INT";
        case EXPR_CHAR:   return "CHAR";
        case EXPR_STRING: return "STRING";
        case EXPR_MINUS:  return "MINUS";
        case EXPR_NOT:    return "NOT";
        case EXPR_ADD:    return "ADD";
        case EXPR_SUB:    return "SUB";
        case EXPR_MUL:    return "MUL";
        case EXPR_DIV:    return "DIV";
        case EXPR_LT:     return "LT";
        case EXPR_GT:     return "GT";
        case EXPR_LE:     return "LE";
        case EXPR_GE:     return "GE";
        case EXPR_EQ:     return "EQ";
        case EXPR_NE:     return "NE";
        case EXPR_AND:    return "AND";
        case EXPR_OR:     return "OR";
        case EXPR_ASSIGN: return "ASSIGN";
        case EXPR_CALL:   return "CALL";
        default:          return "UNKNOWN";
    }
}

const char *cmd_kind_name(cmd_e kind) {
    switch (kind) {
        case CMD_EXPR:     return "EXPR";
        case CMD_BLOCK:    return "BLOCK";
        case CMD_IF:       return "IF";
        case CMD_IF_ELSE:  return "IF_ELSE";
        case CMD_WHILE:    return "WHILE";
        case CMD_LEIA:     return "LEIA";
        case CMD_ESCREVA:  return "ESCREVA";
        case CMD_STRING:   return "STRING";
        case CMD_RETORNE:  return "RETORNE";
        default:           return "UNKNOWN";
    }
}
