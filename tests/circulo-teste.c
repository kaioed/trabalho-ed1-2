/*
 * Testes elaborados do módulo: circulo
 * Testa criação, getters, setters e cálculo de área
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/lib/formas/circulo/circulo.h"

/* ===================== MACROS DE TESTE ===================== */
#define TESTE_PASSOU(nome) printf("[PASSOU] %s\n", nome)
#define TESTE_FALHOU(nome, msg) printf("[FALHOU] %s: %s\n", nome, msg)
#define EPSILON 0.0001
#define FLOAT_IGUAL(a, b) (fabs((a) - (b)) < EPSILON)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int testes_passados = 0;
static int testes_falhados = 0;

/* ===================== TESTES ===================== */

/**
 * Teste 1: Criação de círculo
 */
void teste_criar_circulo() {
    const char* nome = "criar_circulo";
    
    Circulo* c = criar_circulo(10.0, 20.0, 5.0, "red", "blue", 1);
    
    if (c != NULL) {
        TESTE_PASSOU(nome);
        testes_passados++;
        liberar_circulo(c);
    } else {
        TESTE_FALHOU(nome, "Círculo não foi criado");
        testes_falhados++;
    }
}

/**
 * Teste 2: get_x
 */
void teste_get_x() {
    const char* nome = "get_x";
    
    Circulo* c = criar_circulo(15.5, 25.5, 10.0, "red", "blue", 2);
    float x = get_x(c);
    
    if (FLOAT_IGUAL(x, 15.5)) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Coordenada X incorreta");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 3: get_y
 */
void teste_get_y() {
    const char* nome = "get_y";
    
    Circulo* c = criar_circulo(15.5, 25.5, 10.0, "red", "blue", 3);
    float y = get_y(c);
    
    if (FLOAT_IGUAL(y, 25.5)) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Coordenada Y incorreta");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 4: get_raio
 */
void teste_get_raio() {
    const char* nome = "get_raio";
    
    Circulo* c = criar_circulo(0.0, 0.0, 7.5, "red", "blue", 4);
    float r = get_raio(c);
    
    if (FLOAT_IGUAL(r, 7.5)) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Raio incorreto");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 5: get_id_circulo
 */
void teste_get_id() {
    const char* nome = "get_id_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 5.0, "red", "blue", 42);
    int id = get_id_circulo(c);
    
    if (id == 42) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "ID incorreto");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 6: area_circulo
 */
void teste_area_circulo() {
    const char* nome = "area_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 2.0, "red", "blue", 5);
    float area = area_circulo(c);
    float area_esperada = M_PI * 2.0 * 2.0;
    
    if (FLOAT_IGUAL(area, area_esperada)) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        char msg[100];
        snprintf(msg, 100, "Área esperada %.4f, obtida %.4f", area_esperada, area);
        TESTE_FALHOU(nome, msg);
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 7: get_corPreenchimento_circulo
 */
void teste_get_corPreenchimento() {
    const char* nome = "get_corPreenchimento_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 5.0, "#FF0000", "#0000FF", 6);
    const char* cor = get_corPreenchimento_circulo(c);
    
    if (cor != NULL && strcmp(cor, "#FF0000") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Cor de preenchimento incorreta");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 8: get_corBorda_circulo
 */
void teste_get_corBorda() {
    const char* nome = "get_corBorda_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 5.0, "#FF0000", "#0000FF", 7);
    const char* cor = get_corBorda_circulo(c);
    
    if (cor != NULL && strcmp(cor, "#0000FF") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Cor de borda incorreta");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 9: set_corPreenchimento_circulo
 */
void teste_set_corPreenchimento() {
    const char* nome = "set_corPreenchimento_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 5.0, "red", "blue", 8);
    set_corPreenchimento_circulo(c, "#00FF00");
    const char* cor = get_corPreenchimento_circulo(c);
    
    if (cor != NULL && strcmp(cor, "#00FF00") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Cor de preenchimento não foi alterada corretamente");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 10: set_corBorda_circulo
 */
void teste_set_corBorda() {
    const char* nome = "set_corBorda_circulo";
    
    Circulo* c = criar_circulo(0.0, 0.0, 5.0, "red", "blue", 9);
    set_corBorda_circulo(c, "#FFFF00");
    const char* cor = get_corBorda_circulo(c);
    
    if (cor != NULL && strcmp(cor, "#FFFF00") == 0) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Cor de borda não foi alterada corretamente");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 11: Círculo com raio zero
 */
void teste_raio_zero() {
    const char* nome = "criar_circulo com raio zero";
    
    Circulo* c = criar_circulo(5.0, 5.0, 0.0, "red", "blue", 10);
    float area = area_circulo(c);
    
    if (FLOAT_IGUAL(area, 0.0)) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "Área deveria ser zero para raio zero");
        testes_falhados++;
    }
    
    liberar_circulo(c);
}

/**
 * Teste 12: Múltiplos círculos
 */
void teste_multiplos_circulos() {
    const char* nome = "criar múltiplos círculos";
    
    Circulo* c1 = criar_circulo(0.0, 0.0, 1.0, "red", "blue", 1);
    Circulo* c2 = criar_circulo(10.0, 10.0, 2.0, "green", "yellow", 2);
    Circulo* c3 = criar_circulo(20.0, 20.0, 3.0, "black", "white", 3);
    
    int id1 = get_id_circulo(c1);
    int id2 = get_id_circulo(c2);
    int id3 = get_id_circulo(c3);
    
    if (id1 == 1 && id2 == 2 && id3 == 3) {
        TESTE_PASSOU(nome);
        testes_passados++;
    } else {
        TESTE_FALHOU(nome, "IDs dos círculos incorretos");
        testes_falhados++;
    }
    
    liberar_circulo(c1);
    liberar_circulo(c2);
    liberar_circulo(c3);
}

/* ===================== MAIN ===================== */
int main() {
    printf("========================================\n");
    printf("   TESTES DO MÓDULO CIRCULO\n");
    printf("========================================\n\n");
    
    teste_criar_circulo();
    teste_get_x();
    teste_get_y();
    teste_get_raio();
    teste_get_id();
    teste_area_circulo();
    teste_get_corPreenchimento();
    teste_get_corBorda();
    teste_set_corPreenchimento();
    teste_set_corBorda();
    teste_raio_zero();
    teste_multiplos_circulos();
    
    printf("\n========================================\n");
    printf("   RESUMO: %d passados, %d falhados\n", testes_passados, testes_falhados);
    printf("========================================\n");
    
    return testes_falhados > 0 ? 1 : 0;
}
