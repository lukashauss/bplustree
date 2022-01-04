#include "bptree.h"
#include <stdlib.h>
#include <stdio.h>

struct leafNode* empty_leaf() {
    struct leafNode* leaf = (struct leafNode*) malloc(sizeof(struct leafNode));
    leaf->super.parent = NULL;
    leaf->super.type = LEAF;
    leaf->prev = NULL;
    leaf->next = NULL;
    leaf->super.filled = 0;


    return leaf;
}

struct innerNode* empty_innerNode() {
    struct innerNode* node = (struct innerNode*) malloc(sizeof(struct innerNode));
    node->super.type = INNER;
    node->super.parent = NULL;
    node->super.filled = 0;

    return node;
}

struct leafNode* splitLeaf(struct leafNode* leaf, int key, int value) {
    //create new Leaf and insert key and value
    struct leafNode* newLeaf = empty_leaf();

    //reorder linked list
    newLeaf->prev = leaf;
    newLeaf->next = leaf->next;

    if (leaf->next != NULL)
        leaf->next->prev = newLeaf;

    leaf->next = newLeaf;

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

    //no overflow
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
        //split node and insert medKey into parent node
        
        struct innerNode* newNode = split_node(node, child, key); 
        int medKey = node->super.keys[node->super.filled];

        //node is root
        if (node->super.parent == NULL) {
            root = (struct node*) empty_innerNode();
            root->keys[0] = medKey;
            root->filled = 1;

            ((struct innerNode*)root)->children[0] = (struct node*) node;
            ((struct innerNode*)root)->children[1] = (struct node*) newNode;

            node->super.parent = (struct innerNode*) root;
            newNode->super.parent = (struct innerNode*) root;
        } else {
            //insert medKey and newNode into parent
            root = insert_node(root, node->super.parent, (struct node*) newNode, medKey);

        }
    }
    return root;
} 

struct innerNode* split_node(struct innerNode* node, struct node* child, int key) {
    
    //create new Node and insert key and child
    //child is right to the inserted key
    struct innerNode* newNode = empty_innerNode();

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
     
    //insert into new node
    int j;
    for (j = med+1; j < MAX_NODE_SIZE; j++) {
        //copy keys and reorganize children
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
    //tree is empty -> create new leaf node
    
    if (root == NULL) {
        struct leafNode *newNode = empty_leaf();
        newNode->super.keys[0] = key;
        newNode->values[0] = value;
        newNode->super.filled = 1;

        return (struct node*) newNode;
    }

    //traverse to leaf
    
    struct node *curr = root;

    while (curr->type == INNER) {
        //find index for next child
        int i = 0;
        while (i < curr->filled && curr->keys[i] < key)
            i++;

        curr = ((struct innerNode*)curr)->children[i];
    }
    
    //insert key and value in leaf
    //in case of overflow split node 
    
    //find index for insertion
    int i = 0;
    while (i < curr->filled && curr->keys[i] < key)
        i++; 

    //no overflow
    if (curr->filled < MAX_NODE_SIZE) {
        //insert key and value in right place
        for (int j = curr->filled; j >= i; j--) {
            curr->keys[j] = curr->keys[j-1];
            ((struct leafNode*)curr)->values[j] = ((struct leafNode*)curr)->values[j-1];
        }
        curr->keys[i] = key;
        ((struct leafNode*)curr)->values[i] = value;
        curr->filled++;

     //overflow 
    } else {

        //slit leafNode
        struct leafNode* newLeaf = splitLeaf((struct leafNode*) curr, key, value);

        //insert median key into parent Node
        int medKey = newLeaf->super.keys[0];

        //curr is root
        if (curr->parent == NULL) {
            root = (struct node*) empty_innerNode();
            root->filled = 1;
            root->keys[0] = medKey;
            ((struct innerNode*)root)->children[0] = curr;
            ((struct innerNode*)root)->children[1] = (struct node*) newLeaf;
            curr->parent = (struct innerNode*) root;
            newLeaf->super.parent = (struct innerNode*) root;
        } else {
            //insert medKey in parent node
            root = insert_node(root, curr->parent, (struct node*) newLeaf, medKey);
        }

    }
        
    return root;     

}

int lookup(struct node* root, int key) {
    if (root == NULL) {
        printf("Tree is empty.\n");
        return -1;
    }
    struct node* curr = root;
    //traverse to leaf
    while (curr->type != LEAF) {
        //find next child
        int i = 0;
        while (i < curr->filled && curr->keys[i] <= key)
            i++;
        curr = ((struct innerNode*)curr)->children[i];
    }
    //find key in leaf
    int i = 0;
    while (i < curr->filled && curr->keys[i] < key)
        i++;
    if (i == curr->filled) {
        printf("Key not found in the tree.\n");
        return -1;
    } else {
        return ((struct leafNode*)curr)->values[i];
    }

}

/*
struct node* delete(struct node* root, int key) {

    if (root == NULL) {
        printf("Tree is empty.\n");
        return NULL;
    }

    //traverse to leaf node
    struct node* curr = root;
    while (curr->type != LEAF) {
        //find next child
        int i = 0;
        while (i < curr->filled && curr->keys[i] <=key)
            i++;
        curr = ((struct innerNode*)curr)->children[i];
    }

    //find index of key
    int i = 0;
    while (i < curr->filled && curr->keys[i] != key)
        i++;

    //key not in the tree
    if (i == curr->filled)
        return root;     
    
    
    //curr is root
    if (curr->parent == NULL) {
        free(curr);
        return NULL;

    //leaf has more than minimum amount of keys
    } else if (curr->filled > MIN_NODE_SIZE) {

        //  key is also in inner node (first key in the leaf node and not the first leaf node)
        if (i == 0 && curr != ((struct innerNode*)curr->parent)->children[0]) {
            //replace key in inner node with successor
            int succ = curr->keys[i+1];
            struct innerNode* parent = curr->parent;
            int j = 0;
            while (parent != NULL) {
                if (j == 0 && parent->super.keys[j] > key) {
                    parent = parent->super.parent;
                } else if (parent->super.keys[j] == key){
                    parent->super.keys[j] = succ;                    
                    break;
                } else {
                    j++;
                }
            }
        } 
        //delete key and value
        for (int j = i; j < curr->filled; j++) {
            curr->keys[j] = curr->keys[j+1];

            ((struct leafNode*)curr)->values[j] = ((struct leafNode*)curr)->values[j+1];
        }
        curr->filled--;
    

    //leaf already has minimum size
    //-> borrow key from the next sibling (left)
    } else {
        //key is in inner node
        if (i == 0 && curr != ((struct innerNode*)curr->parent)->children[0]) {

        } else {
            //there is no left sibling -> check for right
            if (((struct leafNode*)curr)->prev == NULL) {
                if (((struct leafNode*)curr)->next->super.filled > MIN_NODE_SIZE) {
                    struct leafNode* sibling = ((struct leafNode*)curr)->next;
                    sibling->super.filled--;
                    int borrowed = sibling->super.keys[0];
                    int borrowed_val = sibling->values[0];
                    //remove borrowed in sibling
                    for (int j = 0; j < sibling->super.filled; j++) {
                        sibling->super.keys[j] = sibling->super.keys[j+1];
                        sibling->values[j] = sibling->values[j+1];
                    }
                    //replace value in parent node
                    int p = 0;
                    while (curr->parent->super.keys[p] < key)
                        p++;
                    curr->parent->super.keys[p] = sibling->super.keys[1];
                    
                    //insert borrowed in leaf
                    curr->keys[i] = borrowed;
                    ((struct leafNode*)curr)->values[i] = borrowed_val;
                    for (int j = i; j < curr->filled-1; j++) {
                        int t = curr->keys[j];
                        curr->keys[j] = curr->keys[j+1];
                        curr->keys[j+1] = t;
                        t = ((struct leafNode*)curr)->values[j];
                        ((struct leafNode*)curr)->values[j] = ((struct leafNode*)curr)->values[j+1];
                        ((struct leafNode*)curr)->values[j+1] = t;
                    }
                }

            //check if left sibling has enough keys
            } else if (((struct leafNode*)curr)->prev->super.filled > MIN_NODE_SIZE) {
                //borrow key from sibling and replace deleted key with it
                struct leafNode* sibling = ((struct leafNode*)curr)->prev;
                sibling->super.filled--;
                int borrowed = sibling->super.keys[sibling->super.filled];
                int borrowed_val = sibling->values[sibling->super.filled];
                //replace value in parent node
                int p = 0;
                while (curr->parent->super.keys[p] < borrowed)
                    p++;
                curr->parent->super.keys[p] = borrowed;

                //delete value in leaf
                //insert borrowed in leaf
                curr->keys[i] = borrowed;
                ((struct leafNode*)curr)->values[i] = borrowed_val;
                for (int j = i; j > 0; j--) {
                    int t = curr->keys[j];
                    curr->keys[j] = curr->keys[j-1];
                    curr->keys[j-1] = t;
                    t = ((struct leafNode*)curr)->values[j];
                    ((struct leafNode*)curr)->values[j] = ((struct leafNode*)curr)->values[j-1];
                    ((struct leafNode*)curr)->values[j-1] = t;
                }
                

            }

        }

    }
    return root;

}

*/

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
        if (root->type == LEAF)
            printf(": %d ", ((struct leafNode*)root)->values[i]);
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

