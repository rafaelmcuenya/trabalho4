#include "leitor.h"
#include "trataNomeArquivo.h"
#include "quadra.h"
#include "habitante.h"
#include "morador.h"
#include "hashfile.h"
#include "svg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINHA 1024
#define TAM_BUFFER_SERIAL 4096

static HashFile *hfQuadras = NULL;
static HashFile *hfHabitantes = NULL;
static HashFile *hfMoradores = NULL;

static char corPreenchimento[32] = "white";
static char corBorda[32] = "black";
static double espessuraBorda = 1.0;

static char nomeBaseGlobal[FILE_NAME_LEN];
static char outputDirGlobal[PATH_LEN];

static FILE *arquivoTxt = NULL;
static FILE *svgFileQry = NULL;

static int homensCenso = 0;
static int mulheresCenso = 0;
static int homensMoradoresCenso = 0;
static int mulheresMoradoresCenso = 0;
static int semTetoHomensCenso = 0;
static int semTetoMulheresCenso = 0;

static int pqFaceN = 0;
static int pqFaceS = 0;
static int pqFaceL = 0;
static int pqFaceO = 0;
static char pqCepAtual[32];

static void construirCaminhoCompleto(const char *baseDir, const char *arquivo, char *caminhoCompleto) {
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

static void abrirArquivo(FILE **f, const char *caminho) {
    *f = fopen(caminho, "r");
    if (!(*f)) {
        fprintf(stderr, "Erro: Não foi possível abrir %s\n", caminho);
        exit(1);
    }
}

static void cmdQ(const char *cep, double x, double y, double w, double h,
                 const char *cfill, const char *cstrk, double sw) {
    Quadra q = criaQuadra(cep, x, y, w, h, cfill, cstrk, sw);
    if (!q) {
        fprintf(stderr, "Erro: falha ao criar quadra %s\n", cep);
        return;
    }

    size_t tam = tamSerialQuadra();
    void *buffer = malloc(tam);
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

static void cmdCq(double sw, const char *cfill, const char *cstrk) {
    espessuraBorda = sw;
    strncpy(corPreenchimento, cfill, 31);
    corPreenchimento[31] = '\0';
    strncpy(corBorda, cstrk, 31);
    corBorda[31] = '\0';
}

static void cmdP(const char *cpf, const char *nome, const char *sobrenome,
                 char sexo, const char *nasc) {
    size_t tamExistente = tamSerialHabitante();
    void *bufferExistente = malloc(tamExistente);
    if (bufferExistente) {
        if (buscaHF(hfHabitantes, cpf, bufferExistente, &tamExistente) == 1) {
            fprintf(stderr, "Aviso: habitante com CPF %s já existe\n", cpf);
            free(bufferExistente);
            return;
        }
        free(bufferExistente);
    }

    Habitante h = criaHabitante(cpf, nome, sobrenome, sexo, nasc);
    if (!h) {
        fprintf(stderr, "Erro: falha ao criar habitante %s\n", cpf);
        return;
    }

    size_t tam = tamSerialHabitante();
    void *buffer = malloc(tam);
    if (!buffer) {
        fprintf(stderr, "Erro: falha de alocação ao serializar habitante\n");
        freeHabitante(h);
        return;
    }

    serialHabitante(h, buffer, tam);
    int ret = insertHF(hfHabitantes, cpf, buffer, tam);

    if (ret < 0) {
        fprintf(stderr, "Erro: falha ao inserir habitante %s no hashfile\n", cpf);
    }

    free(buffer);
    freeHabitante(h);
}

static void cmdM(const char *cpf, const char *cep, char face, int num, const char *compl) {
    size_t tamHab = tamSerialHabitante();
    void *bufferHab = malloc(tamHab);
    if (!bufferHab) return;

    if (buscaHF(hfHabitantes, cpf, bufferHab, &tamHab) != 1) {
        fprintf(stderr, "Erro: habitante com CPF %s não encontrado para moradia\n", cpf);
        free(bufferHab);
        return;
    }

    Habitante h = desserialHabitante(bufferHab, tamHab);
    free(bufferHab);

    if (!h) return;

    if (isMoradorHabitante(h)) {
        fprintf(stderr, "Aviso: habitante %s já possui moradia\n", cpf);
        freeHabitante(h);
        return;
    }

    char endereco[128];
    if (compl && strlen(compl) > 0) {
        snprintf(endereco, sizeof(endereco), "%s/%c/%d/%s", cep, face, num, compl);
    } else {
        snprintf(endereco, sizeof(endereco), "%s/%c/%d", cep, face, num);
    }
    setIdMoradiaHabitante(h, endereco);

    size_t tamHabAtualizado = tamSerialHabitante();
    void *bufferAtualizado = malloc(tamHabAtualizado);
    if (!bufferAtualizado) {
        freeHabitante(h);
        return;
    }

    serialHabitante(h, bufferAtualizado, tamHabAtualizado);
    refreshHF(hfHabitantes, cpf, bufferAtualizado, tamHabAtualizado);
    free(bufferAtualizado);

    Morador m = criaMorador(cpf, getNomeHabitante(h), getSobrenomeHabitante(h),
                            getSexoHabitante(h), getNascHabitante(h),
                            cep, face, num, compl);

    if (!m) {
        fprintf(stderr, "Erro: falha ao criar morador %s\n", cpf);
        freeHabitante(h);
        return;
    }

    size_t tamMor = tamSerialMorador();
    void *bufferMor = malloc(tamMor);
    if (!bufferMor) {
        freeMorador(m);
        freeHabitante(h);
        return;
    }

    serialMorador(m, bufferMor, tamMor);
    int ret = insertHF(hfMoradores, cpf, bufferMor, tamMor);

    if (ret == 0) {
        fprintf(stderr, "Aviso: morador com CPF %s já existe\n", cpf);
    } else if (ret < 0) {
        fprintf(stderr, "Erro: falha ao inserir morador %s no hashfile\n", cpf);
    }

    free(bufferMor);
    freeMorador(m);
    freeHabitante(h);
}

static void callbackRq(const char *chave, const void *dado, sizeT tamDado, void *contexto) {
    const char *cep = (const char*)contexto;

    Morador m = desserialMorador((void*)dado, tamDado);
    if (!m) return;

    if (strcmp(getCepMorador(m), cep) == 0) {
        fprintf(arquivoTxt, "  CPF: %s - Nome: %s %s\n",
                getCpfMorador(m), getNomeMorador(m), getSobrenomeMorador(m));

        size_t tamHab = tamSerialHabitante();
        void *bufferHab = malloc(tamHab);
        if (bufferHab) {
            if (buscaHF(hfHabitantes, getCpfMorador(m), bufferHab, &tamHab) == 1) {
                Habitante h = desserialHabitante(bufferHab, tamHab);
                if (h) {
                    setIdMoradiaHabitante(h, NULL);
                    size_t tamAtualizado = tamSerialHabitante();
                    void *bufferAtualizado = malloc(tamAtualizado);
                    if (bufferAtualizado) {
                        serialHabitante(h, bufferAtualizado, tamAtualizado);
                        refreshHF(hfHabitantes, getCpfMorador(m), bufferAtualizado, tamAtualizado);
                        free(bufferAtualizado);
                    }
                    freeHabitante(h);
                }
            }
            free(bufferHab);
        }

        deletarItemHF(hfMoradores, getCpfMorador(m));
    }

    freeMorador(m);
}

static void cmdRq(const char *cep) {
    size_t tamQuadra = tamSerialQuadra();
    void *bufferQuadra = malloc(tamQuadra);
    if (!bufferQuadra) return;

    Quadra q = NULL;
    if (buscaHF(hfQuadras, cep, bufferQuadra, &tamQuadra) == 1) {
        q = desserialQuadra(bufferQuadra, tamQuadra);
    }
    free(bufferQuadra);

    if (!q) {
        fprintf(stderr, "Erro: quadra %s não encontrada para remoção\n", cep);
        return;
    }

    fprintf(arquivoTxt, "[*] rq %s\n", cep);
    fprintf(arquivoTxt, "Moradores removidos:\n");

    iterarHF(hfMoradores, callbackRq, (void*)cep);

    if (svgFileQry) {
        svgMarcarRemocaoQuadra(svgFileQry, q);
    }

    freeQuadra(q);
    deletarItemHF(hfQuadras, cep);
}

static void callbackPq(const char *chave, const void *dado, sizeT tamDado, void *contexto) {
    Morador m = desserialMorador((void*)dado, tamDado);
    if (!m) return;

    if (strcmp(getCepMorador(m), pqCepAtual) == 0) {
        char face = getFaceMorador(m);
        if (face == 'N') pqFaceN++;
        else if (face == 'S') pqFaceS++;
        else if (face == 'L') pqFaceL++;
        else if (face == 'O') pqFaceO++;
    }

    freeMorador(m);
}

static void cmdPq(const char *cep) {
    pqFaceN = 0;
    pqFaceS = 0;
    pqFaceL = 0;
    pqFaceO = 0;
    strncpy(pqCepAtual, cep, 31);
    pqCepAtual[31] = '\0';

    iterarHF(hfMoradores, callbackPq, NULL);

    int totalQuadra = pqFaceN + pqFaceS + pqFaceL + pqFaceO;

    fprintf(arquivoTxt, "[*] pq %s\n", cep);
    fprintf(arquivoTxt, "Face N: %d moradores\n", pqFaceN);
    fprintf(arquivoTxt, "Face S: %d moradores\n", pqFaceS);
    fprintf(arquivoTxt, "Face L: %d moradores\n", pqFaceL);
    fprintf(arquivoTxt, "Face O: %d moradores\n", pqFaceO);
    fprintf(arquivoTxt, "Total: %d moradores\n", totalQuadra);

    if (svgFileQry) {
        size_t tamQ = tamSerialQuadra();
        void *bufQ = malloc(tamQ);
        if (bufQ && buscaHF(hfQuadras, cep, bufQ, &tamQ) == 1) {
            Quadra q = desserialQuadra(bufQ, tamQ);
            if (q) {
                svgMarcarMoradoresPorFace(svgFileQry, q, pqFaceN, pqFaceS, pqFaceL, pqFaceO);
                freeQuadra(q);
            }
        }
        free(bufQ);
    }
}

static void callbackCensoHab(const char *chave, const void *dado, sizeT tamDado, void *contexto) {
    Habitante h = desserialHabitante((void*)dado, tamDado);
    if (!h) return;

    char sexo = getSexoHabitante(h);
    if (sexo == 'M') {
        homensCenso++;
        if (!isMoradorHabitante(h)) {
            semTetoHomensCenso++;
        }
    } else if (sexo == 'F') {
        mulheresCenso++;
        if (!isMoradorHabitante(h)) {
            semTetoMulheresCenso++;
        }
    }

    freeHabitante(h);
}

static void callbackCensoMor(const char *chave, const void *dado, sizeT tamDado, void *contexto) {
    Morador m = desserialMorador((void*)dado, tamDado);
    if (!m) return;

    char sexo = getSexoMorador(m);
    if (sexo == 'M') {
        homensMoradoresCenso++;
    } else if (sexo == 'F') {
        mulheresMoradoresCenso++;
    }

    freeMorador(m);
}

static void cmdCenso(void) {
    homensCenso = 0;
    mulheresCenso = 0;
    homensMoradoresCenso = 0;
    mulheresMoradoresCenso = 0;
    semTetoHomensCenso = 0;
    semTetoMulheresCenso = 0;

    iterarHF(hfHabitantes, callbackCensoHab, NULL);
    iterarHF(hfMoradores, callbackCensoMor, NULL);

    int totalHabitantes = homensCenso + mulheresCenso;
    int totalMoradores = homensMoradoresCenso + mulheresMoradoresCenso;
    int semTeto = totalHabitantes - totalMoradores;
    int semTetoHomens = semTetoHomensCenso;
    int semTetoMulheres = semTetoMulheresCenso;

    fprintf(arquivoTxt, "[*] censo\n");
    fprintf(arquivoTxt, "Total de habitantes: %d\n", totalHabitantes);
    fprintf(arquivoTxt, "Total de moradores: %d\n", totalMoradores);

    if (totalHabitantes > 0) {
        fprintf(arquivoTxt, "Proporção moradores/habitantes: %.2f%%\n",
                100.0 * totalMoradores / totalHabitantes);
    }

    fprintf(arquivoTxt, "Homens: %d\n", homensCenso);
    fprintf(arquivoTxt, "Mulheres: %d\n", mulheresCenso);

    if (totalHabitantes > 0) {
        fprintf(arquivoTxt, "%% Homens: %.2f%%\n", 100.0 * homensCenso / totalHabitantes);
        fprintf(arquivoTxt, "%% Mulheres: %.2f%%\n", 100.0 * mulheresCenso / totalHabitantes);
    }

    if (totalMoradores > 0) {
        fprintf(arquivoTxt, "%% Moradores homens: %.2f%%\n", 100.0 * homensMoradoresCenso / totalMoradores);
        fprintf(arquivoTxt, "%% Moradores mulheres: %.2f%%\n", 100.0 * mulheresMoradoresCenso / totalMoradores);
    }

    fprintf(arquivoTxt, "Sem-teto: %d\n", semTeto);

    if (semTeto > 0) {
        fprintf(arquivoTxt, "%% Sem-teto homens: %.2f%%\n", 100.0 * semTetoHomens / semTeto);
        fprintf(arquivoTxt, "%% Sem-teto mulheres: %.2f%%\n", 100.0 * semTetoMulheres / semTeto);
    }
}

static void cmdH(const char *cpf) {
    fprintf(arquivoTxt, "[*] h? %s\n", cpf);

    size_t tamHab = tamSerialHabitante();
    void *bufferHab = malloc(tamHab);
    if (!bufferHab) return;

    if (buscaHF(hfHabitantes, cpf, bufferHab, &tamHab) == 1) {
        Habitante h = desserialHabitante(bufferHab, tamHab);
        if (h) {
            fprintf(arquivoTxt, "CPF: %s\n", getCpfHabitante(h));
            fprintf(arquivoTxt, "Nome: %s %s\n", getNomeHabitante(h), getSobrenomeHabitante(h));
            fprintf(arquivoTxt, "Sexo: %c\n", getSexoHabitante(h));
            fprintf(arquivoTxt, "Nascimento: %s\n", getNascHabitante(h));

            if (isMoradorHabitante(h)) {
                fprintf(arquivoTxt, "Endereço: %s\n", getIdMoradiaHabitante(h));
            } else {
                fprintf(arquivoTxt, "Situação: Sem-teto\n");
            }

            freeHabitante(h);
        }
    } else {
        fprintf(arquivoTxt, "Habitante não encontrado\n");
    }

    free(bufferHab);
}

static void cmdNasc(const char *cpf, const char *nome, const char *sobrenome,
                    char sexo, const char *nasc) {
    fprintf(arquivoTxt, "[*] nasc %s %s %s %c %s\n", cpf, nome, sobrenome, sexo, nasc);

    size_t tamExistente = tamSerialHabitante();
    void *bufferExistente = malloc(tamExistente);
    if (bufferExistente) {
        if (buscaHF(hfHabitantes, cpf, bufferExistente, &tamExistente) == 1) {
            fprintf(arquivoTxt, "Erro: CPF %s já existe\n", cpf);
            free(bufferExistente);
            return;
        }
        free(bufferExistente);
    }

    Habitante h = criaHabitante(cpf, nome, sobrenome, sexo, nasc);
    if (!h) {
        fprintf(arquivoTxt, "Erro ao criar habitante\n");
        return;
    }

    size_t tam = tamSerialHabitante();
    void *buffer = malloc(tam);
    if (buffer) {
        serialHabitante(h, buffer, tam);
        insertHF(hfHabitantes, cpf, buffer, tam);
        free(buffer);
    }

    fprintf(arquivoTxt, "Habitante criado: %s %s\n", nome, sobrenome);
    freeHabitante(h);
}

static void cmdRip(const char *cpf) {
    fprintf(arquivoTxt, "[*] rip %s\n", cpf);

    size_t tamHab = tamSerialHabitante();
    void *bufferHab = malloc(tamHab);
    if (!bufferHab) return;

    if (buscaHF(hfHabitantes, cpf, bufferHab, &tamHab) == 1) {
        Habitante h = desserialHabitante(bufferHab, tamHab);
        if (h) {
            fprintf(arquivoTxt, "Falecido: %s %s\n", getNomeHabitante(h), getSobrenomeHabitante(h));
            fprintf(arquivoTxt, "CPF: %s\n", getCpfHabitante(h));
            fprintf(arquivoTxt, "Sexo: %c\n", getSexoHabitante(h));
            fprintf(arquivoTxt, "Nascimento: %s\n", getNascHabitante(h));

            if (isMoradorHabitante(h)) {
                const char *idMoradia = getIdMoradiaHabitante(h);
                fprintf(arquivoTxt, "Endereço: %s\n", idMoradia);

                if (svgFileQry && idMoradia) {
                    char cep[32], face;
                    int num;
                    if (sscanf(idMoradia, "%[^/]/%c/%d", cep, &face, &num) == 3) {
                        size_t tamQ = tamSerialQuadra();
                        void *bufQ = malloc(tamQ);
                        if (bufQ && buscaHF(hfQuadras, cep, bufQ, &tamQ) == 1) {
                            Quadra q = desserialQuadra(bufQ, tamQ);
                            if (q) {
                                double x, y;
                                getPontoEndQuadra(q, face, (double)num, &x, &y);
                                svgMarcarFalecimento(svgFileQry, x, y);
                                freeQuadra(q);
                            }
                        }
                        free(bufQ);
                    }
                }

                deletarItemHF(hfMoradores, cpf);
            }

            freeHabitante(h);
        }

        deletarItemHF(hfHabitantes, cpf);
    } else {
        fprintf(arquivoTxt, "Habitante não encontrado\n");
    }

    free(bufferHab);
}

static void cmdMud(const char *cpf, const char *cep, char face, int num, const char *compl) {
    fprintf(arquivoTxt, "[*] mud %s %s %c %d %s\n", cpf, cep, face, num, compl ? compl : "");

    size_t tamHab = tamSerialHabitante();
    void *bufferHab = malloc(tamHab);
    if (!bufferHab) return;

    if (buscaHF(hfHabitantes, cpf, bufferHab, &tamHab) != 1) {
        fprintf(arquivoTxt, "Habitante não encontrado\n");
        free(bufferHab);
        return;
    }

    Habitante h = desserialHabitante(bufferHab, tamHab);
    free(bufferHab);

    if (!h) return;

    size_t tamQ = tamSerialQuadra();
    void *bufQ = malloc(tamQ);
    Quadra q = NULL;
    if (bufQ && buscaHF(hfQuadras, cep, bufQ, &tamQ) == 1) {
        q = desserialQuadra(bufQ, tamQ);
    }
    free(bufQ);

    deletarItemHF(hfMoradores, cpf);

    char endereco[128];
    if (compl && strlen(compl) > 0) {
        snprintf(endereco, sizeof(endereco), "%s/%c/%d/%s", cep, face, num, compl);
    } else {
        snprintf(endereco, sizeof(endereco), "%s/%c/%d", cep, face, num);
    }
    setIdMoradiaHabitante(h, endereco);

    size_t tamAtualizado = tamSerialHabitante();
    void *bufferAtualizado = malloc(tamAtualizado);
    if (bufferAtualizado) {
        serialHabitante(h, bufferAtualizado, tamAtualizado);
        refreshHF(hfHabitantes, cpf, bufferAtualizado, tamAtualizado);
        free(bufferAtualizado);
    }

    Morador m = criaMorador(cpf, getNomeHabitante(h), getSobrenomeHabitante(h),
                            getSexoHabitante(h), getNascHabitante(h),
                            cep, face, num, compl);
    if (m) {
        size_t tamMor = tamSerialMorador();
        void *bufferMor = malloc(tamMor);
        if (bufferMor) {
            serialMorador(m, bufferMor, tamMor);
            insertHF(hfMoradores, cpf, bufferMor, tamMor);
            free(bufferMor);
        }
        freeMorador(m);
    }

    if (svgFileQry && q) {
        double x, y;
        getPontoEndQuadra(q, face, (double)num, &x, &y);
        svgMarcarMudanca(svgFileQry, x, y, cpf);
    }

    if (q) freeQuadra(q);
    freeHabitante(h);
    fprintf(arquivoTxt, "Mudança realizada\n");
}

static void cmdDspj(const char *cpf) {
    fprintf(arquivoTxt, "[*] dspj %s\n", cpf);

    size_t tamMor = tamSerialMorador();
    void *bufferMor = malloc(tamMor);
    if (!bufferMor) return;

    if (buscaHF(hfMoradores, cpf, bufferMor, &tamMor) == 1) {
        Morador m = desserialMorador(bufferMor, tamMor);
        if (m) {
            fprintf(arquivoTxt, "Despejado: %s %s\n", getNomeMorador(m), getSobrenomeMorador(m));
            fprintf(arquivoTxt, "CPF: %s\n", getCpfMorador(m));

            const char *endereco = getEnderecoCompletoMorador(m);
            fprintf(arquivoTxt, "Endereço do despejo: %s\n", endereco);
            free((void*)endereco);

            if (svgFileQry) {
                const char *cep = getCepMorador(m);
                char face = getFaceMorador(m);
                int num = getNumMorador(m);

                size_t tamQ = tamSerialQuadra();
                void *bufQ = malloc(tamQ);
                if (bufQ && buscaHF(hfQuadras, cep, bufQ, &tamQ) == 1) {
                    Quadra q = desserialQuadra(bufQ, tamQ);
                    if (q) {
                        double x, y;
                        getPontoEndQuadra(q, face, (double)num, &x, &y);
                        svgMarcarDespejo(svgFileQry, x, y);
                        freeQuadra(q);
                    }
                }
                free(bufQ);
            }

            freeMorador(m);
        }

        deletarItemHF(hfMoradores, cpf);

        size_t tamHab = tamSerialHabitante();
        void *bufferHab = malloc(tamHab);
        if (bufferHab) {
            if (buscaHF(hfHabitantes, cpf, bufferHab, &tamHab) == 1) {
                Habitante h = desserialHabitante(bufferHab, tamHab);
                if (h) {
                    setIdMoradiaHabitante(h, NULL);
                    size_t tamAtualizado = tamSerialHabitante();
                    void *bufferAtualizado = malloc(tamAtualizado);
                    if (bufferAtualizado) {
                        serialHabitante(h, bufferAtualizado, tamAtualizado);
                        refreshHF(hfHabitantes, cpf, bufferAtualizado, tamAtualizado);
                        free(bufferAtualizado);
                    }
                    freeHabitante(h);
                }
            }
            free(bufferHab);
        }
    } else {
        fprintf(arquivoTxt, "Morador não encontrado\n");
    }

    free(bufferMor);
}

static void processarComandoGeo(const char *linha) {
    if (linha[0] == '\n' || linha[0] == '#' || linha[0] == '\0') return;

    char comando[10];
    sscanf(linha, "%s", comando);

    if (strcmp(comando, "q") == 0) {
        char cep[32];
        double x, y, w, h;
        if (sscanf(linha, "%*s %s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5) {
            cmdQ(cep, x, y, w, h, corPreenchimento, corBorda, espessuraBorda);
        }
    }
    else if (strcmp(comando, "cq") == 0) {
        char swStr[16], cfill[32], cstrk[32];
        if (sscanf(linha, "%*s %s %s %s", swStr, cfill, cstrk) == 3) {
            double sw = atof(swStr);
            cmdCq(sw, cfill, cstrk);
        }
    }
}

static void processarComandoPm(const char *linha) {
    if (linha[0] == '\n' || linha[0] == '#' || linha[0] == '\0') return;

    char comando[10];
    sscanf(linha, "%s", comando);

    if (strcmp(comando, "p") == 0) {
        char cpf[32], nome[100], sobrenome[100], sexo[4], nasc[11];
        if (sscanf(linha, "%*s %s %s %s %s %s", cpf, nome, sobrenome, sexo, nasc) == 5) {
            cmdP(cpf, nome, sobrenome, sexo[0], nasc);
        }
    }
    else if (strcmp(comando, "m") == 0) {
        char cpf[32], cep[32], face[4];
        int num;
        char resto[128];
        int campos = sscanf(linha, "%*s %s %s %s %d %[^\n]", cpf, cep, face, &num, resto);
        if (campos >= 4) {
            char *compl = NULL;
            if (campos == 5) {
                char *p = resto;
                while (*p == ' ') p++;
                if (strlen(p) > 0) compl = p;
            }
            cmdM(cpf, cep, face[0], num, compl);
        }
    }
}

static void processarComandoQry(const char *linha) {
    if (linha[0] == '\n' || linha[0] == '#' || linha[0] == '\0') return;

    char comando[10];
    sscanf(linha, "%s", comando);

    if (strcmp(comando, "rq") == 0) {
        char cep[32];
        if (sscanf(linha, "%*s %s", cep) == 1) {
            cmdRq(cep);
        }
    }
    else if (strcmp(comando, "pq") == 0) {
        char cep[32];
        if (sscanf(linha, "%*s %s", cep) == 1) {
            cmdPq(cep);
        }
    }
    else if (strcmp(comando, "censo") == 0) {
        cmdCenso();
    }
    else if (strcmp(comando, "h?") == 0 || strcmp(comando, "h\?") == 0) {
        char cpf[32];
        if (sscanf(linha, "%*s %s", cpf) == 1) {
            cmdH(cpf);
        }
    }
    else if (strcmp(comando, "nasc") == 0) {
        char cpf[32], nome[100], sobrenome[100], sexo[4], nasc[11];
        if (sscanf(linha, "%*s %s %s %s %s %s", cpf, nome, sobrenome, sexo, nasc) == 5) {
            cmdNasc(cpf, nome, sobrenome, sexo[0], nasc);
        }
    }
    else if (strcmp(comando, "rip") == 0) {
        char cpf[32];
        if (sscanf(linha, "%*s %s", cpf) == 1) {
            cmdRip(cpf);
        }
    }
    else if (strcmp(comando, "mud") == 0) {
      char cpf[32], cep[32], faceLetra;
      int num;
      char resto[128];
      int campos = sscanf(linha, "%*s %s %s %c %d %[^\n]", cpf, cep, &faceLetra, &num, resto);
      if (campos >= 4) {
        char *compl = NULL;
        if (campos == 5) {
            char *p = resto;
            while (*p == ' ') p++;
            if (strlen(p) > 0) compl = p;
        }
        cmdMud(cpf, cep, faceLetra, num, compl);
       }
    }
    else if (strcmp(comando, "dspj") == 0) {
        char cpf[32];
        if (sscanf(linha, "%*s %s", cpf) == 1) {
            cmdDspj(cpf);
        }
    }
}

void inicializarSistema(const char *nomeBase, const char *outputDir) {
    strncpy(nomeBaseGlobal, nomeBase, FILE_NAME_LEN - 1);
    nomeBaseGlobal[FILE_NAME_LEN - 1] = '\0';

    strncpy(outputDirGlobal, outputDir, PATH_LEN - 1);
    outputDirGlobal[PATH_LEN - 1] = '\0';

    char nomeHF[PATH_LEN];

    snprintf(nomeHF, sizeof(nomeHF), "%s-quadras", nomeBase);
    hfQuadras = criarHF(nomeHF, 4, 2);

    snprintf(nomeHF, sizeof(nomeHF), "%s-habitantes", nomeBase);
    hfHabitantes = criarHF(nomeHF, 4, 2);

    snprintf(nomeHF, sizeof(nomeHF), "%s-moradores", nomeBase);
    hfMoradores = criarHF(nomeHF, 4, 2);

    printf("[SISTEMA] Sistema inicializado\n");
}

void finalizarSistema(void) {
    if (hfQuadras) {
        gerarDumpHF(hfQuadras);
        freeHF(hfQuadras);
        hfQuadras = NULL;
    }

    if (hfHabitantes) {
        gerarDumpHF(hfHabitantes);
        freeHF(hfHabitantes);
        hfHabitantes = NULL;
    }

    if (hfMoradores) {
        gerarDumpHF(hfMoradores);
        freeHF(hfMoradores);
        hfMoradores = NULL;
    }

    if (arquivoTxt) {
        fclose(arquivoTxt);
        arquivoTxt = NULL;
    }

    if (svgFileQry) {
        fclose(svgFileQry);
        svgFileQry = NULL;
    }

    printf("[SISTEMA] Sistema finalizado\n");
}

void processarArquivoGeo(const char *caminho, const char *inputDir,
                         const char *nomeBase, const char *outputDir) {
    char caminhoCompleto[PATH_LEN];
    construirCaminhoCompleto(inputDir, caminho, caminhoCompleto);

    FILE *f;
    abrirArquivo(&f, caminhoCompleto);

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        processarComandoGeo(linha);
    }

    fclose(f);

    char caminhoSvg[PATH_LEN];
    gerarNomeGeoSvg(nomeBase, outputDir, caminhoSvg);
    svgGeo(caminhoSvg, hfQuadras);
    printf("[GEO] Arquivo SVG base gerado: %s\n", caminhoSvg);
}

void processarArquivoPm(const char *caminho, const char *inputDir, const char *nomeBase) {
    char caminhoCompleto[PATH_LEN];
    construirCaminhoCompleto(inputDir, caminho, caminhoCompleto);

    FILE *f;
    abrirArquivo(&f, caminhoCompleto);

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        processarComandoPm(linha);
    }

    fclose(f);

    printf("[PM] Arquivo processado\n");
}

void processarArquivoQry(const char *caminho, const char *inputDir, const char *nomeBase, const char *outputDir) {
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

    svgFileQry = fopen(caminhoSvg, "w");
    if (svgFileQry) {
        fprintf(svgFileQry, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(svgFileQry, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
                            "viewBox=\"0 0 9000 4500\">\n");
    }

    FILE *f;
    abrirArquivo(&f, caminhoCompleto);

    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        processarComandoQry(linha);
    }

    fclose(f);

    if (svgFileQry) {
        desenharQuadrasSvg(hfQuadras, svgFileQry);
        fprintf(svgFileQry, "</svg>");
        fclose(svgFileQry);
        svgFileQry = NULL;
    }

    printf("[QRY] Arquivo SVG final gerado: %s\n", caminhoSvg);
    printf("[QRY] Arquivo TXT gerado: %s\n", caminhoTxt);
}
