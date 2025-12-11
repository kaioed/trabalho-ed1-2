/**
 * @file qry.h
 * @brief Interface para processamento de consultas (queries) e gerenciamento de arena de formas.
 *
 * Define operações para criar uma arena de formas, adicionar formas, processar consultas
 * e verificar colisões entre formas.
 */
#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../geo/geo.h"

/**
 * @brief Tipo opaco para representar uma arena de formas.
 */
typedef void* Arena;

/**
 * @brief Cria uma nova arena vazia.
 *
 * @return ponteiro para a arena criada
 */
Arena criar_arena();

/**
 * @brief Adiciona uma forma à arena.
 *
 * @param a ponteiro para a arena
 * @param forma ponteiro para a forma a ser adicionada
 */
void arena_adicionar_forma(Arena a, Forma forma);

/**
 * @brief Libera todos os recursos alocados para uma arena.
 *
 * @param a ponteiro para a arena a ser destruída
 */
void destruir_arena(Arena a);

/**
 * @brief Obtém o tipo de uma forma.
 *
 * @param forma ponteiro para a forma
 * @return tipo da forma (TIPO_CIRCULO, TIPO_RETANGULO, TIPO_LINHA ou TIPO_TEXTO)
 */
TipoForma forma_get_tipo(Forma forma);

/**
 * @brief Obtém o ID original de uma forma.
 *
 * @param forma ponteiro para a forma
 * @return ID original da forma
 */
int forma_get_id_original(Forma forma);

/**
 * @brief Define o status de destruição de uma forma.
 *
 * @param forma ponteiro para a forma
 * @param status `true` para marcar como destruída, `false` caso contrário
 */
void forma_set_destruida(Forma forma, bool status);

/**
 * @brief Verifica se duas formas colidem.
 *
 * @param f1 ponteiro para a primeira forma
 * @param f2 ponteiro para a segunda forma
 * @return `true` se as formas colidem, `false` caso contrário
 */
bool formas_colidem(void* f1, void* f2);

/**
 * @brief Processa um arquivo de consultas (queries) e gera arquivos de saída.
 *
 * @param qry arquivo contendo as consultas
 * @param dir_saida diretório de saída para os arquivos gerados
 * @param nome_base nome base para os arquivos de saída
 * @param ground terreno contendo as formas
 * @param txt arquivo de saída em texto
 * @param svg_geral arquivo de saída SVG consolidado
 */
void process_qry(FILE *qry, const char* dir_saida, const char* nome_base, Ground ground, FILE *txt, FILE *svg_geral);

#endif
