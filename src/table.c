#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

/* Cria a pilha vazia (nenhum escopo) */
TableNode* create_stack() {
    return NULL;
}

/* Libera todos os escopos e todas as entradas */
void free_stack(TableNode *node) {
    while (node != NULL) {
        node = remove_table_node(node);
    }
}

/* Cria um novo escopo no topo da pilha */
TableNode* create_table_node(TableNode *node) {
    TableNode *new_node = malloc(sizeof(TableNode));
    if (!new_node) {
        fprintf(stderr, "Erro de memoria em create_table_node\n");
        exit(EXIT_FAILURE);
    }
    new_node->entry = NULL;
    new_node->size  = 0;
    new_node->pai   = node;
    return new_node;
}

/* Remove o escopo do topo da pilha, liberando todas as entradas daquele escopo */
TableNode* remove_table_node(TableNode *node) {
    if (node == NULL) return NULL;

    TableEntry *aux = node->entry;
    while (aux) {
        TableEntry *temp = aux;
        aux = aux->prox;
        free(temp->name);
        free(temp->argtipo);  /* ok chamar free(NULL) */
        free(temp);
    }

    TableNode *pai = node->pai;
    free(node);
    return pai;
}

/* Busca subindo na pilha: escopo atual, depois pai, etc. */
TableEntry* find(TableNode *node, char *name) {
    while (node != NULL) {
        TableEntry *aux = node->entry;
        while (aux != NULL) {
            if (strcmp(aux->name, name) == 0) {
                return aux;
            }
            aux = aux->prox;
        }
        node = node->pai;
    }
    return NULL;
}

// Busca **apenas** no escopo atual (não sobe para o pai).
    
TableEntry* find_local(TableNode *node, char *name) {
    if (!node) return NULL;
    TableEntry *aux = node->entry;
    while (aux != NULL) {
        if (strcmp(aux->name, name) == 0) {
            return aux;
        }
        aux = aux->prox;
    }
    return NULL;
}

/* Cria uma entrada genérica (variável, parâmetro ou função) */
static TableEntry* create_entry(char *name, CatSimbolo category,
                                Tipo tipo, int pos) {
    TableEntry *new_entry = malloc(sizeof(TableEntry));
    if (!new_entry) {
        fprintf(stderr, "Erro de memoria em create_entry\n");
        exit(EXIT_FAILURE);
    }

    new_entry->name = strdup(name);
    if (!new_entry->name) {
        fprintf(stderr, "Erro de memoria em create_entry (strdup)\n");
        exit(EXIT_FAILURE);
    }

    new_entry->category = category;
    new_entry->tipo     = tipo;
    new_entry->pos      = pos;

    new_entry->narg     = 0;
    new_entry->argtipo  = NULL;
    new_entry->prox     = NULL;

    return new_entry;
}

/* Insere no início da lista do escopo atual */
static TableEntry* insert(TableNode *node, TableEntry *new_entry) {
    new_entry->prox = node->entry;
    node->entry     = new_entry;
    node->size++;
    return new_entry;
}

/* Insere variável local ou global no escopo atual */
TableEntry* insert_variable(TableNode *node, char *name, Tipo tipo, int pos) {
    return insert(node, create_entry(name, var, tipo, pos));
}

/* Insere parâmetro formal no escopo atual */
TableEntry* insert_parameter(TableNode *node, char *name, Tipo tipo, int pos) {
    return insert(node, create_entry(name, par, tipo, pos));
}

/* Insere função (com vetor de tipos dos parâmetros) no escopo atual */
TableEntry* insert_function(TableNode *node, char *name, Tipo tipo,
                            int narg, Tipo *argtipo) {
    TableEntry *new_entry = create_entry(name, func, tipo, 0);
    new_entry->narg = narg;

    if (narg > 0) {
        new_entry->argtipo = malloc(narg * sizeof(Tipo));
        if (!new_entry->argtipo) {
            fprintf(stderr, "Erro de memoria em insert_function\n");
            exit(EXIT_FAILURE);
        }
        memcpy(new_entry->argtipo, argtipo, narg * sizeof(Tipo));
    }

    return insert(node, new_entry);
}

/* Imprime o escopo atual (para debug) */
void print_table(TableNode *node) {
    if (!node) return;

    printf("Tabela de simbolos (size %d):\n", node->size);
    TableEntry *aux = node->entry;

    while (aux) {
        printf("  Nome: %s | Cat: %d | Tipo: %d | Pos: %d",
               aux->name, aux->category, aux->tipo, aux->pos);

        if (aux->category == func) {
            printf(" | Args: %d", aux->narg);
        }

        printf("\n");
        aux = aux->prox;
    }
}
