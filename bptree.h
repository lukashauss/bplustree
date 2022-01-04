#define MAX_DEGREE 6
#define MAX_NODE_SIZE (MAX_DEGREE-1)
#define MIN_NODE_SIZE MAX_DEGREE / 2 - 1

enum NodeTypes {
    INNER, LEAF
};


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

struct leafNode* empty_leaf();

struct innerNode* empty_innerNode();

int lookup(struct node* root, int key);

void printTree(struct node* root);

void print_dot(struct node* root, int from, int to);
