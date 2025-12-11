/**
 * @file lista.h
 * @brief Header adicionado automaticamente.
 */
#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

/**
 * @brief Tipo opaco para a estrutura da lista.
 */
typedef void* Lista;

/**
 * @brief Tipo opaco para iterar sobre a lista (representa um nó/posição).
 */
typedef void* Posic;

/**
 * @brief Inicializa uma nova lista encadeada vazia.
 * @param list Ponteiro para a variável que receberá a lista.
 */
void iniciar_lista(Lista* list);

/**
 * @brief Libera toda a memória usada pela estrutura da lista e seus nós.
 * @note Não libera os dados (void*) armazenados, apenas a estrutura.
 * @param list Ponteiro para a lista a ser liberada.
 */
void liberar_lista(Lista* list);

/**
 * @brief Insere um elemento no início da lista.
 * @param list Ponteiro para a lista.
 * @param value Ponteiro para o dado a ser armazenado.
 */
void inserir_lista(Lista* list, void* value);

/**
 * @brief Insere um elemento no final da lista.
 * @param list Ponteiro para a lista.
 * @param value Ponteiro para o dado a ser armazenado.
 */
void inserir_fim_lista(Lista* list, void* value);

/**
 * @brief Remove o primeiro elemento da lista e retorna seu valor.
 * @param list Ponteiro para a lista.
 * @param out_value Ponteiro onde o valor removido será armazenado (opcional).
 * @return true se removeu com sucesso, false se a lista estava vazia.
 */
bool remover_inicio_lista(Lista* list, void** out_value);

/**
 * @brief Retorna o número de elementos na lista.
 * @param list A lista.
 * @return Inteiro com o tamanho.
 */
int tamanho_lista(Lista list);

/**
 * @brief Retorna a posição do primeiro elemento da lista.
 * @param list A lista.
 * @return Posic do primeiro elemento ou NULL se vazia.
 */
Posic get_primeiro_lista(Lista list);

/**
 * @brief Retorna a posição do próximo elemento em relação a 'p'.
 * @param list A lista.
 * @param p Posição atual.
 * @return Posic do próximo elemento ou NULL se 'p' for o último.
 */
Posic get_proximo_lista(Lista list, Posic p);

/**
 * @brief Retorna a posição do último elemento da lista.
 * @param list A lista.
 * @return Posic do último elemento ou NULL se vazia.
 */
Posic get_ultimo_lista(Lista list);

/**
 * @brief Retorna a posição do elemento anterior a 'p'.
 * @param list A lista.
 * @param p Posição atual.
 * @return Posic do anterior ou NULL se 'p' for o primeiro.
 */
Posic get_anterior_lista(Lista list, Posic p);

/**
 * @brief Obtém o valor armazenado na posição 'p'.
 * @param list A lista.
 * @param p Posição válida na lista.
 * @return Ponteiro para o dado armazenado.
 */
void* get_valor_lista(Lista list, Posic p);

#endif
