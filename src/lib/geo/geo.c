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
    inserir_fim_lista(&((GroundStruct*)g)->formas, f);
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
            
            // CORREÇÃO: Tratamento da âncora (text-anchor)
            char anchor_svg[10] = "start";
            char a = get_anchor_texto(t);
            if (a == 'm') strcpy(anchor_svg, "middle");
            else if (a == 'f') strcpy(anchor_svg, "end");

            fprintf(svg,
                "<text x=\"%f\" y=\"%f\" fill=\"%s\" stroke=\"%s\" text-anchor=\"%s\">%s</text>\n",
                get_x_texto(t), get_y_texto(t),
                get_corPreenchimento_texto(t), // Fill é o preenchimento
                get_corBorda_texto(t),         // Stroke é a borda
                anchor_svg,
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

    while (remover_inicio_lista(&gr->formas, &val)) {
        free(val);
    }

    while (remover_inicio_lista(&gr->clones, &val)) {
       // Não libera o valor (wrapper) aqui pois é o mesmo ponteiro da lista formas
    }

    liberar_lista(&gr->formas);
    liberar_lista(&gr->clones);

    free(gr);
}

Ground process_geo(FILE *geo, FILE *svg) {
    Ground g = criar_ground(svg);
    char comando[10];
    
    while (fscanf(geo, "%s", comando) != EOF) {
        if (strcmp(comando, "c") == 0) {
            int id;
            float x, y, r;
            char corB[32], corP[32];
            
            fscanf(geo, "%d %f %f %f %s %s", &id, &x, &y, &r, corB, corP);
            
            Circulo c = criar_circulo(x, y, r, corP, corB, id);
            ground_inserir_forma(g, TIPO_CIRCULO, c, id);
        }
        else if (strcmp(comando, "r") == 0) {
            int id;
            float x, y, w, h;
            char corB[32], corP[32];
            
            fscanf(geo, "%d %f %f %f %f %s %s", &id, &x, &y, &w, &h, corB, corP);
            
            Retangulo ret = criar_retangulo(x, y, w, h, corP, corB, id);
            ground_inserir_forma(g, TIPO_RETANGULO, ret, id);
        }
        else if (strcmp(comando, "l") == 0) {
            int id;
            float x1, y1, x2, y2;
            char cor[32];
            
            fscanf(geo, "%d %f %f %f %f %s", &id, &x1, &y1, &x2, &y2, cor);
            
            Linha l = criar_linha(x1, y1, x2, y2, cor, id);
            ground_inserir_forma(g, TIPO_LINHA, l, id);
        }
        else if (strcmp(comando, "t") == 0) {
            int id;
            float x, y;
            char corB[32], corP[32];
            char ancora;
            char conteudo[256];
            
            // CORREÇÃO: Espaço antes de %c para ignorar whitespace anterior
            fscanf(geo, "%d %f %f %s %s %c", &id, &x, &y, corB, corP, &ancora);
            
            // Lê o resto da linha (o texto em si)
            fgets(conteudo, 256, geo);
            size_t len = strlen(conteudo);
            if (len > 0 && conteudo[len-1] == '\n') conteudo[len-1] = '\0';
            
            // Remove carriage return se existir (Windows/WSL)
            if (len > 1 && conteudo[len-2] == '\r') conteudo[len-2] = '\0';

            Texto t = criar_texto(x, y, corB, corP, ancora, conteudo, "sans-serif", id);
            ground_inserir_forma(g, TIPO_TEXTO, t, id);
        }
        else if (strcmp(comando, "ts") == 0) {
            char buffer[256];
            fgets(buffer, 256, geo);
        }
    }

    if (svg) {
        fprintf(svg, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\">\n");
        ground_escrever_svg(g);
        fprintf(svg, "</svg>\n");
    }

    return g;
}