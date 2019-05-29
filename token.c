#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "9ninecc.h"

// 入力プログラム
char *user_input;

// トーク結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];
int pos = 0;

// 次のトークンが期待した値かチェックして
// 期待した値の場合だけ入力を1トークン読み進めてそのトークンを返す
// それ以外はNULLを返す
Token *consume(int ty) {
    if (tokens[pos].ty != ty)
        return NULL;

    return &tokens[pos++];
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *msg) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize() {
    char *p = user_input;

    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (*p == '=' && *(p+1) == '=') {
            tokens[i].ty = TK_EQ;
            tokens[i].input = p;
            i++;
            p+=2;
            continue;
        }

        if (*p == '!' && *(p+1) == '=') {
            tokens[i].ty = TK_NE;
            tokens[i].input = p;
            i++;
            p+=2;
            continue;
        }

        if (*p == '>') {
            if (*(p+1) == '=') {
                tokens[i].ty = TK_GE;
                tokens[i].input = p;
                i++;
                p+=2;
                continue;
            }

            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (*p == '<') {
            if (*(p+1) == '=') {
                tokens[i].ty = TK_LE;
                tokens[i].input = p;
                i++;
                p+=2;
                continue;
            }

            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}
