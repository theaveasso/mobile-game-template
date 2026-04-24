#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

extern int _tests_run;
extern int _tests_failed;

#define TEST_ASSERT(cond) do {                                        \
    if (!(cond)) {                                                    \
        _tests_failed++;                                               \
        fprintf(stderr, "  FAIL: %s:%d: %s\n",                         \
                __FILE__, __LINE__, #cond);                            \
    }                                                                  \
} while (0)

#define TEST_ASSERT_EQ(a, b) do {                                     \
    long long _va = (long long)(a);                                   \
    long long _vb = (long long)(b);                                   \
    if (_va != _vb) {                                                 \
        _tests_failed++;                                               \
        fprintf(stderr, "  FAIL: %s:%d: %s == %s (got %lld vs %lld)\n",\
                __FILE__, __LINE__, #a, #b, _va, _vb);                 \
    }                                                                  \
} while (0)

#define TEST_CASE(name) static void test_##name(void)

#define TEST_RUN(name) do {                                           \
    _tests_run++;                                                     \
    printf("  %s\n", #name);                                           \
    test_##name();                                                     \
} while (0)

#define TEST_SUMMARY_AND_EXIT() do {                                  \
    printf("\n%d tests run, %d failed\n", _tests_run, _tests_failed);  \
    return _tests_failed > 0 ? 1 : 0;                                  \
} while (0)

#endif
