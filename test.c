#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

void expect(int line, int expected, int actual) {
    if (expected == actual)
        return;

    fprintf(stderr, "%d: %d expected, but got %d\n",
            line, expected, actual);
    exit(1);
}

int string_equal(char *x, char *y) {
    if (x == NULL || y == NULL) {
        return x == y;
    }

    return strcmp(x, y) == 0;
}

void expect_string(int line, char *expected, char* actual) {
    if (string_equal(expected, actual)) {
        return;
    }

    fprintf(stderr, "%d: %s expected, but got %s\n",
            line, expected, actual);
    exit(1);
}

void test_vector() {
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++) {
        int *p = malloc(sizeof(int));
        *p = i;
        vec_push(vec, p);
    }


    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, *(int *)vec->data[0]);
    expect(__LINE__, 50, *(int *)vec->data[50]);
    expect(__LINE__, 99, *(int *)vec->data[99]);
}

void test_map() {
    Map *map = new_map();
    expect(__LINE__, 0, (long)map_get(map, "foo"));

    map_put(map, "foo", (void *)2);
    expect(__LINE__, 2, (long)map_get(map, "foo"));

    /* map_put(map, "bar", (void *)4); */
    /* expect(__LINE__, 4, (long)map_get(map, "bar")); */
    /*  */
    /* map_put(map, "foo", (void *)6); */
    /* expect(__LINE__, 6, (long)map_get(map, "foo")); */
}

void test_strprintf() {
    expect_string(__LINE__, "", strprintf(""));
    expect_string(__LINE__, "hoge", strprintf("hoge"));
    expect_string(__LINE__, "hoge", strprintf("%s", "hoge"));
    expect_string(__LINE__, "42", strprintf("%d", 42));
    expect_string(__LINE__, "42", strprintf("%ld", (long)42));
}

void runtest() {
    test_vector();
    test_map();
    test_strprintf();

    printf("OK\n");
}
