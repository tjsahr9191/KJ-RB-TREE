#include "rbtree.h"

#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *) calloc(1, sizeof(rbtree));
    if (p == NULL) {
        return NULL;
    }

#ifdef SENTINEL
    node_t *nil_node = (node_t *) malloc(sizeof(node_t));
    if (nil_node == NULL) {
        free(p);
        return NULL;
    }
    nil_node->color = RBTREE_BLACK;
    nil_node->parent = nil_node;
    nil_node->left = nil_node;
    nil_node->right = nil_node;

    p->nil = nil_node;
    p->root = p->nil;
#else
    p->root = NULL;
    p->nil = NULL;
#endif

    return p;
}

void delete_rbtree(rbtree *t) {
    // TODO: reclaim the tree nodes's memory
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *new_node = (node_t *) malloc(sizeof(node_t));
    memory_allocate_check(new_node);

    init_new_node(t, key, new_node);

    node_t *cur = t->root;
    node_t *parent = t->nil;
    while (cur != NULL) {
        parent = cur;
        if (key <= cur->key) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }

    new_node->parent = parent;

    if (parent == t->nil) {
        t->root = new_node;
    } else if (key < parent->key) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }

    rebalance(t, new_node);

    return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *root = t->root;
    while (root != NULL) {
        if (key < root->key) {
            root = root->left;
        } else if (key > root->key) {
            root = root->right;
        } else {
            return root;
        }
    }
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
    // TODO: implement find
    return t->root;
}

node_t *rbtree_max(const rbtree *t) {
    // TODO: implement find
    return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // TODO: implement to_array
    return 0;
}

void memory_allocate_check(node_t *new_node) {
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

void rebalance(rbtree *t, node_t *node) {
    node_t *parent = getParent(node);
    node_t *uncle = getUncle(node);
    node_t *grandParent = getParent(parent);

    if (parent == NULL) {
        node->color = RBTREE_BLACK;
        return;
    }
    if (parent->color == RBTREE_BLACK) {
        return;
    }

    if (uncle != NULL && uncle->color == RBTREE_RED && parent->color == RBTREE_RED) {
        parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        grandParent->color = RBTREE_RED;
        rebalance(t, grandParent);
        return;
    }
    if ((uncle == NULL || uncle->color == RBTREE_BLACK) && parent->color == RBTREE_RED) {
        node_t **gp_ptr = get_grand_parent_ptr(t, grandParent);

        if (grandParent->left == parent) {
            if (parent->left == node) {
                exchange_color(parent, grandParent);
                right_rotate(gp_ptr);
            } else {
                left_right_rotate(gp_ptr);
            }
        } else {
            if (parent->left == node) {
                right_left_rotate(gp_ptr);
            } else {
                exchange_color(parent, grandParent);
                left_rotate(gp_ptr);
            }
        }
    }
}

void init_new_node(rbtree *t, const key_t key, node_t *new_node) {
    if (new_node == NULL) return;

    new_node->key = key;
    new_node->color = RBTREE_RED;
    new_node->parent = t->nil;
    new_node->left = t->nil;
    new_node->right = t->nil;
}

node_t *getParent(node_t *node) {
    if (node == NULL) return NULL;
    return node->parent;
}

node_t *getUncle(node_t *node) {
    node_t *parent = getParent(node);
    if (parent == NULL) return NULL;

    node_t *grandParent = getParent(parent);
    if (grandParent == NULL) return NULL;

    if (grandParent->right == parent)
        return grandParent->left;
    return grandParent->right;
}

node_t **get_grand_parent_ptr(rbtree *t, node_t *grand_parent) {
    node_t *ggp = grand_parent->parent;

    if (ggp == t->nil)
        return &t->root;
    if (grand_parent == ggp->left)
        return &ggp->left;
    return &ggp->right;
}

void right_rotate(node_t **node) {
    node_t *cur = *node;
    node_t *child = cur->left;

    cur->left = child->right;
    if (child->right != NULL)
        child->right->parent = cur;

    child->parent = cur->parent;
    child->right = cur;
    cur->parent = child;
    *node = child;
}

void left_rotate(node_t **node) {
    node_t *cur = *node;
    node_t *child = cur->right;

    cur->right = child->left;
    if (child->left != NULL)
        child->left->parent = cur;

    child->parent = cur->parent;
    child->left = cur;
    cur->parent = child;
    *node = child;
}

void exchange_color(node_t *node1, node_t *node2) {
    color_t temp = node1->color;
    node1->color = node2->color;
    node2->color = temp;
}

void left_right_rotate(node_t **node) {
    left_rotate(&((*node)->left));
    exchange_color(*node, (*node)->left);
    right_rotate(node);
}

void right_left_rotate(node_t **node) {
    right_rotate(&((*node)->right));
    exchange_color(*node, (*node)->right);
    left_rotate(node);
}


