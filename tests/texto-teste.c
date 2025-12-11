/*
 * Testes elaborados do módulo: texto
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/lib/formas/texto/texto.h"

#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)
#define EPSILON 0.0001
#define FLOAT_IGUAL(a, b) (fabs((a) - (b)) < EPSILON)

static int testes_passados = 0;
static int testes_falhados = 0;

void teste_criar_texto() {
    Texto* t = criar_texto(10.0, 20.0, "black", "white", 'm', "Hello", "Arial", 1);
    if (t != NULL) { TESTE_PASSOU("criar_texto"); testes_passados++; liberar_texto(t); }
    else { TESTE_FALHOU("criar_texto", "Não criado"); testes_falhados++; }
}

void teste_get_posicao() {
    Texto* t = criar_texto(15.5, 25.5, "black", "white", 'm', "Test", "Arial", 2);
    if (FLOAT_IGUAL(get_x_texto(t), 15.5) && FLOAT_IGUAL(get_y_texto(t), 25.5)) {
        TESTE_PASSOU("get_x/y_texto"); testes_passados++;
    } else { TESTE_FALHOU("get_x/y_texto", "Incorreto"); testes_falhados++; }
    liberar_texto(t);
}

void teste_get_id() {
    Texto* t = criar_texto(0.0, 0.0, "black", "white", 'm', "Test", "Arial", 42);
    if (get_id_texto(t) == 42) { TESTE_PASSOU("get_id_texto"); testes_passados++; }
    else { TESTE_FALHOU("get_id_texto", "ID incorreto"); testes_falhados++; }
    liberar_texto(t);
}

void teste_get_conteudo() {
    Texto* t = criar_texto(0.0, 0.0, "black", "white", 'm', "Hello World", "Arial", 3);
    if (strcmp(get_conteudo_texto(t), "Hello World") == 0) {
        TESTE_PASSOU("get_conteudo_texto"); testes_passados++;
    } else { TESTE_FALHOU("get_conteudo_texto", "Incorreto"); testes_falhados++; }
    liberar_texto(t);
}

void teste_area_texto() {
    Texto* t = criar_texto(0.0, 0.0, "black", "white", 'm', "Hello", "Arial", 4);
    float esperado = 20.0 * 5; /* 5 caracteres */
    if (FLOAT_IGUAL(area_texto(t), esperado)) { TESTE_PASSOU("area_texto"); testes_passados++; }
    else { TESTE_FALHOU("area_texto", "Área incorreta"); testes_falhados++; }
    liberar_texto(t);
}

void teste_anchor() {
    Texto* t = criar_texto(0.0, 0.0, "black", "white", 'i', "Test", "Arial", 5);
    if (get_anchor_texto(t) == 'i') { TESTE_PASSOU("get_anchor_texto"); testes_passados++; }
    else { TESTE_FALHOU("get_anchor_texto", "Incorreto"); testes_falhados++; }
    liberar_texto(t);
}

void teste_cores() {
    Texto* t = criar_texto(0.0, 0.0, "#000000", "#FFFFFF", 'm', "Test", "Arial", 6);
    if (strcmp(get_corBorda_texto(t), "#000000") == 0 &&
        strcmp(get_corPreenchimento_texto(t), "#FFFFFF") == 0) {
        TESTE_PASSOU("get_cor texto"); testes_passados++;
    } else { TESTE_FALHOU("get_cor texto", "Incorreto"); testes_falhados++; }
    
    set_corBorda_texto(t, "#FF0000");
    set_corPreenchimento_texto(t, "#00FF00");
    if (strcmp(get_corBorda_texto(t), "#FF0000") == 0 &&
        strcmp(get_corPreenchimento_texto(t), "#00FF00") == 0) {
        TESTE_PASSOU("set_cor texto"); testes_passados++;
    } else { TESTE_FALHOU("set_cor texto", "Incorreto"); testes_falhados++; }
    liberar_texto(t);
}

void teste_fonte() {
    Texto* t = criar_texto(0.0, 0.0, "black", "white", 'm', "Test", "Arial", 7);
    if (strcmp(get_fonte_texto(t), "Arial") == 0) { TESTE_PASSOU("get_fonte_texto"); testes_passados++; }
    else { TESTE_FALHOU("get_fonte_texto", "Incorreto"); testes_falhados++; }
    
    set_fonte_texto(t, "Times New Roman");
    if (strcmp(get_fonte_texto(t), "Times New Roman") == 0) {
        TESTE_PASSOU("set_fonte_texto"); testes_passados++;
    } else { TESTE_FALHOU("set_fonte_texto", "Incorreto"); testes_falhados++; }
    liberar_texto(t);
}

int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO TEXTO\n");
    printf("========================================\n\n");
    teste_criar_texto();
    teste_get_posicao();
    teste_get_id();
    teste_get_conteudo();
    teste_area_texto();
    teste_anchor();
    teste_cores();
    teste_fonte();
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    return testes_falhados > 0 ? 1 : 0;
}
