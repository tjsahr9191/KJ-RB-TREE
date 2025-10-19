#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);


void rebalance(rbtree *, node_t * node);
node_t * getParent(node_t * node);
node_t * getUncle(node_t * node);
void right_rotate(node_t ** node);
void left_right_rotate(node_t ** node);
void right_left_rotate(node_t ** node);
void left_rotate(node_t ** node);
void exchange_color(node_t *node1, node_t *node2);
node_t **get_grand_parent_ptr(rbtree *t, node_t *grand_parent);
void init_new_node(rbtree *t, const key_t key, node_t *new_node);
void memory_allocate_check(node_t *new_node);


#endif  // _RBTREE_H_
