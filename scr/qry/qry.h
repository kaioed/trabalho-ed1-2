#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../lib/geo/geo.h"

/**
 * @brief Tipo opaco para Arena, usada para gerenciar formas em uma arena de jogo.
 */
typedef void* Arena;

/**
 * @brief Cria uma nova arena vazia.
 * @return Ponteiro para a arena criada, ou NULL em caso de erro.
 */
Arena criar_arena();

/**
 * @brief Adiciona uma forma à arena.
 * @param a Ponteiro para a arena
 * @param forma Ponteiro para a forma a ser adicionada
 */
void arena_adicionar_forma(Arena a, Forma forma);

/**
 * @brief Libera toda a memória associada à arena.
 * @param a Ponteiro para a arena a ser destruída
 */
void destruir_arena(Arena a);

/**
 * @brief Obtém o tipo de uma forma.
 * @param forma Ponteiro para a forma
 * @return TipoForma da forma, ou (TipoForma)-1 se inválida
 */
TipoForma forma_get_tipo(Forma forma);

/**
 * @brief Obtém o ID original de uma forma.
 * @param forma Ponteiro para a forma
 * @return ID original da forma, ou -1 se inválida
 */
int forma_get_id_original(Forma forma);

/**
 * @brief Define o status de destruição de uma forma.
 * @param forma Ponteiro para a forma
 * @param status true se destruída, false caso contrário
 */
void forma_set_destruida(Forma forma, bool status);

/**
 * @brief Verifica se duas formas colidem usando AABB (Axis-Aligned Bounding Box).
 * @param f1 Primeira forma
 * @param f2 Segunda forma
 * @return true se as formas colidem, false caso contrário
 */
bool formas_colidem(void* f1, void* f2);

/**
 * @brief Processa comandos de query a partir de um arquivo.
 * @param qry Arquivo de entrada contendo comandos de query
 * @param svg Arquivo de saída SVG para desenhar resultado
 * @param ground Estrutura Ground com as formas carregadas
 * @param txt Arquivo de saída para mensagens/resultados em texto
 */
void process_qry(FILE *qry, FILE *svg, void* ground, FILE *txt);

#endif
