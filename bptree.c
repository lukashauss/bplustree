#include "bptree.h"
#include <stdlib.h>
#include <stdio.h>

struct leafNode* splitLeaf(struct leafNode* leaf, int key, int value) {
    //create new Leaf and insert key and value
    struct leafNode* newLeaf = (struct leafNode*) malloc(sizeof(struct leafNode));
    newLeaf->super.parent = NULL;
    newLeaf->super.type = LEAF;
    newLeaf->super.filled = 0;

    //find index for insertion
    int i = 0;
    while (i < leaf->super.filled && leaf->super.keys[i] < key)
        i++; 
    
    //reorder leaf
    int med = (MAX_NODE_SIZE+1) / 2;
    int tempK = key;
    int tempV = value;
    while (i < MAX_NODE_SIZE) {
        int t = leaf->super.keys[i];
        leaf->super.keys[i] = tempK;
        tempK = t;
        t = leaf->values[i];
        leaf->values[i] = tempV;
        tempV = t;
        i++;
    }

    //copy keys and values into new Leaf
    int j;
    for (j = 0; j+med < MAX_NODE_SIZE; j++) {
        newLeaf->super.keys[j] = leaf->super.keys[j+med];
        newLeaf->values[j] = leaf->values[j+med];
        newLeaf->super.filled++;
    }
    newLeaf->super.keys[j] = tempK;
    newLeaf->values[j] = tempV;
    newLeaf->super.filled++;

    leaf->super.filled = med;

    newLeaf->super.parent = leaf->super.parent;
    return newLeaf;
    
}

struct node* insert_node(struct node *root, struct innerNode* node, struct node* child, int key) {
    if (node == NULL)
        return root;

    //insert key and child
    if (node->super.filled < MAX_NODE_SIZE) {
        //swap key and child until key is in the right place
        node->super.keys[node->super.filled] = key;
        node->children[node->super.filled+1] = child;
        for (int i = node->super.filled; i > 0 && node->super.keys[i] < node->super.keys[i-1]; i--) {
            int t = node->super.keys[i-1];
            node->super.keys[i-1] = node->super.keys[i];
            node->super.keys[i] = t;

            struct node* temp = node->children[i+1];
            node->children[i+1] = node->children[i];
            node->children[i] = temp;
        }
        node->super.filled++;

    //node overflow
    } else {
        struct innerNode* newNode = split_node(node, child, key); 
        int medKey = node->super.keys[node->super.filled];
        //insert medKey into parent node

        //node is root
        if (node->super.parent == NULL) {
            root = (struct node*) malloc(sizeof(struct innerNode));
            root->parent = NULL;            
            root->type = INNER;
            root->keys[0] = medKey;
            root->filled = 1;
            ((struct innerNode*)root)->children[0] = (struct node*) node;
            ((struct innerNode*)root)->children[1] = (struct node*) newNode;
            node->super.parent = (struct innerNode*) root;
            newNode->super.parent = (struct innerNode*) root;
        } else {
            root = insert_node(root, node->super.parent, (struct node*) newNode, medKey);

        }
    }
    return root;
} 

struct innerNode* split_node(struct innerNode* node, struct node* child, int key) {
    
    //create new Node and insert key and child
    //child is right to the inserted key
    struct innerNode* newNode = (struct innerNode*) malloc(sizeof(struct innerNode));
    newNode->super.parent = NULL;
    newNode->super.type = INNER;
    newNode->super.filled = 0;

    //find index for insertion
    int i = 0;
    while (i < node->super.filled && node->super.keys[i] < key)
        i++; 
    
    int med = MAX_NODE_SIZE / 2;

    //reorder node
    int tempK = key;
    struct node* tempN = child;
    while (i < MAX_NODE_SIZE) {
        int t = node->super.keys[i];
        node->super.keys[i] = tempK;
        tempK = t;
        struct node* t_n = node->children[i+1];
        node->children[i+1] = tempN;
        tempN = t_n;
        i++;
    }
    //tempK / tempN contain the overflown elements
    
    node->super.filled = med;
     
    //insert into new Node
    int j;
    for (j = med+1; j < MAX_NODE_SIZE; j++) {
        newNode->super.keys[j-med-1] = node->super.keys[j];
        newNode->children[j-med-1] = node->children[j];
        newNode->children[j-med-1]->parent = newNode;

        newNode->super.filled++;
    }
    newNode->super.keys[j-med-1] = tempK;
    newNode->children[j-med-1] = node->children[j];
    newNode->children[j-med-1]->parent = newNode;
    j++;
    newNode->children[j-med-1] = tempN;
    newNode->children[j-med-1]->parent = newNode;
    newNode->super.filled++;

    newNode->super.parent = node->super.parent;

    return newNode;
}

struct node* insert(struct node *root, int key, int value) {
    //empty tree
    if (root == NULL) {
        struct leafNode *newNode = (struct leafNode*) malloc(sizeof(struct leafNode));
        newNode->super.parent = NULL;
        newNode->super.type = LEAF;
        newNode->super.keys[0] = key;
        newNode->values[0] = value;
        newNode->super.filled = 1;

        return (struct node*) newNode;
    }

    struct node *curr = root;

    //traverse to leaf
    while (curr->type == INNER) {
        //find index for next child
        int i = 0;
        while (i < curr->filled && curr->keys[i] < key)
            i++;
        curr = ((struct innerNode*)curr)->children[i];
    }
    
    //insert key and value
    //split nodes in case of overflow -> bottom up
    
    //find index for insertion
    int i = 0;
    while (i < curr->filled && curr->keys[i] < key)
        i++; 

    //no overflow
    if (curr->filled < MAX_NODE_SIZE) {
        for (int j = curr->filled; j >= i; j--) {
            curr->keys[j] = curr->keys[j-1];
        }
        curr->keys[i] = key;
        curr->filled++;

     //overflow 
    } else {
        //TODO: handle linked list pointers on leaf nodes
        //slit leafNode
        struct leafNode* newLeaf = splitLeaf((struct leafNode*) curr, key, value);
        //insert median key into parent Node
        int medKey = newLeaf->super.keys[0];

        //curr is root
        if (curr->parent == NULL) {
            root = (struct node*) malloc(sizeof(struct innerNode));
            root->parent = NULL;
            root->filled = 1;
            root->keys[0] = medKey;
            ((struct innerNode*)root)->children[0] = curr;
            ((struct innerNode*)root)->children[1] = (struct node*) newLeaf;
            curr->parent = (struct innerNode*) root;
            newLeaf->super.parent = (struct innerNode*) root;
        } else {
            root = insert_node(root, curr->parent, (struct node*) newLeaf, medKey);
        }

    }
        
    return root;     

}

void printTree(struct node* root) {
/*    for (int i = 0; i < root->filled; i++)
        printf("%d  ", root->keys[i]);
    printf("\n");
    if (root->type == INNER) {
        for (int i = 0; i <= root->filled; i++)
            printTree(((struct innerNode*)root)->children[i]);

    }*/

    printf("digraph {\n");
    printf("\tnode [shape=record];\n");
    
    print_dot(root, 1, 1);

    printf("}\n");

}

void print_dot(struct node* root, int from, int to) {
    printf("\tstruct%d [label=\"", to);
    for (int i = 0; i < root->filled; i++) {
        printf(" %d ", root->keys[i]);
        if (i < root->filled-1)
            printf("|");
    }
    printf("\"]\n");

    if (root->parent != NULL) 
        printf("\tstruct%d -> struct%d;\n", from, to);

    if (root->type == INNER) {
        for (int i = 0; i <= root->filled; i++) {
            print_dot(((struct innerNode*)root)->children[i], to, to*10 + i + 1);
        }
    }
}

int main() {

    struct node* tree = NULL;
    for (int i = 0; i < 30; i++) {
        int j = rand() % 100;
        tree = insert(tree, j, j);
        printTree(tree);
    }

    return 0;
}
