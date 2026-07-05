#include "unity.h"
#include "hashfile.h"
#include <stdlib.h>
#include <string.h>

#define TAM_BUFFER 1024

static HashFile* hf;
static char buffer[TAM_BUFFER];
static size_t tam;

void setUp(void) {
    hf = criarHF("teste_hash", 4, 2);
    TEST_ASSERT_NOT_NULL(hf);
}

void tearDown(void) {
    if (hf) {
        freeHF(hf);
        hf = NULL;
    }
}



void test_criarHF_sucesso(void) {
    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_EQUAL_INT(2, getProfGlobalHF(hf));
    TEST_ASSERT_EQUAL_INT(4, getTamBucketHF(hf));
    TEST_ASSERT_EQUAL_INT(0, totalRegistrosHF(hf));
}

void test_criarHF_parametros_invalidos(void) {
    HashFile* hf_invalido = criarHF(NULL, 4, 2);
    TEST_ASSERT_NULL(hf_invalido);
    
    hf_invalido = criarHF("teste", 0, 2);
    TEST_ASSERT_NULL(hf_invalido);
    
    hf_invalido = criarHF("teste", 4, 0);
    TEST_ASSERT_NULL(hf_invalido);
}

void test_abrirHF_existente(void) {
    int dado = 123;
    insertHF(hf, "chave1", &dado, sizeof(int));
    freeHF(hf);
    
    hf = abrirHF("teste_hash");
    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_EQUAL_INT(1, totalRegistrosHF(hf));
}



void test_insertHF_sucesso(void) {
    int dado = 42;
    int ret = insertHF(hf, "chave1", &dado, sizeof(int));
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_INT(1, totalRegistrosHF(hf));
}

void test_insertHF_chave_duplicada(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    
    int ret = insertHF(hf, "chave1", &dado, sizeof(int));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, totalRegistrosHF(hf));
}

void test_insertHF_parametros_invalidos(void) {
    int dado = 42;
    TEST_ASSERT_EQUAL_INT(-1, insertHF(NULL, "chave", &dado, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, insertHF(hf, NULL, &dado, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, insertHF(hf, "chave", NULL, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, insertHF(hf, "chave", &dado, 0));
}

void test_insertHF_multiplos_registros(void) {
    int dados[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        char chave[10];
        sprintf(chave, "chave%d", i);
        int ret = insertHF(hf, chave, &dados[i], sizeof(int));
        TEST_ASSERT_EQUAL_INT(1, ret);
    }
    TEST_ASSERT_EQUAL_INT(5, totalRegistrosHF(hf));
}



void test_buscaHF_encontrado(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    
    int saida;
    size_t tamSaida = sizeof(saida);
    int ret = buscaHF(hf, "chave1", &saida, &tamSaida);
    
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_INT(42, saida);
    TEST_ASSERT_EQUAL_INT(sizeof(int), tamSaida);
}

void test_buscaHF_nao_encontrado(void) {
    int saida;
    size_t tamSaida = sizeof(saida);
    int ret = buscaHF(hf, "chave_inexistente", &saida, &tamSaida);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_buscaHF_sem_buffer(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    
    size_t tamSaida = 0;
    int ret = buscaHF(hf, "chave1", NULL, &tamSaida);
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_INT(sizeof(int), tamSaida);
}

void test_buscaHF_buffer_insuficiente(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    
    char bufferPequeno[1];
    size_t tamSaida = 1;
    int ret = buscaHF(hf, "chave1", bufferPequeno, &tamSaida);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_buscaHF_parametros_invalidos(void) {
    int saida;
    size_t tamSaida = sizeof(saida);
    TEST_ASSERT_EQUAL_INT(-1, buscaHF(NULL, "chave", &saida, &tamSaida));
    TEST_ASSERT_EQUAL_INT(-1, buscaHF(hf, NULL, &saida, &tamSaida));
}



void test_refreshHF_sucesso(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    
    int novoDado = 99;
    int ret = refreshHF(hf, "chave1", &novoDado, sizeof(int));
    TEST_ASSERT_EQUAL_INT(1, ret);
    
    int saida;
    size_t tamSaida = sizeof(saida);
    buscaHF(hf, "chave1", &saida, &tamSaida);
    TEST_ASSERT_EQUAL_INT(99, saida);
}

void test_refreshHF_chave_inexistente(void) {
    int novoDado = 99;
    int ret = refreshHF(hf, "chave_inexistente", &novoDado, sizeof(int));
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_refreshHF_parametros_invalidos(void) {
    int novoDado = 99;
    TEST_ASSERT_EQUAL_INT(-1, refreshHF(NULL, "chave", &novoDado, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, refreshHF(hf, NULL, &novoDado, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, refreshHF(hf, "chave", NULL, sizeof(int)));
    TEST_ASSERT_EQUAL_INT(-1, refreshHF(hf, "chave", &novoDado, 0));
}



void test_deletarItemHF_sucesso(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    TEST_ASSERT_EQUAL_INT(1, totalRegistrosHF(hf));
    
    int ret = deletarItemHF(hf, "chave1");
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_INT(0, totalRegistrosHF(hf));
    
    int saida;
    size_t tamSaida = sizeof(saida);
    ret = buscaHF(hf, "chave1", &saida, &tamSaida);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_deletarItemHF_chave_inexistente(void) {
    int ret = deletarItemHF(hf, "chave_inexistente");
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_deletarItemHF_parametros_invalidos(void) {
    TEST_ASSERT_EQUAL_INT(-1, deletarItemHF(NULL, "chave"));
    TEST_ASSERT_EQUAL_INT(-1, deletarItemHF(hf, NULL));
}



typedef struct {
    int count;
    int soma;
} Contexto;

static void callbackTeste(const char* chave, const void* dado, size_t tamDado, void* contexto) {
    Contexto* ctx = (Contexto*)contexto;
    ctx->count++;
    ctx->soma += *(int*)dado;
}

void test_iterarHF_sucesso(void) {
    int dados[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        char chave[10];
        sprintf(chave, "chave%d", i);
        insertHF(hf, chave, &dados[i], sizeof(int));
    }
    
    Contexto ctx = {0, 0};
    int ret = iterarHF(hf, callbackTeste, &ctx);
    TEST_ASSERT_EQUAL_INT(3, ret);
    TEST_ASSERT_EQUAL_INT(3, ctx.count);
    TEST_ASSERT_EQUAL_INT(60, ctx.soma);
}

void test_iterarHF_vazio(void) {
    Contexto ctx = {0, 0};
    int ret = iterarHF(hf, callbackTeste, &ctx);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, ctx.count);
}

void test_iterarHF_parametros_invalidos(void) {
    TEST_ASSERT_EQUAL_INT(-1, iterarHF(NULL, callbackTeste, NULL));
    TEST_ASSERT_EQUAL_INT(-1, iterarHF(hf, NULL, NULL));
}



void test_getProfGlobalHF(void) {
    TEST_ASSERT_EQUAL_INT(2, getProfGlobalHF(hf));
    TEST_ASSERT_EQUAL_INT(-1, getProfGlobalHF(NULL));
}

void test_getTamBucketHF(void) {
    TEST_ASSERT_EQUAL_INT(4, getTamBucketHF(hf));
    TEST_ASSERT_EQUAL_INT(-1, getTamBucketHF(NULL));
}

void test_getInfoBucketHF(void) {
    int qtd, prof;
    int ret = getInfoBucketHF(hf, 0, &qtd, &prof);
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_INT(0, qtd);
    TEST_ASSERT_EQUAL_INT(2, prof);
}

void test_getInfoBucketHF_indice_invalido(void) {
    int qtd, prof;
    int ret = getInfoBucketHF(hf, 999, &qtd, &prof);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_getInfoBucketHF_NULL(void) {
    int ret = getInfoBucketHF(NULL, 0, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(0, ret);
}



void test_gerarDumpHF_sucesso(void) {
    int dado = 42;
    insertHF(hf, "chave1", &dado, sizeof(int));
    insertHF(hf, "chave2", &dado, sizeof(int));
    
    int ret = gerarDumpHF(hf);
    TEST_ASSERT_EQUAL_INT(1, ret);
    
    FILE* dump = fopen("teste_hash.hfd", "r");
    TEST_ASSERT_NOT_NULL(dump);
    fclose(dump);
}

void test_gerarDumpHF_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, gerarDumpHF(NULL));
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criarHF_sucesso);
    RUN_TEST(test_criarHF_parametros_invalidos);
    RUN_TEST(test_abrirHF_existente);
    RUN_TEST(test_insertHF_sucesso);
    RUN_TEST(test_insertHF_chave_duplicada);
    RUN_TEST(test_insertHF_parametros_invalidos);
    RUN_TEST(test_insertHF_multiplos_registros);
    RUN_TEST(test_buscaHF_encontrado);
    RUN_TEST(test_buscaHF_nao_encontrado);
    RUN_TEST(test_buscaHF_sem_buffer);
    RUN_TEST(test_buscaHF_buffer_insuficiente);
    RUN_TEST(test_buscaHF_parametros_invalidos);
    RUN_TEST(test_refreshHF_sucesso);
    RUN_TEST(test_refreshHF_chave_inexistente);
    RUN_TEST(test_refreshHF_parametros_invalidos);
    RUN_TEST(test_deletarItemHF_sucesso);
    RUN_TEST(test_deletarItemHF_chave_inexistente);
    RUN_TEST(test_deletarItemHF_parametros_invalidos);
    RUN_TEST(test_iterarHF_sucesso);
    RUN_TEST(test_iterarHF_vazio);
    RUN_TEST(test_iterarHF_parametros_invalidos);
    RUN_TEST(test_getProfGlobalHF);
    RUN_TEST(test_getTamBucketHF);
    RUN_TEST(test_getInfoBucketHF);
    RUN_TEST(test_getInfoBucketHF_indice_invalido);
    RUN_TEST(test_getInfoBucketHF_NULL);
    RUN_TEST(test_gerarDumpHF_sucesso);
    RUN_TEST(test_gerarDumpHF_NULL);

    return UNITY_END();
}
