#ifndef SVG_H
#define SVG_H

#include "grafo.h"
#include "hashfile.h"
#include "quadra.h"
#include <stdio.h>
#include <stdbool.h>

/*
   Módulo responsável pela geração de arquivos SVG.
   Fornece funções para desenhar elementos geométricos no formato SVG,
   como quadras, bounding boxes, percursos e marcadores.
*/

typedef void* SVGFile;


SVGFile criarSVG(const char* caminho, double viewBoxX, double viewBoxY, 
                 double viewBoxW, double viewBoxH);
/*
   Função que cria um arquivo SVG.
   Primeiro parâmetro é o caminho do arquivo.
   Segundo parâmetro é a coordenada X do viewBox.
   Terceiro parâmetro é a coordenada Y do viewBox.
   Quarto parâmetro é a largura do viewBox.
   Quinto parâmetro é a altura do viewBox.
   Retorna um ponteiro para a estrutura do arquivo SVG.
*/

void fecharSVG(SVGFile svg);
/*
   Função que fecha o arquivo SVG.
   Recebe como parâmetro a estrutura do SVG.
*/

FILE* getArquivoSVG(SVGFile svg);
/*
   Função que retorna o ponteiro FILE do SVG.
   Recebe como parâmetro a estrutura do SVG.
   Retorna o ponteiro FILE para escrita.
*/


void desenharQuadraSvg(FILE* svgFile, Quadra q);
/*
   Função que desenha uma quadra no SVG.
   Primeiro parâmetro é o arquivo SVG.
   Segundo parâmetro é a quadra a ser desenhada.
   Desenha um retângulo com as cores e espessura da borda definidas,
   e o CEP no centro da quadra.
*/

void desenharQuadrasSvg(HashFile* hfQuadras, FILE* svgFile);
/*
   Função que desenha todas as quadras de um HashFile no SVG.
   Primeiro parâmetro é o HashFile contendo as quadras.
   Segundo parâmetro é o arquivo SVG.
*/


void desenharLinha(SVGFile svg, double x1, double y1, double x2, double y2,
                   const char* stroke, double strokeWidth, const char* strokeDasharray);
/*
   Função que desenha uma linha no SVG. Será usado para desenhar ruas e arestas do grafo.
*/

void desenharRetangulo(SVGFile svg, double x, double y, double w, double h,
                       const char* fill, const char* stroke, double strokeWidth, double opacity);
/*
   Função que desenha um retângulo no SVG. Será usado para desenhar bounding boxes
   dos componentes conexos.
*/

void desenharTexto(SVGFile svg, double x, double y, const char* texto,
                   const char* fill, double fontSize, const char* textAnchor);
/*
   Função que desenha um texto no SVG.
*/

void desenharCaminho(SVGFile svg, const char* d, const char* stroke, 
                     double strokeWidth, const char* fill, double opacity);
/*
   Função que desenha um caminho SVG.
*/

void desenharCaminhoAnimado(SVGFile svg, const char* d, const char* stroke, 
                            double strokeWidth, const char* fill, double opacity,
                            double duracao, const char* repeatCount);
/*
   Função que desenha um caminho animado no SVG.
*/


void desenharPontoReferencia(SVGFile svg, double x, double y, const char* rotulo);
/*
   Função que desenha um ponto de referência no SVG.
   Primeiro parâmetro é o SVG.
   Segundo parâmetro é a coordenada X.
   Terceiro parâmetro é a coordenada Y.
   Quarto parâmetro é o rótulo (I para início, F para fim, ou número do registrador).
   Desenha um círculo vermelho, linha vertical pontilhada e o rótulo.
*/

void desenharArestaSelecionadaAGM(SVGFile svg, Vertice origem, Vertice destino,
                                   const char* cor, double strokeWidth);
/*
   Função que desenha uma aresta selecionada pela AGM.
   Primeiro parâmetro é o SVG.
   Segundo parâmetro é o vértice de origem.
   Terceiro parâmetro é o vértice de destino.
   Quarto parâmetro é a cor (vermelho para AGM).
   Quinto parâmetro é a espessura da linha (grossa para AGM).
*/

#endif
