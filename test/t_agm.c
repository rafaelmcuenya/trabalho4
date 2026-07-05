#include "unity.h"
#include "agm.h"
#include "grafo.h"
#include "disjoint.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Grafo g;
static ArvoreGeradoraMinima agm;

void setUp(void) {
    g = criaGrafo();
    TEST_ASSERT_NOT_NULL(g);
    
    adicionarVertice(g, "A", 0.0, 0.0);
    adicionarVertice(g, "B", 10.0, 0.0);
    adicionarVertice(g, "C", 20.0, 0.0);
    adicionarVertice(g, "D", 30.0, 0.0);
    adicionarVertice(g, "E", 40.0, 0.0);
    
    adicionarAresta(g, "A", "B", "c1", "c2", 10.0, 5.0, "Rua_AB");
    adicionarAresta(g, "B", "C", "c3", "c4", 20.0, 10.0, "Rua_BC");
    adicionarAresta(g, "A", "C", "c5", "c6", 30.0, 15.0, "Rua_AC");
    adicionarAresta(g, "C", "D", "c7", "c8", 15.0, 8.0, "Rua_CD");
    adicionarAresta(g, "D", "E", "c9", "c10", 25.0, 12.0, "Rua_DE");
    adicionarAresta(g, "B", "D", "c11", "c12", 35.0, 18.0, "Rua_BD");
    
    agm = NULL;
}

void tearDown(void) {
    if (agm) {
        deletaAGM(agm);
        agm = NULL;
    }
    if (g) {
        deletaGrafo(g);
        g = NULL;
    }
}

void test_calcularAGM_sucesso(void) {
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_EQUAL_INT(4, tamanhoAGM(agm));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 70.0, getCustoTotalAGM(agm));
}

void test_calcularAGM_todas_arestas_selecionadas(void) {
    agm = calcularAGM(g, 30.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_EQUAL_INT(4, tamanhoAGM(agm));
}

void test_calcularAGM_nenhuma_aresta_selecionada(void) {
    agm = calcularAGM(g, 0.0);
    TEST_ASSERT_NULL(agm);
}

void test_calcularAGM_grafo_vazio(void) {
    Grafo g2 = criaGrafo();
    agm = calcularAGM(g2, 10.0);
    TEST_ASSERT_NULL(agm);
    deletaGrafo(g2);
}

void test_calcularAGM_parametros_invalidos(void) {
    agm = calcularAGM(NULL, 10.0);
    TEST_ASSERT_NULL(agm);
}

void test_getArestaAGM(void) {
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_EQUAL_INT(4, tamanhoAGM(agm));
    
    Aresta a0 = getArestaAGM(agm, 0);
    Aresta a1 = getArestaAGM(agm, 1);
    Aresta a2 = getArestaAGM(agm, 2);
    Aresta a3 = getArestaAGM(agm, 3);
    
    TEST_ASSERT_NOT_NULL(a0);
    TEST_ASSERT_NOT_NULL(a1);
    TEST_ASSERT_NOT_NULL(a2);
    TEST_ASSERT_NOT_NULL(a3);
    
    TEST_ASSERT_EQUAL_STRING("Rua_AB", getNomeRua(a0));
    TEST_ASSERT_EQUAL_STRING("Rua_CD", getNomeRua(a1));
    TEST_ASSERT_EQUAL_STRING("Rua_BC", getNomeRua(a2));
    TEST_ASSERT_EQUAL_STRING("Rua_DE", getNomeRua(a3));
}

void test_getArestaAGM_indice_invalido(void) {
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    
    Aresta a = getArestaAGM(agm, 99);
    TEST_ASSERT_NULL(a);
    
    a = getArestaAGM(agm, -1);
    TEST_ASSERT_NULL(a);
}

void test_getCustoTotalAGM(void) {
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 70.0, getCustoTotalAGM(agm));
}

void test_deletaAGM_NULL(void) {
    deletaAGM(NULL);
}

void test_calcularAGM_grafo_desconexo(void) {
    adicionarVertice(g, "F", 50.0, 50.0);
    
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_EQUAL_INT(4, tamanhoAGM(agm));
}

void test_calcularAGM_com_arestas_velocidade_insuficiente(void) {
    adicionarAresta(g, "C", "E", "c13", "c14", 5.0, 2.0, "Rua_CE");
    
    agm = calcularAGM(g, 3.0);
    TEST_ASSERT_NOT_NULL(agm);
    TEST_ASSERT_EQUAL_INT(4, tamanhoAGM(agm));
    
    int encontrouCE = 0;
    for (int i = 0; i < tamanhoAGM(agm); i++) {
        Aresta a = getArestaAGM(agm, i);
        if (a && strcmp(getNomeRua(a), "Rua_CE") == 0) {
            encontrouCE = 1;
        }
    }
    TEST_ASSERT_TRUE(encontrouCE);
}

void test_calcularAGM_arestas_desativadas(void) {
    Aresta a = buscarAresta(g, "A", "B");
    offAresta(a);
    
    agm = calcularAGM(g, 20.0);
    TEST_ASSERT_NOT_NULL(agm);
    
    int encontrouAB = 0;
    for (int i = 0; i < tamanhoAGM(agm); i++) {
        Aresta a2 = getArestaAGM(agm, i);
        if (a2 && strcmp(getNomeRua(a2), "Rua_AB") == 0) {
            encontrouAB = 1;
        }
    }
    TEST_ASSERT_FALSE(encontrouAB);
    
    onAresta(a);
}

void test_tamanhoAGM_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, tamanhoAGM(NULL));
}

void test_getCustoTotalAGM_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getCustoTotalAGM(NULL));
}

void test_getArestaAGM_NULL(void) {
    TEST_ASSERT_NULL(getArestaAGM(NULL, 0));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_calcularAGM_sucesso);
    RUN_TEST(test_calcularAGM_todas_arestas_selecionadas);
    RUN_TEST(test_calcularAGM_nenhuma_aresta_selecionada);
    RUN_TEST(test_calcularAGM_grafo_vazio);
    RUN_TEST(test_calcularAGM_parametros_invalidos);
    RUN_TEST(test_getArestaAGM);
    RUN_TEST(test_getArestaAGM_indice_invalido);
    RUN_TEST(test_getCustoTotalAGM);
    RUN_TEST(test_deletaAGM_NULL);
    RUN_TEST(test_calcularAGM_grafo_desconexo);
    RUN_TEST(test_calcularAGM_com_arestas_velocidade_insuficiente);
    RUN_TEST(test_calcularAGM_arestas_desativadas);
    RUN_TEST(test_tamanhoAGM_NULL);
    RUN_TEST(test_getCustoTotalAGM_NULL);
    RUN_TEST(test_getArestaAGM_NULL);
    
    return UNITY_END();
}
