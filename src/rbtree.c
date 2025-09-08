#include <rbtree.h>

#include <stdlib.h>

// ======================================== //
//         Implementações privadas.         //
// ======================================== //

/* Rotaçao a esquerda no nó x.
- Vizualização da rotação:
        y            x
       / \          / \
      x   T3  ==>  T1  y
     / \              / \
    T1  T2           T2  T3
*/
static void rb_rotate_left(rb_tree *tree, rb_node *x) {
  rb_node *y = x->right; // y é o filho direito de x.
  x->right =
      y->left; // A sub-árvore esquerda de y vira a sub-árvore direita de x.

  if (y->left != tree->NIL)
    y->left->parent = x; // Atualiza o pai da sub-árvore movida.

  y->parent = x->parent; // O pai de y se torna o pai de x.

  // Conecta o antigo pai de x a y.
  if (x->parent == tree->NIL) {
    tree->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;   // x se torna filho esquerdo de y.
  x->parent = y; // E o pai de x agora é y.
}

// Rotaçao a direita no nó y (o contrario da rotação à esquerda).
static void rb_rotate_right(rb_tree *tree, rb_node *y) {
  rb_node *x = y->left;
  y->left = x->right;

  if (x->right != tree->NIL)
    x->right->parent = y;

  x->parent = y->parent;

  if (y->parent == tree->NIL)
    tree->root = x;
  else if (y == y->parent->right)
    y->parent->right = x;
  else
    y->parent->left = x;

  x->right = y;
  y->parent = x;
}

// Encontra o nó com o menor valor em uma sub-árvore.
static rb_node *rb_minimum(rb_tree *tree, rb_node *node) {
  while (node->left != tree->NIL) {
    node = node->left;
  }
  return node;
}

// Busca por um nó com um valor especificado.
static rb_node *rb_find_node(rb_tree *tree, rb_node *node, void *value) {
  while (node != tree->NIL) {
    int cmp = tree->function_compare(value, node->value);
    if (cmp < 0) {
      node = node->left;
    } else if (cmp > 0) {
      node = node->right;
    } else {
      return node; // Encontrado.
    }
  }
  return tree->NIL; // Não encontrado.
}

// Função auxiliar que substitui a sub-árvore `u` pela sub-árvore `v`.
static void rb_transplant(rb_tree *tree, rb_node *u, rb_node *v) {
  if (u->parent == tree->NIL)
    tree->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

// Restaura as propriedades da árvore Rubro-Negra após a inserção de `z`.
static void rb_insert_fixup(rb_tree *tree, rb_node *z) {
  // O loop continua enquanto o pai de z for Vermelho (violando a Regra 4).
  while (z->parent->color == RB_RED) {
    // Se o pai de z é um filho ESQUERDO do avô
    if (z->parent == z->parent->parent->left) {
      rb_node *y = z->parent->parent->right; // y é o TIO de z.

      // CASO 1: O tio é VERMELHO.
      if (y->color == RB_RED) {
        z->parent->color = RB_BLACK;       // Pai -> Preto
        y->color = RB_BLACK;               // Tio -> Preto
        z->parent->parent->color = RB_RED; // Avo -> Vermelho
        z = z->parent->parent;             // Volta o foco para o avo
      } else {
        // CASO 2: O tio é PRETO (e z é um filho DIREITO).
        if (z == z->parent->right) {
          z = z->parent; // Move o foco para o pai.
          rb_rotate_left(
              tree, z); // Rotaciona para transformar em um caso "linha reta".
        }
        // CASO 3: O tio é PRETO (e z é um filho ESQUERDO).
        z->parent->color = RB_BLACK;              // Pai -> Preto
        z->parent->parent->color = RB_RED;        // Avô -> Vermelho
        rb_rotate_right(tree, z->parent->parent); // Rotaciona o avô.
      }
    } else { // Se o pai de z é um filho DIREITO do avô (código espelhado).
      rb_node *y = z->parent->parent->left; // y é o TIO de z.

      // CASO 1: O tio é VERMELHO.
      if (y->color == RB_RED) {
        z->parent->color = RB_BLACK;
        y->color = RB_BLACK;
        z->parent->parent->color = RB_RED;
        z = z->parent->parent;
      } else {
        // CASO 2: O tio é PRETO (e z é um filho ESQUERDO).
        if (z == z->parent->left) {
          z = z->parent;
          rb_rotate_right(tree, z);
        }
        // CASO 3: O tio é PRETO (e z é um filho DIREITO).
        z->parent->color = RB_BLACK;
        z->parent->parent->color = RB_RED;
        rb_rotate_left(tree, z->parent->parent);
      }
    }
  }
  // Garante que a raiz seja sempre Preta (Regra 2).
  tree->root->color = RB_BLACK;
}

// Restaura as propriedades da árvore binaria Rubro-Negra após uma remoção.
static void rb_remove_fixup(rb_tree *tree, rb_node *x) {
  // O loop continua enquanto `x` for "Preto-Duplo" e não for a raiz.
  while (x != tree->root && x->color == RB_BLACK) {
    // Se x é um filho esquerdo.
    if (x == x->parent->left) {
      rb_node *w = x->parent->right; // w é o IRMÃO de x.

      // CASO 1: O irmão é vermelho.
      if (w->color == RB_RED) {
        w->color = RB_BLACK;
        x->parent->color = RB_RED;
        rb_rotate_left(tree, x->parent);
        w = x->parent->right;
      }
      // CASO 2: O irmão é preto e seus dois filhos são pretos.
      if (w->left->color == RB_BLACK && w->right->color == RB_BLACK) {
        w->color = RB_RED;
        x = x->parent;
      } else {
        // CASO 3: O irmão é preto, filho esquerdo vermelho, filho direito preto
        if (w->right->color == RB_BLACK) {
          w->left->color = RB_BLACK;
          w->color = RB_RED;
          rb_rotate_right(tree, w);
          w = x->parent->right;
        }
        // CASO 4: O irmão é preto, filho direito vermelho.
        w->color = x->parent->color;
        x->parent->color = RB_BLACK;
        w->right->color = RB_BLACK;
        rb_rotate_left(tree, x->parent);
        x = tree->root;
      }
    } else { // Se x é um filho direito (código espelhado).
      rb_node *w = x->parent->left; // w é o IRMÃO de x.

      // CASO 1
      if (w->color == RB_RED) {
        w->color = RB_BLACK;
        x->parent->color = RB_RED;
        rb_rotate_right(tree, x->parent);
        w = x->parent->left;
      }
      // CASO 2
      if (w->right->color == RB_BLACK && w->left->color == RB_BLACK) {
        w->color = RB_RED;
        x = x->parent;
      } else {
        // CASO 3
        if (w->left->color == RB_BLACK) {
          w->right->color = RB_BLACK;
          w->color = RB_RED;
          rb_rotate_left(tree, w);
          w = x->parent->left;
        }
        // CASO 4
        w->color = x->parent->color;
        x->parent->color = RB_BLACK;
        w->left->color = RB_BLACK;
        rb_rotate_right(tree, x->parent);
        x = tree->root;
      }
    }
  }
  x->color = RB_BLACK;
}

// Função recursiva para destruir todos os nós.
static void rb_destroy_recursive(rb_tree *tree, rb_node *node) {
  if (node != tree->NIL) {
    rb_destroy_recursive(tree, node->left);
    rb_destroy_recursive(tree, node->right);
    tree->function_destroy(node->value);
    free(node);
  }
}

rb_tree *rb_create_tree(rb_function_compare compare, rb_function_copy copy,
                   rb_function_destroy destroy) {
  rb_tree *tree = (rb_tree *)malloc(sizeof(rb_tree));
  tree->function_compare = compare;
  tree->function_copy = copy;
  tree->function_destroy = destroy;
  tree->size = 0;

  // Aloca o nó NIL (sentinela).
  tree->NIL = (rb_node *)malloc(sizeof(rb_node));
  tree->NIL->color = RB_BLACK; // O sentinela é sempre Preto.
  tree->NIL->parent = tree->NIL;
  tree->NIL->left = tree->NIL;
  tree->NIL->right = tree->NIL;

  // A raiz inicialmente aponta para o NIL (sentinela).
  tree->root = tree->NIL;

  return tree;
}

// ======================================== //
//         Implementações publicas.         //
// ======================================== //

void rb_clear(rb_tree *tree) {
  if (tree == NULL || tree->root == tree->NIL)
    return; 

  // Chama a função recursiva para liberar todos os nossos nós
  rb_destroy_recursive(tree, tree->root);

  tree->root = tree->NIL;
  tree->size = 0;
}

void rb_destroy_tree(rb_tree *tree) {
  if (tree == NULL)
    return;
  rb_destroy_recursive(tree, tree->root);
  free(tree->NIL); // Libera o sentinela.
  free(tree);      // Libera a estrutura da árvore.
}

int rb_insert(rb_tree *tree, void *value) {
  int rs;
  // Cria o novo nó.
  rb_node *z = (rb_node *)malloc(sizeof(rb_node));
  z->value = tree->function_copy(value);
  z->left = tree->NIL;
  z->right = tree->NIL;
  z->color = RB_RED; // Todos os novos nós sempre são vermelhos.

  // Acha a posição correta na árvore para inserir (lógica da arvore binaria
  // padrão (iterativa)).
  rb_node *y = tree->NIL;
  rb_node *x = tree->root;
  while (x != tree->NIL) {
    y = x;
    rs = tree->function_compare(z->value, x->value);
    if (rs < 0) {
      x = x->left;
    } else if (rs > 0) {
      x = x->right;
    } else {
      // Valor duplicado. Aborta a inserção.
      tree->function_destroy(z->value);
      free(z);
      return 0;
    }
  }

  // Conecta o novo nó `z` ao seu pai `y`.
  z->parent = y;
  if (y == tree->NIL) {
    tree->root = z; // Árvore estava vazia.
  } else if (tree->function_compare(z->value, y->value) < 0) {
    y->left = z;
  } else {
    y->right = z;
  }

  // Aumenta o tamanho e chama a função de correção.
  tree->size++;
  rb_insert_fixup(tree, z);
  return 1;
}

int rb_remove(rb_tree *tree, void *value) {
  rb_node *z = rb_find_node(tree, tree->root, value);
  if (z == tree->NIL)
    return 0; // Nó não encontrado.
  
  rb_node *y = z; // y é o nó que será fisicamente removido.
  rb_node *x;     // x é o filho que tomará o lugar de y.
  int y_original_color = y->color;

  if (z->left == tree->NIL) {
    x = z->right;
    rb_transplant(tree, z, z->right);
  } else if (z->right == tree->NIL) {
    x = z->left;
    rb_transplant(tree, z, z->left);
  } else {
    y = rb_minimum(tree, z->right); // y é o sucessor de z.
    y_original_color = y->color;
    x = y->right;

    if (y->parent == z) {
      x->parent = y;
    } else {
      rb_transplant(tree, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    rb_transplant(tree, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  // Libera a memória do nó removido.
  tree->function_destroy(z->value);
  free(z);
  tree->size--;

  // Se o nó removido era Preto, a árvore pode estar desbalanceada.
  if (y_original_color == RB_BLACK) 
    rb_remove_fixup(tree, x);

  return 1;
}

void *rb_search(rb_tree *tree, void *value) {
  if (tree == NULL)
    return NULL;
  rb_node *node = rb_find_node(tree, tree->root, value);
  if (node != tree->NIL) {
    return node->value;
  }
  return NULL;
}

unsigned int rb_size(rb_tree *tree) {
  if (tree != NULL) {
    return tree->size;
  }
  return 0;
}