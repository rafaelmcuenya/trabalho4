#include "unity.h"
#include "conexos.h"
#include "grafo.h"
#include "lista.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Grafo g;
static ListaComponentes componentes;

void setUp(void) {
    g = criaGrafo();
    TEST_ASSERT_NOT_NULL(g);
    
    adicionarVertice(g, "A", 0.0, 0.0);
    adicionarVertice(g, "B", 10.0, 0.0);
    adicionarVertice(g, "C", 20.0, 0.0);
    adicionarVertice(g, "D", 0.0, 10.0);
    adicionarVertice(g, "E", 10.0, 10.0);
    adicionarVertice(g, "F", 20.0, 10.0);
    
    adicionarAresta(g, "A", "B", "c1", "c2", 10.0, 10.0, "Rua_AB");
    adicionarAresta(g, "B", "C", "c3", "c4", 10.0, 10.0, "Rua_BC");
    adicionarAresta(g, "D", "E", "c5", "c6", 10.0, 5.0, "Rua_DE");
    adicionarAresta(g, "E", "F", "c7", "c8", 10.0, 5.0, "Rua_EF");
    
    componentes = NULL;
}

void tearDown(void) {
    if (componentes) {
        deletaListaComponentes(componentes);
        componentes = NULL;
    }
    if (g) {
        deletaGrafo(g);
        g = NULL;
    }
}

void test_encontrarComponentes_dois_componentes(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(2, quantComponentes(componentes));
    
    Componente c1 = getComponente(componentes, 0);
    Componente c2 = getComponente(componentes, 1);
    TEST_ASSERT_NOT_NULL(c1);
    TEST_ASSERT_NOT_NULL(c2);
}

void test_encontrarComponentes_um_componente(void) {
    componentes = encontrarComponentes(g, 3.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(1, quantComponentes(componentes));
}

void test_encontrarComponentes_todos_desconectados(void) {
    Grafo g2 = criaGrafo();
    adicionarVertice(g2, "X", 0.0, 0.0);
    adicionarVertice(g2, "Y", 10.0, 0.0);
    adicionarVertice(g2, "Z", 20.0, 0.0);
    
    componentes = encontrarComponentes(g2, 0.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(3, quantComponentes(componentes));
    
    deletaGrafo(g2);
}

void test_encontrarComponentes_grafo_vazio(void) {
    Grafo g2 = criaGrafo();
    componentes = encontrarComponentes(g2, 10.0);
    TEST_ASSERT_NULL(componentes);
    deletaGrafo(g2);
}

void test_encontrarComponentes_parametros_invalidos(void) {
    componentes = encontrarComponentes(NULL, 10.0);
    TEST_ASSERT_NULL(componentes);
}

void test_tamanhoComponente(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(2, quantComponentes(componentes));
    
    Componente c1 = getComponente(componentes, 0);
    Componente c2 = getComponente(componentes, 1);
    
    TEST_ASSERT_EQUAL_INT(3, tamanhoComponente(c1));
    TEST_ASSERT_EQUAL_INT(3, tamanhoComponente(c2));
}

void test_getVerticeComponente(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    
    Componente c = getComponente(componentes, 0);
    TEST_ASSERT_NOT_NULL(c);
    
    Vertice v1 = getVerticeComponente(c, 0);
    Vertice v2 = getVerticeComponente(c, 1);
    Vertice v3 = getVerticeComponente(c, 2);
    
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_NOT_NULL(v3);
    TEST_ASSERT_NULL(getVerticeComponente(c, 3));
}

void test_boundingBox_componente(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    
    Componente c1 = getComponente(componentes, 0);
    Componente c2 = getComponente(componentes, 1);
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getBBoxX(c1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getBBoxY(c1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 22.0, getBBoxW(c1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, getBBoxH(c1));
    
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getBBoxX(c2));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 9.0, getBBoxY(c2));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 22.0, getBBoxW(c2));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, getBBoxH(c2));
}

void test_getCorComponente(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(2, quantComponentes(componentes));
    
    Componente c1 = getComponente(componentes, 0);
    Componente c2 = getComponente(componentes, 1);
    
    const char* cor1 = getCorComponente(c1);
    const char* cor2 = getCorComponente(c2);
    
    TEST_ASSERT_NOT_NULL(cor1);
    TEST_ASSERT_NOT_NULL(cor2);
    TEST_ASSERT_NOT_EQUAL_STRING(cor1, cor2);
}

void test_getComponente_indice_invalido(void) {
    componentes = encontrarComponentes(g, 7.0);
    TEST_ASSERT_NOT_NULL(componentes);
    
    Componente c = getComponente(componentes, 99);
    TEST_ASSERT_NULL(c);
    
    c = getComponente(componentes, -1);
    TEST_ASSERT_NULL(c);
}

void test_deletaListaComponentes_NULL(void) {
    deletaListaComponentes(NULL);
}

void test_encontrarComponentes_com_arestas_desativadas(void) {
    Aresta a = buscarAresta(g, "A", "B");
    offAresta(a);
    
    componentes = encontrarComponentes(g, 10.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(4, quantComponentes(componentes));
    
    onAresta(a);
}

void test_boundingBox_componente_unico_vertice(void) {
    Grafo g2 = criaGrafo();
    adicionarVertice(g2, "X", 50.0, 50.0);
    
    componentes = encontrarComponentes(g2, 10.0);
    TEST_ASSERT_NOT_NULL(componentes);
    TEST_ASSERT_EQUAL_INT(1, quantComponentes(componentes));
    
    Componente c = getComponente(componentes, 0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 49.0, getBBoxX(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 49.0, getBBoxY(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, getBBoxW(c));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, getBBoxH(c));
    
    deletaGrafo(g2);
}

void test_quantComponentes_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, quantComponentes(NULL));
}

void test_tamanhoComponente_NULL(void) {
    TEST_ASSERT_EQUAL_INT(0, tamanhoComponente(NULL));
}

void test_getBBoxX_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getBBoxX(NULL));
}

void test_getBBoxY_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getBBoxY(NULL));
}

void test_getBBoxW_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getBBoxW(NULL));
}

void test_getBBoxH_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0, getBBoxH(NULL));
}

void test_getCorComponente_NULL(void) {
    TEST_ASSERT_NULL(getCorComponente(NULL));
}

void test_getVerticeComponente_NULL(void) {
    TEST_ASSERT_NULL(getVerticeComponente(NULL, 0));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_encontrarComponentes_dois_componentes);
    RUN_TEST(test_encontrarComponentes_um_componente);
    RUN_TEST(test_encontrarComponentes_todos_desconectados);
    RUN_TEST(test_encontrarComponentes_grafo_vazio);
    RUN_TEST(test_encontrarComponentes_parametros_invalidos);
    RUN_TEST(test_tamanhoComponente);
    RUN_TEST(test_getVerticeComponente);
    RUN_TEST(test_boundingBox_componente);
    RUN_TEST(test_getCorComponente);
    RUN_TEST(test_getComponente_indice_invalido);
    RUN_TEST(test_deletaListaComponentes_NULL);
    RUN_TEST(test_encontrarComponentes_com_arestas_desativadas);
    RUN_TEST(test_boundingBox_componente_unico_vertice);
    RUN_TEST(test_quantComponentes_NULL);
    RUN_TEST(test_tamanhoComponente_NULL);
    RUN_TEST(test_getBBoxX_NULL);
    RUN_TEST(test_getBBoxY_NULL);
    RUN_TEST(test_getBBoxW_NULL);
    RUN_TEST(test_getBBoxH_NULL);
    RUN_TEST(test_getCorComponente_NULL);
    RUN_TEST(test_getVerticeComponente_NULL);
    
    return UNITY_END();
}
