#include "unity.h"
#include "dijkstra.h"
#include "grafo.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Grafo g;
static Vertice vA, vB, vC, vD, vE;

void setUp(void) {
    g = criaGrafo();
    TEST_ASSERT_NOT_NULL(g);
    
    vA = adicionarVertice(g, "A", 0.0, 0.0);
    vB = adicionarVertice(g, "B", 10.0, 0.0);
    vC = adicionarVertice(g, "C", 20.0, 0.0);
    vD = adicionarVertice(g, "D", 30.0, 0.0);
    vE = adicionarVertice(g, "E", 40.0, 0.0);
    
    adicionarAresta(g, "A", "B", "c1", "c2", 10.0, 5.0, "Rua_AB");
    adicionarAresta(g, "B", "C", "c3", "c4", 10.0, 10.0, "Rua_BC");
    adicionarAresta(g, "A", "D", "c5", "c6", 20.0, 20.0, "Rua_AD");
    adicionarAresta(g, "D", "C", "c7", "c8", 5.0, 2.0, "Rua_DC");
    adicionarAresta(g, "B", "D", "c9", "c10", 15.0, 7.0, "Rua_BD");
}

void tearDown(void) {
    if (g) {
        deletaGrafo(g);
        g = NULL;
    }
}

void test_menorCaminho_sucesso(void) {
    Caminho c = menorCaminho(g, "A", "C");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(2, tamanhoCaminho(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 15.0, getCustoTotal(c));
    
    Aresta a1 = getArestaCaminho(c, 0);
    Aresta a2 = getArestaCaminho(c, 1);
    TEST_ASSERT_NOT_NULL(a1);
    TEST_ASSERT_NOT_NULL(a2);
    TEST_ASSERT_EQUAL_STRING("Rua_AB", getNomeRua(a1));
    TEST_ASSERT_EQUAL_STRING("Rua_BD", getNomeRua(a2));
    
    deletaCaminho(c);
}

void test_caminhoMaisRapido_sucesso(void) {
    Caminho c = caminhoMaisRapido(g, "A", "C");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(2, tamanhoCaminho(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, getCustoTotal(c));
    
    Aresta a1 = getArestaCaminho(c, 0);
    Aresta a2 = getArestaCaminho(c, 1);
    TEST_ASSERT_NOT_NULL(a1);
    TEST_ASSERT_NOT_NULL(a2);
    TEST_ASSERT_EQUAL_STRING("Rua_AD", getNomeRua(a1));
    TEST_ASSERT_EQUAL_STRING("Rua_DC", getNomeRua(a2));
    
    deletaCaminho(c);
}

void test_menorCaminho_mesmo_vertice(void) {
    Caminho c = menorCaminho(g, "A", "A");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(0, tamanhoCaminho(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getCustoTotal(c));
    deletaCaminho(c);
}

void test_menorCaminho_vertice_inexistente(void) {
    Caminho c = menorCaminho(g, "A", "Z");
    TEST_ASSERT_NULL(c);
    
    c = menorCaminho(g, "Z", "A");
    TEST_ASSERT_NULL(c);
    
    c = menorCaminho(g, "Z", "W");
    TEST_ASSERT_NULL(c);
}

void test_menorCaminho_destino_inacessivel(void) {
    adicionarVertice(g, "F", 50.0, 0.0);
    
    Caminho c = menorCaminho(g, "A", "F");
    TEST_ASSERT_NULL(c);
}

void test_caminhoMaisRapido_destino_inacessivel(void) {
    adicionarVertice(g, "F", 50.0, 0.0);
    
    Caminho c = caminhoMaisRapido(g, "A", "F");
    TEST_ASSERT_NULL(c);
}

void test_menorCaminho_aresta_desativada(void) {
    Aresta a = buscarAresta(g, "A", "B");
    offAresta(a);
    
    Caminho c = menorCaminho(g, "A", "B");
    TEST_ASSERT_NULL(c);
    
    onAresta(a);
}

void test_caminhoMaisRapido_aresta_desativada(void) {
    Aresta a = buscarAresta(g, "A", "B");
    offAresta(a);
    
    Caminho c = caminhoMaisRapido(g, "A", "B");
    TEST_ASSERT_NULL(c);
    
    onAresta(a);
}

void test_menorCaminho_grafo_completo(void) {
    adicionarAresta(g, "C", "E", "c11", "c12", 10.0, 10.0, "Rua_CE");
    adicionarAresta(g, "B", "E", "c13", "c14", 25.0, 15.0, "Rua_BE");
    
    Caminho c = menorCaminho(g, "A", "E");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(3, tamanhoCaminho(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 30.0, getCustoTotal(c));
    deletaCaminho(c);
}

void test_caminhoMaisRapido_grafo_completo(void) {
    adicionarAresta(g, "C", "E", "c11", "c12", 10.0, 10.0, "Rua_CE");
    adicionarAresta(g, "B", "E", "c13", "c14", 25.0, 15.0, "Rua_BE");
    
    Caminho c = caminhoMaisRapido(g, "A", "E");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(3, tamanhoCaminho(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 8.5, getCustoTotal(c));
    deletaCaminho(c);
}

void test_getArestaCaminho_indices(void) {
    Caminho c = menorCaminho(g, "A", "C");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(2, tamanhoCaminho(c));
    
    Aresta a0 = getArestaCaminho(c, 0);
    Aresta a1 = getArestaCaminho(c, 1);
    Aresta a2 = getArestaCaminho(c, 2);
    
    TEST_ASSERT_NOT_NULL(a0);
    TEST_ASSERT_NOT_NULL(a1);
    TEST_ASSERT_NULL(a2);
    
    deletaCaminho(c);
}

void test_getArestaCaminho_indice_negativo(void) {
    Caminho c = menorCaminho(g, "A", "C");
    TEST_ASSERT_NOT_NULL(c);
    
    Aresta a = getArestaCaminho(c, -1);
    TEST_ASSERT_NULL(a);
    
    deletaCaminho(c);
}

void test_getCustoTotal_caminho_vazio(void) {
    Caminho c = menorCaminho(g, "A", "A");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getCustoTotal(c));
    deletaCaminho(c);
}

void test_tamanhoCaminho_caminho_vazio(void) {
    Caminho c = menorCaminho(g, "A", "A");
    TEST_ASSERT_NOT_NULL(c);
    TEST_ASSERT_EQUAL_INT(0, tamanhoCaminho(c));
    deletaCaminho(c);
}

void test_deletaCaminho_NULL(void) {
    deletaCaminho(NULL);
}

void test_getCustoTotal_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getCustoTotal(NULL));
}

void test_tamanhoCaminho_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, tamanhoCaminho(NULL));
}

void test_getArestaCaminho_NULL(void) {
    TEST_ASSERT_NULL(getArestaCaminho(NULL, 0));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_menorCaminho_sucesso);
    RUN_TEST(test_caminhoMaisRapido_sucesso);
    RUN_TEST(test_menorCaminho_mesmo_vertice);
    RUN_TEST(test_menorCaminho_vertice_inexistente);
    RUN_TEST(test_menorCaminho_destino_inacessivel);
    RUN_TEST(test_caminhoMaisRapido_destino_inacessivel);
    RUN_TEST(test_menorCaminho_aresta_desativada);
    RUN_TEST(test_caminhoMaisRapido_aresta_desativada);
    RUN_TEST(test_menorCaminho_grafo_completo);
    RUN_TEST(test_caminhoMaisRapido_grafo_completo);
    RUN_TEST(test_getArestaCaminho_indices);
    RUN_TEST(test_getArestaCaminho_indice_negativo);
    RUN_TEST(test_getCustoTotal_caminho_vazio);
    RUN_TEST(test_tamanhoCaminho_caminho_vazio);
    RUN_TEST(test_deletaCaminho_NULL);
    RUN_TEST(test_getCustoTotal_NULL);
    RUN_TEST(test_tamanhoCaminho_NULL);
    RUN_TEST(test_getArestaCaminho_NULL);
    
    return UNITY_END();
}
