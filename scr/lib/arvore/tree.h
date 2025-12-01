#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

/**
 * @brief Tipo opaco para nó/raiz de árvore binária.
 */
typedef void* TreeNode;

/**
 * @brief Inicializa a árvore (define raiz como NULL).
 * @param root ponteiro para a raiz da árvore
 */
void iniciar_tree(TreeNode* root);

/**
 * @brief Insere um valor na árvore binária de busca.
 * @param root ponteiro para a raiz da árvore
 * @param value valor inteiro a inserir
 */
void incerir_tree(TreeNode* root, int value);

/**
 * @brief Busca um valor inteiro na árvore.
 * @param root raiz da árvore onde buscar
 * @param value valor a buscar
 * @return true se encontrado, false caso contrário
 */
bool buscar_tree(TreeNode root, int value);

/**
 * @brief Remove um valor da árvore, se existir.
 * @param root ponteiro para a raiz da árvore
 * @param value valor a remover
 * @return 1 se removido com sucesso, 0 se não encontrado, valor negativo em erro
 */
int remover_tree(TreeNode* root, int value);

/**
 * @brief Calcula a altura da árvore.
 * @param root raiz da árvore
 * @return altura (número de níveis) da árvore
 */
int altura_tree(TreeNode root);

/**
 * @brief Retorna o número de nós presentes na árvore.
 * @param root raiz da árvore
 * @return número de nós
 */
int numero_nodos(TreeNode root);

#endif