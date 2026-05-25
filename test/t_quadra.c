#include "unity.h"
#include "quadra.h"
#include <stdlib.h>
#include <string.h>

static Quadra q1, q2;

void setUp(void) {
    q1 = criaQuadra("cep15", 100.0, 200.0, 80.0, 60.0, "#FF0000", "#00FF00", 2.0);
    q2 = criaQuadra("cep07", 300.0, 150.0, 50.0, 40.0, "#0000FF", "#FFFF00", 1.5);
}

void tearDown(void) {
    if (q1) { freeQuadra(q1); q1 = NULL; }
    if (q2) { freeQuadra(q2); q2 = NULL; }
}

void test_criaQuadra_sucesso(void) {
    TEST_ASSERT_NOT_NULL(q1);
    TEST_ASSERT_NOT_NULL(q2);
}

void test_criaQuadra_cep_invalido(void) {
    Quadra q = criaQuadra("", 10.0, 20.0, 30.0, 40.0, "#000000", "#FFFFFF", 1.0);
    TEST_ASSERT_NULL(q);
    q = criaQuadra(NULL, 10.0, 20.0, 30.0, 40.0, "#000000", "#FFFFFF", 1.0);
    TEST_ASSERT_NULL(q);
}

void test_criaQuadra_dimensoes_invalidas(void) {
    Quadra q = criaQuadra("teste", 10.0, 20.0, 0, 40.0, "#000000", "#FFFFFF", 1.0);
    TEST_ASSERT_NULL(q);
    q = criaQuadra("teste", 10.0, 20.0, 30.0, -5.0, "#000000", "#FFFFFF", 1.0);
    TEST_ASSERT_NULL(q);
}

void test_criaQuadra_espessura_invalida(void) {
    Quadra q = criaQuadra("teste", 10.0, 20.0, 30.0, 40.0, "#000000", "#FFFFFF", 0);
    TEST_ASSERT_NULL(q);
    q = criaQuadra("teste", 10.0, 20.0, 30.0, 40.0, "#000000", "#FFFFFF", -1.0);
    TEST_ASSERT_NULL(q);
}

void test_getCepQuadra(void) {
    TEST_ASSERT_EQUAL_STRING("cep15", getCepQuadra(q1));
    TEST_ASSERT_EQUAL_STRING("cep07", getCepQuadra(q2));
}

void test_getCepQuadra_NULL(void) {
    TEST_ASSERT_NULL(getCepQuadra(NULL));
}

void test_getXQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.0, getXQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 300.0, getXQuadra(q2));
}

void test_getYQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 200.0, getYQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 150.0, getYQuadra(q2));
}

void test_getLQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 80.0, getLQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 50.0, getLQuadra(q2));
}

void test_getHQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 60.0, getHQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 40.0, getHQuadra(q2));
}

void test_getCorBQuadra(void) {
    TEST_ASSERT_EQUAL_STRING("#FF0000", getCorBQuadra(q1));
    TEST_ASSERT_EQUAL_STRING("#0000FF", getCorBQuadra(q2));
}

void test_getCorPQuadra(void) {
    TEST_ASSERT_EQUAL_STRING("#00FF00", getCorPQuadra(q1));
    TEST_ASSERT_EQUAL_STRING("#FFFF00", getCorPQuadra(q2));
}

void test_getEspBQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, getEspBQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.5, getEspBQuadra(q2));
}

void test_areaQuadra(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, 4800.0, areaQuadra(q1));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2000.0, areaQuadra(q2));
}

void test_areaQuadra_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, areaQuadra(NULL));
}

void test_validaFace(void) {
    TEST_ASSERT_TRUE(validaFace('N'));
    TEST_ASSERT_TRUE(validaFace('n'));
    TEST_ASSERT_TRUE(validaFace('S'));
    TEST_ASSERT_TRUE(validaFace('s'));
    TEST_ASSERT_TRUE(validaFace('L'));
    TEST_ASSERT_TRUE(validaFace('l'));
    TEST_ASSERT_TRUE(validaFace('O'));
    TEST_ASSERT_TRUE(validaFace('o'));
    TEST_ASSERT_FALSE(validaFace('X'));
    TEST_ASSERT_FALSE(validaFace('1'));
}

void test_validaCor(void) {
    TEST_ASSERT_TRUE(validaCor("FF0000"));
    TEST_ASSERT_TRUE(validaCor("#00FF00"));
    TEST_ASSERT_TRUE(validaCor("ABCDEF"));
    TEST_ASSERT_TRUE(validaCor("steelblue"));
    TEST_ASSERT_TRUE(validaCor("MistyRose"));
    TEST_ASSERT_FALSE(validaCor(""));
    TEST_ASSERT_FALSE(validaCor(NULL));
}

void test_setCorBQuadra(void) {
    setCorBQuadra(q1, "#ABCDEF");
    TEST_ASSERT_EQUAL_STRING("#ABCDEF", getCorBQuadra(q1));
}

void test_setCorPQuadra(void) {
    setCorPQuadra(q1, "#123456");
    TEST_ASSERT_EQUAL_STRING("#123456", getCorPQuadra(q1));
}

void test_setEspBQuadra(void) {
    setEspBQuadra(q1, 3.5);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, getEspBQuadra(q1));
}

void test_setEspBQuadra_invalida(void) {
    setEspBQuadra(q1, 3.5);
    setEspBQuadra(q1, 0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, getEspBQuadra(q1));
    setEspBQuadra(q1, -1.0);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, getEspBQuadra(q1));
}

void test_getPontoEndQuadra(void) {
    double x, y;
    getPontoEndQuadra(q1, 'N', 30.0, &x, &y);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 130.0, x);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 200.0, y);

    getPontoEndQuadra(q1, 'S', 20.0, &x, &y);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 120.0, x);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 140.0, y);

    getPontoEndQuadra(q1, 'L', 15.0, &x, &y);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.0, x);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 185.0, y);

    getPontoEndQuadra(q1, 'O', 10.0, &x, &y);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.0, x);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 190.0, y);
}

void test_getPontoEndQuadra_invalida(void) {
    double x = 999, y = 999;
    getPontoEndQuadra(NULL, 'N', 10.0, &x, &y);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 999.0, x);
}

void test_serialDesserialQuadra(void) {
    size_t tam = tamSerialQuadra();
    void *buffer = malloc(tam);
    TEST_ASSERT_NOT_NULL(buffer);

    int ret = serialQuadra(q1, buffer, tam);
    TEST_ASSERT_EQUAL_INT(1, ret);

    Quadra reconstruida = desserialQuadra(buffer, tam);
    TEST_ASSERT_NOT_NULL(reconstruida);

    TEST_ASSERT_EQUAL_STRING(getCepQuadra(q1), getCepQuadra(reconstruida));
    TEST_ASSERT_FLOAT_WITHIN(0.01, getXQuadra(q1), getXQuadra(reconstruida));
    TEST_ASSERT_FLOAT_WITHIN(0.01, getYQuadra(q1), getYQuadra(reconstruida));
    TEST_ASSERT_FLOAT_WITHIN(0.01, getLQuadra(q1), getLQuadra(reconstruida));
    TEST_ASSERT_FLOAT_WITHIN(0.01, getHQuadra(q1), getHQuadra(reconstruida));
    TEST_ASSERT_FLOAT_WITHIN(0.01, getEspBQuadra(q1), getEspBQuadra(reconstruida));

    freeQuadra(reconstruida);
    free(buffer);
}

void test_serialQuadra_buffer_insuficiente(void) {
    char buffer[10];
    int ret = serialQuadra(q1, buffer, 10);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_desserialQuadra_buffer_insuficiente(void) {
    char buffer[10];
    Quadra q = desserialQuadra(buffer, 10);
    TEST_ASSERT_NULL(q);
}

void test_freeQuadra_NULL(void) {
    freeQuadra(NULL);
}

void test_cor_sem_hashtag(void) {
    Quadra q = criaQuadra("teste", 10.0, 20.0, 30.0, 40.0, "AA00BB", "00CCDD", 1.0);
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("AA00BB", getCorBQuadra(q));
    TEST_ASSERT_EQUAL_STRING("00CCDD", getCorPQuadra(q));
    freeQuadra(q);
}

void test_getters_com_NULL(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1000.0, getXQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1000.0, getYQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getLQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getHQuadra(NULL));
    TEST_ASSERT_NULL(getCorBQuadra(NULL));
    TEST_ASSERT_NULL(getCorPQuadra(NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.01, -1.0, getEspBQuadra(NULL));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criaQuadra_sucesso);
    RUN_TEST(test_criaQuadra_cep_invalido);
    RUN_TEST(test_criaQuadra_dimensoes_invalidas);
    RUN_TEST(test_criaQuadra_espessura_invalida);
    RUN_TEST(test_getCepQuadra);
    RUN_TEST(test_getCepQuadra_NULL);
    RUN_TEST(test_getXQuadra);
    RUN_TEST(test_getYQuadra);
    RUN_TEST(test_getLQuadra);
    RUN_TEST(test_getHQuadra);
    RUN_TEST(test_getCorBQuadra);
    RUN_TEST(test_getCorPQuadra);
    RUN_TEST(test_getEspBQuadra);
    RUN_TEST(test_areaQuadra);
    RUN_TEST(test_areaQuadra_NULL);
    RUN_TEST(test_validaFace);
    RUN_TEST(test_validaCor);
    RUN_TEST(test_setCorBQuadra);
    RUN_TEST(test_setCorPQuadra);
    RUN_TEST(test_setEspBQuadra);
    RUN_TEST(test_setEspBQuadra_invalida);
    RUN_TEST(test_getPontoEndQuadra);
    RUN_TEST(test_getPontoEndQuadra_invalida);
    RUN_TEST(test_serialDesserialQuadra);
    RUN_TEST(test_serialQuadra_buffer_insuficiente);
    RUN_TEST(test_desserialQuadra_buffer_insuficiente);
    RUN_TEST(test_freeQuadra_NULL);
    RUN_TEST(test_cor_sem_hashtag);
    RUN_TEST(test_getters_com_NULL);

    return UNITY_END();
}
