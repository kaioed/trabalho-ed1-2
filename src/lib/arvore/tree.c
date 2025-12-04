#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNodeStruct;  

void iniciar_tree (TreeNode* root) {
    *root = NULL;
}

void incerir_tree(TreeNode* root, int value) {
    if (*root == NULL) {
        TreeNodeStruct* newNode = (TreeNodeStruct*)malloc(sizeof(TreeNodeStruct));
        newNode->data = value;
        newNode->left = NULL;
        newNode->right = NULL;
        *root = (TreeNode)newNode;
    } else {
        TreeNodeStruct* current = (TreeNodeStruct*)(*root);
        if (value < current->data) {
            incerir_tree((TreeNode*)&(current->left), value);
        } else {
            incerir_tree((TreeNode*)&(current->right), value);
        }
    }
}

bool buscar_tree(TreeNode root, int value) {
    if (root == NULL) {
        return false;
    }
    TreeNodeStruct* current = (TreeNodeStruct*)root;
    if (current->data == value) {
        return true;
    } else if (value < current->data) {
        return buscar_tree((TreeNode)current->left, value);
    } else {
        return buscar_tree((TreeNode)current->right, value);
    }
}

int altura_tree(TreeNode root) {
    if (root == NULL) {
        return -1;
    }
    TreeNodeStruct* current = (TreeNodeStruct*)root;
    int leftHeight = altura_tree((TreeNode)current->left);
    int rightHeight = altura_tree((TreeNode)current->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

void liberar_tree(TreeNode* root) {
    if (*root != NULL) {
        TreeNodeStruct* current = (TreeNodeStruct*)(*root);
        liberar_tree((TreeNode*)&(current->left));
        liberar_tree((TreeNode*)&(current->right));
        free(current);
        *root = NULL;
    }
}

int remover_tree(TreeNode* root, int value) {
    if (*root == NULL) {
        return 0; 
    }
    TreeNodeStruct* current = (TreeNodeStruct*)(*root);
    if (value < current->data) {
        return remover_tree((TreeNode*)&(current->left), value);
    } else if (value > current->data) {
        return remover_tree((TreeNode*)&(current->right), value);
    } else {
        if (current->left == NULL && current->right == NULL) {
            free(current);
            *root = NULL;
        } else if (current->left == NULL) {
            TreeNodeStruct* temp = (TreeNodeStruct*)current->right;
            free(current);
            *root = (TreeNode)temp;
        } else if (current->right == NULL) {
            TreeNodeStruct* temp = (TreeNodeStruct*)current->left;
            free(current);
            *root = (TreeNode)temp;
        } else {
            TreeNodeStruct* successorParent = current;
            TreeNodeStruct* successor = (TreeNodeStruct*)current->right;
            while (successor->left != NULL) {
                successorParent = successor;
                successor = (TreeNodeStruct*)successor->left;
            }
            current->data = successor->data;
            if (successorParent->left == successor) {
                remover_tree((TreeNode*)&(successorParent->left), successor->data);
            } else {
                remover_tree((TreeNode*)&(successorParent->right), successor->data);
            }
        }
        return 1; 
    }
}

int numero_nodos(TreeNode root) {
    if (root == NULL) {
        return 0;
    }
    TreeNodeStruct* current = (TreeNodeStruct*)root;
    return 1 + numero_nodos((TreeNode)current->left) + numero_nodos((TreeNode)current->right);
}