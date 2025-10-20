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

void delete_sub_tree(node_t * node) {
    if (node == NULL) return;

    delete_sub_tree(node->left);
    delete_sub_tree(node->right);
    free(node);
}

void delete_rbtree(rbtree *t) {
    if (t->root == NULL) return;

    delete_sub_tree(t->root);
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

    rebalanceAfterInsertion(t, new_node);

    return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t *cur = t->root;
    while (cur != NULL) {
        if (key < cur->key) {
            cur = cur->left;
        } else if (key > cur->key) {
            cur = cur->right;
        } else {
            return cur;
        }
    }
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
    if (t->root == NULL) return NULL;

    node_t *cur = t->root;
    while (cur->left != NULL)
        cur = cur->left;

    return cur;
}

node_t *rbtree_max(const rbtree *t) {
    if (t->root == NULL) return NULL;

    node_t *cur = t->root;
    while (cur->right != NULL)
        cur = cur->right;

    return cur;
}

int rbtree_erase(rbtree *t, node_t *p) {
    if (t == NULL || p == NULL || t->root == NULL) {
        return 0;
    }

    node_t *find_node = rbtree_find(t, p->key);
    if (find_node == NULL) {
        return 0;
    }

    node_t *removed_node = NULL;
    node_t *replacement_node = NULL;
    node_t *parent_of_replacement = NULL;
    color_t removed_color;

    int is_root_deleted = find_node == t->root;

    // Case 1: 자녀가 하나 혹은 없을 경우
    if (find_node->left == NULL || find_node->right == NULL) {
        removed_color = find_node->color;
        parent_of_replacement = find_node->parent;
        replacement_node = (find_node->left != NULL) ? find_node->left : find_node->right;

        removed_node = remove_node_with_one_or_zero(&find_node);

        if (is_root_deleted) {
            t->root = find_node;
        }
    }
    // Case 2: 자녀가 두 개일 경우
    else {
        node_t *successor = find_successor(find_node);

        removed_color = successor->color;

        replacement_node = successor->right;
        parent_of_replacement = successor->parent;

        if (parent_of_replacement == find_node) {
             parent_of_replacement = successor;
        }

        removed_node = remove_node_with_one_or_zero(&successor);

        find_node->key = successor->key;
    }

    if (removed_color == RBTREE_BLACK) {
        rebalanceAfterDeletion(t, replacement_node, parent_of_replacement);
    }

    if (removed_node != NULL) {
        free(removed_node);
    }

    return 1;
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

void rebalanceAfterInsertion(rbtree *t, node_t *node) {
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
        rebalanceAfterInsertion(t, grandParent);
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

node_t* remove_node_with_one_or_zero(node_t ** node) {
    if (node == NULL || *node == NULL) {
        return NULL;
    }

    node_t *original_node_to_free = *node;
    node_t *parent = original_node_to_free->parent;
    node_t *child = (original_node_to_free->left != NULL) ? original_node_to_free->left : original_node_to_free->right;

    if (child != NULL) {
        child->parent = parent;
    }

    if (parent == NULL) {
        *node = child;
    } else {
        if (parent->left == original_node_to_free) {
            parent->left = child;
        } else {
            parent->right = child;
        }
    }

    return original_node_to_free;
}

node_t * find_successor(node_t * node) {
    if (node == NULL || node->right == NULL) {
        return NULL;
    }

    node_t *cur = node->right;
    while (cur->left != NULL) {
        cur = cur->left;
    }
    return cur;
}

void rebalanceAfterDeletion(rbtree *t, node_t *node, node_t *parent) {
    node_t *sibling;

    // node가 루트가 아니고, "doubly black" 속성을 가질 동안 반복
    while (node != t->root && (node == NULL || node->color == RBTREE_BLACK)) {
        // node가 NULL이거나 parent가 없으면 더 이상 진행할 수 없습니다.
        if (parent == NULL) break;

        // Case: node가 왼쪽 자식인 경우
        if (node == parent->left) {
            sibling = parent->right;

            // Case 1: 형제 sibling가 RED
            if (sibling != NULL && sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                parent->color = RBTREE_RED;

                node_t **ptr_to_parent = (parent->parent == NULL) ? &(t->root)
                                             : (parent == parent->parent->left ? &(parent->parent->left) : &(parent->parent->right));
                left_rotate(ptr_to_parent);
                sibling = parent->right; // 회전 후 형제가 바뀌었으므로 갱신
            }

            // Case 2: 형제 sibling가 BLACK이고, sibling의 두 자식이 모두 BLACK
            if (sibling == NULL || ((sibling->left == NULL || sibling->left->color == RBTREE_BLACK) &&
                              (sibling->right == NULL || sibling->right->color == RBTREE_BLACK))) {
                if (sibling != NULL) sibling->color = RBTREE_RED;
                node = parent; // 문제를 부모로 전가
                parent = node->parent;
                continue; // 다음 루프에서 처리
            }

            // Case 3: 형제 sibling이 BLACK, sibling의 왼쪽 자식이 RED, 오른쪽 자식이 BLACK
            if (sibling->right == NULL || sibling->right->color == RBTREE_BLACK) {
                if (sibling->left != NULL) sibling->left->color = RBTREE_BLACK;
                sibling->color = RBTREE_RED;
                right_rotate(&(parent->right)); // sibling는 parent의 오른쪽 자식이므로 &(parent->right)를 전달
                sibling = parent->right; // 회전 후 형제가 바뀌었으므로 갱신
            }

            // Case 4: 형제 node가 BLACK, node의 오른쪽 자식이 RED
            sibling->color = parent->color;
            parent->color = RBTREE_BLACK;
            if (sibling->right != NULL) sibling->right->color = RBTREE_BLACK;

            node_t **ptr_to_parent = (parent->parent == NULL) ? &(t->root)
                                    : (parent == parent->parent->left ? &(parent->parent->left) : &(parent->parent->right));
            left_rotate(ptr_to_parent);

            node = t->root; // 모든 문제가 해결되었으므로 루프 종료

        } else { // Case: node가 오른쪽 자식인 경우
            sibling = parent->left;

            // Case 1: 형제 sibling가 RED
            if (sibling != NULL && sibling->color == RBTREE_RED) {
                sibling->color = RBTREE_BLACK;
                parent->color = RBTREE_RED;

                node_t **ptr_to_parent = (parent->parent == NULL) ? &(t->root)
                                             : (parent == parent->parent->left ? &(parent->parent->left) : &(parent->parent->right));
                right_rotate(ptr_to_parent);
                sibling = parent->left;
            }

            // Case 2: 형제 sibling가 BLACK이고, sibling의 두 자식이 모두 BLACK
            if (sibling == NULL || ((sibling->left == NULL || sibling->left->color == RBTREE_BLACK) &&
                              (sibling->right == NULL || sibling->right->color == RBTREE_BLACK))) {
                if (sibling != NULL) sibling->color = RBTREE_RED;
                node = parent;
                parent = node->parent;
                continue;
            }

            // Case 3: 형제 sibling가 BLACK, sibling의 오른쪽 자식이 RED, 왼쪽 자식이 BLACK
            if (sibling->left == NULL || sibling->left->color == RBTREE_BLACK) {
                if (sibling->right != NULL) sibling->right->color = RBTREE_BLACK;
                sibling->color = RBTREE_RED;
                left_rotate(&(parent->left));
                sibling = parent->left;
            }

            // Case 4: 형제 sibling가 BLACK, sibling의 왼쪽 자식이 RED
            sibling->color = parent->color;
            parent->color = RBTREE_BLACK;
            if (sibling->left != NULL) sibling->left->color = RBTREE_BLACK;

            node_t **ptr_to_parent = (parent->parent == NULL) ? &(t->root)
                                    : (parent == parent->parent->left ? &(parent->parent->left) : &(parent->parent->right));
            right_rotate(ptr_to_parent);
            node = t->root;
        }
    }

    if (node != NULL) {
        node->color = RBTREE_BLACK;
    }
}


