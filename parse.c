#include <stdlib.h>
#include "9ninecc.h"

// 2項演算子のノードを作る
Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値のノードを作る
Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

// 識別子のノードを作る
Node *new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

Node *expr();

// termのパーサ
Node *term() {
    if (consume('(')) {
        Node *node = expr();
        if (!consume(')'))
            error_at(TOKEN(pos)->input, "開きカッコに対応する閉じカッコがありません");
        return node;
    }

    Token *token;
    if ((token = consume(TK_NUM)) != NULL)
        return new_node_num(token->val);

    if ((token = consume(TK_IDENT)) != NULL)
        return new_node_ident(*(token->input));

    error_at(TOKEN(pos)->input, "数値でも開きカッコでも識別子でもないトークンです");
}

// unaryのパーサ
Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

// mulのパーサ
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

// addのパーサ
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

// 大小関係のバーサ
Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume('<'))
            node = new_node('<', node, add());
        else if (consume(TK_LE))
            node = new_node(ND_LE, node, add());
        else if (consume('>'))
            node = new_node('<', add(), node);
        else if (consume(TK_GE))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

// ==と!=のパーサ
Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume(TK_EQ))
            node = new_node(ND_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

// 代入式のパーサ
Node *assign() {
    Node *node = equality();
    if (consume('='))
        node = new_node('=', node, assign());
    return node;
}

// 式のパーサ
Node *expr() {
    return assign();
}

// 文のパーサ
Node *stmt() {
    Node *node = expr();
    if (!consume(';'))
        error_at(TOKEN(pos)->input, "';'ではないトークンです");
    return node;
}

// プログラムを保存する配列
Node *code[100];

// プログラムのパーサ
void program() {
    int i = 0;
    while (TOKEN(pos)->ty != TK_EOF)
        code[i++] = stmt();
    code[i] = NULL;
}
