#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
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


