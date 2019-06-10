#include <ctype.h>
#include <stdarg.h>
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

// 次のトークンが期待した値か調べます
int next_token_is(int ty) {
    return TOKEN(pos)->ty == ty;
}

// ポインタで示されたところから名前の長さを求める
size_t name_length(char *p) {
    size_t r = 0;
    while (is_alnum(*p)) {
        p++;
        r++;
    }

    return r;
}


// ポインタで示されたところから名前を切り出して
// メモリを確保して複製して返す。
// rpがNULLでなければ名前のつぎの文字を指すポインタを書き込む
char *dup_name(char *p, char **rp) {
    size_t l = name_length(p);
    char *r = malloc(l + 1);
    memcpy(r, p, l);
    r[l] = '\0';

    if (rp) {
        *rp = p + l;
    }

    return r;
}

Token *push_token(int ty, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    vec_push(token_vector, token);
    return token;
}

// ポインタの指すところにキーワードがあるか。
// *np にキーワードの次の文字のポインタを返す
int keyword(char *p, char **np, char *name) {
    int l = strlen(name);
    if (strncmp(p, name, l) == 0 && !is_alnum(p[l])) {
        *np = p + l;
        return 1;
    }
    
    return 0;
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

        // 行コメントをスキップ
        if (strncmp(p, "//", 2) == 0) {
            p += 2;
            while (*p != '\n')
                p++;
            continue;
        }

        // ブロックコメントをスキップ
        if (strncmp(p, "/*", 2) == 0) {
            char *q = strstr(p + 2, "*/");
            if (!q)
                error_at(p, "コメントが閉じられていません");
            p = q + 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
                || *p == '&'
                || *p == '(' || *p == ')' || *p == ';' || *p == ','
                || *p == '{' || *p == '}'
                || *p == '[' || *p == ']'
        ) {
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

        // 文字列トークン。とりあえずエスケープなどはなし
        if (*p == '"') {
            Token *s = push_token(TK_STRING, p);

            char *q = p + 1;

            while(*q != '"') {
                if (*q == '\0') {
                    error_at(p, "文字列リテラルの終端がありません");
                }
                q++;
            }

            size_t len = q - (p + 1);
            s->str = malloc(len + 1);
            strncpy(s->str, p + 1, len);
            s->str[len] = '\0';

            p = q + 1;
            continue;
        }

        char *np;
        if (keyword(p, &np, "return")) {
            push_token(TK_RETURN, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "if")) {
            push_token(TK_IF, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "else")) {
            push_token(TK_ELSE, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "while")) {
            push_token(TK_WHILE, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "for")) {
            push_token(TK_FOR, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "int")) {
            push_token(TK_INT, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "char")) {
            push_token(TK_CHAR, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "sizeof")) {
            push_token(TK_SIZEOF, p);
            p = np;
            continue;
        }

        if (keyword(p, &np, "struct")) {
            push_token(TK_STRUCT, p);
            p = np;
            continue;
        }


        if (isdigit(*p)) {
            Token *num_token = push_token(TK_NUM, p);
            num_token->val = strtol(p, &p, 10);
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            Token *ident_token = push_token(TK_IDENT, p);
            ident_token->name = dup_name(p, &p);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    push_token(TK_EOF, p);
}
