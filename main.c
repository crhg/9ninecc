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

    // トークナイズする
    user_input = argv[1];
    tokenize();
    Node *node = expr();

    if (!consume(TK_EOF))
        error_at(tokens[pos].input, "EOFでありません");

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // コード生成
    gen(node);

    // スタックトップに残っている式全体の値を
    // RAXにロードして関数からの返値にする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
