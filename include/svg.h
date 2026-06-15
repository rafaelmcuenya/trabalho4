#ifndef SVG_H
#define SVG_H

#include <stdio.h>
#include "quadra.h"
#include "hashfile.h"

/*
   Módulo responsável pela geração de arquivos SVG.
   Cria também possíveis símbolos para algumas das operações do arquivo qyr.
*/

void svgGeo(const char *caminhoCompleto, HashFile *hfQuadras);
/*
   Gera o arquivo SVG inicial com as quadras da cidade.
   Primeiro parâmetro é o caminho completo do arquivo SVG a ser gerado.
   Segundo parâmetro é o hashfile de quadras.
*/

void svgQry(const char *caminhoCompleto, HashFile *hfQuadras, HashFile *hfMoradores);
/*
   Gera o arquivo SVG final após processar o arquivo .qry, incluindo as quadras e as marcações das operações.
   Primeiro parâmetro é o caminho completo do arquivo SVG a ser gerado.
   Segundo parâmetro é o hashfile de quadras.
   Terceiro parâmetro é o hashfile de moradores.
*/

void desenharQuadrasSvg(HashFile *hfQuadras, FILE *svgFile);
/*
   Desenha a quadra no arquivo SVG.
   Primeiro parâmetro é o hashfile das quadras.
   Segundo parâmetro é o arquivo SVG aberto para escrita
*/

#endif
