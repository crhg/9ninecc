#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    if (strcmp(argv[1], "-test") == 0) {
        runtest();
        exit(0);
    }

    // トークナイズする
    user_input = argv[1];
    tokenize();

    // パース
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");

    // コード生成
    for (int i = 0; i < functions->len; i++) {
        gen(functions->data[i]);
    }

    return 0;
}
