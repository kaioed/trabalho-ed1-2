#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

/**
 * @file tree.h
 * @brief Interface para árvore binária de busca (opaca).
 *
 * Os nós da árvore são representados de forma opaca através do tipo
 * `TreeNode`. As funções abaixo permitem inicializar a raiz, inserir e
 * remover nós, buscar o menor elemento e liberar os recursos.
 */

/**
 * @brief Tipo opaco para um nó da árvore.
 *
 * Deve ser tratado como um ponteiro genérico; a implementação define a
 * estrutura interna.
 */
typedef void* TreeNode;

/**
 * @brief Inicializa a árvore definindo a raiz.
 *
 * @param root ponteiro para a variável que contém a raiz da árvore.
 *             Após a chamada, a árvore estará inicializada (por exemplo, `NULL`).
 */
void iniciar_tree(TreeNode* root);

/**
 * @brief Insere um novo nó na árvore.
 *
 * @param root ponteiro para a raiz da árvore.
 * @param key  chave (valor numérico) usada para ordenação na árvore.
 * @param data ponteiro para os dados associados ao nó (opaco para a árvore).
 */
void inserir_tree(TreeNode* root, double key, void* data);

/**
 * @brief Remove um nó da árvore.
 *
 * Alterado: a função exige o `data` para confirmar que o nó correto está
 * sendo removido quando existem chaves duplicadas.
 *
 * @param root ponteiro para a raiz da árvore.
 * @param key  chave do nó a ser removido.
 * @param data ponteiro para os dados que devem corresponder ao nó a remover.
 * @return `true` se um nó foi removido com sucesso, `false` caso contrário.
 */
bool remover_tree(TreeNode* root, double key, void* data);

/**
 * @brief Retorna os dados do menor elemento (menor chave) da árvore.
 *
 * @param root raiz da árvore (pode ser `NULL`).
 * @return ponteiro para os dados do nó com menor chave, ou `NULL` se a árvore estiver vazia.
 */
void* buscar_min_tree(TreeNode root);

/**
 * @brief Libera todos os recursos da árvore e zera a raiz.
 *
 * @param root ponteiro para a raiz da árvore que será liberada.
 */
void liberar_tree(TreeNode* root);

typedef void (*TreeCallback)(void* data, void* ctx);
void tree_iterar(TreeNode root, TreeCallback fn, void* ctx);

#endif