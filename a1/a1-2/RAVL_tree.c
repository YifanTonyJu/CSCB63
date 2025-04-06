/*
 *  RAVL (augmented with Rank AVL) tree implementation.
 *  Author (starter code): Anya Tafliovich.
 *  Based on materials developed by F. Estrada.
 */

#include "RAVL_tree.h"

/*************************************************************************
 ** Suggested helper functions
 *************************************************************************/

/* Returns the height (number of nodes on the longest root-to-leaf path) of
 * the tree rooted at node 'node'. Returns 0 if 'node' is NULL.  Note: this
 * should be an O(1) operation.
 */
int height(RAVL_Node* node) {
  if (node == NULL) {
    return 0;
  }
  return node->height;
}

/* Returns the size (number of nodes) of the tree rooted at node 'node'.
 * Returns 0 if 'node' is NULL.  Note: this should be an O(1) operation.
 */
int size(RAVL_Node* node) {
  if (node == NULL) {
    return 0;
  }
  return node->size;
}

/* Updates the height of the tree rooted at node 'node' based on the heights
 * of its children. Note: this should be an O(1) operation.
 */
void updateHeight(RAVL_Node* node) {
  if (node == NULL) {
    return;
  }
  int lch = height(node->left);
  int rch = height(node->right);
  if (lch > rch) {
    node->height = lch + 1;
  } else {
    node->height = rch + 1;
  }
}

/* Updates the size of the tree rooted at node 'node' based on the sizes
 * of its children. Note: this should be an O(1) operation.
 */
void updateSize(RAVL_Node* node) {
  if (node == NULL) {
    return;
  }
  int lcs = size(node->left);
  int rcs = size(node->right);
  node->size = lcs + rcs + 1;
}

/* Returns the balance factor (height of left subtree - height of right
 * subtree) of node 'node'. Returns 0 if node is NULL.  Note: this should be
 * an O(1) operation.
 */
int balanceFactor(RAVL_Node* node) {
  if (node == NULL) {
    return 0;
  }
  return height(node->left) - height(node->right);
}

/* Returns the result of performing the corresponding rotation in the RAVL
 * tree rooted at 'node'.
 */
// single rotations: right/clockwise
RAVL_Node* rightRotation(RAVL_Node* node) {
  if (node == NULL || node->left == NULL) {
    return node;
  }

  RAVL_Node* xnode = node->left;
  RAVL_Node* bnode = xnode->right;

  xnode->right = node;
  node->left = bnode;

  updateHeight(node);
  updateSize(node);
  updateHeight(xnode);
  updateSize(xnode);

  return xnode;
}
// single rotations: left/counter-clockwise
RAVL_Node* leftRotation(RAVL_Node* node) {
  if (node == NULL || node->right == NULL) {
    return node;
  }

  RAVL_Node* xnode = node->right;
  RAVL_Node* bnode = xnode->left;

  xnode->left = node;
  node->right = bnode;

  updateHeight(node);
  updateSize(node);
  updateHeight(xnode);
  updateSize(xnode);

  return xnode;
}
// double rotation: right/clockwise then left/counter-clockwise
RAVL_Node* rightLeftRotation(RAVL_Node* node) {
  if (node == NULL || node->right == NULL) {
    return node;
  }

  RAVL_Node* xnode = node->right;
  RAVL_Node* ynode = xnode->left;

  if (ynode == NULL) {
    return node;
  }

  xnode->left = ynode->right;
  ynode->right = xnode;

  node->right = ynode->left;
  ynode->left = node;

  updateHeight(xnode);
  updateSize(xnode);
  updateHeight(node);
  updateSize(node);
  updateHeight(ynode);
  updateSize(ynode);

  return ynode;
}
// double rotation: left/counter-clockwise then right/clockwise
RAVL_Node* leftRightRotation(RAVL_Node* node) {
  if (node == NULL || node->left == NULL) {
    return node;
  }

  RAVL_Node* xnode = node->left;
  RAVL_Node* ynode = xnode->right;
  if (ynode == NULL) {
    return node;
  }

  xnode->right = ynode->left;
  ynode->left = xnode;

  node->left = ynode->right;
  ynode->right = node;

  updateHeight(xnode);
  updateSize(xnode);
  updateHeight(node);
  updateSize(node);
  updateHeight(ynode);
  updateSize(ynode);

  return ynode;
}

/* Returns the successor node of 'node'. */
RAVL_Node* successor(RAVL_Node* node) {
  if (node == NULL || node->right == NULL) {
    return NULL;
  }

  RAVL_Node* suc = node->right;
  while (suc->left != NULL) {
    suc = suc->left;
  }
  return suc;
}

/* Creates and returns an RAVL tree node with key 'key', value 'value', height
 * and size of 1, and left and right subtrees NULL.
 */
RAVL_Node* createNode(int key, void* value) {
  RAVL_Node* newNode = (RAVL_Node*)malloc(sizeof(RAVL_Node));
  newNode->key = key;
  newNode->value = value;
  newNode->height = 1;
  newNode->size = 1;
  newNode->left = NULL;
  newNode->right = NULL;
  return newNode;
}

/**
 * Rebalance the 'node' and returns the pointer to the root of the new tree
 */
RAVL_Node* rebalance(RAVL_Node* node) {
  if (node == NULL) {
    return node;
  }

  int bafa = balanceFactor(node);

  if (bafa > 1) {
    RAVL_Node* xnode = node->left;
    if (height(xnode->left) >= height(xnode->right)) {
      return rightRotation(node);
    }
    return leftRightRotation(node);
  }

  if (bafa < -1) {
    RAVL_Node* xnode = node->right;

    if (height(xnode->left) <= height(xnode->right)) {
      return leftRotation(node);
    }
    return rightLeftRotation(node);
  }
  return node;
}

/*************************************************************************
 ** Provided functions
 *************************************************************************/

void printTreeInorder_(RAVL_Node* node, int offset) {
  if (node == NULL) {return;}
  printTreeInorder_(node->right, offset + 1);
  printf("%*s %d [%d / %d]\n", offset, "", node->key, node->height, node->size);
  printTreeInorder_(node->left, offset + 1);
}

void printTreeInorder(RAVL_Node* node) { printTreeInorder_(node, 0); }

void deleteTree(RAVL_Node* node) {
  if (node == NULL) {return;}
  deleteTree(node->left);
  deleteTree(node->right);
  free(node);
}

/*************************************************************************
 ** Required functions
 ** Must run in O(log n) where n is the number of nodes in a tree rooted
 **  at 'node'.
 *************************************************************************/

RAVL_Node* search(RAVL_Node* node, int key) {
  if (node == NULL) {
    return NULL;
  }
  if (node->key == key) {
    return node;
  }

  if (key < node->key) {
    return search(node->left, key);
  }
  return search(node->right, key);
}

RAVL_Node* insert(RAVL_Node* node, int key, void* value) {
  if (node == NULL) {
    return createNode(key, value);
  }
  if (node->key == key) {
    node->value = value;
    return node;
  }
  if (key < node->key) {
    node->left = insert(node->left, key, value);
  } else {
    node->right = insert(node->right, key, value);
  }

  updateHeight(node);
  updateSize(node);

  return rebalance(node);
}

RAVL_Node* delete(RAVL_Node* node, int key) {
  if (node == NULL) {
    return NULL;
  }
  if (node->key == key) {
    if (node->left == NULL) {
      RAVL_Node* root = node->right;
      free(node);
      return root;
    }
    if (node->right == NULL) {
      RAVL_Node* root = node->left;
      free(node);
      return root;
    }
    RAVL_Node* suc = successor(node);
    node->key = suc->key;
    node->value = suc->value;
    node->right = delete (node->right, suc->key);
  }

  else if (key < node->key) {
    node->left = delete (node->left, key);
  } else {
    node->right = delete (node->right, key);
  }

  updateHeight(node);
  updateSize(node);

  return rebalance(node);
}

int rank(RAVL_Node* node, int key) {
  if (node == NULL || search(node, key) == NULL) {
    return NOTIN;
  }
  if (node->key == key) {
    return size(node->left) + 1;
  }
  if (node->key < key) {
    return size(node->left) + 1 + rank(node->right, key);
  }
  return rank(node->left, key);
}

RAVL_Node* findRank(RAVL_Node* node, int rank) {
  if (node == NULL || rank <= 0) {
    return NULL;
  }
  int rank_root = size(node->left) + 1;
  if (rank == rank_root) {
    return node;
  }
  if (rank_root > rank) {
    return findRank(node->left, rank);
  }
  return findRank(node->right, rank - rank_root);
}
