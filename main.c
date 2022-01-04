#include "bptree.h"
#include <stdlib.h>
#include <stdio.h>

int main() {


    struct node* tree = NULL;
    for (int i = 0; i < 30; i++) {
        tree = insert(tree, i, i+10);
        printTree(tree);
    }
    
    return 0;
}
