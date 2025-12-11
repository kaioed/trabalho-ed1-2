/*
 * Testes elaborados do módulo: retangulo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/lib/formas/retangulo/retangulo.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)
#define EPSILON 0.0001
#define FLOAT_IGUAL(a, b) (fabs((a) - (b)) < EPSILON)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_criar_retangulo() {
    Retangulo* r = criar_retangulo(10.0, 20.0, 100.0, 50.0, "red", "blue", 1);
    if (r != NULL) { TESTE_PASSOU("criar_retangulo"); testes_passados++; liberar_retangulo(r); }
    else { TESTE_FALHOU("criar_retangulo", "Não criado"); testes_falhados++; }
}

void teste_get_posicao() {
    Retangulo* r = criar_retangulo(15.5, 25.5, 100.0, 50.0, "red", "blue", 2);
    if (FLOAT_IGUAL(get_x_retangulo(r), 15.5) && FLOAT_IGUAL(get_y_retangulo(r), 25.5)) {
        TESTE_PASSOU("get_x/y_retangulo"); testes_passados++;
    } else { TESTE_FALHOU("get_x/y_retangulo", "Incorreto"); testes_falhados++; }
    liberar_retangulo(r);
}

void teste_get_dimensoes() {
    Retangulo* r = criar_retangulo(0.0, 0.0, 100.0, 50.0, "red", "blue", 3);
    if (FLOAT_IGUAL(get_largura(r), 100.0) && FLOAT_IGUAL(get_altura(r), 50.0)) {
        TESTE_PASSOU("get_largura/altura"); testes_passados++;
    } else { TESTE_FALHOU("get_largura/altura", "Incorreto"); testes_falhados++; }
    liberar_retangulo(r);
}

void teste_get_id() {
    Retangulo* r = criar_retangulo(0.0, 0.0, 10.0, 10.0, "red", "blue", 42);
    if (get_id_retangulo(r) == 42) { TESTE_PASSOU("get_id_retangulo"); testes_passados++; }
    else { TESTE_FALHOU("get_id_retangulo", "ID incorreto"); testes_falhados++; }
    liberar_retangulo(r);
}

void teste_area_retangulo() {
    Retangulo* r = criar_retangulo(0.0, 0.0, 10.0, 5.0, "red", "blue", 4);
    if (FLOAT_IGUAL(area_retangulo(r), 50.0)) { TESTE_PASSOU("area_retangulo"); testes_passados++; }
    else { TESTE_FALHOU("area_retangulo", "Área incorreta"); testes_falhados++; }
    liberar_retangulo(r);
}

void teste_cores() {
    Retangulo* r = criar_retangulo(0.0, 0.0, 10.0, 10.0, "#FF0000", "#0000FF", 5);
    if (strcmp(get_corPreenchimento_retangulo(r), "#FF0000") == 0 &&
        strcmp(get_corBorda_retangulo(r), "#0000FF") == 0) {
        TESTE_PASSOU("get_cor retangulo"); testes_passados++;
    } else { TESTE_FALHOU("get_cor retangulo", "Incorreto"); testes_falhados++; }
    
    set_corPreenchimento_retangulo(r, "#00FF00");
    set_corBorda_retangulo(r, "#FFFF00");
    if (strcmp(get_corPreenchimento_retangulo(r), "#00FF00") == 0 &&
        strcmp(get_corBorda_retangulo(r), "#FFFF00") == 0) {
        TESTE_PASSOU("set_cor retangulo"); testes_passados++;
    } else { TESTE_FALHOU("set_cor retangulo", "Incorreto"); testes_falhados++; }
    liberar_retangulo(r);
}

void teste_retangulo_quadrado() {
    Retangulo* r = criar_retangulo(0.0, 0.0, 10.0, 10.0, "red", "blue", 6);
    if (FLOAT_IGUAL(area_retangulo(r), 100.0)) { TESTE_PASSOU("retangulo quadrado"); testes_passados++; }
    else { TESTE_FALHOU("retangulo quadrado", "Área incorreta"); testes_falhados++; }
    liberar_retangulo(r);
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO RETANGULO\n");
    printf("========================================\n\n");
    teste_criar_retangulo();
    teste_get_posicao();
    teste_get_dimensoes();
    teste_get_id();
    teste_area_retangulo();
    teste_cores();
    teste_retangulo_quadrado();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
