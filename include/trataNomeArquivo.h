#ifndef TRATANOMEARQUIVO_H
#define TRATANOMEARQUIVO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PATH_LEN 256
#define FILE_NAME_LEN 100

/*
   Módulo responsável por tratar os nomes dos arquivos durante o processamento do programa.
*/

void criarDiretorioRecursivo(const char* path);
/*
   Cria diretórios recursivamente
*/

void extrairNomeBase(const char* caminhoCompleto, char* nomeBase);
/*
   Extrai o nome base dos arquivos .geo/.qyr
*/

void gerarNomeGeoSvg(const char* nomeBase, const char* outputDir, char* caminhoCompleto);
/*
   Gera o nome do SVG do arquivo .geo
*/

void gerarNomeTxt(const char* nomeBase, const char* outputDir, char* caminhoCompleto);
/*
   Gera o nome do arquivo txt utilizado para os relatórios.
*/

void gerarNomeQrySvg(const char* nomeBaseGeo, const char* nomeBaseQry, const char* outputDir, char* caminhoCompleto);
/*
   Gera o nome do SVG do arquivo .qry.
*/

void gerarNomeQryTxt(const char* nomeBaseGeo, const char* nomeBaseQry, const char* outputDir, char* caminhoCompleto);
/*
   Gera o nome do TXT do arquivo .qry.
*/

#endif
