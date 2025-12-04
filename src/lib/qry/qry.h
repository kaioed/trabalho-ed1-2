#ifndef QRY_H
#define QRY_H

#include <stdio.h>
#include <stdbool.h>
#include "../geo/geo.h"

/**
 * @file qry.h
 * @brief Interface do módulo QRY: arena, operações sobre formas e processamento de arquivos .qry
 */

/**
 * @brief Tipo opaco que representa uma arena (coleção/estrutura auxiliar).
 */
typedef void* Arena;

/**
 * @brief Cria uma nova arena vazia.
 *
 * @return `Arena` alocada ou NULL em caso de erro.
 */
Arena criar_arena();

/**
 * @brief Adiciona uma forma à arena.
 *
 * @param a arena destino
 * @param forma forma a ser adicionada (ponteiro retornado por `process_geo` ou clones)
 */
void arena_adicionar_forma(Arena a, Forma forma);

/**
 * @brief Libera toda a memória associada à arena.
 *
 * @param a arena a ser destruída (pode ser NULL)
 */
void destruir_arena(Arena a);

/**
 * @brief Retorna o tipo de uma forma.
 *
 * @param forma ponteiro opaco para a forma
 * @return `TipoForma` (ex.: `TIPO_CIRCULO`, `TIPO_RETANGULO`, ...), ou (TipoForma)-1 se inválido
 */
TipoForma forma_get_tipo(Forma forma);

/**
 * @brief Retorna o ID original associado a uma forma.
 *
 * @param forma ponteiro opaco para a forma
 * @return id original ou -1 se inválido
 */
int forma_get_id_original(Forma forma);

/**
 * @brief Marca/desmarca uma forma como destruída.
 *
 * @param forma ponteiro opaco para a forma
 * @param status true para marcar como destruída, false para limpar a marca
 */
void forma_set_destruida(Forma forma, bool status);

/**
 * @brief Verifica colisão entre duas formas (por exemplo AABB ou outra heurística).
 *
 * @param f1 ponteiro para a primeira forma (opaco)
 * @param f2 ponteiro para a segunda forma (opaco)
 * @return true se colidem, false caso contrário
 */
bool formas_colidem(void* f1, void* f2);

/**
 * @brief Processa um arquivo .qry aplicando operações sobre as formas carregadas em `ground`.
 *
 * A função lê os comandos do arquivo `qry`, gera saídas no `svg` e escreve mensagens/resultados
 * em `txt`. A assinatura usa `Ground` (definido em `geo.h`) para integração com o carregamento
 * realizado por `process_geo`.
 *
 * @param qry arquivo .qry aberto para leitura
 * @param svg arquivo .svg aberto para escrita (estado inicial já presente, posição de escrita gerida pelo chamador)
 * @param ground estrutura `Ground` retornada por `process_geo` com as formas carregadas
 * @param txt arquivo .txt para saída de mensagens/relatórios
 */
void process_qry(FILE *qry, const char* dir_saida, const char* nome_base, Ground ground, FILE *txt);

#endif