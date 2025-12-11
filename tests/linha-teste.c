/*
 * Testes elaborados do módulo: linha
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/lib/formas/linha/linha.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)
#define EPSILON 0.0001
#define FLOAT_IGUAL(a, b) (fabs((a) - (b)) < EPSILON)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_criar_linha() {
    Linha* l = criar_linha(0.0, 0.0, 10.0, 10.0, "black", 1);
    if (l != NULL) { TESTE_PASSOU("criar_linha"); testes_passados++; liberar_linha(l); }
    else { TESTE_FALHOU("criar_linha", "Linha não criada"); testes_falhados++; }
}

void teste_get_coordenadas() {
    Linha* l = criar_linha(5.5, 10.0, 20.0, 30.0, "red", 2);
    float x1 = get_x1_linha(l), y1 = get_y1_linha(l);
    float x2 = get_x2_linha(l), y2 = get_y2_linha(l);
    if (FLOAT_IGUAL(x1, 5.5) && FLOAT_IGUAL(y1, 10.0) && 
        FLOAT_IGUAL(x2, 20.0) && FLOAT_IGUAL(y2, 30.0)) {
        TESTE_PASSOU("get_coordenadas"); testes_passados++;
    } else { TESTE_FALHOU("get_coordenadas", "Incorreto"); testes_falhados++; }
    liberar_linha(l);
}

void teste_get_id() {
    Linha* l = criar_linha(0.0, 0.0, 10.0, 10.0, "black", 99);
    if (get_id_linha(l) == 99) { TESTE_PASSOU("get_id_linha"); testes_passados++; }
    else { TESTE_FALHOU("get_id_linha", "ID incorreto"); testes_falhados++; }
    liberar_linha(l);
}

void teste_comprimento() {
    Linha* l = criar_linha(0.0, 0.0, 3.0, 4.0, "black", 3);
    if (FLOAT_IGUAL(comprimento_linha(l), 5.0)) { TESTE_PASSOU("comprimento_linha"); testes_passados++; }
    else { TESTE_FALHOU("comprimento_linha", "Incorreto"); testes_falhados++; }
    liberar_linha(l);
}

void teste_area() {
    Linha* l = criar_linha(0.0, 0.0, 3.0, 4.0, "black", 4);
    if (FLOAT_IGUAL(area_linha(l), 10.0)) { TESTE_PASSOU("area_linha"); testes_passados++; }
    else { TESTE_FALHOU("area_linha", "Incorreto"); testes_falhados++; }
    liberar_linha(l);
}

void teste_cor() {
    Linha* l = criar_linha(0.0, 0.0, 10.0, 10.0, "#FF5500", 5);
    if (strcmp(get_cor_linha(l), "#FF5500") == 0) { TESTE_PASSOU("get_cor_linha"); testes_passados++; }
    else { TESTE_FALHOU("get_cor_linha", "Incorreto"); testes_falhados++; }
    set_cor_linha(l, "#00FF00");
    if (strcmp(get_cor_linha(l), "#00FF00") == 0) { TESTE_PASSOU("set_cor_linha"); testes_passados++; }
    else { TESTE_FALHOU("set_cor_linha", "Incorreto"); testes_falhados++; }
    liberar_linha(l);
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO LINHA\n");
    printf("========================================\n\n");
    teste_criar_linha();
    teste_get_coordenadas();
    teste_get_id();
    teste_comprimento();
    teste_area();
    teste_cor();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
