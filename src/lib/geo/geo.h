/**
 * @file geo.h
 * @brief Interface para processamento de formas geométricas e geração de SVG.
 *
 * Define tipos e funções para manipular o terreno (Ground) contendo formas geométricas
 * (círculos, retângulos, linhas, textos) e suas clones, com suporte para exportação SVG.
 */
#ifndef GEO_H
#define GEO_H

#include <stdio.h>
#include "../lista/lista.h" 

/**
 * @brief Tipo opaco para representar o terreno (Ground).
 */
typedef void* Ground;

/**
 * @brief Tipo opaco para representar uma forma geométrica.
 */
typedef void* Forma;

/**
 * @brief Enumeração dos tipos de formas geométricas suportadas.
 */
typedef enum {
	TIPO_CIRCULO = 0,
	TIPO_RETANGULO = 1,
	TIPO_LINHA = 2,
	TIPO_TEXTO = 3
} TipoForma;

/**
 * @brief Processa um arquivo de formas geométricas e escreve no arquivo SVG.
 *
 * @param geo arquivo contendo definição das formas geométricas
 * @param svg arquivo de saída no formato SVG
 * @return ponteiro para o terreno (Ground) criado com as formas
 */
Ground process_geo(FILE *geo, FILE *svg);

/**
 * @brief Libera todos os recursos alocados para um terreno.
 *
 * @param g ponteiro para o terreno a ser destruído
 */
void destruir_ground(Ground g);

/**
 * @brief Obtém a lista de formas do terreno.
 *
 * @param g terreno do qual recuperar a lista de formas
 * @return lista contendo as formas geométricas
 */
Lista get_ground_lista_formas(Ground g);

/**
 * @brief Obtém a lista de clones do terreno.
 *
 * @param g terreno do qual recuperar a lista de clones
 * @return lista contendo os clones das formas
 */
Lista get_ground_lista_clones(Ground g);

/**
 * @brief Escreve a representação SVG do terreno em arquivo.
 *
 * @param g terreno a ser exportado
 * @param svg arquivo de saída para o SVG
 */
void ground_escrever_svg(Ground g, FILE* svg);

#endif
