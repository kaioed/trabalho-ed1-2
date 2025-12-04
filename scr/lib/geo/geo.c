#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "geo.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"
#include "../lista/lista.h"

typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;
    bool foi_destruida;
    bool foi_clonada;
    float x_centro;
    float y_centro;
    int id_disparador_origem;
} FormaStruct;

typedef struct {
    Lista formas;
    Lista clones;
    FILE* svg;
} GroundStruct;

Ground criar_ground(FILE* svg) {
    GroundStruct* g = malloc(sizeof(GroundStruct));
    g->svg = svg;
    iniciar_lista(&g->formas);
    iniciar_lista(&g->clones);
    return g;
}

Lista get_ground_lista_formas(Ground g) {
    return ((GroundStruct*)g)->formas;
}

Lista get_ground_lista_clones(Ground g) {
    return ((GroundStruct*)g)->clones;
}

void* criar_forma_wrapper(TipoForma tipo, void* forma, int id) {
    FormaStruct* f = malloc(sizeof(FormaStruct));
    f->id_original = id;
    f->tipo = tipo;
    f->dados_forma = forma;
    f->foi_destruida = false;
    f->foi_clonada = false;
    f->id_disparador_origem = -1;

    switch (tipo) {
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)forma;
            f->x_centro = get_x(c);
            f->y_centro = get_y(c);
        } break;

        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)forma;
            float x = get_x_retangulo(r);
            float y = get_y_retangulo(r);
            float w = get_largura(r);
            float h = get_altura(r);
            f->x_centro = x + w/2.0;
            f->y_centro = y + h/2.0;
        } break;

        case TIPO_LINHA: {
            Linha l = (Linha)forma;
            f->x_centro = (get_x1_linha(l) + get_x2_linha(l)) / 2.0;
            f->y_centro = (get_y1_linha(l) + get_y2_linha(l)) / 2.0;
        } break;

        case TIPO_TEXTO: {
            Texto t = (Texto)forma;
            f->x_centro = get_x_texto(t);
            f->y_centro = get_y_texto(t);
        } break;
    }

    return f;
}

void ground_inserir_forma(Ground g, TipoForma tipo, void* forma, int id) {
    FormaStruct* f = criar_forma_wrapper(tipo, forma, id);
    inserir_fim_lista(((GroundStruct*)g)->formas, f);
}

void escrever_svg_forma(FILE* svg, FormaStruct* f) {
    switch (f->tipo) {
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)f->dados_forma;
            fprintf(svg,
                "<circle cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"%s\" stroke=\"%s\" />\n",
                get_x(c), get_y(c), get_raio(c),
                get_corPreenchimento_circulo(c),
                get_corBorda_circulo(c));
        } break;

        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)f->dados_forma;
            fprintf(svg,
                "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"%s\" stroke=\"%s\" />\n",
                get_x_retangulo(r), get_y_retangulo(r),
                get_largura(r), get_altura(r),
                get_corPreenchimento_retangulo(r),
                get_corBorda_retangulo(r));
        } break;

        case TIPO_LINHA: {
            Linha l = (Linha)f->dados_forma;
            fprintf(svg,
                "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"%s\" />\n",
                get_x1_linha(l), get_y1_linha(l),
                get_x2_linha(l), get_y2_linha(l),
                get_cor_linha(l));
        } break;

        case TIPO_TEXTO: {
            Texto t = (Texto)f->dados_forma;
            fprintf(svg,
                "<text x=\"%f\" y=\"%f\" fill=\"%s\">%s</text>\n",
                get_x_texto(t), get_y_texto(t),
                get_corBorda_texto(t),
                get_conteudo_texto(t));
        } break;
    }
}

void ground_escrever_svg(Ground g) {
    GroundStruct* gr = (GroundStruct*)g;
    Lista lst = gr->formas;
    Posic p = get_primeiro_lista(lst);

    while (p) {
        FormaStruct* f = get_valor_lista(lst, p);
        if (!f->foi_destruida)
            escrever_svg_forma(gr->svg, f);
        p = get_proximo_lista(lst, p);
    }
}

void destruir_ground(Ground g) {
    GroundStruct* gr = (GroundStruct*)g;

    void* val;
    while (remover_inicio_lista(&gr->formas, &val))
        free(val);

    while (remover_inicio_lista(&gr->clones, &val))
        free(val);

    free(gr);
}
