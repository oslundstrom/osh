#include "unity.h"
#include "osh.h"

void tearDown(void) {
    
}

void setUp(void) {
    
}

void test_split_to_cmds(void) {
    char str[] = "sleep 2 && echo hej";
    char **tokens;

    tokens = split_to_cmds(str);

    TEST_ASSERT_EQUAL_STRING(tokens[0], "sleep 2");
    TEST_ASSERT_EQUAL_STRING(tokens[1], "echo hej");
}

void test_strsplit(void) {
    char str[] = "sleep 2 && echo hej";
    char *token;

    token = strsplit(str, "&&");
    TEST_ASSERT_EQUAL_STRING("sleep 2 ", token);
    token = strsplit(NULL, "&&");
    TEST_ASSERT_EQUAL_STRING(" echo hej", token);
    token = strsplit(NULL, "&&");
    TEST_ASSERT_EQUAL_STRING(NULL, token);
}

void test_strsplit_multi(void) {
    char str2[] = "sleep 2 || echo hej";
    char *token;

    token = strsplit(str2, "&&,||");
    TEST_ASSERT_EQUAL_STRING("sleep 2 ", token);
    token = strsplit(NULL, "&&,||");
    TEST_ASSERT_EQUAL_STRING(" echo hej", token);
    token = strsplit(NULL, "&&,||");
    TEST_ASSERT_EQUAL_STRING(NULL, token);
}

void test_split_tokens(void) {
    char str[] = "&&,||";
    char **tokens;
    
    tokens = split_tokens(str, ",");
    TEST_ASSERT_EQUAL_STRING("&&", tokens[0]);
    TEST_ASSERT_EQUAL_STRING("||", tokens[1]);
}

void test_split_to_args(void) {
    char str[] = "echo hej";
    char **tokens;

    tokens = split_to_args(str);

    TEST_ASSERT_EQUAL_STRING(tokens[0], "echo");
    TEST_ASSERT_EQUAL_STRING(tokens[1], "hej");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_split_to_args);
    RUN_TEST(test_strsplit);
    RUN_TEST(test_strsplit_multi);
    RUN_TEST(test_split_tokens);
    return UNITY_END();
}