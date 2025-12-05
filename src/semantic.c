#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"

/* Contador global de erros semânticos */
static int error_count = 0;

/* =========================
 * Mensagens de Erro
 * ========================= */

void semantic_error(int line, const char *format, ...) {
    fprintf(stderr, "ERRO SEMANTICO (linha %d): ", line);
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    error_count++;
}

int get_error_count(void) {
    return error_count;
}

void reset_error_count(void) {
    error_count = 0;
}

/* =========================
 * Conversores de Tipos
 * ========================= */

Tipo ast_type_to_table_type(types_t ast_type) {
    switch (ast_type) {
        case TYPE_INT:  return Tipe_int;
        case TYPE_CAR:  return Tipe_car;
        default:
            fprintf(stderr, "ERRO INTERNO: tipo AST desconhecido\n");
            exit(EXIT_FAILURE);
    }
}

types_t table_type_to_ast_type(Tipo table_type) {
    switch (table_type) {
        case Tipe_int:  return TYPE_INT;
        case Tipe_car:  return TYPE_CAR;
        default:
            fprintf(stderr, "ERRO INTERNO: tipo Table desconhecido\n");
            exit(EXIT_FAILURE);
    }
}

/* =========================
 * Verificações Auxiliares
 * ========================= */

int types_compatible(types_t t1, types_t t2) {
    return t1 == t2;
}

int check_redeclaration(TableNode *scope, char *name, int line) {
    TableEntry *entry = find_local(scope, name);
    if (entry) {
        semantic_error(line, "identificador '%s' ja declarado neste escopo", name);
        return 1;
    }
    return 0;
}

/* =========================
 * Análise de Expressões
 * ========================= */

types_t analyze_expr(expr_t *expr, TableNode *scope) {
    if (!expr) return TYPE_VOID;
    
    switch (expr->kind) {
        case EXPR_INT:
            return TYPE_INT;
            
        case EXPR_CHAR:
            return TYPE_CAR;
            
        case EXPR_STRING:
            /* Strings são tratadas como arrays de char, mas para simplicidade
               retornamos TYPE_CAR (compatível com escreva) */
            return TYPE_CAR;
            
        case EXPR_ID: {
            TableEntry *entry = find(scope, expr->id);
            if (!entry) {
                semantic_error(expr->line, "variavel '%s' nao declarada", expr->id);
                return TYPE_VOID;
            }
            if (entry->category == func) {
                semantic_error(expr->line, "'%s' e uma funcao, nao pode ser usada como variavel", expr->id);
                return TYPE_VOID;
            }
            return table_type_to_ast_type(entry->tipo);
        }
        
        case EXPR_ASSIGN: {
            /* Verifica se variável existe */
            TableEntry *entry = find(scope, expr->id);
            if (!entry) {
                semantic_error(expr->line, "variavel '%s' nao declarada", expr->id);
                return TYPE_VOID;
            }
            if (entry->category == func) {
                semantic_error(expr->line, "'%s' e uma funcao, nao pode receber atribuicao", expr->id);
                return TYPE_VOID;
            }
            if (entry->category == par) {
                /* Em Goianinha, parâmetros podem receber atribuição (passagem por valor) */
            }
            
            /* Analisa expressão do lado direito */
            types_t right_type = analyze_expr(expr->right, scope);
            types_t var_type = table_type_to_ast_type(entry->tipo);
            
            /* Verifica compatibilidade de tipos */
            if (!types_compatible(var_type, right_type)) {
                semantic_error(expr->line, 
                    "tipos incompativeis na atribuicao: '%s' e %s mas expressao e %s",
                    expr->id,
                    var_type == TYPE_INT ? "int" : "car",
                    right_type == TYPE_INT ? "int" : "car");
                return TYPE_VOID;
            }
            
            return var_type;
        }
        
        case EXPR_CALL: {
            /* Verifica se função existe */
            TableEntry *func_entry = find(scope, expr->id);
            if (!func_entry) {
                semantic_error(expr->line, "funcao '%s' nao declarada", expr->id);
                return TYPE_VOID;
            }
            if (func_entry->category != func) {
                semantic_error(expr->line, "'%s' nao e uma funcao", expr->id);
                return TYPE_VOID;
            }
            
            /* Verifica argumentos */
            if (analyze_call_args(expr->args, scope, func_entry, expr->line) != 0) {
                return TYPE_VOID;
            }
            
            return table_type_to_ast_type(func_entry->tipo);
        }
        
        /* Operadores unários */
        case EXPR_MINUS: {
            types_t operand_type = analyze_expr(expr->left, scope);
            if (operand_type != TYPE_INT) {
                semantic_error(expr->line, "operador '-' requer operando do tipo int");
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        
        case EXPR_NOT: {
            types_t operand_type = analyze_expr(expr->left, scope);
            if (operand_type != TYPE_INT) {
                semantic_error(expr->line, "operador '!' requer operando do tipo int");
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        
        /* Operadores aritméticos */
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV: {
            types_t left_type = analyze_expr(expr->left, scope);
            types_t right_type = analyze_expr(expr->right, scope);
            
            if (left_type != TYPE_INT) {
                semantic_error(expr->line, "operador aritmetico requer operando esquerdo do tipo int");
                return TYPE_VOID;
            }
            if (right_type != TYPE_INT) {
                semantic_error(expr->line, "operador aritmetico requer operando direito do tipo int");
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        
        /* Operadores relacionais */
        case EXPR_LT:
        case EXPR_GT:
        case EXPR_LE:
        case EXPR_GE:
        case EXPR_EQ:
        case EXPR_NE: {
            types_t left_type = analyze_expr(expr->left, scope);
            types_t right_type = analyze_expr(expr->right, scope);
            
            if (!types_compatible(left_type, right_type)) {
                semantic_error(expr->line, 
                    "operador relacional requer operandos de mesmo tipo");
                return TYPE_VOID;
            }
            return TYPE_INT; /* Resultado de comparação é int (0 ou 1) */
        }
        
        /* Operadores lógicos */
        case EXPR_AND:
        case EXPR_OR: {
            types_t left_type = analyze_expr(expr->left, scope);
            types_t right_type = analyze_expr(expr->right, scope);
            
            if (left_type != TYPE_INT) {
                semantic_error(expr->line, "operador logico requer operando esquerdo do tipo int");
                return TYPE_VOID;
            }
            if (right_type != TYPE_INT) {
                semantic_error(expr->line, "operador logico requer operando direito do tipo int");
                return TYPE_VOID;
            }
            return TYPE_INT;
        }
        
        default:
            semantic_error(expr->line, "expressao desconhecida");
            return TYPE_VOID;
    }
}

int analyze_call_args(expr_list_t *args, TableNode *scope, 
                      TableEntry *func_entry, int line) {
    /* Conta argumentos fornecidos */
    int num_args = 0;
    expr_list_t *arg = args;
    while (arg) {
        num_args++;
        arg = arg->next;
    }
    
    /* Verifica número de argumentos */
    if (num_args != func_entry->narg) {
        semantic_error(line, 
            "funcao '%s' espera %d argumento(s), mas %d foi(ram) fornecido(s)",
            func_entry->name, func_entry->narg, num_args);
        return -1;
    }
    
    /* Verifica tipo de cada argumento */
    arg = args;
    for (int i = 0; i < num_args; i++) {
        types_t arg_type = analyze_expr(arg->expr, scope);
        types_t expected_type = table_type_to_ast_type(func_entry->argtipo[i]);
        
        if (!types_compatible(arg_type, expected_type)) {
            semantic_error(line,
                "argumento %d de '%s': tipo incompativel (esperado %s, recebido %s)",
                i + 1, func_entry->name,
                expected_type == TYPE_INT ? "int" : "car",
                arg_type == TYPE_INT ? "int" : "car");
            return -1;
        }
        
        arg = arg->next;
    }
    
    return 0;
}

/* =========================
 * Análise de Comandos
 * ========================= */

int analyze_cmd(cmd_t *cmd, TableNode *scope, types_t func_return_type) {
    if (!cmd) return 0;
    
    switch (cmd->kind) {
        case CMD_EXPR: {
            analyze_expr(cmd->expr, scope);
            return 0;
        }
        
        case CMD_BLOCK: {
            return analyze_block(cmd->block, &scope, func_return_type);
        }
        
        case CMD_IF: {
            types_t cond_type = analyze_expr(cmd->expr, scope);
            if (cond_type != TYPE_INT && cond_type != TYPE_VOID) {
                semantic_error(cmd->line, 
                    "condicao do 'se' deve ser do tipo int");
            }
            return analyze_cmd(cmd->body, scope, func_return_type);
        }
        
        case CMD_IF_ELSE: {
            types_t cond_type = analyze_expr(cmd->expr, scope);
            if (cond_type != TYPE_INT && cond_type != TYPE_VOID) {
                semantic_error(cmd->line, 
                    "condicao do 'se' deve ser do tipo int");
            }
            int err1 = analyze_cmd(cmd->body, scope, func_return_type);
            int err2 = analyze_cmd(cmd->else_body, scope, func_return_type);
            return err1 || err2;
        }
        
        case CMD_WHILE: {
            types_t cond_type = analyze_expr(cmd->expr, scope);
            if (cond_type != TYPE_INT && cond_type != TYPE_VOID) {
                semantic_error(cmd->line, 
                    "condicao do 'enquanto' deve ser do tipo int");
            }
            return analyze_cmd(cmd->body, scope, func_return_type);
        }
        
        case CMD_LEIA: {
            TableEntry *entry = find(scope, cmd->id);
            if (!entry) {
                semantic_error(cmd->line, 
                    "variavel '%s' nao declarada", cmd->id);
                return -1;
            }
            if (entry->category == func) {
                semantic_error(cmd->line, 
                    "'%s' e uma funcao, nao pode ser usada com 'leia'", cmd->id);
                return -1;
            }
            return 0;
        }
        
        case CMD_ESCREVA: {
            analyze_expr(cmd->expr, scope);
            return 0;
        }
        
        case CMD_STRING: {
            /* Strings são sempre válidas */
            return 0;
        }
        
        case CMD_RETORNE: {
            if (func_return_type == TYPE_VOID) {
                semantic_error(cmd->line, 
                    "'retorne' usado fora de uma funcao");
                return -1;
            }
            
            types_t expr_type = analyze_expr(cmd->expr, scope);
            if (!types_compatible(expr_type, func_return_type)) {
                semantic_error(cmd->line,
                    "tipo do retorno incompativel: funcao retorna %s mas expressao e %s",
                    func_return_type == TYPE_INT ? "int" : "car",
                    expr_type == TYPE_INT ? "int" : "car");
                return -1;
            }
            return 0;
        }
        
        default:
            return 0;
    }
}

int analyze_cmd_list(cmd_list_t *cmds, TableNode *scope, types_t func_return_type) {
    int errors = 0;
    while (cmds) {
        if (analyze_cmd(cmds->cmd, scope, func_return_type) != 0) {
            errors = 1;
        }
        cmds = cmds->next;
    }
    return errors ? -1 : 0;
}

/* =========================
 * Análise de Blocos
 * ========================= */

int analyze_local_declarations(local_decl_t *locals, TableNode *scope) {
    int errors = 0;
    int pos = 0;
    
    while (locals) {
        /* Verifica redeclaração no escopo local */
        if (check_redeclaration(scope, locals->id, locals->line)) {
            errors = 1;
        } else {
            /* Insere na tabela */
            Tipo table_type = ast_type_to_table_type(locals->type);
            insert_variable(scope, locals->id, table_type, pos);
            pos++;
        }
        
        locals = locals->next;
    }
    
    return errors ? -1 : 0;
}

int analyze_block(block_t *block, TableNode **scope, types_t func_return_type) {
    if (!block) return 0;
    
    /* Cria novo escopo */
    *scope = create_table_node(*scope);
    
    /* Processa declarações locais */
    int errors = analyze_local_declarations(block->locals, *scope);
    
    /* Processa comandos (PROPAGA o tipo de retorno da função) */
    if (analyze_cmd_list(block->cmds, *scope, func_return_type) != 0) {
        errors = 1;
    }
    
    /* Remove escopo */
    *scope = remove_table_node(*scope);
    
    return errors ? -1 : 0;
}

/* =========================
 * Análise de Funções
 * ========================= */

int analyze_function(global_decl_t *func_decl, TableNode **scope) {
    /* Cria escopo para a função */
    *scope = create_table_node(*scope);
    
    /* Insere parâmetros no escopo da função */
    param_t *param = func_decl->params;
    int pos = 0;
    while (param) {
        if (check_redeclaration(*scope, param->id, param->line)) {
            /* Erro: parâmetro duplicado */
        } else {
            Tipo table_type = ast_type_to_table_type(param->type);
            insert_parameter(*scope, param->id, table_type, pos);
            pos++;
        }
        param = param->next;
    }
    
    /* Processa declarações locais do corpo da função */
    int errors = 0;
    if (func_decl->body->locals) {
        if (analyze_local_declarations(func_decl->body->locals, *scope) != 0) {
            errors = 1;
        }
    }
    
    /* Verifica se há variável local com mesmo nome de parâmetro no bloco mais externo */
    local_decl_t *local = func_decl->body->locals;
    while (local) {
        param_t *p = func_decl->params;
        while (p) {
            if (strcmp(local->id, p->id) == 0) {
                semantic_error(local->line,
                    "variavel local '%s' tem mesmo nome de parametro formal", local->id);
                errors = 1;
            }
            p = p->next;
        }
        local = local->next;
    }
    
    /* Analisa comandos da função (passa o tipo de retorno) */
    if (analyze_cmd_list(func_decl->body->cmds, *scope, func_decl->type) != 0) {
        errors = 1;
    }
    
    /* Remove escopo da função */
    *scope = remove_table_node(*scope);
    
    return errors ? -1 : 0;
}

/* =========================
 * Análise de Declarações Globais
 * ========================= */

int analyze_global_declarations(global_decl_t *globals, TableNode **scope) {
    int errors = 0;
    int var_pos = 0;
    
    global_decl_t *decl = globals;
    while (decl) {
        /* Verifica redeclaração */
        if (check_redeclaration(*scope, decl->id, decl->line)) {
            errors = 1;
            decl = decl->next;
            continue;
        }
        
        if (decl->body == NULL) {
            /* Variável global */
            Tipo table_type = ast_type_to_table_type(decl->type);
            insert_variable(*scope, decl->id, table_type, var_pos);
            var_pos++;
        } else {
            /* Função: monta vetor de tipos dos parâmetros */
            int narg = 0;
            param_t *p = decl->params;
            while (p) {
                narg++;
                p = p->next;
            }
            
            Tipo *argtipo = NULL;
            if (narg > 0) {
                argtipo = malloc(narg * sizeof(Tipo));
                p = decl->params;
                for (int i = 0; i < narg; i++) {
                    argtipo[i] = ast_type_to_table_type(p->type);
                    p = p->next;
                }
            }
            
            /* Insere função na tabela */
            Tipo ret_type = ast_type_to_table_type(decl->type);
            insert_function(*scope, decl->id, ret_type, narg, argtipo);
            
            if (argtipo) free(argtipo);
            
            /* Analisa corpo da função */
            if (analyze_function(decl, scope) != 0) {
                errors = 1;
            }
        }
        
        decl = decl->next;
    }
    
    return errors ? -1 : 0;
}

/* =========================
 * Análise Semântica Principal
 * ========================= */

int semantic_analysis(program_t *program) {
    if (!program) {
        fprintf(stderr, "ERRO: programa NULL\n");
        return -1;
    }
    
    reset_error_count();
    
    printf("\n=== INICIANDO ANALISE SEMANTICA ===\n\n");
    
    /* Cria escopo global */
    TableNode *global_scope = create_stack();
    global_scope = create_table_node(global_scope);
    
    /* Analisa declarações globais */
    if (program->globals) {
        analyze_global_declarations(program->globals, &global_scope);
    }
    
    /* Analisa bloco principal */
    if (program->main_block) {
        analyze_block(program->main_block, &global_scope, TYPE_VOID);
    }
    
    /* Libera tabela de símbolos */
    free_stack(global_scope);
    
    printf("\n=== FIM DA ANALISE SEMANTICA ===\n");
    
    if (error_count > 0) {
        printf("✗ Analise semantica: %d erro(s) encontrado(s)\n\n", error_count);
        return -1;
    } else {
        printf("✓ Analise semantica: SUCESSO (nenhum erro encontrado)\n\n");
        return 0;
    }
}
