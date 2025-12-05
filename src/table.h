#ifndef TABLE_H
#define TABLE_H

/* Categoria do símbolo: variável, parâmetro formal ou função */
typedef enum {
    var,
    par,
    func
} CatSimbolo;

/* Tipos da linguagem Goianinha: apenas int e car */
typedef enum {
    Tipe_int,
    Tipe_car
} Tipo;

/* Entrada da tabela de símbolos (um identificador) */
typedef struct TableEntry {
    char *name;              /* nome do identificador */
    CatSimbolo category;     /* var, par ou func     */
    Tipo tipo;               /* tipo do identificador (Tipe_int ou Tipe_car) */
    int pos;                 /* posição (pode ser usada como offset, etc.)   */

    /* Campos usados apenas para funções */
    int narg;                /* número de parâmetros formais                 */
    Tipo *argtipo;           /* vetor com os tipos dos parâmetros            */

    struct TableEntry *prox; /* próximo símbolo no mesmo escopo              */
} TableEntry;

/* Nó da pilha de escopos */
typedef struct TableNode {
    TableEntry *entry;       /* lista encadeada de entradas neste escopo */
    int size;                /* número de entradas neste escopo          */
    struct TableNode *pai;   /* ponteiro para o escopo pai (mais externo) */
} TableNode;

/* Cria a pilha vazia (nenhum escopo ainda) */
TableNode* create_stack();

/* Libera toda a pilha de escopos e todas as entradas */
void free_stack(TableNode *node);

/* Cria um novo escopo no topo da pilha (retorna o novo topo) */
TableNode* create_table_node(TableNode *node);

/* Remove o escopo do topo da pilha (retorna o escopo pai) */
TableNode* remove_table_node(TableNode *node);

/* Insere uma variável no escopo atual */
TableEntry* insert_variable(TableNode *node, char *name, Tipo tipo, int pos);

/* Insere um parâmetro formal no escopo atual */
TableEntry* insert_parameter(TableNode *node, char *name, Tipo tipo, int pos);

/* Insere uma função no escopo global (ou atual) */
TableEntry* insert_function(TableNode *node, char *name, Tipo tipo,
                            int narg, Tipo *argtipo);

/* Busca um símbolo subindo na pilha de escopos (escopo atual, depois pai, etc.) */
TableEntry* find(TableNode *node, char *name);

/* Busca um símbolo **apenas** no escopo atual (não sobe para os pais) */
TableEntry* find_local(TableNode *node, char *name);

/* Imprime a tabela do escopo atual (para debug) */
void print_table(TableNode *node);

#endif
