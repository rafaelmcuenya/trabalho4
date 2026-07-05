#include "unity.h"
#include "grafo.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

static Grafo g;
static Vertice v1, v2, v3, v4;
static Aresta a1, a2;

void setUp(void) {
    g = criaGrafo();
    TEST_ASSERT_NOT_NULL(g);
    
    v1 = adicionarVertice(g, "A", 10.0, 20.0);
    v2 = adicionarVertice(g, "B", 30.0, 40.0);
    v3 = adicionarVertice(g, "C", 50.0, 60.0);
    v4 = adicionarVertice(g, "D", 70.0, 80.0);
    
    a1 = adicionarAresta(g, "A", "B", "cep1", "cep2", 100.0, 10.0, "Rua_A");
    a2 = adicionarAresta(g, "B", "C", "cep3", "cep4", 150.0, 20.0, "Rua_B");
}

void tearDown(void) {
    if (g) {
        deletaGrafo(g);
        g = NULL;
    }
}

void test_criaGrafo_sucesso(void) {
    Grafo g2 = criaGrafo();
    TEST_ASSERT_NOT_NULL(g2);
    TEST_ASSERT_EQUAL_INT(0, quantVertices(g2));
    TEST_ASSERT_EQUAL_INT(0, quantArestas(g2));
    deletaGrafo(g2);
}

void test_adicionarVertice_sucesso(void) {
    Vertice v = adicionarVertice(g, "X", 100.0, 200.0);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_INT(5, quantVertices(g));
    TEST_ASSERT_EQUAL_STRING("X", getVerticeId(v));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.0, getVerticeX(v));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 200.0, getVerticeY(v));
}

void test_adicionarVertice_duplicado(void) {
    Vertice v = adicionarVertice(g, "A", 99.0, 99.0);
    TEST_ASSERT_NULL(v);
    TEST_ASSERT_EQUAL_INT(4, quantVertices(g));
}

void test_adicionarVertice_parametros_invalidos(void) {
    Vertice v = adicionarVertice(NULL, "X", 10.0, 20.0);
    TEST_ASSERT_NULL(v);
    
    v = adicionarVertice(g, NULL, 10.0, 20.0);
    TEST_ASSERT_NULL(v);
}

void test_buscarVertice_encontrado(void) {
    Vertice v = buscarVertice(g, "B");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("B", getVerticeId(v));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 30.0, getVerticeX(v));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 40.0, getVerticeY(v));
}

void test_buscarVertice_nao_encontrado(void) {
    Vertice v = buscarVertice(g, "Z");
    TEST_ASSERT_NULL(v);
}

void test_buscarVertice_parametros_invalidos(void) {
    Vertice v = buscarVertice(NULL, "A");
    TEST_ASSERT_NULL(v);
    
    v = buscarVertice(g, NULL);
    TEST_ASSERT_NULL(v);
}

void test_getVerticeId(void) {
    TEST_ASSERT_EQUAL_STRING("A", getVerticeId(v1));
    TEST_ASSERT_EQUAL_STRING("B", getVerticeId(v2));
    TEST_ASSERT_NULL(getVerticeId(NULL));
}

void test_getVerticeX(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, getVerticeX(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 30.0, getVerticeX(v2));
}

void test_getVerticeY(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.0, getVerticeY(v1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 40.0, getVerticeY(v2));
}

void test_getIndiceVertice(void) {
    TEST_ASSERT_EQUAL_INT(0, getIndiceVertice(v1));
    TEST_ASSERT_EQUAL_INT(1, getIndiceVertice(v2));
    TEST_ASSERT_EQUAL_INT(2, getIndiceVertice(v3));
    TEST_ASSERT_EQUAL_INT(3, getIndiceVertice(v4));
}

void test_adicionarAresta_sucesso(void) {
    Aresta a = adicionarAresta(g, "C", "D", "cep5", "cep6", 200.0, 30.0, "Rua_C");
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_EQUAL_INT(3, quantArestas(g));
    
    Vertice origem = getOrigem(a);
    Vertice destino = getDestino(a);
    TEST_ASSERT_EQUAL_STRING("C", getVerticeId(origem));
    TEST_ASSERT_EQUAL_STRING("D", getVerticeId(destino));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 200.0, getComprimento(a));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 30.0, getVelocidadeMedia(a));
    TEST_ASSERT_EQUAL_STRING("Rua_C", getNomeRua(a));
    TEST_ASSERT_EQUAL_STRING("cep5", getLdir(a));
    TEST_ASSERT_EQUAL_STRING("cep6", getLesq(a));
    TEST_ASSERT_TRUE(arestaAtiva(a));
}

void test_adicionarAresta_duplicada(void) {
    Aresta a = adicionarAresta(g, "A", "B", "cepX", "cepY", 50.0, 5.0, "Rua_X");
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_EQUAL_INT(2, quantArestas(g));
}

void test_adicionarAresta_vertices_inexistentes(void) {
    Aresta a = adicionarAresta(g, "X", "Y", "cep1", "cep2", 100.0, 10.0, "Rua_X");
    TEST_ASSERT_NULL(a);
    
    a = adicionarAresta(g, "A", "Z", "cep1", "cep2", 100.0, 10.0, "Rua_X");
    TEST_ASSERT_NULL(a);
}

void test_adicionarAresta_parametros_invalidos(void) {
    Aresta a = adicionarAresta(NULL, "A", "B", "c1", "c2", 100.0, 10.0, "Rua");
    TEST_ASSERT_NULL(a);
    
    a = adicionarAresta(g, NULL, "B", "c1", "c2", 100.0, 10.0, "Rua");
    TEST_ASSERT_NULL(a);
    
    a = adicionarAresta(g, "A", NULL, "c1", "c2", 100.0, 10.0, "Rua");
    TEST_ASSERT_NULL(a);
    
    a = adicionarAresta(g, "A", "B", "c1", "c2", 0.0, 10.0, "Rua");
    TEST_ASSERT_NULL(a);
    
    a = adicionarAresta(g, "A", "B", "c1", "c2", 100.0, 0.0, "Rua");
    TEST_ASSERT_NULL(a);
}

void test_buscarAresta_encontrada(void) {
    Aresta a = buscarAresta(g, "A", "B");
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_EQUAL_STRING("Rua_A", getNomeRua(a));
}

void test_buscarAresta_nao_encontrada(void) {
    Aresta a = buscarAresta(g, "C", "D");
    TEST_ASSERT_NULL(a);
    
    a = buscarAresta(g, "B", "A");
    TEST_ASSERT_NULL(a);
}

void test_buscarAresta_parametros_invalidos(void) {
    Aresta a = buscarAresta(NULL, "A", "B");
    TEST_ASSERT_NULL(a);
    
    a = buscarAresta(g, NULL, "B");
    TEST_ASSERT_NULL(a);
    
    a = buscarAresta(g, "A", NULL);
    TEST_ASSERT_NULL(a);
}

void test_offAresta(void) {
    TEST_ASSERT_TRUE(arestaAtiva(a1));
    offAresta(a1);
    TEST_ASSERT_FALSE(arestaAtiva(a1));
}

void test_onAresta(void) {
    offAresta(a1);
    TEST_ASSERT_FALSE(arestaAtiva(a1));
    onAresta(a1);
    TEST_ASSERT_TRUE(arestaAtiva(a1));
}

void test_arestaAtiva_NULL(void) {
    TEST_ASSERT_FALSE(arestaAtiva(NULL));
}

void test_getOrigem(void) {
    Vertice v = getOrigem(a1);
    TEST_ASSERT_EQUAL_STRING("A", getVerticeId(v));
    TEST_ASSERT_NULL(getOrigem(NULL));
}

void test_getDestino(void) {
    Vertice v = getDestino(a1);
    TEST_ASSERT_EQUAL_STRING("B", getVerticeId(v));
    TEST_ASSERT_NULL(getDestino(NULL));
}

void test_getComprimento(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.0, getComprimento(a1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getComprimento(NULL));
}

void test_getVelocidadeMedia(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 10.0, getVelocidadeMedia(a1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getVelocidadeMedia(NULL));
}

void test_setVelocidadeMedia(void) {
    setVelocidadeMedia(a1, 25.0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 25.0, getVelocidadeMedia(a1));
    setVelocidadeMedia(NULL, 10.0);
    setVelocidadeMedia(a1, -5.0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 25.0, getVelocidadeMedia(a1));
}

void test_getNomeRua(void) {
    TEST_ASSERT_EQUAL_STRING("Rua_A", getNomeRua(a1));
    TEST_ASSERT_NULL(getNomeRua(NULL));
}

void test_getLdir(void) {
    TEST_ASSERT_EQUAL_STRING("cep1", getLdir(a1));
    TEST_ASSERT_NULL(getLdir(NULL));
}

void test_getLesq(void) {
    TEST_ASSERT_EQUAL_STRING("cep2", getLesq(a1));
    TEST_ASSERT_NULL(getLesq(NULL));
}

void test_quantVertices(void) {
    TEST_ASSERT_EQUAL_INT(4, quantVertices(g));
    TEST_ASSERT_EQUAL_INT(0, quantVertices(NULL));
}

void test_quantArestas(void) {
    TEST_ASSERT_EQUAL_INT(2, quantArestas(g));
    TEST_ASSERT_EQUAL_INT(0, quantArestas(NULL));
}

void test_primeiroVertice(void) {
    Vertice v = primeiroVertice(g);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("A", getVerticeId(v));
    TEST_ASSERT_NULL(primeiroVertice(NULL));
}

void test_proximoVertice(void) {
    Vertice v = primeiroVertice(g);
    v = proximoVertice(g, v);
    TEST_ASSERT_EQUAL_STRING("B", getVerticeId(v));
    v = proximoVertice(g, v);
    TEST_ASSERT_EQUAL_STRING("C", getVerticeId(v));
    v = proximoVertice(g, v);
    TEST_ASSERT_EQUAL_STRING("D", getVerticeId(v));
    v = proximoVertice(g, v);
    TEST_ASSERT_NULL(v);
}

void test_proximoVertice_parametros_invalidos(void) {
    TEST_ASSERT_NULL(proximoVertice(NULL, v1));
    TEST_ASSERT_NULL(proximoVertice(g, NULL));
}

void test_primeiraArestaAdj(void) {
    Aresta a = primeiraArestaAdj(v1);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_EQUAL_STRING("Rua_A", getNomeRua(a));
    TEST_ASSERT_NULL(primeiraArestaAdj(NULL));
}

void test_proximaArestaAdj(void) {
    Vertice v = adicionarVertice(g, "X", 100.0, 100.0);
    adicionarAresta(g, "A", "X", "c1", "c2", 50.0, 5.0, "Rua_X");
    
    Aresta a = primeiraArestaAdj(v1);
    TEST_ASSERT_NOT_NULL(a);
    a = proximaArestaAdj(v1, a);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_EQUAL_STRING("Rua_X", getNomeRua(a));
    a = proximaArestaAdj(v1, a);
    TEST_ASSERT_NULL(a);
}

void test_proximaArestaAdj_parametros_invalidos(void) {
    TEST_ASSERT_NULL(proximaArestaAdj(NULL, a1));
    TEST_ASSERT_NULL(proximaArestaAdj(v1, NULL));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_criaGrafo_sucesso);
    RUN_TEST(test_adicionarVertice_sucesso);
    RUN_TEST(test_adicionarVertice_duplicado);
    RUN_TEST(test_adicionarVertice_parametros_invalidos);
    RUN_TEST(test_buscarVertice_encontrado);
    RUN_TEST(test_buscarVertice_nao_encontrado);
    RUN_TEST(test_buscarVertice_parametros_invalidos);
    RUN_TEST(test_getVerticeId);
    RUN_TEST(test_getVerticeX);
    RUN_TEST(test_getVerticeY);
    RUN_TEST(test_getIndiceVertice);
    RUN_TEST(test_adicionarAresta_sucesso);
    RUN_TEST(test_adicionarAresta_duplicada);
    RUN_TEST(test_adicionarAresta_vertices_inexistentes);
    RUN_TEST(test_adicionarAresta_parametros_invalidos);
    RUN_TEST(test_buscarAresta_encontrada);
    RUN_TEST(test_buscarAresta_nao_encontrada);
    RUN_TEST(test_buscarAresta_parametros_invalidos);
    RUN_TEST(test_offAresta);
    RUN_TEST(test_onAresta);
    RUN_TEST(test_arestaAtiva_NULL);
    RUN_TEST(test_getOrigem);
    RUN_TEST(test_getDestino);
    RUN_TEST(test_getComprimento);
    RUN_TEST(test_getVelocidadeMedia);
    RUN_TEST(test_setVelocidadeMedia);
    RUN_TEST(test_getNomeRua);
    RUN_TEST(test_getLdir);
    RUN_TEST(test_getLesq);
    RUN_TEST(test_quantVertices);
    RUN_TEST(test_quantArestas);
    RUN_TEST(test_primeiroVertice);
    RUN_TEST(test_proximoVertice);
    RUN_TEST(test_proximoVertice_parametros_invalidos);
    RUN_TEST(test_primeiraArestaAdj);
    RUN_TEST(test_proximaArestaAdj);
    RUN_TEST(test_proximaArestaAdj_parametros_invalidos);
    
    return UNITY_END();
}
