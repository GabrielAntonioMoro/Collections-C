#include <avltree.h>

#include <stdlib.h>
#include <stddef.h>

// ======================================== //
//         Implementações privadas.         //
// ======================================== //

static avl_node *avl_biggest_node(avl_node *node) {
  if (node->right != NULL)
    return avl_biggest_node(node->right);
  return node;
}

static avl_node *avl_smallest_node(avl_node *node) {
  if (node->left != NULL)
    return avl_smallest_node(node->left);
  return node;
}

// Compara dois valores, e retorna o maior
static int avl_max(int a, int b) { return (a > b) ? a : b; }

// Retorna a altura do nó.
int avl_get_height(avl_node *a) {
  if (a == NULL)
    return 0;

  return a->height;
}

// Retorna o fator de balanço do nó.
static int avl_get_balance(avl_node *a) {
  if (a == NULL)
    return 0;
  return avl_get_height(a->left) - avl_get_height(a->right);
}

/*
      a (-2)                        b (0)
     / \                           / \
    y   b (-1)     ====>      (0) a   z
       / \                       / \
      c  z                      y   c
*/
// Faz uma rotação a esquerda (--)
static avl_node *avl_rotate_left(avl_node *a) {
  avl_node *b = a->right;
  avl_node *c = b->left;

  b->left = a;
  a->right = c;

  b->height = 1 + avl_max(avl_get_height(b->left), avl_get_height(b->right));
  a->height = 1 + avl_max(avl_get_height(a->left), avl_get_height(a->right));

  return b;
}

// Faz uma rotação a direita (++)
static avl_node *avl_rotate_right(avl_node *a) {
  avl_node *b = a->left;
  avl_node *c = b->right;

  b->right = a;
  a->left = c;

  a->height = 1 + avl_max(avl_get_height(a->left), avl_get_height(a->right));
  b->height = 1 + avl_max(avl_get_height(b->left), avl_get_height(b->right));

  return b;
}

// Cria um nó para a árvore avl.
static avl_node *avl_create_node(avl_tree *tree, void *value) {
  avl_node *node = (avl_node *)malloc(sizeof(avl_node));

  node->value = tree->function_copy(value);
  node->height = 1;
  node->left = NULL;
  node->right = NULL;

  tree->size++;

  return node;
}

// Destroi o nó criado anteriormente para a árvore.
static void avl_destroy_node(avl_tree *tree, avl_node *node) {
  tree->function_destroy(node->value);
  free(node);
}

// Faz uma inserção recursiva na árvore.
static avl_node *avl_impl_insert(avl_tree *tree, avl_node *node, void *value) {
  if (node == NULL)
    return avl_create_node(
        tree,
        value); // Retornamos o nó se achamos uma posição para adicionar o nó.

  int r = tree->function_compare(value, node->value);

  if (r > 0)
    node->right = avl_impl_insert(tree, node->right, value);
  else if (r < 0)
    node->left = avl_impl_insert(tree, node->left, value);
  else
    return NULL;

  node->height =
      1 + avl_max(avl_get_height(node->left), avl_get_height(node->right));
  int balance = avl_get_balance(node);

  if (balance > 1 && tree->function_compare(value, node->left->value) < 0)
    return avl_rotate_right(node);

  if (balance < -1 && tree->function_compare(value, node->right->value) > 0)
    return avl_rotate_left(node);

  if (balance > 1 && tree->function_compare(value, node->left->value) > 0) {
    node->left = avl_rotate_left(node->left);
    return avl_rotate_right(node);
  }

  if (balance < -1 && tree->function_compare(value, node->right->value) < 0) {
    node->right = avl_rotate_right(node->right);
    return avl_rotate_left(node);
  }

  return node;
}

// Faz uma remoção recursiva na árvore.
static avl_node *avl_impl_remove(avl_tree *tree, avl_node *node, void *value) {
  if (node == NULL)
    return NULL;

  int r = tree->function_compare(value, node->value);
  if (r > 0)
    node->right = avl_impl_remove(tree, node->right, value);
  else if (r < 0)
    node->left = avl_impl_remove(tree, node->left, value);
  else {
    if (node->left != NULL &&
        node->right != NULL) { // Se o nosso nó tiver os dois filhos.

      avl_node *sucessor = avl_smallest_node(node->right); // Sucessor

      void *value_temp = node->value;

      node->value = sucessor->value;
      sucessor->value = value_temp;

      node->right = avl_impl_remove(tree, node->right, value_temp);

    } else { // Se o nosso nó tiver um ou nenhum filho.
      avl_node *tmp = node;
      avl_node *filho = (node->left != NULL) ? node->left : node->right;

      tree->function_destroy(tmp->value);
      free(tmp);
      --tree->size;

      return filho;
    }
  }

  if (node == NULL)
    return node;

  node->height =
      1 + avl_max(avl_get_height(node->left), avl_get_height(node->right));
  int balance = avl_get_balance(node);

  // Caso Esquerda (Simples).
  if (balance > 1 && avl_get_balance(node->left) >= 0)
    return avl_rotate_right(node);

  // Caso Esquerda-Direita (Dupla)
  if (balance > 1 && avl_get_balance(node->left) < 0) {
    node->left = avl_rotate_left(node->left);
    return avl_rotate_right(node);
  }

  // Caso Direita (Simples).
  if (balance < -1 && avl_get_balance(node->right) <= 0)
    return avl_rotate_left(node);

  if (balance < -1 && avl_get_balance(node->right) > 0) {
    node->right = avl_rotate_right(node->right);
    return avl_rotate_left(node);
  }

  return node;
}

// Faz uma pesquisa recursiva na árvore.
static avl_node *avl_impl_search(avl_tree *tree, avl_node *node, void *value) {
  if (node == NULL)
    return NULL; // Não encontrado.

  int r = tree->function_compare(value, node->value);

  if (r > 0)
    return avl_impl_search(tree, node->right, value);
  else if (r < 0)
    return avl_impl_search(tree, node->left, value);
  else
    return node;
}

// Função recursiva para a deleção dos nós.
static void avl_impl_clear(avl_tree *tree, avl_node *node) {
  if (node == NULL)
    return;

  avl_impl_clear(tree, node->left);
  avl_impl_clear(tree, node->right);

  avl_destroy_node(tree, node);
  
}

// ======================================== //
//         Implementações publicas.         //
// ======================================== //

avl_tree *avl_create_tree(avl_function_compare fcompare,
                          avl_function_copy fcopy,
                          avl_function_destroy fdestroy) {
  avl_tree *tree = (avl_tree *)malloc(sizeof(avl_tree));

  tree->function_compare = fcompare;
  tree->function_copy = fcopy;
  tree->function_destroy = fdestroy;
  tree->root = NULL;
  tree->size = 0;

  return tree;
}

void avl_clear(avl_tree *tree) {
  if (tree != NULL) {
    avl_impl_clear(tree, tree->root);
    tree->root = NULL;
    tree->size = 0;
  }
}

void avl_destroy_tree(avl_tree *tree) {
  if (tree == NULL)
    return;

  avl_clear(tree);
  free(tree);
}

int avl_insert(avl_tree *tree, void *value) {
  if (tree == NULL)
    return 0;

  unsigned int old_size = tree->size;
  tree->root = avl_impl_insert(tree, tree->root, value);

  if (tree->size > old_size)
    return 1; // Sucesso na inserção.

  return 0; // Duplicata encontrada.
}

int avl_remove(avl_tree *tree, void *value) {
  if (tree == NULL || tree->root == NULL)
    return 0;

  unsigned int old_size = tree->size;
  tree->root = avl_impl_remove(tree, tree->root, value);
  if (tree->size > old_size)
    return 1; // Sucesso na remoção.
  
  return 0; // Elemento não encontrado na árvore.
}

void *avl_search(avl_tree *tree, void *value) {
  if (tree == NULL)
    return NULL;

  avl_node *node = avl_impl_search(tree, tree->root, value);
  if (node != NULL)
    return node->value;
  return NULL;
}

unsigned int avl_size(avl_tree *tree) {
  if (tree != NULL)
    return tree->size;
  return 0;
}
