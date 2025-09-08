#ifndef RBTREE_H
#define RBTREE_H

#define RB_RED 1
#define RB_BLACK 0

typedef int (*rb_function_compare)(void *, void *);
typedef void *(*rb_function_copy)(void *);
typedef void (*rb_function_destroy)(void*);

// Estrutura de no da arvore rubro-negra
typedef struct rb_node {
    void* value;
    int color; // RB_RED (1) e RB_BLACK (0)

    struct rb_node* parent;
    struct rb_node* right;
    struct rb_node* left;
} rb_node;

// Estrutura da arvore binaria rubro-negra.
typedef struct rb_tree {
    rb_node* root;
    unsigned int size;

    // Substituto para as verificaçoes (node == NULL).
    // Simplifica demais a implementaçao da árvore rubro-negra.
    rb_node* NIL;

    // Função de cópia, para copiar o conteudo dos elementos.
    rb_function_copy function_copy;
    // O programador define como comparar os elementos dentro da arvore.
    rb_function_compare function_compare;
    // E necessario destruir a copia dos elementos copiados.
    rb_function_destroy function_destroy;
} rb_tree;

// Cria uma arvore para a Rubro-Negra.
rb_tree* rb_create_tree(rb_function_compare, rb_function_copy, rb_function_destroy);

// Limpa toda a arvore rubro-negra.
void rb_clear(rb_tree*);

// Destroi a arvore rubro-negra.
void rb_destroy_tree(rb_tree*);

// Insere um elemento na arvore rubro-negra. (nao aceita duplicatas)
int rb_insert(rb_tree*, void* value);

// Remove um elemento, se existir, da arvore rubro-negra.
int rb_remove(rb_tree*, void* value);

// Busca por um elemento, se existir, da arvore rubro-negra.
void* rb_search(rb_tree*, void* value);

// Retorna a quantidade de elementos da arvore rubro-negra.
unsigned int rb_size(rb_tree*);

#endif