#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "lib/arg/args.h"
#include "lib/geo/geo.h"
#include "lib/qry/qry.h"
#include "lib/poligono/poligono.h"

void remover_extensao(const char *nome_arquivo, char *buffer) {
    if (!nome_arquivo || !buffer) return;
    strcpy(buffer, nome_arquivo);
    char *ponto = strrchr(buffer, '.');
    if (ponto) {
        *ponto = '\0';
    }
}

void montar_caminho(const char *diretorio, const char *nome, char *buffer) {
    if (diretorio && strlen(diretorio) > 0) {
        sprintf(buffer, "%s/%s", diretorio, nome);
    } else {
        strcpy(buffer, nome);
    }
}

int main(int argc, char *argv[]) {

    setlocale(LC_NUMERIC, "C");
    
    char *dir_base_entrada = obter_valor_opcao(argc, argv, "e");
    char *nome_arquivo_geo = obter_valor_opcao(argc, argv, "f");
    char *dir_base_saida   = obter_valor_opcao(argc, argv, "o");
    char *nome_arquivo_qry = obter_valor_opcao(argc, argv, "q");
    char *valor_i          = obter_valor_opcao(argc, argv, "i");
    char *valor_to         = obter_valor_opcao(argc, argv, "to");

    if (!nome_arquivo_geo || !dir_base_saida) {
        fprintf(stderr, "Uso incorreto.\nObrigatorio: -f <arquivo.geo> -o <diretorio_saida>\nOpcional: -e <diretorio_entrada> -q <arquivo.qry> -i <limiar_sort> -to <tipo_sort>\n");
        return 1;
    }

    if (valor_i) {
        int limiar = atoi(valor_i);
        definir_limiar_ordenacao(limiar);
        printf("Limiar de ordenacao definido para: %d\n", limiar);
    }

    if (valor_to) {
        definir_tipo_ordenacao(valor_to[0]);
        printf("Tipo de ordenacao definido para: %c\n", valor_to[0]);
    }

    char caminho_geo[512];
    char caminho_qry[512];
    char caminho_svg[512];
    char caminho_txt[512];

    char geo_sem_ext[256];
    char qry_sem_ext[256];

    montar_caminho(dir_base_entrada, nome_arquivo_geo, caminho_geo);

    const char *apenas_nome_geo = strrchr(nome_arquivo_geo, '/');
    if (apenas_nome_geo) {
        apenas_nome_geo++;
    } else {
        apenas_nome_geo = nome_arquivo_geo;
    }

    remover_extensao(apenas_nome_geo, geo_sem_ext);

    sprintf(caminho_svg, "%s/%s.svg", dir_base_saida, geo_sem_ext);

    FILE *f_geo = fopen(caminho_geo, "r");
    if (!f_geo) {
        fprintf(stderr, "Erro ao abrir arquivo GEO: %s\n", caminho_geo);
        return 1;
    }

    FILE *f_svg = fopen(caminho_svg, "w");
    if (!f_svg) {
        fprintf(stderr, "Erro ao criar arquivo SVG: %s\n", caminho_svg);
        fclose(f_geo);
        return 1;
    }

    printf("Processando GEO: %s -> %s\n", caminho_geo, caminho_svg);
    Ground ground = process_geo(f_geo, f_svg);

    fclose(f_geo);
    fclose(f_svg);

    if (nome_arquivo_qry) {
        montar_caminho(dir_base_entrada, nome_arquivo_qry, caminho_qry);

        const char *apenas_nome_qry = strrchr(nome_arquivo_qry, '/');
        if (apenas_nome_qry) {
            apenas_nome_qry++;
        } else {
            apenas_nome_qry = nome_arquivo_qry;
        }

        remover_extensao(apenas_nome_qry, qry_sem_ext);

        char nome_txt[600];
        sprintf(nome_txt, "%s-%s.txt", geo_sem_ext, qry_sem_ext);
        montar_caminho(dir_base_saida, nome_txt, caminho_txt);
        
        char nome_svg_qry[600];
        sprintf(nome_svg_qry, "%s-%s.svg", geo_sem_ext, qry_sem_ext);
        char caminho_svg_qry[600];
        montar_caminho(dir_base_saida, nome_svg_qry, caminho_svg_qry);

        FILE *f_qry = fopen(caminho_qry, "r");
        FILE *f_txt = fopen(caminho_txt, "w");
        FILE *f_svg_qry = fopen(caminho_svg_qry, "w");

        if (!f_qry) {
            fprintf(stderr, "Erro ao abrir arquivo QRY: %s\n", caminho_qry);
            if (f_svg_qry) fclose(f_svg_qry);
        } else {
            if (!f_txt) {
                fprintf(stderr, "Erro ao criar arquivo TXT: %s\n", caminho_txt);
            } else if (!f_svg_qry) {
                fprintf(stderr, "Erro ao criar arquivo SVG QRY: %s\n", caminho_svg_qry);
            } else {
                printf("Processando QRY: %s\n", caminho_qry);
                
                fprintf(f_svg_qry, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1000\" height=\"1000\">\n");
                
                char nome_base_combinado[600];
                sprintf(nome_base_combinado, "%s-%s", geo_sem_ext, qry_sem_ext);

                process_qry(f_qry, dir_base_saida, nome_base_combinado, ground, f_txt, f_svg_qry);
                
                ground_escrever_svg(ground, f_svg_qry);
                
                fprintf(f_svg_qry, "</svg>\n");

                fclose(f_txt);
                fclose(f_svg_qry);
            }
            fclose(f_qry);
        }
    }

    destruir_ground(ground);

    printf("Concluido.\n");
    return 0;
}