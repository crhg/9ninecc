#include <stdlib.h>
#include "9ninecc.h"

// ローカル変数のマップ
Map *local_var_map;

// ローカル変数のオフセットを取得する
// マップに未登録なら登録する。
int get_local_var_offset(char *name) {
    int *offset_p = map_get(local_var_map, name);
    if (offset_p != NULL) {
        return *offset_p;
    }

    offset_p = malloc(sizeof(int));
    *offset_p = local_var_map->keys->len * 8;
    map_put(local_var_map, name, offset_p);

    return *offset_p;
}

// ノードを作る
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
Node *new_node_ident(char *name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->offset = get_local_var_offset(name);
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

    if ((token = consume(TK_IDENT)) != NULL) {
        if (!consume('(')) {
            // ただの変数参照
            return new_node_ident(token->name);
        } else {
            // カッコがあれば関数呼び出し
            Node *node = new_node(ND_CALL, NULL, NULL);
            Vector *params = new_vector();
            node->name = token->name;
            node->params = params;

            if (consume(')')) {
               return node;
            }

            vec_push(params, expr());

            while (consume(',')) {
                vec_push(params, expr());
            }

            if (!consume(')'))
                error_at(TOKEN(pos)->input, "閉じカッコがありません");

            return node;
        }
    }

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
    Node *node;

    if (consume(TK_IF)) {
        node = new_node(ND_IF, NULL, NULL);

        if (!consume('('))
            error_at(TOKEN(pos)->input, "'('ではないトークンです");

        node->cond = expr();

        if (!consume(')'))
            error_at(TOKEN(pos)->input, "')'ではないトークンです");

        node->stmt = stmt();

        if (consume(TK_ELSE)) {
            node->else_stmt = stmt();
        } else {
            node->else_stmt = NULL;
        }

        return node;
    }

    if (consume(TK_WHILE)) {
        node = new_node(ND_WHILE, NULL, NULL);

        if (!consume('('))
            error_at(TOKEN(pos)->input, "'('ではないトークンです");

        node->cond = expr();

        if (!consume(')'))
            error_at(TOKEN(pos)->input, "')'ではないトークンです");

        node->stmt = stmt();

        return node;
    }

    if (consume(TK_FOR)) {
        node = new_node(ND_FOR, NULL, NULL);

        if (!consume('('))
            error_at(TOKEN(pos)->input, "'('ではないトークンです");

        if (consume(';')) {
            node->init = NULL;
        } else {
            node->init = expr();
            if (!consume(';'))
                error_at(TOKEN(pos)->input, "';'ではないトークンです");
        }

        if (consume(';')) {
            node->cond = NULL;
        } else {
            node->cond = expr();
            if (!consume(';'))
                error_at(TOKEN(pos)->input, "';'ではないトークンです");
        }

        if (consume(')')) {
            node->next = NULL;
        } else {
            node->next = expr();
            if (!consume(')'))
                error_at(TOKEN(pos)->input, "')'ではないトークンです");
        }

        node->stmt = stmt();

        return node;
    }

    if (consume('{')) {
        node = new_node(ND_BLOCK, NULL, NULL);
        Vector *stmts = new_vector();
        node->stmts = stmts;

        while (!consume('}')) {
            vec_push(stmts, stmt());
        }

        return node;
    }




    if (consume(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
    } else {
        node = new_node(ND_EXPR, expr(), NULL);
    }

    if (!consume(';'))
        error_at(TOKEN(pos)->input, "';'ではないトークンです");
    return node;
}

// プログラムを保存する配列
Node *code[100];

// プログラムのパーサ
void program() {
    local_var_map = new_map();

    int i = 0;
    while (TOKEN(pos)->ty != TK_EOF)
        code[i++] = stmt();
    code[i] = NULL;
}
