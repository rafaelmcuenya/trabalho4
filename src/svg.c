#include "svg.h"
#include "quadra.h"
#include "hashfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef struct SVGFileStruct {
    FILE* file;
    char* caminho;
    double viewBoxX;
    double viewBoxY;
    double viewBoxW;
    double viewBoxH;
} SVGFileStruct;


SVGFile criarSVG(const char* caminho, double viewBoxX, double viewBoxY, 
                  double viewBoxW, double viewBoxH) {
    assert(caminho != NULL);
    assert(viewBoxW > 0);
    assert(viewBoxH > 0);
    
    SVGFileStruct* svg = malloc(sizeof(SVGFileStruct));
    if (svg == NULL) return NULL;
    
    svg->caminho = malloc(strlen(caminho) + 1);
    if (svg->caminho == NULL) {
        free(svg);
        return NULL;
    }
    strcpy(svg->caminho, caminho);
    
    svg->viewBoxX = viewBoxX;
    svg->viewBoxY = viewBoxY;
    svg->viewBoxW = viewBoxW;
    svg->viewBoxH = viewBoxH;
    
    svg->file = fopen(caminho, "w");
    if (svg->file == NULL) {
        free(svg->caminho);
        free(svg);
        return NULL;
    }
    
    fprintf(svg->file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(svg->file, "<svg xmlns=\"http://www.w3.org/2000/svg\" ");
    fprintf(svg->file, "xmlns:svg=\"http://www.w3.org/2000/svg\" ");
    fprintf(svg->file, "version=\"1.1\" ");
    fprintf(svg->file, "viewBox=\"%.2f %.2f %.2f %.2f\">\n", 
            viewBoxX, viewBoxY, viewBoxW, viewBoxH);
    
    return svg;
}

void fecharSVG(SVGFile svg) {
    if (svg == NULL) return;
    
    SVGFileStruct* s = (SVGFileStruct*) svg;
    
    fprintf(s->file, "</svg>\n");
    fclose(s->file);
    free(s->caminho);
    free(s);
}

FILE* getArquivoSVG(SVGFile svg) {
    assert(svg != NULL);
    return ((SVGFileStruct*) svg)->file;
}


void desenharQuadraSvg(FILE* svgFile, Quadra q) {
    if (svgFile == NULL || q == NULL) return;
    
    double x = getXQuadra(q);
    double y = getYQuadra(q);
    double w = getLQuadra(q);
    double h = getHQuadra(q);
    const char* corP = getCorPQuadra(q);
    const char* corB = getCorBQuadra(q);
    double sw = getEspBQuadra(q);
    const char* cep = getCepQuadra(q);
    
    fprintf(svgFile, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"%.1f\" "
            "fill-opacity=\"0.5\"/>\n", x, y, w, h, corP, corB, sw);
    
    double centroX = x + w / 2.0;
    double centroY = y + h / 2.0;
    fprintf(svgFile, "<text x=\"%.2f\" y=\"%.2f\" font-size=\"6\" "
            "text-anchor=\"middle\" fill=\"%s\">%s</text>\n",
            centroX, centroY, corB, cep);
}

static void callbackDesenharQuadra(const char* chave, const void* dado, 
                                   size_t tamDado, void* contexto) {
    FILE* svgFile = (FILE*)contexto;
    Quadra q = desserialQuadra((void*)dado, tamDado);
    if (q) {
        desenharQuadraSvg(svgFile, q);
        freeQuadra(q);
    }
}

void desenharQuadrasSvg(HashFile* hfQuadras, FILE* svgFile) {
    if (hfQuadras == NULL || svgFile == NULL) return;
    iterarHF(hfQuadras, callbackDesenharQuadra, svgFile);
}


void desenharLinha(SVGFile svg, double x1, double y1, double x2, double y2,
                   const char* stroke, double strokeWidth, const char* strokeDasharray) {
    assert(svg != NULL);
    assert(stroke != NULL);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ", 
            x1, y1, x2, y2);
    fprintf(f, "stroke=\"%s\" ", stroke);
    if (strokeWidth > 0) fprintf(f, "stroke-width=\"%.1f\" ", strokeWidth);
    if (strokeDasharray != NULL) fprintf(f, "stroke-dasharray=\"%s\" ", strokeDasharray);
    fprintf(f, "/>\n");
}

void desenharRetangulo(SVGFile svg, double x, double y, double w, double h,
                       const char* fill, const char* stroke, 
                       double strokeWidth, double opacity) {
    assert(svg != NULL);
    assert(w > 0);
    assert(h > 0);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" ", x, y, w, h);
    if (fill != NULL) fprintf(f, "fill=\"%s\" ", fill);
    if (stroke != NULL) fprintf(f, "stroke=\"%s\" ", stroke);
    if (strokeWidth > 0) fprintf(f, "stroke-width=\"%.1f\" ", strokeWidth);
    if (opacity >= 0 && opacity <= 1) fprintf(f, "fill-opacity=\"%.2f\" ", opacity);
    fprintf(f, "/>\n");
}

void desenharTexto(SVGFile svg, double x, double y, const char* texto,
                   const char* fill, double fontSize, const char* textAnchor) {
    assert(svg != NULL);
    assert(texto != NULL);
    assert(fill != NULL);
    assert(fontSize > 0);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<text x=\"%.2f\" y=\"%.2f\" ", x, y);
    fprintf(f, "font-size=\"%.1f\" ", fontSize);
    fprintf(f, "fill=\"%s\" ", fill);
    if (textAnchor != NULL) fprintf(f, "text-anchor=\"%s\" ", textAnchor);
    fprintf(f, ">%s</text>\n", texto);
}

void desenharCaminho(SVGFile svg, const char* d, const char* stroke, 
                     double strokeWidth, const char* fill, double opacity) {
    assert(svg != NULL);
    assert(d != NULL);
    assert(stroke != NULL);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<path d=\"%s\" ", d);
    fprintf(f, "stroke=\"%s\" ", stroke);
    if (strokeWidth > 0) fprintf(f, "stroke-width=\"%.1f\" ", strokeWidth);
    if (fill != NULL) fprintf(f, "fill=\"%s\" ", fill);
    if (opacity >= 0 && opacity <= 1) fprintf(f, "fill-opacity=\"%.2f\" ", opacity);
    fprintf(f, "/>\n");
}


void desenharCaminhoAnimado(SVGFile svg, const char* d, const char* stroke, 
                            double strokeWidth, const char* fill, double opacity,
                            double duracao, const char* repeatCount) {
    assert(svg != NULL);
    assert(d != NULL);
    assert(stroke != NULL);
    assert(duracao > 0);
    
    FILE* f = getArquivoSVG(svg);
    
    char id[64];
    snprintf(id, sizeof(id), "caminho_%p", (void*)f);
    
    fprintf(f, "<path id=\"%s\" d=\"%s\" fill=\"none\" stroke=\"none\"/>\n", id, d);
    
    fprintf(f, "<path d=\"%s\" stroke=\"%s\" stroke-width=\"%.1f\" ", d, stroke, strokeWidth);
    if (fill != NULL) fprintf(f, "fill=\"%s\" ", fill);
    if (opacity >= 0 && opacity <= 1) fprintf(f, "fill-opacity=\"%.2f\" ", opacity);
    fprintf(f, "/>\n");
    
    fprintf(f, "<circle r=\"6\" fill=\"%s\">\n", stroke);
    fprintf(f, "  <animateMotion dur=\"%.1fs\" repeatCount=\"%s\">\n", 
            duracao, repeatCount ? repeatCount : "indefinite");
    fprintf(f, "    <mpath href=\"#%s\"/>\n", id);
    fprintf(f, "  </animateMotion>\n");
    fprintf(f, "</circle>\n");
}


void desenharPontoReferencia(SVGFile svg, double x, double y, const char* rotulo) {
    assert(svg != NULL);
    assert(rotulo != NULL);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ", 
            x, y, x, 0.0);
    fprintf(f, "stroke=\"red\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n");
    
    fprintf(f, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"4\" ", x, y);
    fprintf(f, "fill=\"red\" stroke=\"black\" stroke-width=\"1\"/>\n");
    
    fprintf(f, "<text x=\"%.2f\" y=\"%.2f\" font-size=\"8\" ", x, 10.0);
    fprintf(f, "text-anchor=\"middle\" fill=\"red\" font-weight=\"bold\">%s</text>\n", rotulo);
}

void desenharArestaSelecionadaAGM(SVGFile svg, Vertice origem, Vertice destino,
                                   const char* cor, double strokeWidth) {
    assert(svg != NULL);
    assert(origem != NULL);
    assert(destino != NULL);
    assert(cor != NULL);
    assert(strokeWidth > 0);
    
    double x1 = getVerticeX(origem);
    double y1 = getVerticeY(origem);
    double x2 = getVerticeX(destino);
    double y2 = getVerticeY(destino);
    
    FILE* f = getArquivoSVG(svg);
    
    fprintf(f, "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" ", 
            x1, y1, x2, y2);
    fprintf(f, "stroke=\"%s\" ", cor);
    fprintf(f, "stroke-width=\"%.1f\" ", strokeWidth);
    fprintf(f, "/>\n");
}
