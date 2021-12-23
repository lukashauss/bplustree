#define MAX_DEGREE 5

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
    int keys[MAX_DEGREE - 1];
    //amount of values currently stored
    size_t filled;
};

struct innerNode {
    struct node super;
    struct node *parent;
    struct node *children[MAX_DEGREE];

};

struct leafNode {
    struct node super;
    int values[MAX_DEGREE - 1];
    struct leafNode *prev;
    struct leafNode *next;
};

struct node* insert(struct node* root, int key, int value); 
