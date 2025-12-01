#ifndef GEO_H
#define GEO_H

#include <stdio.h>

#include "../lista/lista.h" 


typedef void* Ground;

/**
 * @brief Tipo opaco para representar uma forma (wrapper usado internamente)
 */
typedef void* Forma;

/**
 * @brief Enumeração dos tipos de forma suportados.
 */
typedef enum {
	TIPO_CIRCULO = 0,
	TIPO_RETANGULO = 1,
	TIPO_LINHA = 2,
	TIPO_TEXTO = 3
} TipoForma;

/**
 * @brief Processa um arquivo .geo, carrega todas as formas na estrutura Ground
 * e desenha o estado inicial no arquivo SVG.
 */
Ground process_geo(FILE *geo, FILE *svg);

/**
 * @brief Libera toda a memória associada à estrutura Ground, incluindo formas originais e clones.
 */
void destruir_ground(Ground g);

/**
 * @brief Obtém a lista de formas da estrutura Ground.
 * (Anteriormente get_ground_fila)
 */
Lista get_ground_lista_formas(Ground g);

/**
 * @brief Obtém a lista de clones da estrutura Ground.
 * (Anteriormente get_ground_pilha_clones)
 */
Lista get_ground_lista_clones(Ground g);

#endif