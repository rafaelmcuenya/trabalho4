#include "leitor.h"
#include "trataNomeArquivo.h"
#include "quadra.h"
#include "hashfile.h"
#include "grafo.h"
#include "dijkstra.h"
#include "conexos.h"
#include "agm.h"
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#define MAX_LINHA 1024
#define TAM_BUFFER_SERIAL 4096
#define NUM_REGISTRADORES 11
#define INFINITO DBL_MAX


typedef struct {
    double x;
    double y;
    bool ativo;
} Registrador;


static HashFile* hfQuadras = NULL;
static Grafo grafo = NULL;
static char corPreenchimento[32] = "#808080";
static char corBorda[32] = "#ff00ff";
static double espessuraBorda = 1.0;
static Registrador registradores[NUM_REGISTRADORES];
static char nomeBaseGlobal[FILE_NAME_LEN];
static char outputDirGlobal[PATH_LEN];
static FILE* arquivoTxt = NULL;
static SVGFile svgFile = NULL;


static void construirCaminhoCompleto(const char* baseDir, const char* arquivo, char* caminhoCompleto) {
    if (!baseDir || strlen(baseDir) == 0) {
        strcpy(caminhoCompleto, arquivo);
        return;
    }

    if (baseDir[strlen(baseDir) - 1] == '/') {
        snprintf(caminhoCompleto, PATH_LEN, "%s%s", baseDir, arquivo);
    } else {
        snprintf(caminhoCompleto, PATH_LEN, "%s/%s", baseDir, arquivo);
    }
}


static void abrirArquivo(FILE** f, const char* caminho) {
    *f = fopen(caminho, "r");
    if (!(*f)) {
        fprintf(stderr, "Erro: Não foi possível abrir %s\n", caminho);
        exit(1);
    }
}


static void inicializarRegistradores(void) {
    for (int i = 0; i < NUM_REGISTRADORES; i++) {
        registradores[i].x = 0.0;
        registradores[i].y = 0.0;
        registradores[i].ativo = false;
    }
}


static int obterIndiceRegistrador(const char* reg) {
    if (reg[0] != 'R' && reg[0] != 'r') return -1;
    int idx = atoi(reg + 1);
    if (idx < 0 || idx >= NUM_REGISTRADORES) return -1;
    return idx;
}


static Vertice encontrarVerticeMaisProximo(Grafo g, double x, double y) {
    Vertice maisProximo = NULL;
    double menorDistancia = INFINITO;

    printf("[DEBUG] Buscando vértice mais próximo de (%.2f, %.2f)\n", x, y);

    Vertice v = primeiroVertice(g);
    int count = 0;
    while (v != NULL) {
        count++;
        double vx = getVerticeX(v);
        double vy = getVerticeY(v);
        double dx = vx - x;
        double dy = vy - y;
        double dist = sqrt(dx*dx + dy*dy);

        if (dist < menorDistancia) {
            menorDistancia = dist;
            maisProximo = v;
        }
        v = proximoVertice(g, v);
    }

    printf("[DEBUG] Vértices verificados: %d\n", count);
    if (maisProximo) {
        printf("[DEBUG] Vértice mais próximo: %s (distância: %.2f)\n", 
               getVerticeId(maisProximo), menorDistancia);
    } else {
        printf("[DEBUG] Nenhum vértice encontrado!\n");
    }

    return maisProximo;
}


static void gerarDescricaoPercurso(FILE* txt, Caminho caminho, const char* tipo) {
    if (!caminho || tamanhoCaminho(caminho) == 0) {
        fprintf(txt, "  %s: Nenhum caminho encontrado\n", tipo);
        return;
    }

    fprintf(txt, "  %s (custo total: %.2f):\n", tipo, getCustoTotal(caminho));

    int numArestas = tamanhoCaminho(caminho);
    for (int i = 0; i < numArestas; i++) {
        Aresta a = getArestaCaminho(caminho, i);
        if (a) {
            Vertice origem = getOrigem(a);
            Vertice destino = getDestino(a);
            const char* nomeRua = getNomeRua(a);
            double comp = getComprimento(a);

            fprintf(txt, "    %d: %s -> %s (%s, %.2fm)\n",
                    i + 1, getVerticeId(origem), getVerticeId(destino),
                    nomeRua ? nomeRua : "sem nome", comp);
        }
    }
}


static void cmdQ(const char* cep, double x, double y, double w, double h) {
    Quadra q = criaQuadra(cep, x, y, w, h, corPreenchimento, corBorda, espessuraBorda);
    if (!q) {
        fprintf(stderr, "Erro: falha ao criar quadra %s\n", cep);
        return;
    }

    size_t tam = tamSerialQuadra();
    void* buffer = malloc(tam);
    if (!buffer) {
        fprintf(stderr, "Erro: falha de alocação ao serializar quadra\n");
        freeQuadra(q);
        return;
    }

    serialQuadra(q, buffer, tam);
    int ret = insertHF(hfQuadras, cep, buffer, tam);

    if (ret == 0) {
        fprintf(stderr, "Aviso: quadra com CEP %s já existe\n", cep);
    } else if (ret < 0) {
        fprintf(stderr, "Erro: falha ao inserir quadra %s no hashfile\n", cep);
    }

    free(buffer);
    freeQuadra(q);
}

static void cmdCq(double sw, const char* cfill, const char* cstrk) {
    espessuraBorda = sw;
    strncpy(corPreenchimento, cfill, 31);
    corPreenchimento[31] = '\0';
    strncpy(corBorda, cstrk, 31);
    corBorda[31] = '\0';
}


static void processarArquivoViaInterno(FILE* f) {
    char linha[MAX_LINHA];
    int numVertices = 0;
    int verticesLidos = 0;

    if (fgets(linha, sizeof(linha), f)) {
        numVertices = atoi(linha);
        printf("[VIA] Número de vértices: %d\n", numVertices);
    }

    while (verticesLidos < numVertices && fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '\n' || linha[0] == '#') continue;

        char comando[10];
        sscanf(linha, "%s", comando);

        if (strcmp(comando, "v") == 0) {
            char id[64];
            double x, y;
            if (sscanf(linha, "%*s %s %lf %lf", id, &x, &y) == 3) {
                adicionarVertice(grafo, id, x, y);
                verticesLidos++;
            }
        }
    }

    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '\n' || linha[0] == '#') continue;

        char comando[10];
        sscanf(linha, "%s", comando);

        if (strcmp(comando, "e") == 0) {
            char origem[64], destino[64], ldir[64], lesq[64], nomeRua[128];
            double comp, velMedia;
            if (sscanf(linha, "%*s %s %s %s %s %lf %lf %[^\n]", 
                       origem, destino, ldir, lesq, &comp, &velMedia, nomeRua) == 7) {
                adicionarAresta(grafo, origem, destino, ldir, lesq, comp, velMedia, nomeRua);
            }
        }
    }
}


static void cmdAtO(const char* reg, const char* cep, char face, int num) {
    int idx = obterIndiceRegistrador(reg);
    if (idx < 0) {
        fprintf(stderr, "Erro: registrador inválido %s\n", reg);
        return;
    }

    size_t tam = tamSerialQuadra();
    void* buffer = malloc(tam);
    if (!buffer) return;

    if (buscaHF(hfQuadras, cep, buffer, &tam) != 1) {
        fprintf(stderr, "Erro: quadra %s não encontrada\n", cep);
        free(buffer);
        return;
    }

    Quadra q = desserialQuadra(buffer, tam);
    free(buffer);

    if (!q) return;

    double x, y;
    getPontoEndQuadra(q, face, (double)num, &x, &y);
    registradores[idx].x = x;
    registradores[idx].y = y;
    registradores[idx].ativo = true;

    fprintf(arquivoTxt, "[*] @o? R%d %s %c %d\n", idx, cep, face, num);
    fprintf(arquivoTxt, "Coordenada: (%.2f, %.2f)\n", x, y);

    char rotulo[8];
    snprintf(rotulo, sizeof(rotulo), "R%d", idx);
    desenharPontoReferencia(svgFile, x, y, rotulo);

    freeQuadra(q);
}


static void cmdMvm(double x, double y, double w, double h, double v) {
    fprintf(arquivoTxt, "[*] mvm %.2f %.2f %.2f %.2f %.2f\n", x, y, w, h, v);

    Vertice vertice = primeiroVertice(grafo);
    int arestasAtualizadas = 0;

    while (vertice != NULL) {
        Aresta a = primeiraArestaAdj(vertice);
        while (a != NULL) {
            Vertice origem = getOrigem(a);
            Vertice destino = getDestino(a);
            double x1 = getVerticeX(origem);
            double y1 = getVerticeY(origem);
            double x2 = getVerticeX(destino);
            double y2 = getVerticeY(destino);

            double midX = (x1 + x2) / 2.0;
            double midY = (y1 + y2) / 2.0;

            if (midX >= x && midX <= x + w && midY >= y && midY <= y + h) {
                setVelocidadeMedia(a, v);
                arestasAtualizadas++;
            }
            a = proximaArestaAdj(vertice, a);
        }
        vertice = proximoVertice(grafo, vertice);
    }

    fprintf(arquivoTxt, "Arestas atualizadas: %d\n", arestasAtualizadas);
}


static void cmdRegs(double vl) {
    fprintf(arquivoTxt, "[*] regs %.2f\n", vl);

    ListaComponentes componentes = encontrarComponentes(grafo, vl);
    if (!componentes) {
        fprintf(arquivoTxt, "Número de componentes conexos: 0\n");
        return;
    }

    int numComp = quantComponentes(componentes);
    fprintf(arquivoTxt, "Número de componentes conexos: %d\n", numComp);

    for (int i = 0; i < numComp; i++) {
        Componente c = getComponente(componentes, i);
        if (c) {
            double x = getBBoxX(c);
            double y = getBBoxY(c);
            double w = getBBoxW(c);
            double h = getBBoxH(c);
            const char* cor = getCorComponente(c);

            desenharRetangulo(svgFile, x, y, w, h, cor, cor, 2.0, 0.5);
        }
    }

    deletaListaComponentes(componentes);
}


static void cmdExp(double vl) {
    fprintf(arquivoTxt, "[*] exp %.2f\n", vl);

    ArvoreGeradoraMinima agm = calcularAGM(grafo, vl);
    if (!agm) {
        fprintf(arquivoTxt, "Nenhuma aresta selecionada\n");
        return;
    }

    int numArestas = tamanhoAGM(agm);
    fprintf(arquivoTxt, "Arestas selecionadas para ampliação: %d\n", numArestas);

    for (int i = 0; i < numArestas; i++) {
        Aresta a = getArestaAGM(agm, i);
        if (a) {
            Vertice origem = getOrigem(a);
            Vertice destino = getDestino(a);
            double velAntiga = getVelocidadeMedia(a);
            double velNova = velAntiga * 1.5;

            desenharArestaSelecionadaAGM(svgFile, origem, destino, "#FF0000", 4.0);
            setVelocidadeMedia(a, velNova);

            fprintf(arquivoTxt, "  %s -> %s: %.2f -> %.2f (rua: %s)\n",
                    getVerticeId(origem), getVerticeId(destino),
                    velAntiga, velNova,
                    getNomeRua(a) ? getNomeRua(a) : "sem nome");
        }
    }

    deletaAGM(agm);
}


static void cmdP(const char* reg1, const char* reg2, const char* corCurto, const char* corRapido) {
    int idx1 = obterIndiceRegistrador(reg1);
    int idx2 = obterIndiceRegistrador(reg2);

    if (idx1 < 0 || idx2 < 0) {
        fprintf(stderr, "Erro: registrador inválido\n");
        return;
    }

    fprintf(arquivoTxt, "[*] p? R%d R%d %s %s\n", idx1, idx2, corCurto, corRapido);

    if (!registradores[idx1].ativo || !registradores[idx2].ativo) {
        fprintf(arquivoTxt, "Erro: registrador não inicializado\n");
        return;
    }

    Vertice vOrigem = encontrarVerticeMaisProximo(grafo, registradores[idx1].x, registradores[idx1].y);
    Vertice vDestino = encontrarVerticeMaisProximo(grafo, registradores[idx2].x, registradores[idx2].y);

    if (!vOrigem || !vDestino) {
        fprintf(arquivoTxt, "Erro: não foi possível encontrar vértices próximos\n");
        return;
    }

    const char* idOrigem = getVerticeId(vOrigem);
    const char* idDestino = getVerticeId(vDestino);

    Caminho caminhoCurto = menorCaminho(grafo, idOrigem, idDestino);
    Caminho caminhoRapido = caminhoMaisRapido(grafo, idOrigem, idDestino);

    fprintf(arquivoTxt, "Origem: (%.2f, %.2f) -> vértice %s\n",
            registradores[idx1].x, registradores[idx1].y, idOrigem);
    fprintf(arquivoTxt, "Destino: (%.2f, %.2f) -> vértice %s\n",
            registradores[idx2].x, registradores[idx2].y, idDestino);

    gerarDescricaoPercurso(arquivoTxt, caminhoCurto, "Caminho mais curto");
    gerarDescricaoPercurso(arquivoTxt, caminhoRapido, "Caminho mais rápido");

    if (!caminhoCurto && !caminhoRapido) {
        fprintf(arquivoTxt, "Destino inacessível!\n");
        deletaCaminho(caminhoCurto);
        deletaCaminho(caminhoRapido);
        return;
    }

    desenharPontoReferencia(svgFile, registradores[idx1].x, registradores[idx1].y, "I");
    desenharPontoReferencia(svgFile, registradores[idx2].x, registradores[idx2].y, "F");

    if (caminhoCurto) {
        char d[16384] = "";
        char segmento[256];
        int numArestas = tamanhoCaminho(caminhoCurto);
        
        for (int i = 0; i < numArestas; i++) {
            Aresta a = getArestaCaminho(caminhoCurto, i);
            if (a) {
                Vertice origem = getOrigem(a);
                Vertice destino = getDestino(a);
                double x1 = getVerticeX(origem);
                double y1 = getVerticeY(origem);
                double x2 = getVerticeX(destino);
                double y2 = getVerticeY(destino);
                
                if (i == 0) {
                    snprintf(segmento, sizeof(segmento), "M%.2f %.2f L%.2f %.2f", x1, y1, x2, y2);
                } else {
                    snprintf(segmento, sizeof(segmento), " L%.2f %.2f", x2, y2);
                }
                strncat(d, segmento, sizeof(d) - strlen(d) - 1);
            }
        }
        
        if (strlen(d) > 0) {
            desenharCaminhoAnimado(svgFile, d, corCurto, 3.0, "none", 1.0, 3.0, "1");
        }
    }

    if (caminhoRapido) {
        char d[16384] = "";
        char segmento[256];
        int numArestas = tamanhoCaminho(caminhoRapido);
        
        for (int i = 0; i < numArestas; i++) {
            Aresta a = getArestaCaminho(caminhoRapido, i);
            if (a) {
                Vertice origem = getOrigem(a);
                Vertice destino = getDestino(a);
                double x1 = getVerticeX(origem);
                double y1 = getVerticeY(origem);
                double x2 = getVerticeX(destino);
                double y2 = getVerticeY(destino);
                
                if (i == 0) {
                    snprintf(segmento, sizeof(segmento), "M%.2f %.2f L%.2f %.2f", x1, y1, x2, y2);
                } else {
                    snprintf(segmento, sizeof(segmento), " L%.2f %.2f", x2, y2);
                }
                strncat(d, segmento, sizeof(d) - strlen(d) - 1);
            }
        }
        
        if (strlen(d) > 0) {
            desenharCaminhoAnimado(svgFile, d, corRapido, 3.0, "none", 1.0, 3.0, "1");
        }
    }

    deletaCaminho(caminhoCurto);
    deletaCaminho(caminhoRapido);
}


static void processarComandoGeo(const char* linha) {
    if (linha[0] == '\n' || linha[0] == '#' || linha[0] == '\0') return;

    char comando[10];
    sscanf(linha, "%s", comando);

    if (strcmp(comando, "q") == 0) {
        char cep[32];
        double x, y, w, h;
        if (sscanf(linha, "%*s %s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5) {
            cmdQ(cep, x, y, w, h);
        }
    } else if (strcmp(comando, "cq") == 0) {
        char swStr[16], cfill[32], cstrk[32];
        if (sscanf(linha, "%*s %s %s %s", swStr, cfill, cstrk) == 3) {
            double sw = atof(swStr);
            cmdCq(sw, cfill, cstrk);
        }
    }
}


static void processarComandoQry(const char* linha) {
    if (linha[0] == '\n' || linha[0] == '#' || linha[0] == '\0') return;

    char comando[10];
    sscanf(linha, "%s", comando);

    if (strcmp(comando, "@o?") == 0) {
        char reg[4], cep[32], face[4];
        int num;
    if (sscanf(linha, "%*s %s %s %s %d", reg, cep, face, &num) == 4) {
        cmdAtO(reg, cep, face[0], num);  
     }
   } else if (strcmp(comando, "mvm") == 0) {
        double x, y, w, h, v;
        if (sscanf(linha, "%*s %lf %lf %lf %lf %lf", &x, &y, &w, &h, &v) == 5) {
            cmdMvm(x, y, w, h, v);
         }
    } else if (strcmp(comando, "regs") == 0) {
        double vl;
        if (sscanf(linha, "%*s %lf", &vl) == 1) {
            cmdRegs(vl);
        }
    } else if (strcmp(comando, "exp") == 0) {
        double vl;
        if (sscanf(linha, "%*s %lf", &vl) == 1) {
            cmdExp(vl);
        }
    } else if (strcmp(comando, "p?") == 0) {
    char reg1[4], reg2[4], corCurto[32], corRapido[32];
    if (sscanf(linha, "%*s %s %s %s %s", reg1, reg2, corCurto, corRapido) == 4) {
        cmdP(reg1, reg2, corCurto, corRapido);
      }
  }
}




void inicializarSistema(const char* nomeBase, const char* outputDir) {
    strncpy(nomeBaseGlobal, nomeBase, FILE_NAME_LEN - 1);
    nomeBaseGlobal[FILE_NAME_LEN - 1] = '\0';

    strncpy(outputDirGlobal, outputDir, PATH_LEN - 1);
    outputDirGlobal[PATH_LEN - 1] = '\0';

    char nomeHF[PATH_LEN];

    snprintf(nomeHF, sizeof(nomeHF), "%s-quadras", nomeBase);
    hfQuadras = criarHF(nomeHF, 4, 2);

    grafo = criaGrafo();
    inicializarRegistradores();
    printf("[SISTEMA] Sistema inicializado\n");
}


void finalizarSistema(void) {
    if (hfQuadras) {
        gerarDumpHF(hfQuadras);
        freeHF(hfQuadras);
        hfQuadras = NULL;
    }

    if (grafo) {
        deletaGrafo(grafo);
        grafo = NULL;
    }

    if (arquivoTxt) {
        fclose(arquivoTxt);
        arquivoTxt = NULL;
    }

    if (svgFile) {
        fecharSVG(svgFile);
        svgFile = NULL;
    }

    printf("[SISTEMA] Sistema finalizado\n");
}


void processarArquivoGeo(const char* caminho, const char* inputDir,
                         const char* nomeBase, const char* outputDir) {
    char caminhoCompleto[PATH_LEN];
    construirCaminhoCompleto(inputDir, caminho, caminhoCompleto);

    FILE* f;
    abrirArquivo(&f, caminhoCompleto);

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        processarComandoGeo(linha);
    }

    fclose(f);

    char caminhoSvg[PATH_LEN];
    gerarNomeGeoSvg(nomeBase, outputDir, caminhoSvg);

    svgFile = criarSVG(caminhoSvg, 0, 0, 9000, 4500);
    if (svgFile) {
        FILE* file = getArquivoSVG(svgFile);
        desenharQuadrasSvg(hfQuadras, file);
        fecharSVG(svgFile);
        svgFile = NULL;
    }

    printf("[GEO] Arquivo SVG base gerado: %s\n", caminhoSvg);
}


void processarArquivoVia(const char* caminho, const char* inputDir) {
    char caminhoCompleto[PATH_LEN];
    construirCaminhoCompleto(inputDir, caminho, caminhoCompleto);

    FILE* f;
    abrirArquivo(&f, caminhoCompleto);
    processarArquivoViaInterno(f);
    fclose(f);

    int nV = quantVertices(grafo);
    int nA = quantArestas(grafo);
    printf("[VIA] Grafo construído com %d vértices e %d arestas\n", nV, nA);
    
    Vertice v = primeiroVertice(grafo);
    if (v) {
        printf("[VIA] Primeiro vértice: %s (%.2f, %.2f)\n", 
               getVerticeId(v), getVerticeX(v), getVerticeY(v));
    } else {
        printf("[VIA] ERRO: Nenhum vértice no grafo!\n");
    }
}


void processarArquivoQry(const char* caminho, const char* inputDir,
                         const char* nomeBase, const char* outputDir) {
    char caminhoCompleto[PATH_LEN];
    construirCaminhoCompleto(inputDir, caminho, caminhoCompleto);

    char nomeBaseQry[FILE_NAME_LEN];
    extrairNomeBase(caminho, nomeBaseQry);

    char caminhoTxt[PATH_LEN];
    gerarNomeQryTxt(nomeBase, nomeBaseQry, outputDir, caminhoTxt);

    arquivoTxt = fopen(caminhoTxt, "w");
    if (!arquivoTxt) {
        fprintf(stderr, "Erro: Não foi possível criar %s\n", caminhoTxt);
        return;
    }

    char caminhoSvg[PATH_LEN];
    gerarNomeQrySvg(nomeBase, nomeBaseQry, outputDir, caminhoSvg);

    svgFile = criarSVG(caminhoSvg, 0, 0, 9000, 4500);
    if (svgFile) {
        FILE* file = getArquivoSVG(svgFile);
        desenharQuadrasSvg(hfQuadras, file);
    }

    FILE* f;
    abrirArquivo(&f, caminhoCompleto);

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        processarComandoQry(linha);
    }

    fclose(f);

    if (svgFile) {
        fecharSVG(svgFile);
        svgFile = NULL;
    }

    if (arquivoTxt) {
        fclose(arquivoTxt);
        arquivoTxt = NULL;
    }

    printf("[QRY] Arquivo SVG final gerado: %s\n", caminhoSvg);
    printf("[QRY] Arquivo TXT gerado: %s\n", caminhoTxt);
}
