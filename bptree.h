#define MAX_DEGREE 5
#define MAX_NODE_SIZE (MAX_DEGREE-1)

enum NodeTypes {
    INNER, LEAF
};

/*
struct keyList {
    struct keyList *next;
    int key;
}
*/

//polymorphic node 
struct node {
    //type of node
    char type;
    struct innerNode *parent;
    int keys[MAX_DEGREE - 1];
    //amount of values currently stored
    int filled;
};

struct innerNode {
    struct node super;
    struct node *children[MAX_DEGREE];

};

struct leafNode {
    struct node super;
    int values[MAX_DEGREE - 1];
    struct leafNode *prev;
    struct leafNode *next;
};

struct node* insert(struct node* root, int key, int value); 

struct innerNode* split_node(struct innerNode* node, struct node* child, int key);

struct node* insert_node(struct node *root, struct innerNode* node, struct node* child, int key);

struct leafNode* splitLeaf(struct leafNode* leaf, int key, int value);

void printTree(struct node* root);
void print_dot(struct node* root, int from, int to);
