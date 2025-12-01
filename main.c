#include <stdio.h>
#include <stdlib.h>
#include "../scr/lib/tree.h"


void main() {
    TreeNode root;
    iniciar_tree(&root);

    incerir_tree(&root, 10);
    incerir_tree(&root, 5);
    incerir_tree(&root, 15);

    if (buscar_tree(root, 10)) {
        printf("Value 10 found in the tree.\n");
    } else {
        printf("Value 10 not found in the tree.\n");
    }

    if (buscar_tree(root, 7)) {
        printf("Value 7 found in the tree.\n");
    } else {
        printf("Value 7 not found in the tree.\n");
    }

    int height = altura_tree(root);
    printf("Height of the tree: %d\n", height); 

    int nodes = numero_nodos(root);
    printf("Number of nodes in the tree: %d\n", nodes);

    liberar_tree(&root);

    return 0;
}