#include "unity.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

static Lista lista;
static int valor1, valor2, valor3, valor4;
static No no1, no2, no3;

void setUp(void) {
    lista = criaLista();
    valor1 = 10;
    valor2 = 20;
    valor3 = 30;
    valor4 = 40;
    no1 = NULL;
    no2 = NULL;
    no3 = NULL;
}

void tearDown(void) {
    deletaLista(lista);
    lista = NULL;
}



void test_criaLista_sucesso(void) {
    TEST_ASSERT_NOT_NULL(lista);
    TEST_ASSERT_TRUE(listaVazia(lista));
    TEST_ASSERT_EQUAL_INT(0, tamanhoLista(lista));
    TEST_ASSERT_NULL(primeiroNo(lista));
    TEST_ASSERT_NULL(ultimoNo(lista));
}



void test_inserirInicio_lista_vazia(void) {
    inserirInicio(lista, &valor1);
    TEST_ASSERT_EQUAL_INT(1, tamanhoLista(lista));
    TEST_ASSERT_FALSE(listaVazia(lista));
    
    No primeiro = primeiroNo(lista);
    TEST_ASSERT_NOT_NULL(primeiro);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(primeiro));
    TEST_ASSERT_NULL(proximoNo(primeiro));
    TEST_ASSERT_NULL(anteriorNo(primeiro));
    TEST_ASSERT_EQUAL_PTR(primeiro, ultimoNo(lista));
}

void test_inserirInicio_lista_com_elementos(void) {
    inserirInicio(lista, &valor1);
    inserirInicio(lista, &valor2);
    inserirInicio(lista, &valor3);
    
    TEST_ASSERT_EQUAL_INT(3, tamanhoLista(lista));
    
    No primeiro = primeiroNo(lista);
    TEST_ASSERT_EQUAL_INT(valor3, *(int*)getInfo(primeiro));
    
    No segundo = proximoNo(primeiro);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(segundo));
    
    No terceiro = proximoNo(segundo);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(terceiro));
    TEST_ASSERT_NULL(proximoNo(terceiro));
    TEST_ASSERT_EQUAL_PTR(terceiro, ultimoNo(lista));
}

void test_inserirFim_lista_vazia(void) {
    inserirFim(lista, &valor1);
    TEST_ASSERT_EQUAL_INT(1, tamanhoLista(lista));
    TEST_ASSERT_FALSE(listaVazia(lista));
    
    No ultimo = ultimoNo(lista);
    TEST_ASSERT_NOT_NULL(ultimo);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(ultimo));
    TEST_ASSERT_EQUAL_PTR(ultimo, primeiroNo(lista));
}

void test_inserirFim_lista_com_elementos(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirFim(lista, &valor3);
    
    TEST_ASSERT_EQUAL_INT(3, tamanhoLista(lista));
    
    No ultimo = ultimoNo(lista);
    TEST_ASSERT_EQUAL_INT(valor3, *(int*)getInfo(ultimo));
    
    No anterior = anteriorNo(ultimo);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(anterior));
    
    No primeiro = primeiroNo(lista);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(primeiro));
}

void test_inserirInicio_inserirFim_misto(void) {
    inserirInicio(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirInicio(lista, &valor3);
    inserirFim(lista, &valor4);
    
    TEST_ASSERT_EQUAL_INT(4, tamanhoLista(lista));
    
    No primeiro = primeiroNo(lista);
    TEST_ASSERT_EQUAL_INT(valor3, *(int*)getInfo(primeiro));
    
    No segundo = proximoNo(primeiro);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(segundo));
    
    No terceiro = proximoNo(segundo);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(terceiro));
    
    No quarto = proximoNo(terceiro);
    TEST_ASSERT_EQUAL_INT(valor4, *(int*)getInfo(quarto));
    TEST_ASSERT_EQUAL_PTR(quarto, ultimoNo(lista));
}



void test_removerNo_meio_da_lista(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirFim(lista, &valor3);
    
    No noRemover = primeiroNo(lista);
    noRemover = proximoNo(noRemover);
    
    removerNo(lista, noRemover);
    
    TEST_ASSERT_EQUAL_INT(2, tamanhoLista(lista));
    
    No primeiro = primeiroNo(lista);
    TEST_ASSERT_EQUAL_INT(valor1, *(int*)getInfo(primeiro));
    
    No ultimo = ultimoNo(lista);
    TEST_ASSERT_EQUAL_INT(valor3, *(int*)getInfo(ultimo));
    TEST_ASSERT_EQUAL_PTR(ultimo, proximoNo(primeiro));
    TEST_ASSERT_EQUAL_PTR(primeiro, anteriorNo(ultimo));
}

void test_removerNo_primeiro_da_lista(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirFim(lista, &valor3);
    
    No primeiro = primeiroNo(lista);
    removerNo(lista, primeiro);
    
    TEST_ASSERT_EQUAL_INT(2, tamanhoLista(lista));
    
    No novoPrimeiro = primeiroNo(lista);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(novoPrimeiro));
    TEST_ASSERT_NULL(anteriorNo(novoPrimeiro));
}

void test_removerNo_ultimo_da_lista(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirFim(lista, &valor3);
    
    No ultimo = ultimoNo(lista);
    removerNo(lista, ultimo);
    
    TEST_ASSERT_EQUAL_INT(2, tamanhoLista(lista));
    
    No novoUltimo = ultimoNo(lista);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(novoUltimo));
    TEST_ASSERT_NULL(proximoNo(novoUltimo));
}

void test_removerNo_unico_elemento(void) {
    inserirFim(lista, &valor1);
    
    No unico = primeiroNo(lista);
    removerNo(lista, unico);
    
    TEST_ASSERT_EQUAL_INT(0, tamanhoLista(lista));
    TEST_ASSERT_TRUE(listaVazia(lista));
    TEST_ASSERT_NULL(primeiroNo(lista));
    TEST_ASSERT_NULL(ultimoNo(lista));
}

void test_removerNo_NULL(void) {
    inserirFim(lista, &valor1);
    removerNo(lista, NULL);
    TEST_ASSERT_EQUAL_INT(1, tamanhoLista(lista));
}

void test_removerNo_lista_NULL(void) {
    removerNo(NULL, NULL);
}



void test_buscarNo_encontrado(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    inserirFim(lista, &valor3);
    
    No encontrado = buscarNo(lista, &valor2);
    TEST_ASSERT_NOT_NULL(encontrado);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(encontrado));
}

void test_buscarNo_nao_encontrado(void) {
    inserirFim(lista, &valor1);
    inserirFim(lista, &valor2);
    
    int valorNaoExiste = 99;
    No encontrado = buscarNo(lista, &valorNaoExiste);
    TEST_ASSERT_NULL(encontrado);
}

void test_buscarNo_lista_vazia(void) {
    No encontrado = buscarNo(lista, &valor1);
    TEST_ASSERT_NULL(encontrado);
}

void test_buscarNo_lista_NULL(void) {
    No encontrado = buscarNo(NULL, &valor1);
    TEST_ASSERT_NULL(encontrado);
}



void test_primeiroNo_lista_vazia(void) {
    TEST_ASSERT_NULL(primeiroNo(lista));
}

void test_ultimoNo_lista_vazia(void) {
    TEST_ASSERT_NULL(ultimoNo(lista));
}

void test_proximoNo_NULL(void) {
    TEST_ASSERT_NULL(proximoNo(NULL));
}

void test_anteriorNo_NULL(void) {
    TEST_ASSERT_NULL(anteriorNo(NULL));
}

void test_getInfo_NULL(void) {
    TEST_ASSERT_NULL(getInfo(NULL));
}

void test_setInfo_NULL(void) {
    inserirFim(lista, &valor1);
    No no = primeiroNo(lista);
    setInfo(no, &valor2);
    TEST_ASSERT_EQUAL_INT(valor2, *(int*)getInfo(no));
    setInfo(NULL, &valor2);
}



void test_tamanhoLista_lista_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, tamanhoLista(NULL));
}

void test_listaVazia_lista_NULL(void) {
    TEST_ASSERT_TRUE(listaVazia(NULL));
}

void test_tamanhoListapos_operacoes(void) {
    TEST_ASSERT_EQUAL_INT(0, tamanhoLista(lista));
    inserirInicio(lista, &valor1);
    TEST_ASSERT_EQUAL_INT(1, tamanhoLista(lista));
    inserirFim(lista, &valor2);
    TEST_ASSERT_EQUAL_INT(2, tamanhoLista(lista));
    removerNo(lista, primeiroNo(lista));
    TEST_ASSERT_EQUAL_INT(1, tamanhoLista(lista));
    removerNo(lista, primeiroNo(lista));
    TEST_ASSERT_EQUAL_INT(0, tamanhoLista(lista));
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criaLista_sucesso);
    RUN_TEST(test_inserirInicio_lista_vazia);
    RUN_TEST(test_inserirInicio_lista_com_elementos);
    RUN_TEST(test_inserirFim_lista_vazia);
    RUN_TEST(test_inserirFim_lista_com_elementos);
    RUN_TEST(test_inserirInicio_inserirFim_misto);
    RUN_TEST(test_removerNo_meio_da_lista);
    RUN_TEST(test_removerNo_primeiro_da_lista);
    RUN_TEST(test_removerNo_ultimo_da_lista);
    RUN_TEST(test_removerNo_unico_elemento);
    RUN_TEST(test_removerNo_NULL);
    RUN_TEST(test_removerNo_lista_NULL);
    RUN_TEST(test_buscarNo_encontrado);
    RUN_TEST(test_buscarNo_nao_encontrado);
    RUN_TEST(test_buscarNo_lista_vazia);
    RUN_TEST(test_buscarNo_lista_NULL);
    RUN_TEST(test_primeiroNo_lista_vazia);
    RUN_TEST(test_ultimoNo_lista_vazia);
    RUN_TEST(test_proximoNo_NULL);
    RUN_TEST(test_anteriorNo_NULL);
    RUN_TEST(test_getInfo_NULL);
    RUN_TEST(test_setInfo_NULL);
    RUN_TEST(test_tamanhoLista_lista_NULL);
    RUN_TEST(test_listaVazia_lista_NULL);
    RUN_TEST(test_tamanhoListapos_operacoes);

    return UNITY_END();
}
