#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "qry.h"
#include "../geo/geo.h"
#include "../poligono/poligono.h"
#include "../lista/lista.h"
#include "../formas/circulo/circulo.h"
#include "../formas/retangulo/retangulo.h"
#include "../formas/linha/linha.h"
#include "../formas/texto/texto.h"

typedef struct {
    int id_original;
    TipoForma tipo;
    void* dados_forma;     
    bool foi_destruida;
    bool foi_clonada;
    float x_centro, y_centro; 
    int id_disparador_origem;
} FormaStruct;

static int proximo_id_clone = 20000;

const char* obter_nome_tipo(TipoForma t) {
    switch(t) {
        case TIPO_CIRCULO: return "circulo";
        case TIPO_RETANGULO: return "retangulo";
        case TIPO_LINHA: return "linha";
        case TIPO_TEXTO: return "texto";
        default: return "desconhecido";
    }
}

void desenhar_cenario_atual(FILE* svg, Lista lista_formas) {
    if (!svg || !lista_formas) return;
    
    Posic p = get_primeiro_lista(lista_formas);
    while (p) {
        FormaStruct* f = (FormaStruct*) get_valor_lista(lista_formas, p);
        if (!f->foi_destruida) {
             // ALTERADO: 
             // fill-opacity='0.5': Cores mais vivas (50%) mas ainda transparentes.
             // stroke-opacity='1.0': Bordas totalmente sólidas para definição.
             switch(f->tipo) {
                case TIPO_CIRCULO: {
                    Circulo c = (Circulo)f->dados_forma;
                    fprintf(svg, "<circle id='%d' cx='%.2f' cy='%.2f' r='%.2f' stroke='%s' fill='%s' fill-opacity='0.5' stroke-opacity='1.0' />\n",
                        f->id_original, get_x(c), get_y(c), get_raio(c), get_corBorda_circulo(c), get_corPreenchimento_circulo(c));
                    break;
                }
                case TIPO_RETANGULO: {
                    Retangulo r = (Retangulo)f->dados_forma;
                    fprintf(svg, "<rect id='%d' x='%.2f' y='%.2f' width='%.2f' height='%.2f' stroke='%s' fill='%s' fill-opacity='0.5' stroke-opacity='1.0' />\n",
                        f->id_original, get_x_retangulo(r), get_y_retangulo(r), get_largura(r), get_altura(r),
                        get_corBorda_retangulo(r), get_corPreenchimento_retangulo(r));
                    break;
                }
                case TIPO_LINHA: {
                    Linha l = (Linha)f->dados_forma;
                    fprintf(svg, "<line id='%d' x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' stroke='%s' stroke-opacity='1.0' />\n",
                        f->id_original, get_x1_linha(l), get_y1_linha(l), get_x2_linha(l), get_y2_linha(l), get_cor_linha(l));
                    break;
                }
                case TIPO_TEXTO: {
                    Texto t = (Texto)f->dados_forma;
                    char anchor_svg[10] = "start";
                    char a = get_anchor_texto(t);
                    if (a == 'm') strcpy(anchor_svg, "middle");
                    else if (a == 'f') strcpy(anchor_svg, "end");

                    // Texto um pouco mais opaco (0.8) para legibilidade
                    fprintf(svg, "<text id='%d' x='%.2f' y='%.2f' stroke='%s' fill='%s' fill-opacity='0.8' stroke-opacity='1.0' text-anchor='%s'>%s</text>\n",
                        f->id_original, get_x_texto(t), get_y_texto(t), get_corBorda_texto(t), 
                        get_corPreenchimento_texto(t), anchor_svg, get_conteudo_texto(t));
                    break;
                }
            }
        }
        p = get_proximo_lista(lista_formas, p);
    }
}

double dist_sq(double x1, double y1, double x2, double y2) {
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

// Função robusta que detecta interseção mesmo em casos colineares (sobreposição)
bool segmentos_intersectam(double x1, double y1, double x2, double y2, 
                           double x3, double y3, double x4, double y4) {
    double den = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    
    if (fabs(den) < 1e-9) {
         double cross = (x2-x1)*(y3-y1) - (y2-y1)*(x3-x1);
         if (fabs(cross) > 1e-9) return false; 
         
         double minx1 = fmin(x1, x2), maxx1 = fmax(x1, x2);
         double minx2 = fmin(x3, x4), maxx2 = fmax(x3, x4);
         double miny1 = fmin(y1, y2), maxy1 = fmax(y1, y2);
         double miny2 = fmin(y3, y4), maxy2 = fmax(y3, y4);

         if (maxx1 >= minx2 && maxx2 >= minx1 && maxy1 >= miny2 && maxy2 >= miny1) return true;
         return false;
    }

    double t = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4)) / den;
    double u = -((x1-x2)*(y1-y3) - (y1-y2)*(x1-x3)) / den;

    return (t >= -1e-9 && t <= 1.0 + 1e-9 && u >= -1e-9 && u <= 1.0 + 1e-9);
}

double dist_sq_ponto_segmento(double px, double py, double ax, double ay, double bx, double by) {
    double l2 = dist_sq(ax, ay, bx, by);
    if (l2 == 0) return dist_sq(px, py, ax, ay);
    double t = ((px - ax) * (bx - ax) + (py - ay) * (by - ay)) / l2;
    if (t < 0) return dist_sq(px, py, ax, ay);
    if (t > 1) return dist_sq(px, py, bx, by);
    double projx = ax + t * (bx - ax);
    double projy = ay + t * (by - ay);
    return dist_sq(px, py, projx, projy);
}

bool ponto_dentro_visibilidade(Visibilidade vis, double x, double y) {
    if (!vis) return false;
    int n = VisibilidadeNumPontos(vis);
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        Ponto pi = VisibilidadeGetPonto(vis, i);
        Ponto pj = VisibilidadeGetPonto(vis, j);
        double xi = PontoX(pi), yi = PontoY(pi);
        double xj = PontoX(pj), yj = PontoY(pj);
        free(pi); free(pj);

        if (((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi)) {
            inside = !inside;
        }
    }
    return inside;
}

void desenhar_visibilidade_svg(FILE *svg, Visibilidade vis, double x_bomb, double y_bomb, const char* cor) {
    if (!svg || !vis) return;
    
    fprintf(svg, "<polygon points=\"");
    int n = VisibilidadeNumPontos(vis);
    for (int i = 0; i < n; i++) {
        Ponto p = VisibilidadeGetPonto(vis, i);
        fprintf(svg, "%.2f,%.2f ", PontoX(p), PontoY(p));
        free(p);
    }
    // Visibilidade em 0.5 para consistência
    fprintf(svg, "\" style=\"fill:%s;fill-opacity:0.5;stroke:none\" />\n", cor);
    fprintf(svg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"3\" fill=\"red\" />\n", x_bomb, y_bomb);
}

bool linha_visivel(Visibilidade vis, double x1, double y1, double x2, double y2) {
    if (ponto_dentro_visibilidade(vis, x1, y1) || ponto_dentro_visibilidade(vis, x2, y2)) return true;

    int n = VisibilidadeNumPontos(vis);
    for (int i = 0; i < n; i++) {
        Ponto pa = VisibilidadeGetPonto(vis, i);
        Ponto pb = VisibilidadeGetPonto(vis, (i + 1) % n);
        bool inter = segmentos_intersectam(x1, y1, x2, y2, PontoX(pa), PontoY(pa), PontoX(pb), PontoY(pb));
        free(pa); free(pb);
        if (inter) return true;
    }
    return false;
}

bool retangulo_visivel(Visibilidade vis, double rx, double ry, double w, double h) {
    double pts[4][2] = {{rx, ry}, {rx+w, ry}, {rx+w, ry+h}, {rx, ry+h}};

    for(int i=0; i<4; i++) {
        if (ponto_dentro_visibilidade(vis, pts[i][0], pts[i][1])) return true;
    }

    int n = VisibilidadeNumPontos(vis);
    for(int i=0; i<n; i++) {
        Ponto p = VisibilidadeGetPonto(vis, i);
        double px = PontoX(p), py = PontoY(p);
        free(p);
        if (px >= rx && px <= rx+w && py >= ry && py <= ry+h) return true;
    }

    for (int i = 0; i < n; i++) {
        Ponto pa = VisibilidadeGetPonto(vis, i);
        Ponto pb = VisibilidadeGetPonto(vis, (i + 1) % n);
        double pax = PontoX(pa), pay = PontoY(pa);
        double pbx = PontoX(pb), pby = PontoY(pb);
        free(pa); free(pb);

        for(int j=0; j<4; j++) {
            if (segmentos_intersectam(pax, pay, pbx, pby, pts[j][0], pts[j][1], pts[(j+1)%4][0], pts[(j+1)%4][1])) 
                return true;
        }
    }
    return false;
}

bool circulo_visivel(Visibilidade vis, double cx, double cy, double r) {
    if (ponto_dentro_visibilidade(vis, cx, cy)) return true;

    int n = VisibilidadeNumPontos(vis);
    double r2 = r*r;

    for (int i = 0; i < n; i++) {
        Ponto pa = VisibilidadeGetPonto(vis, i);
        Ponto pb = VisibilidadeGetPonto(vis, (i + 1) % n);
        
        if (dist_sq(PontoX(pa), PontoY(pa), cx, cy) <= r2) {
            free(pa); free(pb); return true;
        }
        if (dist_sq_ponto_segmento(cx, cy, PontoX(pa), PontoY(pa), PontoX(pb), PontoY(pb)) <= r2) {
            free(pa); free(pb); return true;
        }
        free(pa); free(pb);
    }
    return false;
}

bool forma_esta_na_visibilidade(Visibilidade vis, FormaStruct *f) {
    if (!f || f->foi_destruida) return false;

    switch(f->tipo) {
        case TIPO_LINHA: {
            Linha l = (Linha)f->dados_forma;
            return linha_visivel(vis, get_x1_linha(l), get_y1_linha(l), get_x2_linha(l), get_y2_linha(l));
        }
        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)f->dados_forma;
            return retangulo_visivel(vis, get_x_retangulo(r), get_y_retangulo(r), get_largura(r), get_altura(r));
        }
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)f->dados_forma;
            return circulo_visivel(vis, get_x(c), get_y(c), get_raio(c));
        }
        case TIPO_TEXTO: {
            Texto t = (Texto)f->dados_forma;
            return ponto_dentro_visibilidade(vis, get_x_texto(t), get_y_texto(t));
        }
    }
    return false;
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
            break;
        }
        case TIPO_RETANGULO: {
            Retangulo r = (Retangulo)f->dados_forma;
            double x = get_x_retangulo(r);
            double y = get_y_retangulo(r);
            double w = get_largura(r);
            double h = get_altura(r);

            adicionar_segmento_no_poligono(poly, x, y, x+w, y);
            adicionar_segmento_no_poligono(poly, x+w, y, x+w, y+h);
            adicionar_segmento_no_poligono(poly, x+w, y+h, x, y+h);
            adicionar_segmento_no_poligono(poly, x, y+h, x, y);
            break;
        }
        case TIPO_CIRCULO: {
            Circulo c = (Circulo)f->dados_forma;
            double x = get_x(c);
            double y = get_y(c);
            double r = get_raio(c);

            if (orientacao == 'h') { 
                adicionar_segmento_no_poligono(poly, x - r, y, x + r, y);
            } else {
                adicionar_segmento_no_poligono(poly, x, y - r, x, y + r);
            }
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
            else { x1 = x - tamanho/2; x2 = x + tamanho/2; }

            adicionar_segmento_no_poligono(poly, x1, y, x2, y);
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
            float w = get_largura(r);
            float h = get_altura(r);
            dados_clonados = criar_retangulo(nx - w/2, ny - h/2, w, h,
                                             (char*)get_corPreenchimento_retangulo(r),
                                             (char*)get_corBorda_retangulo(r), id_clone);
            break;
        }
        case TIPO_LINHA: {
            Linha l = (Linha)original->dados_forma;
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

void adicionar_retangulo_limite(Poligono poly, Lista lista_formas, double bx, double by) {
    if (!poly || !lista_formas) return;

    double xmin = bx, ymin = by;
    double xmax = bx, ymax = by;
    
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

    if (dx == 0) dx = 10;
    if (dy == 0) dy = 10;

    xmin -= dx;  ymin -= dy;
    xmax += dx;  ymax += dy;

    adicionar_segmento_no_poligono(poly, xmin, ymin, xmax, ymin);
    adicionar_segmento_no_poligono(poly, xmax, ymin, xmax, ymax);
    adicionar_segmento_no_poligono(poly, xmax, ymax, xmin, ymax);
    adicionar_segmento_no_poligono(poly, xmin, ymax, xmin, ymin);
}

void process_qry(FILE *qry, const char* dir_saida, const char* nome_base, void* ground, FILE *txt) {
    if (!qry || !ground) return;

    Lista lista_formas = get_ground_lista_formas(ground);
    Lista lista_clones = get_ground_lista_clones(ground);
    
    char linha[256];
    while (fgets(linha, sizeof(linha), qry)) {
        char cmd[10];
        if (sscanf(linha, "%s", cmd) != 1) continue;

        if (strcmp(cmd, "a") == 0) {
            
            int id_i, id_f;
            char orientacao[5] = "v";
            
            char *args = strstr(linha, "a");
            args += 1;
            sscanf(args, "%d %d %s", &id_i, &id_f, orientacao);
            char or = (orientacao[0] == 'h' || orientacao[0] == 'H') ? 'h' : 'v';

            Lista novos_anteparos = NULL;
            iniciar_lista(&novos_anteparos);

            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                
                if (!f->foi_destruida && f->id_original >= id_i && f->id_original <= id_f) {
                    if (f->tipo == TIPO_CIRCULO || f->tipo == TIPO_RETANGULO || f->tipo == TIPO_TEXTO) {
                        if (txt) fprintf(txt, "Transformado: %s ID %d\n", obter_nome_tipo(f->tipo), f->id_original);
                        f->foi_destruida = true;
                        
                        double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
                        char cor[32];
                        strcpy(cor, "#000000"); 

                        bool gerou_multiplos = false;

                        if (f->tipo == TIPO_RETANGULO) {
                            Retangulo r = (Retangulo)f->dados_forma;
                            double x = get_x_retangulo(r), y = get_y_retangulo(r);
                            double w = get_largura(r), h = get_altura(r);
                            strcpy(cor, get_corBorda_retangulo(r));
                            
                            double coords[4][4] = {
                                {x, y, x + w, y},       
                                {x + w, y, x + w, y + h}, 
                                {x + w, y + h, x, y + h}, 
                                {x, y + h, x, y}        
                            };

                            for(int k=0; k<4; k++) {
                                int novo_id = proximo_id_clone++;
                                Linha nl = criar_linha(coords[k][0], coords[k][1], coords[k][2], coords[k][3], cor, novo_id);
                                FormaStruct* fw = malloc(sizeof(FormaStruct));
                                fw->id_original = novo_id;
                                fw->tipo = TIPO_LINHA;
                                fw->dados_forma = nl;
                                fw->foi_destruida = false;
                                fw->foi_clonada = false;
                                fw->x_centro = (coords[k][0]+coords[k][2])/2;
                                fw->y_centro = (coords[k][1]+coords[k][3])/2;
                                inserir_lista(&novos_anteparos, fw);
                                if (txt) fprintf(txt, "Novo Segmento (Ret): ID %d (%.2f, %.2f) -> (%.2f, %.2f)\n", novo_id, coords[k][0], coords[k][1], coords[k][2], coords[k][3]);
                            }
                            gerou_multiplos = true;
                        }
                        else if (f->tipo == TIPO_CIRCULO) {
                            Circulo c = (Circulo)f->dados_forma;
                            double x = get_x(c), y = get_y(c), r = get_raio(c);
                            strcpy(cor, get_corBorda_circulo(c));
                            if (or == 'h') { x1 = x - r; y1 = y; x2 = x + r; y2 = y; }
                            else { x1 = x; y1 = y - r; x2 = x; y2 = y + r; }
                        }
                        else if (f->tipo == TIPO_TEXTO) {
                            Texto t = (Texto)f->dados_forma;
                            double x = get_x_texto(t), y = get_y_texto(t);
                            char anchor = get_anchor_texto(t);
                            const char* cont = get_conteudo_texto(t);
                            double tamanho = 10.0 * (cont ? strlen(cont) : 0);
                            strcpy(cor, get_corBorda_texto(t));
                            y1 = y; y2 = y;
                            if (anchor == 'i') { x1 = x; x2 = x + tamanho; }
                            else if (anchor == 'f') { x1 = x - tamanho; x2 = x; }
                            else { x1 = x - tamanho/2.0; x2 = x + tamanho/2.0; }
                        }

                        if (!gerou_multiplos) {
                            int novo_id = proximo_id_clone++; 
                            Linha nova_linha = criar_linha(x1, y1, x2, y2, cor, novo_id);
                            FormaStruct* fw = malloc(sizeof(FormaStruct));
                            fw->id_original = novo_id;
                            fw->tipo = TIPO_LINHA;
                            fw->dados_forma = nova_linha;
                            fw->foi_destruida = false;
                            fw->foi_clonada = false;
                            fw->x_centro = (x1+x2)/2;
                            fw->y_centro = (y1+y2)/2;
                            inserir_lista(&novos_anteparos, fw);
                            if (txt) fprintf(txt, "Novo Segmento: ID %d (%.2f, %.2f) -> (%.2f, %.2f)\n", novo_id, x1, y1, x2, y2);
                        }
                    }
                }
                p = get_proximo_lista(lista_formas, p);
            }
            
            void* val;
            while(remover_inicio_lista(&novos_anteparos, &val)) {
                inserir_fim_lista(&lista_formas, val);
            }
            free(novos_anteparos);
        }
        else if (strcmp(cmd, "d") == 0 || strcmp(cmd, "p") == 0 || strcmp(cmd, "cln") == 0) {
            double x, y;
            char sfx[32] = "-";
            char cor[32] = "yellow";
            double dx = 0, dy = 0;
            
            char *args = strstr(linha, cmd) + strlen(cmd);

            if (strcmp(cmd, "d") == 0) {
                 sscanf(args, "%lf %lf %s", &x, &y, sfx);
            } else if (strcmp(cmd, "p") == 0) {
                 sscanf(args, "%lf %lf %s %s", &x, &y, cor, sfx);
            } else { 
                 sscanf(args, "%lf %lf %lf %lf %s", &x, &y, &dx, &dy, sfx);
                 sprintf(cor, "blue"); 
            }

            Poligono anteparos = CriarPoligono(2000);
            adicionar_retangulo_limite(anteparos, lista_formas, x, y);
            
            Posic pa = get_primeiro_lista(lista_formas);
            while(pa){
               FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, pa);
               if(!f->foi_destruida) transformar_forma_em_segmentos(anteparos, f, 'v', NULL);
               pa = get_proximo_lista(lista_formas, pa);
            }
            
            Ponto origem = CriarPonto(x, y);
            Visibilidade vis = CalcularVisibilidade(anteparos, origem);
            
            Lista novos_clones = NULL;
            if (strcmp(cmd, "cln") == 0) {
                iniciar_lista(&novos_clones);
            }

            Posic p = get_primeiro_lista(lista_formas);
            while (p) {
                FormaStruct* f = (FormaStruct*)get_valor_lista(lista_formas, p);
                
                if (!f->foi_destruida && forma_esta_na_visibilidade(vis, f)) {
                    
                    if (strcmp(cmd, "d") == 0) {
                        f->foi_destruida = true;
                        if (txt) fprintf(txt, "Destruido: %s ID %d\n", obter_nome_tipo(f->tipo), f->id_original);
                    } 
                    else if (strcmp(cmd, "p") == 0) {
                         switch(f->tipo) {
                            case TIPO_CIRCULO: 
                                set_corBorda_circulo((Circulo)f->dados_forma, cor);
                                set_corPreenchimento_circulo((Circulo)f->dados_forma, cor);
                                break;
                            case TIPO_RETANGULO:
                                set_corBorda_retangulo((Retangulo)f->dados_forma, cor);
                                set_corPreenchimento_retangulo((Retangulo)f->dados_forma, cor);
                                break;
                            case TIPO_LINHA:
                            case TIPO_TEXTO:
                                break;
                        }
                        if (txt) fprintf(txt, "Pintado: %s ID %d\n", obter_nome_tipo(f->tipo), f->id_original);
                    }
                    else if (strcmp(cmd, "cln") == 0) {
                        FormaStruct* clone = clonar_forma_struct(f, (float)dx, (float)dy);
                        if (clone) {
                            inserir_lista(&novos_clones, clone);
                            if (txt) fprintf(txt, "Clonado: %s ID %d -> %d\n", obter_nome_tipo(f->tipo), f->id_original, clone->id_original);
                        }
                    }
                }
                p = get_proximo_lista(lista_formas, p);
            }

            if (novos_clones) {
                void* val;
                while(remover_inicio_lista(&novos_clones, &val)) {
                    inserir_fim_lista(&lista_formas, val); 
                    inserir_fim_lista(&lista_clones, val); 
                }
                free(novos_clones);
            }

            char nome_arquivo_svg[512];
            sprintf(nome_arquivo_svg, "%s/%s-%s.svg", dir_saida, nome_base, sfx);
            FILE* svg_cmd = fopen(nome_arquivo_svg, "w");
            
            if (svg_cmd) {
               fprintf(svg_cmd, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\">\n");
                desenhar_cenario_atual(svg_cmd, lista_formas);
                desenhar_visibilidade_svg(svg_cmd, vis, x, y, (strcmp(cmd, "p")==0 ? cor : (strcmp(cmd,"cln")==0 ? "blue" : "yellow")));
                fprintf(svg_cmd, "</svg>");
                fclose(svg_cmd);
            }

            free(origem);
            DestruirVisibilidade(vis);
            DestruirPoligono(anteparos);
        }
    }
}

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