#ifndef AVLTREE_H
#define AVLTREE_H

// Função de comparação para os tipos que a árvore vai processar.
/*
  Essa função vai retornar:
  0: caso os dois elementos sejam IGUAIS.
  1: se primeiro parâmetro for MAIOR que o segundo parâmetro.
  -1: se o primeiro parâmetro for MENOR que o segundo parâmetro.
*/
typedef int (*avl_function_compare)(void *, void *);
// Função de cópia para o tipo, que os nós da árvore binária vão usar.
typedef void *(*avl_function_copy)(void *);
// Função de destruir da memória alocada do tipo (Necessário por conta da
// cópia).
typedef void (*avl_function_destroy)(void *);

// Estruutra do nó da árvore binária balanceada (AVL).
typedef struct avl_node {
  void *value; // Valor (Tipo definido e retornado pelo programador através de
               // casts)
  int height;

  struct avl_node *right; // Nó à direita (maior)
  struct avl_node *left;  // Nó à esquerda (menor)
} avl_node;

// Estrutura da árvore binária balanceada (AVL).
typedef struct {
  avl_node *root;    // Raiz da árvore
  unsigned int size;

  avl_function_compare function_compare; // Função de comparação.
  avl_function_copy function_copy;       // Função de copia de memória.
  avl_function_destroy function_destroy; // Função de destruir cópia da memória.

} avl_tree;

// Cria um ponteiro para a nova árvore criada.
avl_tree *avl_create_tree(avl_function_compare, avl_function_copy,
                          avl_function_destroy);

// Limpa todos os dados armazenados na árvore.
void avl_clear(avl_tree *);

// Destruí a árvore desalocando todos os nós.
void avl_destroy_tree(avl_tree *);

// Insere um elemento na árvore, aplicando balanceamento se necessário.
int avl_insert(avl_tree *tree, void *value);

// Remove um elemento na árvore, aplicando balanceamento se necessário.
int avl_remove(avl_tree *tree, void *value);

// Busca por um elemento dentro da árvore.
void *avl_search(avl_tree *tree, void *value);

// Retorna a quantidade de nós totais que a árvore possuí.
unsigned int avl_size(avl_tree *);

#endif