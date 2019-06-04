#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void try_ret(int expected, int actual, char *name) {
    if (expected != actual) {
        printf("%s: expected %d, actual %d\n", name, expected, actual);
        exit(1);
    }
}

char *actual_out;
size_t actual_out_size;
int actual_out_len;

void try_out_start(void) {
    actual_out_size = 100;
    actual_out = malloc(actual_out_size);
    if (actual_out == NULL) {
        perror("try_out_start: malloc失敗");
        abort();
    }

    actual_out[0] = '\0';
    actual_out_len = 0;
}

void try_out_check(char *expected, char *name) {

    // 最後の改行は削る
    if (actual_out_len > 0 && actual_out[actual_out_len - 1] == '\n') {
        actual_out[actual_out_len - 1] = '\0';
    }

    if (strcmp(expected, actual_out) != 0) {
        printf("%s: expected \"%s\", actual \"%s\"\n", name, expected, actual_out);
        exit(1);
    }
}

void try_printf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (;;) {
        int len = vsnprintf(&actual_out[actual_out_len], actual_out_size - actual_out_len, fmt, ap);
        actual_out_len += len;

        // snprintfは切り詰めがあったかどうかわからないので、バッファを1バイト余計に使って検出する
        if (actual_out_len <= actual_out_size - 2)
            break;

        actual_out_size *= 2;
        actual_out = realloc(actual_out, actual_out_size);
        if (actual_out == NULL) {
            perror("try_print: realloc失敗");
            abort();
        }
    }
}

