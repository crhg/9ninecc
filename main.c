#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

// 入力ファイル名
char *filename;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror(fmt, ap);
}

void verror(char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
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
    filename = argv[1];
    user_input = read_file(filename);
    tokenize();

    // パース
    Node *prog = program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");

    // コード生成
    gen(prog);

    return 0;
}
