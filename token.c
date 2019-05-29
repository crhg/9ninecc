#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "9ninecc.h"

// 入力プログラム
char *user_input;

// トークン列を保存するべクター
Vector *token_vector;

// トークンの読み出し位置
int pos = 0;

// 次のトークンが期待した値かチェックして
// 期待した値の場合だけ入力を1トークン読み進めてそのトークンを返す
// それ以外はNULLを返す
Token *consume(int ty) {
    if (TOKEN(pos)->ty != ty)
        return NULL;

    return TOKEN(pos++);
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *msg) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}

Token *push_token(int ty, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    vec_push(token_vector, token);
    return token;
}

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize() {
    char *p = user_input;
    token_vector = new_vector();

    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == ';') {
            push_token(*p, p);
            p++;
            continue;
        }

        if (*p == '=') {
            if (*(p+1) == '=') {
                push_token(TK_EQ, p);
                p+=2;
                continue;
            }

            push_token(*p, p);
            p++;
            continue;
        }

        if (*p == '!' && *(p+1) == '=') {
            push_token(TK_NE, p);
            p+=2;
            continue;
        }

        if (*p == '>') {
            if (*(p+1) == '=') {
                push_token(TK_GE, p);
                p+=2;
                continue;
            }

            push_token(*p, p);
            p++;
            continue;
        }

        if (*p == '<') {
            if (*(p+1) == '=') {
                push_token(TK_LE, p);
                p+=2;
                continue;
            }

            push_token(*p, p);
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            push_token(TK_RETURN, p);
            p += 6;
            continue;
        }

        if (isdigit(*p)) {
            Token *num_token = push_token(TK_NUM, p);
            num_token->val = strtol(p, &p, 10);
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            push_token(TK_IDENT, p);
            p++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    push_token(TK_EOF, p);
}
