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

// エラー・警告表示関数群

// エラーを報告するための関数
// printfと同じ引数を取る

void vwarn(char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
    exit(1);
}

void warn(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn(fmt, ap);
    va_end(ap);
}

void vwarn_at(char *loc, char *fmt, va_list args) {
    fflush(stdout);

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
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at(loc, fmt, ap);
    va_end(ap);
    exit(1);
}

void warn_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at(loc, fmt, ap);
    va_end(ap);
}

void vwarn_at_here(char *fmt, va_list args) {
    vwarn_at(TOKEN(pos)->input, fmt, args);
}

// 現在位置のトークンでエラー
void error_at_here(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_here(fmt, ap);
    va_end(ap);
    exit(1);
}

// 現在位置のトークンで警告
void warn_at_here(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_here(fmt, ap);
    va_end(ap);
}

void vwarn_at_token(Token *token, char *fmt, va_list args) {
    if (token) {
        vwarn_at(token->input, fmt, args);
    } else {
        vwarn(fmt, args);
    }
}

void error_at_token(Token *token, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_token(token, fmt, ap);
    va_end(ap);
    exit(1);
}

void warn_at_token(Token *token, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_token(token, fmt, ap);
    va_end(ap);
}

void assert_at_node(Node *node, int cond, char *fmt, ...) {
    if (cond) return;

    va_list ap;
    va_start(ap, fmt);
    vwarn_at_token(node->token, fmt, ap);
    va_end(ap);
    exit(1);
}

void error_at_node(Node *node, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_token(node->token, fmt, ap);
    va_end(ap);
    exit(1);
}

void warn_at_node(Node *node, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vwarn_at_token(node->token, fmt, ap);
    va_end(ap);
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

// 結果文字列の領域を自分で用意するsprintf
/* char *strprintf(char *fmt, ...) { */
/*     va_list ap; */
/*     va_start(ap, fmt); */
/*  */
/*     size_t size = 16; */
/*     char *s = malloc(size); */
/*     if (s == NULL) { */
/*         perror("strprintf: malloc"); */
/*     } */
/*  */
/*     for (;;) { */
/*         fprintf(stderr, "s=%lx, size=%ld\n", (size_t)s, size); */
/*         s[size-1] = 0; */
/*         fprintf(stderr, "touch ok\n"); */
/*  */
/*  */
/*         fprintf(stderr, "vsnprintf s=%lx, size=%ld, fmt=%s, result=", (size_t)s, size, fmt); */
/*         int l = vfprintf(stderr, fmt, ap); */
/*         fprintf(stderr, ", len=%d\n", l); */
/*  */
/*         int len = vsnprintf(s, size, fmt, ap); */
/*         fprintf(stderr, "vsprintf OK\n"); */
/*         if (len <= size -2) */
/*             return s; */
/*  */
/*         size *= 2; */
/*         s = realloc(s, size); */
/*         if (s == NULL) { */
/*             perror("strprintf: realloc"); */
/*         } */
/*     } */
/* } */
// 何故かSEGVするので簡易実装
// 上限1023文字
char *strprintf(char *fmt, ...) {
    char buf[1024];

    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    char *s = malloc(len+1);
    if (s == NULL) {
        perror("strprintf: malloc");
    }

    strcpy(s, buf);
    return s;
}
