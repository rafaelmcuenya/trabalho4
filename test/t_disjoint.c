#include "unity.h"
#include "disjoint.h"
#include "grafo.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>

static Grafo g;
static Conjunto c;
static Vertice vA, vB, vC, vD, vE;

void setUp(void) {
    g = criaGrafo();
    TEST_ASSERT_NOT_NULL(g);
    
    vA = adicionarVertice(g, "A", 0.0, 0.0);
    vB = adicionarVertice(g, "B", 10.0, 0.0);
    vC = adicionarVertice(g, "C", 20.0, 0.0);
    vD = adicionarVertice(g, "D", 30.0, 0.0);
    vE = adicionarVertice(g, "E", 40.0, 0.0);
    
    c = criarConjunto(g);
    TEST_ASSERT_NOT_NULL(c);
}

void tearDown(void) {
    if (c) {
        deletaConjunto(c);
        c = NULL;
    }
    if (g) {
        deletaGrafo(g);
        g = NULL;
    }
}

void test_criarConjunto_sucesso(void) {
    TEST_ASSERT_EQUAL_INT(5, numConjuntos(c));
    
    TEST_ASSERT_EQUAL_INT(0, find(c, vA));
    TEST_ASSERT_EQUAL_INT(1, find(c, vB));
    TEST_ASSERT_EQUAL_INT(2, find(c, vC));
    TEST_ASSERT_EQUAL_INT(3, find(c, vD));
    TEST_ASSERT_EQUAL_INT(4, find(c, vE));
}

void test_criarConjunto_grafo_vazio(void) {
    Grafo g2 = criaGrafo();
    Conjunto c2 = criarConjunto(g2);
    TEST_ASSERT_NULL(c2);
    deletaGrafo(g2);
}

void test_criarConjunto_parametros_invalidos(void) {
    Conjunto c2 = criarConjunto(NULL);
    TEST_ASSERT_NULL(c2);
}

void test_unionSet_dois_conjuntos(void) {
    unionSet(c, vA, vB);
    TEST_ASSERT_EQUAL_INT(4, numConjuntos(c));
    TEST_ASSERT_TRUE(sameSet(c, vA, vB));
    TEST_ASSERT_FALSE(sameSet(c, vA, vC));
}

void test_unionSet_tres_conjuntos(void) {
    unionSet(c, vA, vB);
    unionSet(c, vB, vC);
    TEST_ASSERT_EQUAL_INT(3, numConjuntos(c));
    TEST_ASSERT_TRUE(sameSet(c, vA, vB));
    TEST_ASSERT_TRUE(sameSet(c, vA, vC));
    TEST_ASSERT_TRUE(sameSet(c, vB, vC));
    TEST_ASSERT_FALSE(sameSet(c, vA, vD));
}

void test_unionSet_todos_conjuntos(void) {
    unionSet(c, vA, vB);
    unionSet(c, vC, vD);
    unionSet(c, vB, vC);
    unionSet(c, vD, vE);
    TEST_ASSERT_EQUAL_INT(1, numConjuntos(c));
    TEST_ASSERT_TRUE(sameSet(c, vA, vE));
    TEST_ASSERT_TRUE(sameSet(c, vB, vD));
}

void test_unionSet_conjuntos_ja_unidos(void) {
    unionSet(c, vA, vB);
    TEST_ASSERT_EQUAL_INT(4, numConjuntos(c));
    unionSet(c, vA, vB);
    TEST_ASSERT_EQUAL_INT(4, numConjuntos(c));
    TEST_ASSERT_TRUE(sameSet(c, vA, vB));
}

void test_unionSet_parametros_invalidos(void) {
    unionSet(NULL, vA, vB);
    unionSet(c, NULL, vB);
    unionSet(c, vA, NULL);
}

void test_sameSet_verdadeiro(void) {
    unionSet(c, vA, vB);
    TEST_ASSERT_TRUE(sameSet(c, vA, vB));
    TEST_ASSERT_TRUE(sameSet(c, vB, vA));
}

void test_sameSet_falso(void) {
    TEST_ASSERT_FALSE(sameSet(c, vA, vB));
    TEST_ASSERT_FALSE(sameSet(c, vC, vE));
}

void test_sameSet_mesmo_vertice(void) {
    TEST_ASSERT_TRUE(sameSet(c, vA, vA));
}

void test_sameSet_parametros_invalidos(void) {
    TEST_ASSERT_FALSE(sameSet(NULL, vA, vB));
    TEST_ASSERT_FALSE(sameSet(c, NULL, vB));
    TEST_ASSERT_FALSE(sameSet(c, vA, NULL));
}

void test_find_compressao_caminho(void) {
    unionSet(c, vA, vB);
    unionSet(c, vC, vD);
    unionSet(c, vB, vC);
    
    int raizA = find(c, vA);
    int raizB = find(c, vB);
    int raizC = find(c, vC);
    int raizD = find(c, vD);
    
    TEST_ASSERT_EQUAL_INT(raizA, raizB);
    TEST_ASSERT_EQUAL_INT(raizB, raizC);
    TEST_ASSERT_EQUAL_INT(raizC, raizD);
}

void test_find_parametros_invalidos(void) {
    TEST_ASSERT_EQUAL_INT(-1, find(NULL, vA));
}

void test_numConjuntos_apos_operacoes(void) {
    TEST_ASSERT_EQUAL_INT(5, numConjuntos(c));
    unionSet(c, vA, vB);
    TEST_ASSERT_EQUAL_INT(4, numConjuntos(c));
    unionSet(c, vB, vC);
    TEST_ASSERT_EQUAL_INT(3, numConjuntos(c));
    unionSet(c, vD, vE);
    TEST_ASSERT_EQUAL_INT(2, numConjuntos(c));
}

void test_numConjuntos_parametros_invalidos(void) {
    TEST_ASSERT_EQUAL_INT(0, numConjuntos(NULL));
}

void test_unionByRank(void) {
    unionSet(c, vA, vB);
    unionSet(c, vC, vD);
    unionSet(c, vA, vC);
    unionSet(c, vD, vE);
    
    int raizA = find(c, vA);
    int raizB = find(c, vB);
    int raizC = find(c, vC);
    int raizD = find(c, vD);
    int raizE = find(c, vE);
    
    TEST_ASSERT_EQUAL_INT(raizA, raizB);
    TEST_ASSERT_EQUAL_INT(raizA, raizC);
    TEST_ASSERT_EQUAL_INT(raizA, raizD);
    TEST_ASSERT_EQUAL_INT(raizA, raizE);
}

void test_deletaConjunto_NULL(void) {
    deletaConjunto(NULL);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_criarConjunto_sucesso);
    RUN_TEST(test_criarConjunto_grafo_vazio);
    RUN_TEST(test_criarConjunto_parametros_invalidos);
    RUN_TEST(test_unionSet_dois_conjuntos);
    RUN_TEST(test_unionSet_tres_conjuntos);
    RUN_TEST(test_unionSet_todos_conjuntos);
    RUN_TEST(test_unionSet_conjuntos_ja_unidos);
    RUN_TEST(test_unionSet_parametros_invalidos);
    RUN_TEST(test_sameSet_verdadeiro);
    RUN_TEST(test_sameSet_falso);
    RUN_TEST(test_sameSet_mesmo_vertice);
    RUN_TEST(test_sameSet_parametros_invalidos);
    RUN_TEST(test_find_compressao_caminho);
    RUN_TEST(test_find_parametros_invalidos);
    RUN_TEST(test_numConjuntos_apos_operacoes);
    RUN_TEST(test_numConjuntos_parametros_invalidos);
    RUN_TEST(test_unionByRank);
    RUN_TEST(test_deletaConjunto_NULL);
    
    return UNITY_END();
}
