#include "unity.h"
#include "leitor.h"
#include "trataNomeArquivo.h"
#include "quadra.h"
#include "hashfile.h"
#include "grafo.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static char nomeBase[FILE_NAME_LEN];
static char outputDir[PATH_LEN];
static char inputDir[PATH_LEN];

void setUp(void) {
    strcpy(nomeBase, "teste");
    strcpy(outputDir, "./saida_teste");
    strcpy(inputDir, "./entrada_teste");
    
    mkdir(outputDir, 0700);
    mkdir(inputDir, 0700);
}

void tearDown(void) {
    system("rm -rf ./saida_teste");
    system("rm -rf ./entrada_teste");
    system("rm -f teste-quadras.hfb teste-quadras.hfd teste-quadras.hfc");
}

void test_inicializarSistema_sucesso(void) {
    inicializarSistema(nomeBase, outputDir);
    finalizarSistema();
}

void test_inicializarSistema_parametros_invalidos(void) {
    inicializarSistema(NULL, outputDir);
    finalizarSistema();
    
    inicializarSistema(nomeBase, NULL);
    finalizarSistema();
}

void test_processarArquivoGeo_sem_cq(void) {
    char caminho[PATH_LEN];
    snprintf(caminho, sizeof(caminho), "%s/teste.geo", inputDir);
    
    FILE* f = fopen(caminho, "w");
    fprintf(f, "q cep01 10.0 20.0 30.0 40.0\n");
    fprintf(f, "q cep02 50.0 60.0 70.0 80.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    
    char caminhoSvg[PATH_LEN];
    snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/teste.svg", outputDir);
    FILE* svg = fopen(caminhoSvg, "r");
    TEST_ASSERT_NOT_NULL(svg);
    fclose(svg);
    
    finalizarSistema();
}

void test_processarArquivoGeo_com_cq(void) {
    char caminho[PATH_LEN];
    snprintf(caminho, sizeof(caminho), "%s/teste.geo", inputDir);
    
    FILE* f = fopen(caminho, "w");
    fprintf(f, "cq 2.0 #FF0000 #00FF00\n");
    fprintf(f, "q cep01 10.0 20.0 30.0 40.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    
    char caminhoSvg[PATH_LEN];
    snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/teste.svg", outputDir);
    FILE* svg = fopen(caminhoSvg, "r");
    TEST_ASSERT_NOT_NULL(svg);
    fclose(svg);
    
    finalizarSistema();
}

void test_processarArquivoGeo_arquivo_inexistente(void) {
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("inexistente.geo", inputDir, nomeBase, outputDir);
    finalizarSistema();
}

void test_processarArquivoVia_sucesso(void) {
    char caminho[PATH_LEN];
    snprintf(caminho, sizeof(caminho), "%s/teste.via", inputDir);
    
    FILE* f = fopen(caminho, "w");
    fprintf(f, "3\n");
    fprintf(f, "v A 10.0 20.0\n");
    fprintf(f, "v B 30.0 40.0\n");
    fprintf(f, "v C 50.0 60.0\n");
    fprintf(f, "e A B cep1 cep2 100.0 10.0 Rua_AB\n");
    fprintf(f, "e B C cep3 cep4 150.0 20.0 Rua_BC\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoVia("teste.via", inputDir);
    finalizarSistema();
}

void test_processarArquivoVia_arquivo_inexistente(void) {
    inicializarSistema(nomeBase, outputDir);
    processarArquivoVia("inexistente.via", inputDir);
    finalizarSistema();
}

void test_processarArquivoQry_sem_comandos(void) {
    char caminhoGeo[PATH_LEN];
    snprintf(caminhoGeo, sizeof(caminhoGeo), "%s/teste.geo", inputDir);
    FILE* f = fopen(caminhoGeo, "w");
    fprintf(f, "q cep01 10.0 20.0 30.0 40.0\n");
    fclose(f);
    
    char caminhoQry[PATH_LEN];
    snprintf(caminhoQry, sizeof(caminhoQry), "%s/teste.qry", inputDir);
    f = fopen(caminhoQry, "w");
    fprintf(f, "\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    processarArquivoQry("teste.qry", inputDir, nomeBase, outputDir);
    
    char caminhoSvg[PATH_LEN];
    snprintf(caminhoSvg, sizeof(caminhoSvg), "%s/teste-teste.svg", outputDir);
    FILE* svg = fopen(caminhoSvg, "r");
    TEST_ASSERT_NOT_NULL(svg);
    fclose(svg);
    
    char caminhoTxt[PATH_LEN];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/teste-teste.txt", outputDir);
    FILE* txt = fopen(caminhoTxt, "r");
    TEST_ASSERT_NOT_NULL(txt);
    fclose(txt);
    
    finalizarSistema();
}

void test_processarArquivoQry_com_atO(void) {
    char caminhoGeo[PATH_LEN];
    snprintf(caminhoGeo, sizeof(caminhoGeo), "%s/teste.geo", inputDir);
    FILE* f = fopen(caminhoGeo, "w");
    fprintf(f, "q cep01 0.0 0.0 100.0 100.0\n");
    fclose(f);
    
    char caminhoVia[PATH_LEN];
    snprintf(caminhoVia, sizeof(caminhoVia), "%s/teste.via", inputDir);
    f = fopen(caminhoVia, "w");
    fprintf(f, "2\n");
    fprintf(f, "v A 10.0 10.0\n");
    fprintf(f, "v B 90.0 90.0\n");
    fprintf(f, "e A B cep1 cep2 100.0 10.0 Rua_AB\n");
    fclose(f);
    
    char caminhoQry[PATH_LEN];
    snprintf(caminhoQry, sizeof(caminhoQry), "%s/teste.qry", inputDir);
    f = fopen(caminhoQry, "w");
    fprintf(f, "@o? R0 cep01 N 50.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    processarArquivoVia("teste.via", inputDir);
    processarArquivoQry("teste.qry", inputDir, nomeBase, outputDir);
    
    char caminhoTxt[PATH_LEN];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/teste-teste.txt", outputDir);
    FILE* txt = fopen(caminhoTxt, "r");
    TEST_ASSERT_NOT_NULL(txt);
    char buffer[256];
    fgets(buffer, sizeof(buffer), txt);
    fclose(txt);
    
    finalizarSistema();
}

void test_processarArquivoQry_com_regs(void) {
    char caminhoGeo[PATH_LEN];
    snprintf(caminhoGeo, sizeof(caminhoGeo), "%s/teste.geo", inputDir);
    FILE* f = fopen(caminhoGeo, "w");
    fprintf(f, "q cep01 0.0 0.0 100.0 100.0\n");
    fclose(f);
    
    char caminhoVia[PATH_LEN];
    snprintf(caminhoVia, sizeof(caminhoVia), "%s/teste.via", inputDir);
    f = fopen(caminhoVia, "w");
    fprintf(f, "4\n");
    fprintf(f, "v A 10.0 10.0\n");
    fprintf(f, "v B 20.0 20.0\n");
    fprintf(f, "v C 30.0 30.0\n");
    fprintf(f, "v D 40.0 40.0\n");
    fprintf(f, "e A B cep1 cep2 10.0 5.0 Rua_AB\n");
    fprintf(f, "e B C cep3 cep4 10.0 5.0 Rua_BC\n");
    fprintf(f, "e C D cep5 cep6 10.0 5.0 Rua_CD\n");
    fclose(f);
    
    char caminhoQry[PATH_LEN];
    snprintf(caminhoQry, sizeof(caminhoQry), "%s/teste.qry", inputDir);
    f = fopen(caminhoQry, "w");
    fprintf(f, "regs 10.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    processarArquivoVia("teste.via", inputDir);
    processarArquivoQry("teste.qry", inputDir, nomeBase, outputDir);
    
    char caminhoTxt[PATH_LEN];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/teste-teste.txt", outputDir);
    FILE* txt = fopen(caminhoTxt, "r");
    TEST_ASSERT_NOT_NULL(txt);
    char buffer[256];
    fgets(buffer, sizeof(buffer), txt);
    fclose(txt);
    
    finalizarSistema();
}

void test_processarArquivoQry_com_mvm(void) {
    char caminhoGeo[PATH_LEN];
    snprintf(caminhoGeo, sizeof(caminhoGeo), "%s/teste.geo", inputDir);
    FILE* f = fopen(caminhoGeo, "w");
    fprintf(f, "q cep01 0.0 0.0 100.0 100.0\n");
    fclose(f);
    
    char caminhoVia[PATH_LEN];
    snprintf(caminhoVia, sizeof(caminhoVia), "%s/teste.via", inputDir);
    f = fopen(caminhoVia, "w");
    fprintf(f, "2\n");
    fprintf(f, "v A 10.0 10.0\n");
    fprintf(f, "v B 20.0 20.0\n");
    fprintf(f, "e A B cep1 cep2 10.0 5.0 Rua_AB\n");
    fclose(f);
    
    char caminhoQry[PATH_LEN];
    snprintf(caminhoQry, sizeof(caminhoQry), "%s/teste.qry", inputDir);
    f = fopen(caminhoQry, "w");
    fprintf(f, "mvm 0.0 0.0 100.0 100.0 20.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    processarArquivoVia("teste.via", inputDir);
    processarArquivoQry("teste.qry", inputDir, nomeBase, outputDir);
    
    char caminhoTxt[PATH_LEN];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/teste-teste.txt", outputDir);
    FILE* txt = fopen(caminhoTxt, "r");
    TEST_ASSERT_NOT_NULL(txt);
    fclose(txt);
    
    finalizarSistema();
}

void test_processarArquivoQry_com_exp(void) {
    char caminhoGeo[PATH_LEN];
    snprintf(caminhoGeo, sizeof(caminhoGeo), "%s/teste.geo", inputDir);
    FILE* f = fopen(caminhoGeo, "w");
    fprintf(f, "q cep01 0.0 0.0 100.0 100.0\n");
    fclose(f);
    
    char caminhoVia[PATH_LEN];
    snprintf(caminhoVia, sizeof(caminhoVia), "%s/teste.via", inputDir);
    f = fopen(caminhoVia, "w");
    fprintf(f, "4\n");
    fprintf(f, "v A 10.0 10.0\n");
    fprintf(f, "v B 20.0 20.0\n");
    fprintf(f, "v C 30.0 30.0\n");
    fprintf(f, "v D 40.0 40.0\n");
    fprintf(f, "e A B cep1 cep2 10.0 3.0 Rua_AB\n");
    fprintf(f, "e B C cep3 cep4 10.0 4.0 Rua_BC\n");
    fprintf(f, "e C D cep5 cep6 10.0 5.0 Rua_CD\n");
    fclose(f);
    
    char caminhoQry[PATH_LEN];
    snprintf(caminhoQry, sizeof(caminhoQry), "%s/teste.qry", inputDir);
    f = fopen(caminhoQry, "w");
    fprintf(f, "exp 5.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    processarArquivoVia("teste.via", inputDir);
    processarArquivoQry("teste.qry", inputDir, nomeBase, outputDir);
    
    char caminhoTxt[PATH_LEN];
    snprintf(caminhoTxt, sizeof(caminhoTxt), "%s/teste-teste.txt", outputDir);
    FILE* txt = fopen(caminhoTxt, "r");
    TEST_ASSERT_NOT_NULL(txt);
    fclose(txt);
    
    finalizarSistema();
}

void test_processarArquivoQry_arquivo_inexistente(void) {
    inicializarSistema(nomeBase, outputDir);
    processarArquivoQry("inexistente.qry", inputDir, nomeBase, outputDir);
    finalizarSistema();
}

void test_finalizarSistema_sem_inicializar(void) {
    finalizarSistema();
}

void test_processarArquivoGeo_com_linhas_vazias(void) {
    char caminho[PATH_LEN];
    snprintf(caminho, sizeof(caminho), "%s/teste.geo", inputDir);
    
    FILE* f = fopen(caminho, "w");
    fprintf(f, "\n");
    fprintf(f, "# comentario\n");
    fprintf(f, "q cep01 10.0 20.0 30.0 40.0\n");
    fprintf(f, "\n");
    fprintf(f, "q cep02 50.0 60.0 70.0 80.0\n");
    fclose(f);
    
    inicializarSistema(nomeBase, outputDir);
    processarArquivoGeo("teste.geo", inputDir, nomeBase, outputDir);
    finalizarSistema();
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_inicializarSistema_sucesso);
    RUN_TEST(test_inicializarSistema_parametros_invalidos);
    RUN_TEST(test_processarArquivoGeo_sem_cq);
    RUN_TEST(test_processarArquivoGeo_com_cq);
    RUN_TEST(test_processarArquivoGeo_arquivo_inexistente);
    RUN_TEST(test_processarArquivoVia_sucesso);
    RUN_TEST(test_processarArquivoVia_arquivo_inexistente);
    RUN_TEST(test_processarArquivoQry_sem_comandos);
    RUN_TEST(test_processarArquivoQry_com_atO);
    RUN_TEST(test_processarArquivoQry_com_regs);
    RUN_TEST(test_processarArquivoQry_com_mvm);
    RUN_TEST(test_processarArquivoQry_com_exp);
    RUN_TEST(test_processarArquivoQry_arquivo_inexistente);
    RUN_TEST(test_finalizarSistema_sem_inicializar);
    RUN_TEST(test_processarArquivoGeo_com_linhas_vazias);
    
    return UNITY_END();
}
