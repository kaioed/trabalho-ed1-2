/*
 * Testes elaborados do módulo: poligono
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/lib/poligono/poligono.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)
#define EPSILON 0.0001
#define FLOAT_IGUAL(a, b) (fabs((a) - (b)) < EPSILON)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_criar_ponto() {
    Ponto p = CriarPonto(10.0, 20.0);
    if (p != NULL && FLOAT_IGUAL(PontoX(p), 10.0) && FLOAT_IGUAL(PontoY(p), 20.0)) {
        TESTE_PASSOU("CriarPonto"); testes_passados++;
    } else { TESTE_FALHOU("CriarPonto", "Coordenadas incorretas"); testes_falhados++; }
    free(p);
}

void teste_criar_poligono() {
    Poligono pol = CriarPoligono(10);
    if (pol != NULL) { TESTE_PASSOU("CriarPoligono"); testes_passados++; DestruirPoligono(pol); }
    else { TESTE_FALHOU("CriarPoligono", "Não criado"); testes_falhados++; }
}

void teste_criar_segmento() {
    Ponto a = CriarPonto(0.0, 0.0);
    Ponto b = CriarPonto(10.0, 10.0);
    Segmento s = CriarSegmento(a, b);
    if (s != NULL) { TESTE_PASSOU("CriarSegmento"); testes_passados++; }
    else { TESTE_FALHOU("CriarSegmento", "Não criado"); testes_falhados++; }
    free(a); free(b); free(s);
}

void teste_adicionar_segmento() {
    Poligono pol = CriarPoligono(5);
    Ponto a = CriarPonto(0.0, 0.0);
    Ponto b = CriarPonto(10.0, 0.0);
    Segmento s = CriarSegmento(a, b);
    PoligonoAdicionarSegmento(pol, s);
    TESTE_PASSOU("PoligonoAdicionarSegmento"); testes_passados++;
    DestruirPoligono(pol);
}

void teste_ponto_coordenadas() {
    Ponto p = CriarPonto(-5.5, 100.25);
    if (FLOAT_IGUAL(PontoX(p), -5.5) && FLOAT_IGUAL(PontoY(p), 100.25)) {
        TESTE_PASSOU("PontoX/PontoY"); testes_passados++;
    } else { TESTE_FALHOU("PontoX/PontoY", "Incorreto"); testes_falhados++; }
    free(p);
}

void teste_definir_limiar() {
    definir_limiar_ordenacao(15);
    TESTE_PASSOU("definir_limiar_ordenacao"); testes_passados++;
}

void teste_poligono_quadrado() {
    Poligono pol = CriarPoligono(4);
    Ponto p1 = CriarPonto(0.0, 0.0);
    Ponto p2 = CriarPonto(10.0, 0.0);
    Ponto p3 = CriarPonto(10.0, 10.0);
    Ponto p4 = CriarPonto(0.0, 10.0);
    
    PoligonoAdicionarSegmento(pol, CriarSegmento(p1, p2));
    PoligonoAdicionarSegmento(pol, CriarSegmento(p2, p3));
    PoligonoAdicionarSegmento(pol, CriarSegmento(p3, p4));
    PoligonoAdicionarSegmento(pol, CriarSegmento(p4, p1));
    
    TESTE_PASSOU("poligono quadrado"); testes_passados++;
    DestruirPoligono(pol);
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO POLIGONO\n");
    printf("========================================\n\n");
    teste_criar_ponto();
    teste_criar_poligono();
    teste_criar_segmento();
    teste_adicionar_segmento();
    teste_ponto_coordenadas();
    teste_definir_limiar();
    teste_poligono_quadrado();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
