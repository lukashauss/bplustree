#include "bptree.h"

struct node* insert(struct node *root, int key, int value) {
    if (root == NULL) {
        struct leafNode *newNode = (struct leafNode*) malloc(sizeof(struct leafNode));
        newNode->keys = key;
        newNode->values = value;

        return (struct node*) newNode;
    }

    //root is a leaf
    if (root->type == LEAF) {
        
    }

    //root is a innerNode
    //find index

}
