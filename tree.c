#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


/* Simplified structure of node. Key is int, key represent also data etc... */
typedef struct node
{
    uint32_t key;
    struct node *left;
    struct node *right;
} node_t;


typedef struct tree
{
    node_t *root;
} tree_t;


tree_t *    tree_init();
tree_t *    tree_init_with_array(uint32_t *, size_t);
void        tree_fini(tree_t *);
void        tree_insert(tree_t *, node_t *);
void        tree_insert_key(tree_t *, uint32_t);
void        tree_delete_key(tree_t *, uint32_t);


/*
 * Alloc and initialize main structure of tree and return it.
 */
tree_t *
tree_init()
{
    tree_t *tree;
    
    tree = (tree_t *) calloc(1, sizeof(tree_t));
    assert(tree);
    
    return tree;
}


/*
 * Initialize tree with array.
 */
tree_t *
tree_init_with_array(uint32_t *array, size_t size)
{
    tree_t *tree;
    size_t idx;
    
    tree = tree_init();
    
    for (idx = 0; idx < size; idx++)
        tree_insert_key(tree, array[idx]);
        
    return tree;
}


/*
 * Clean up main tree structure.
 */
void
tree_fini(tree_t *tree)
{
    
}


/*
 * Insert node
 */
void
tree_insert(tree_t *tree, node_t *node)
{
    printf("inserted %d\n", node->key);
}


/*
 * Wrapper of tree_insert() - we have only key in a node_t therefore we insert
 * only a key into the tree.
 */
void
tree_insert_key(tree_t *tree, uint32_t key)
{
    node_t *node;
    
    node = (node_t *) calloc(1, sizeof(node_t));
    assert(node);
    
    node->key = key;
    tree_insert(tree, node);
}


/*
 * Delete a node of key.
 */
void
tree_delete_key(tree_t *tree, uint32_t key)
{
    
}





int 
main(int argc, char *argv[])
{
    tree_t *tree;
    uint32_t input1[] = {1,2,3,7,6,5,4,0,9,8,10,11};
    
    tree_init_with_array(input1, 12);
    
    return 0;
}