#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tree.h"

typedef struct Node {
    double key;
    void* data;
    struct Node* left;
    struct Node* right;
} NodeStruct;

void iniciar_tree(TreeNode* root) {
    *root = NULL;
}

void inserir_tree(TreeNode* root, double key, void* data) {
    NodeStruct** current = (NodeStruct**)root;
    while (*current != NULL) {
        if (key < (*current)->key) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }
    NodeStruct* newNode = (NodeStruct*)malloc(sizeof(NodeStruct));
    newNode->key = key;
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    *current = newNode;
}

bool remover_tree(TreeNode* root, double key, void* data) {
    NodeStruct** current = (NodeStruct**)root;
    
    while (*current != NULL) {
        if (key == (*current)->key) {
            NodeStruct* toDelete = *current;
            
            if (data == NULL || toDelete->data == data) {
                if (toDelete->left == NULL) {
                    *current = toDelete->right;
                    free(toDelete);
                } else if (toDelete->right == NULL) {
                    *current = toDelete->left;
                    free(toDelete);
                } else {
                    NodeStruct** succParent = &((*current)->right);
                    while ((*succParent)->left != NULL) {
                        succParent = &((*succParent)->left);
                    }
                    NodeStruct* successor = *succParent;
                    
                    toDelete->key = successor->key;
                    toDelete->data = successor->data;
                    
                    *succParent = successor->right;
                    free(successor);
                }
                return true;
            } else {
                current = &((*current)->right);
            }
        } else if (key < (*current)->key) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }
    return false;
}

void* buscar_min_tree(TreeNode root) {
    NodeStruct* current = (NodeStruct*)root;
    if (current == NULL) return NULL;
    while (current->left != NULL) {
        current = current->left;
    }
    return current->data;
}

void liberar_node_rec(NodeStruct* node) {
    if (node == NULL) return;
    liberar_node_rec(node->left);
    liberar_node_rec(node->right);
    free(node);
}

void liberar_tree(TreeNode* root) {
    if (root && *root) {
        liberar_node_rec((NodeStruct*)*root);
        *root = NULL;
    }
}

void tree_iterar_rec(NodeStruct* node, TreeCallback fn, void* ctx) {
    if (node == NULL) return;
    tree_iterar_rec(node->left, fn, ctx);
    fn(node->data, ctx);
    tree_iterar_rec(node->right, fn, ctx);
}

void tree_iterar(TreeNode root, TreeCallback fn, void* ctx) {
    if (root) {
        tree_iterar_rec((NodeStruct*)root, fn, ctx);
    }
}