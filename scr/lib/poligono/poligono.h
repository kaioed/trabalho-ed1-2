#ifndef POLIGONO_H
#define POLIGONO_H

#include <stddef.h>

/**
 * @file poligono.h
 * @brief API de polígono e visibilidade (tipos opacos + operações)
 *
 * Este header expõe tipos opacos e funções para criação, manipulação
 * e cálculo de visibilidade em polígonos. As implementações internas
 * permanecem ocultas (tipos opacos) para preservar encapsulamento.
 */

// ---------- TIPOS OPACOS (sem struct no .h) ----------
/**
 * @brief Tipo opaco que representa um polígono (lista/estrutura interna)
 */
typedef void* Poligono;

/**
 * @brief Tipo opaco que representa o resultado do cálculo de visibilidade
 * (coleção/lista de pontos visíveis a partir de um ponto dado).
 */
typedef void* Visibilidade;

/**
 * @brief Tipo opaco para conjunto de segmentos (estrutura interna, por ex. AVL)
 */
typedef void* SegmentoSet;  // árvore AVL interna

/**
 * @brief Tipo opaco para um segmento (aresta) do polígono
 */
typedef void* Segmento;

/**
 * @brief Tipo opaco para um ponto (x,y)
 */
typedef void* Ponto;

// ---------- CRIAÇÃO E DESTRUIÇÃO ----------
/**
 * @brief Cria um polígono capaz de armazenar até `n` segmentos/arestas.
 * @param n capacidade inicial (número de segmentos esperado)
 * @return `Poligono` válido ou NULL em caso de falha de alocação
 */
Poligono CriarPoligono(int n);

/**
 * @brief Destrói um polígono e libera toda a memória associada.
 * @param p polígono a ser destruído (pode ser NULL)
 */
void DestruirPoligono(Poligono p);

/**
 * @brief Cria um ponto com coordenadas (x, y).
 * @param x coordenada x
 * @param y coordenada y
 * @return Ponto alocado ou NULL em caso de erro
 */
Ponto CriarPonto(double x, double y);

/**
 * @brief Cria um segmento entre os pontos `a` e `b`.
 * @param a ponto inicial
 * @param b ponto final
 * @return Segmento alocado ou NULL em caso de erro
 */
Segmento CriarSegmento(Ponto a, Ponto b);

// ---------- MANIPULAÇÃO DO POLÍGONO ----------
/**
 * @brief Adiciona um segmento ao polígono.
 * @param p polígono destino
 * @param s segmento a ser adicionado
 */
void PoligonoAdicionarSegmento(Poligono p, Segmento s);


/**
 * @brief Calcula a visibilidade a partir do ponto `x` dentro do polígono `p`.
 *
 * O resultado é uma estrutura `Visibilidade` que pode ser consultada
 * através das funções de extração abaixo. O chamador é responsável por
 * liberar a `Visibilidade` com `DestruirVisibilidade`.
 *
 * @param p polígono onde calcular a visibilidade
 * @param x ponto de vista
 * @return Visibilidade (pode ser NULL em caso de erro)
 */
Visibilidade CalcularVisibilidade(Poligono p, Ponto x);

/**
 * @brief Retorna o número de pontos visíveis contidos em `v`.
 * @param v estrutura de visibilidade
 * @return número de pontos visíveis
 */
int VisibilidadeNumPontos(Visibilidade v);

/**
 * @brief Obtém o i-ésimo ponto no resultado de visibilidade.
 * @param v estrutura de visibilidade
 * @param i índice (0-based)
 * @return Ponto correspondente ou NULL se índice inválido
 */
Ponto VisibilidadeGetPonto(Visibilidade v, int i);

/**
 * @brief Retorna a coordenada X do ponto `p`.
 * @param p ponto
 * @return coordenada x
 */
double PontoX(Ponto p);

/**
 * @brief Retorna a coordenada Y do ponto `p`.
 * @param p ponto
 * @return coordenada y
 */
double PontoY(Ponto p);

/**
 * @brief Libera a estrutura de visibilidade e seus recursos associados.
 * @param V Visibilidade a ser destruída (pode ser NULL)
 */
void DestruirVisibilidade(Visibilidade V);

#endif
