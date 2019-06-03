#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

int is_alnum(char c) {
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}

void verror_at(char *loc, char *fmt, va_list args) {
    // locが含まれている行の開始地点と終了地点を取得
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n')
        end++;

    // 見つかった行が全体の何行目なのかを調べる
    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n')
            line_num++;

    // 見つかった行を、ファイル名と行番号と一緒に表示
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    exit(1);
}

void error_at_token(Token *token, char *fmt, ...) {

    va_list ap;
    va_start(ap, fmt);
    verror_at_token(token, fmt, ap);
}

void verror_at_token(Token *token, char *fmt, va_list args) {
    if (token) {
        verror_at(token->input, fmt, args);
    } else {
        verror(fmt, args);
    }
}

void assert_at_node(Node *node, int cond, char *fmt, ...) {
    if (cond) return;

    va_list ap;
    va_start(ap, fmt);
    verror_at_token(node->token, fmt, ap);
}

void error_at_node(Node *node, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at_token(node->token, fmt, ap);
}

void print_loc(char *loc) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
}

void print_token_pos(Token *token) {
    if (token) {
        print_loc(token->input);
    }
}

void print_node_pos(Node *node) {
    print_token_pos(node->token);
}

// 指定されたファイルの内容を返す
char *read_file(char *path) {
  // ファイルを開く
  FILE *fp = fopen(path, "r");
  if (!fp)
    error("cannot open %s: %s", path, strerror(errno));

  // ファイルの長さを調べる
  if (fseek(fp, 0, SEEK_END) == -1)
    error("%s: fseek: %s", path, strerror(errno));
  size_t size = ftell(fp);
  if (fseek(fp, 0, SEEK_SET) == -1)
    error("%s: fseek: %s", path, strerror(errno));

  // ファイル内容を読み込む
  char *buf = malloc(size + 2);
  fread(buf, size, 1, fp);

  // ファイルが必ず"\n\0"で終わっているようにする
  if (size == 0 || buf[size - 1] != '\n')
    buf[size++] = '\n';
  buf[size] = '\0';
  fclose(fp);
  return buf;
}


