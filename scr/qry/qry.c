#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h"
#include "../lib/geo/geo.h"
#include "../lib/poligono/poligono.h"
#include "../lib/lista/lista.h"
#include "../lib/formas/circulo/circulo.h"
#include "../lib/formas/retangulo/retangulo.h"
#include "../lib/formas/linha/linha.h"
#include "../lib/formas/texto/texto.h"


typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;     
    bool foi_destruida;
    bool foi_clonada;
    float x_centro, y_centro; 
    int id_disparador_origem;
} FormaStruct;

// Variável estática para gerar IDs de clones
static int proximo_id_clone = 20000;


bool ponto_dentro_visibilidade(Visibilidade vis, double x, double y) {
    if (!vis) return false;
    int n = VisibilidadeNumPontos(vis);
    if (n < 3) return false;

    bool inside = false;
    // Algoritmo Ray Casting (Jordan Curve Theorem)
    for (int i = 0, j = n - 1; i < n; j = i++) {
        Ponto pi = VisibilidadeGetPonto(vis, i);
        Ponto pj = VisibilidadeGetPonto(vis, j);
        
        double xi = PontoX(pi); 
        double yi = PontoY(pi);
        double xj = PontoX(pj); 
        double yj = PontoY(pj);

        // O 'VisibilidadeGetPonto' cria uma cópia, precisamos liberar
        free(pi);
        free(pj);

        if (((yi > y) != (yj > y)) &&
            (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
            inside = !inside;
        }
    }
    return inside;
}

// Desenha o polígono de visibilidade no SVG
void desenhar_visibilidade_svg(FILE *svg, Visibilidade vis, double x_bomb, double y_bomb, const char* cor) {
    if (!svg || !vis) return;
    
    fprintf(svg, "<polygon points=\"");
    int n = VisibilidadeNumPontos(vis);
    for (int i = 0; i < n; i++) {
        Ponto p = VisibilidadeGetPonto(vis, i);
        fprintf(svg, "%.2f,%.2f ", PontoX(p), PontoY(p));
        free(p);
    }
    // Fecha o polígono e define estilo translúcido
    fprintf(svg, "\" style=\"fill:%s;fill-opacity:0.3;stroke:none\" />\n", cor);
    
    // Marca o centro da bomba
    fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"3\" fill=\"red\" />\n", x_bomb, y_bomb);
}

void adicionar_segmento_no_poligono(Poligono poly, double x1, double y1, double x2, double y2) {
    Ponto p1 = CriarPonto(x1, y1);
    Ponto p2 = CriarPonto(x2, y2);
    if (p1 && p2) {
        Segmento seg = CriarSegmento(p1, p2);
        if (seg) {
            PoligonoAdicionarSegmento(poly, seg);
            free(seg);
        }
    }
    free(p1);
    free(p2);
}

void transformar_forma_em_segmentos(Poligono poly, FormaStruct *f, char orientacao, FILE *txt) {
    if (!f || f->foi_destruida) return;

    switch (f->tipo) {
        case TIPO_LINHA: {
            Linha l = (Linha)f->dados_forma;
            adicionar_segmento_no_poligono(poly, get_x1_linha(l), get_y1_linha(l), get_x2_linha(l), get_y2_linha(l));
            if (txt) fprintf(txt, "Anteparo Linha ID:%d\n", f->id_original);
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)f->dados_forma;
            double x = get_x_retangulo(r);
            double y = get_y_retangulo(r);
            double w = get_largura(r);
            double h = get_altura(r);

            adicionar_segmento_no_poligono(poly, x, y, x+w, y);     // Topo
            adicionar_segmento_no_poligono(poly, x+w, y, x+w, y+h); // Direita
            adicionar_segmento_no_poligono(poly, x+w, y+h, x, y+h); // Base
            adicionar_segmento_no_poligono(poly, x, y+h, x, y);     // Esquerda
            
            if (txt) fprintf(txt, "Anteparo Retangulo ID:%d\n", f->id_original);
            break;
        }
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)f->dados_forma;
            double x = get_x(c);
            double y = get_y(c);
            double r = get_raio(c);

            if (orientacao == 'h') { 
                adicionar_segmento_no_poligono(poly, x - r, y, x + r, y);
            } else { // 'v'
                adicionar_segmento_no_poligono(poly, x, y - r, x, y + r);
            }
            if (txt) fprintf(txt, "Anteparo Circulo ID:%d (%c)\n", f->id_original, orientacao);
            break;
        }
        case TIPO_TEXTO: {
            Texto t = (Texto)f->dados_forma;
            double x = get_x_texto(t);
            double y = get_y_texto(t);
            char anchor = get_anchor_texto(t);
            const char* cont = get_conteudo_texto(t);
            double tamanho = 10.0 * (cont ? strlen(cont) : 0);
            
            double x1 = x, x2 = x;
            if (anchor == 'i') x2 = x + tamanho;
            else if (anchor == 'f') x1 = x - tamanho;
            else { x1 = x - tamanho/2; x2 = x + tamanho/2; } // 'm'

            adicionar_segmento_no_poligono(poly, x1, y, x2, y);
            if (txt) fprintf(txt, "Anteparo Texto ID:%d\n", f->id_original);
            break;
        }
    }
}



FormaStruct *clonar_forma_struct(FormaStruct *original, float dx, float dy) {
    if (!original || !original->dados_forma) return NULL;

    int id_clone = proximo_id_clone++;
    void *dados_clonados = NULL;

    float nx = original->x_centro + dx;
    float ny = original->y_centro + dy;

    switch (original->tipo) {
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)original->dados_forma;
            dados_clonados = criar_circulo(nx, ny, get_raio(c), 
                                           (char*)get_corPreenchimento_circulo(c), 
                                           (char*)get_corBorda_circulo(c), id_clone);
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)original->dados_forma;
            // Retangulo usa (x,y) do canto superior esquerdo, ajustamos o centro
            float w = get_largura(r);
            float h = get_altura(r);
            dados_clonados = criar_retangulo(nx - w/2, ny - h/2, w, h,
                                             (char*)get_corPreenchimento_retangulo(r),
                                             (char*)get_corBorda_retangulo(r), id_clone);
            break;
        }
        case TIPO_LINHA: {
            Linha l = (Linha)original->dados_forma;
            float w = get_x2_linha(l) - get_x1_linha(l);
            float h = get_y2_linha(l) - get_y1_linha(l);
            // Linha é definida por P1, o centro é (P1+P2)/2. 
            // Simplificação: desloca P1 e P2 pelo dx, dy
            float x1 = get_x1_linha(l) + dx;
            float y1 = get_y1_linha(l) + dy;
            float x2 = get_x2_linha(l) + dx;
            float y2 = get_y2_linha(l) + dy;
            dados_clonados = criar_linha(x1, y1, x2, y2, (char*)get_cor_linha(l), id_clone);
            break;
        }
        case TIPO_TEXTO: {
            Texto t = (Texto)original->dados_forma;
            dados_clonados = criar_texto(nx, ny,
                                         (char*)get_corBorda_texto(t), (char*)get_corPreenchimento_texto(t),
                                         get_anchor_texto(t), (char*)get_conteudo_texto(t), 
                                         get_fonte_texto(t), id_clone);
            break;
        }
    }

    if (!dados_clonados) return NULL;

    FormaStruct *wrapper = malloc(sizeof(FormaStruct));
    wrapper->id_original = id_clone;
    wrapper->tipo = original->tipo;
    wrapper->dados_forma = dados_clonados;
    wrapper->foi_destruida = false;
    wrapper->foi_clonada = true;
    wrapper->x_centro = nx;
    wrapper->y_centro = ny;
    wrapper->id_disparador_origem = -1;
    
    return wrapper;
}

void adicionar_retangulo_limite(Poligono poly, Lista lista_formas) {
    if (!poly || !lista_formas) return;

   
    double xmin =  1e30, ymin =  1e30;
    double xmax = -1e30, ymax = -1e30;

    for (Posic p = get_primeiro_lista(lista_formas); p; p = get_proximo_lista(lista_formas, p)) {

        FormaStruct* f = (FormaStruct*) get_valor_lista(lista_formas, p);
        if (!f || f->foi_destruida) continue;

        switch (f->tipo) {

            case TIPO_CIRCULO: {
                Circulo c = (Circulo)f->dados_forma;
                double x = get_x(c), y = get_y(c), r = get_raio(c);

                xmin = fmin(xmin, x - r);
                xmax = fmax(xmax, x + r);
                ymin = fmin(ymin, y - r);
                ymax = fmax(ymax, y + r);
                break;
            }

            case TIPO_RETANGULO: {
                Retangulo r = (Retangulo)f->dados_forma;
                double x = get_x_retangulo(r);
                double y = get_y_retangulo(r);
                double w = get_largura(r);
                double h = get_altura(r);

                xmin = fmin(xmin, x);
                xmax = fmax(xmax, x + w);
                ymin = fmin(ymin, y);
                ymax = fmax(ymax, y + h);
                break;
            }

            case TIPO_LINHA: {
                Linha l = (Linha)f->dados_forma;
                double x1 = get_x1_linha(l);
                double y1 = get_y1_linha(l);
                double x2 = get_x2_linha(l);
                double y2 = get_y2_linha(l);

                xmin = fmin(xmin, fmin(x1, x2));
                xmax = fmax(xmax, fmax(x1, x2));
                ymin = fmin(ymin, fmin(y1, y2));
                ymax = fmax(ymax, fmax(y1, y2));
                break;
            }

            case TIPO_TEXTO: {
                Texto t = (Texto)f->dados_forma;
                double x = get_x_texto(t);
                double y = get_y_texto(t);
                double len = 10.0 * strlen(get_conteudo_texto(t));

                xmin = fmin(xmin, x - len);
                xmax = fmax(xmax, x + len);
                ymin = fmin(ymin, y - 5);
                ymax = fmax(ymax, y + 5);
                break;
            }
        }
    }


    double dx = (xmax - xmin) * 0.10;
    double dy = (ymax - ymin) * 0.10;

    xmin -= dx;  ymin -= dy;
    xmax += dx;  ymax += dy;

    adicionar_segmento_no_poligono(poly, xmin, ymin, xmax, ymin); // Base
    adicionar_segmento_no_poligono(poly, xmax, ymin, xmax, ymax); // Direita
    adicionar_segmento_no_poligono(poly, xmax, ymax, xmin, ymax); // Topo
    adicionar_segmento_no_poligono(poly, xmin, ymax, xmin, ymin); // Esquerda
}


void process_qry(FILE *qry, FILE *svg, void* ground, FILE *txt) {
    if (!qry || !ground) return;

    Poligono anteparos = CriarPoligono(2000); 
    
    Lista lista_formas = get_ground_lista_formas(ground);
    Lista lista_clones = get_ground_lista_clones(ground);
    adicionar_retangulo_limite(anteparos, lista_formas);

    char linha[256];
    while (fgets(linha, sizeof(linha), qry)) {
        char cmd[10];
        if (sscanf(linha, "%s", cmd) != 1) continue;

        if (strcmp(cmd, "a") == 0) {
            int id_i, id_f;
            char orientacao[5] = "v";
            
            char *args = strstr(linha, "a");
            args += 1;
            int lidos = sscanf(args, "%d %d %s", &id_i, &id_f, orientacao);
            char or = (orientacao[0] == 'h' || orientacao[0] == 'H') ? 'h' : 'v';

            // Itera sobre a lista usando as funções de iteração do lista.h
            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                if (!f->foi_destruida && f->id_original >= id_i && f->id_original <= id_f) {
                    transformar_forma_em_segmentos(anteparos, f, or, txt);
                }
                p = get_proximo_lista(lista_formas, p);
            }
        }

        else if (strcmp(cmd, "d") == 0) {
            double x, y;
            char sfx[32] = "-";
            char *args = strstr(linha, "d") + 1;
            sscanf(args, "%lf %lf %s", &x, &y, sfx);
            
            Ponto origem = CriarPonto(x, y);
            Visibilidade vis = CalcularVisibilidade(anteparos, origem);
            
            if (svg) desenhar_visibilidade_svg(svg, vis, x, y, "yellow");
            
            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                if (!f->foi_destruida && ponto_dentro_visibilidade(vis, f->x_centro, f->y_centro)) {
                    f->foi_destruida = true;
                    if (txt) fprintf(txt, "Destruido: ID %d\n", f->id_original);
                }
                p = get_proximo_lista(lista_formas, p);
            }
            free(origem);
            DestruirVisibilidade(vis);
        }

        // --- COMANDO 'p' (Paint Bomb) ---
        else if (strcmp(cmd, "p") == 0) {
            double x, y;
            char cor[32], sfx[32] = "-";
            char *args = strstr(linha, "p") + 1;
            sscanf(args, "%lf %lf %s %s", &x, &y, cor, sfx);
            
            Ponto origem = CriarPonto(x, y);
            Visibilidade vis = CalcularVisibilidade(anteparos, origem);
            
            if (svg) desenhar_visibilidade_svg(svg, vis, x, y, cor); 
            
            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                if (!f->foi_destruida && ponto_dentro_visibilidade(vis, f->x_centro, f->y_centro)) {
                    switch(f->tipo) {
                        case TIPO_CIRCULO: 
                            set_corBorda_circulo((Circulo)f->dados_forma, cor);
                            set_corPreenchimento_circulo((Circulo)f->dados_forma, cor);
                            break;
                        case TIPO_RETANGULO:
                            set_corBorda_retangulo((Retangulo)f->dados_forma, cor);
                            set_corPreenchimento_retangulo((Retangulo)f->dados_forma, cor);
                            break;
                        // Texto e Linha não possuem setters definidos nos seus .h atuais de forma clara
                        // mas poderia ser implementado similarmente
                    }
                    if (txt) fprintf(txt, "Pintado: ID %d\n", f->id_original);
                }
                p = get_proximo_lista(lista_formas, p);
            }
            free(origem);
            DestruirVisibilidade(vis);
        }

        // --- COMANDO 'cln' (Clone Bomb) ---
        else if (strcmp(cmd, "cln") == 0) {
            double x, y, dx, dy;
            char sfx[32] = "-";
            char *args = strstr(linha, "cln") + 1;
            sscanf(args, "%lf %lf %lf %lf %s", &x, &y, &dx, &dy, sfx);
            
            Ponto origem = CriarPonto(x, y);
            Visibilidade vis = CalcularVisibilidade(anteparos, origem);
            
            if (svg) desenhar_visibilidade_svg(svg, vis, x, y, "blue");
            
            // Lista temporária para clones novos (para não iterar sobre eles no mesmo loop)
            Lista novos_clones;
            iniciar_lista(&novos_clones);

            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                if (!f->foi_destruida && ponto_dentro_visibilidade(vis, f->x_centro, f->y_centro)) {
                    FormaStruct* clone = clonar_forma_struct(f, (float)dx, (float)dy);
                    if (clone) {
                        inserir_lista(&novos_clones, clone);
                        if (txt) fprintf(txt, "Clonado: ID %d -> %d\n", f->id_original, clone->id_original);
                    }
                }
                p = get_proximo_lista(lista_formas, p);
            }

            // Move os novos clones para as listas oficiais do Ground
            void* val;
            while(remover_inicio_lista(&novos_clones, &val)) {
                inserir_fim_lista(lista_formas, val); 
                // Também adiciona na lista de clones para liberar memória corretamente no final
                inserir_fim_lista(lista_clones, val); 
            }
            free(novos_clones); // Libera apenas o descritor da lista temporária

            free(origem);
            DestruirVisibilidade(vis);
        }
    }
    
    DestruirPoligono(anteparos);
}

// Funções dummy exigidas pelo header qry.h, se não usadas:
Arena criar_arena() { return NULL; }
void arena_adicionar_forma(Arena a, Forma forma) {}
void destruir_arena(Arena a) {}
bool formas_colidem(void* f1, void* f2) { return false; }
void forma_set_destruida(Forma forma, bool status) {
    if(forma) ((FormaStruct*)forma)->foi_destruida = status;
}
int forma_get_id_original(Forma forma) {
    return forma ? ((FormaStruct*)forma)->id_original : -1;
}
TipoForma forma_get_tipo(Forma forma) {
    return forma ? ((FormaStruct*)forma)->tipo : (TipoForma)-1;
}