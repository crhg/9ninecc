#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

// 入力ファイル名
char *filename;


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

    printf("# %s\n", nodeToStr(prog));

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");

    // コード生成
    gen(prog);

    return 0;
}
