#ifndef POLIGONO_H
#define POLIGONO_H

#include <stddef.h>

/**
 * @file poligono.h
 * @brief API de manipulação de polígonos e cálculo de visibilidade.
 *
 * Este header expõe tipos opacos e funções para criar polígonos,
 * adicionar segmentos, calcular visibilidade a partir de um ponto
 * e extrair os resultados.
 */

/**
 * @brief Tipo opaco para representar um polígono.
 */
typedef void* Poligono;

/**
 * @brief Tipo opaco que representa a estrutura de resultado da visibilidade.
 */
typedef void* Visibilidade;

/**
 * @brief Tipo opaco para conjunto de segmentos (implementação interna, ex.: AVL).
 */
typedef void* SegmentoSet;

/**
 * @brief Tipo opaco para um segmento (aresta) do polígono.
 */
typedef void* Segmento;

/**
 * @brief Tipo opaco para um ponto (coordenadas x,y).
 */
typedef void* Ponto;

/**
 * @brief Cria um polígono com capacidade inicial para `n` segmentos.
 *
 * @param n capacidade inicial (número estimado de segmentos)
 * @return Poligono inicializado, ou NULL em caso de erro de alocação
 */
Poligono CriarPoligono(int n);

/**
 * @brief Destrói um polígono liberando toda a memória associada.
 *
 * @param p polígono a ser destruído (pode ser NULL)
 */
void DestruirPoligono(Poligono p);

/**
 * @brief Cria um ponto com coordenadas (x, y).
 *
 * @param x coordenada X
 * @param y coordenada Y
 * @return Ponto alocado, ou NULL em erro
 */
Ponto CriarPonto(double x, double y);

/**
 * @brief Cria um segmento entre os pontos `a` e `b`.
 *
 * @param a ponto inicial
 * @param b ponto final
 * @return Segmento alocado, ou NULL em erro
 */
Segmento CriarSegmento(Ponto a, Ponto b);

/**
 * @brief Adiciona um segmento ao polígono `p`.
 *
 * @param p polígono destino
 * @param s segmento a ser adicionado (o polígono assume posse ou copia conforme implementação)
 */
void PoligonoAdicionarSegmento(Poligono p, Segmento s);

/**
 * @brief Calcula a visibilidade do polígono `p` a partir do ponto `x`.
 *
 * O resultado é retornado em uma estrutura opaca `Visibilidade` que
 * pode ser consultada com as funções abaixo e liberada com
 * `DestruirVisibilidade`.
 *
 * @param p polígono onde será calculada a visibilidade
 * @param x ponto de observação
 * @return Visibilidade com os pontos visíveis, ou NULL em caso de erro
 */
Visibilidade CalcularVisibilidade(Poligono p, Ponto x);

/**
 * @brief Retorna o número de pontos presentes no resultado de visibilidade `v`.
 *
 * @param v estrutura de visibilidade
 * @return número de pontos visíveis
 */
int VisibilidadeNumPontos(Visibilidade v);

/**
 * @brief Retorna o i-ésimo ponto do resultado de visibilidade.
 *
 * @param v estrutura de visibilidade
 * @param i índice (0-based)
 * @return Ponto correspondente ou NULL se índice inválido
 */
Ponto VisibilidadeGetPonto(Visibilidade v, int i);

/**
 * @brief Retorna a coordenada X do ponto `p`.
 *
 * @param p ponto
 * @return coordenada X
 */
double PontoX(Ponto p);

/**
 * @brief Retorna a coordenada Y do ponto `p`.
 *
 * @param p ponto
 * @return coordenada Y
 */
double PontoY(Ponto p);

/**
 * @brief Libera a estrutura de visibilidade `V`.
 *
 * @param V estrutura de visibilidade a ser liberada (pode ser NULL)
 */
void DestruirVisibilidade(Visibilidade V);

/**
 * @brief Define o limiar (tamanho do subvetor) para alternar entre
 * Merge Sort e Insertion Sort em rotinas internas de ordenação.
 *
 * Se o tamanho do subvetor for <= limiar, usa Insertion Sort.
 *
 * @param limiar inteiro positivo (valor típico entre 10 e 20)
 */
void definir_limiar_ordenacao(int limiar);

void definir_tipo_ordenacao(char tipo);

#endif