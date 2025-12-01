#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclusão dos headers das bibliotecas
#include "scr/lib/arg/args.h"
#include "scr/lib/geo/geo.h"
#include "scr/qry/qry.h"

// Função auxiliar para remover a extensão de um nome de arquivo
// Ex: "arquivo.geo" -> "arquivo"
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
    // 1. Coleta de argumentos via args.h
    char *dir_base_entrada = obter_valor_opcao(argc, argv, "e");
    char *nome_arquivo_geo = obter_valor_opcao(argc, argv, "f");
    char *dir_base_saida   = obter_valor_opcao(argc, argv, "o");
    char *nome_arquivo_qry = obter_valor_opcao(argc, argv, "q");

    if (!nome_arquivo_geo || !dir_base_saida) {
        fprintf(stderr, "Uso incorreto.\nObrigatorio: -f <arquivo.geo> -o <diretorio_saida>\nOpcional: -e <diretorio_entrada> -q <arquivo.qry>\n");
        return 1;
    }

    char caminho_geo[512];
    char caminho_qry[512];
    char caminho_svg[512];
    char caminho_txt[512];
    char geo_sem_ext[256];
    char qry_sem_ext[256];


    montar_caminho(dir_base_entrada, nome_arquivo_geo, caminho_geo);
    remover_extensao(nome_arquivo_geo, geo_sem_ext);

    char nome_svg_base[256];
    if (nome_arquivo_qry) {
        remover_extensao(nome_arquivo_qry, qry_sem_ext);
        sprintf(nome_svg_base, "%s-%s.svg", geo_sem_ext, qry_sem_ext);
    } else {
        sprintf(nome_svg_base, "%s.svg", geo_sem_ext);
    }
    montar_caminho(dir_base_saida, nome_svg_base, caminho_svg);

    // 3. Processamento do GEO
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

    if (nome_arquivo_qry) {
        montar_caminho(dir_base_entrada, nome_arquivo_qry, caminho_qry);
        
        // Define nome do TXT: diretorio_saida/nome_geo-nome_qry.txt
        char nome_txt[256];
        sprintf(nome_txt, "%s-%s.txt", geo_sem_ext, qry_sem_ext);
        montar_caminho(dir_base_saida, nome_txt, caminho_txt);

        FILE *f_qry = fopen(caminho_qry, "r");
        FILE *f_txt = fopen(caminho_txt, "w");

        if (!f_qry) {
            fprintf(stderr, "Erro ao abrir arquivo QRY: %s\n", caminho_qry);
            // Não retorna erro fatal, apenas ignora qry e fecha o resto
        } else {
            if (!f_txt) {
                fprintf(stderr, "Erro ao criar arquivo TXT: %s\n", caminho_txt);
            } else {
                printf("Processando QRY: %s\n", caminho_qry);
                
                fseek(f_svg, -7, SEEK_END); 
                
                process_qry(f_qry, f_svg, ground, f_txt);
                
                fprintf(f_svg, "\n</svg>");
                
                fclose(f_txt);
            }
            fclose(f_qry);
        }
    }

    // 5. Limpeza
    fclose(f_svg);
    destruir_ground(ground);

    printf("Concluido.\n");
    return 0;
}