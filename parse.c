#include <stdlib.h>
#include "9ninecc.h"

// ローカル変数のマップ
Map *local_var_map;

// ローカル変数を新しく登録する
// オフセットを返す(オフセットは0にはならないことに注意)
int new_local_var_offset(char *name) {
    int offset = (local_var_map->keys->len +1 ) * 8;
    map_put(local_var_map, name, (void*)offset);

    return offset;
}

// ローカル変数のオフセットを取得する
// マップに未登録なら0を返す(有効なオフセットは0ではない)
int get_local_var_offset(char *name) {
    return (int)map_get(local_var_map, name);
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
Node *new_node_ident(Token *token) {
    int offset = get_local_var_offset(token->name);
    if (offset == 0) {
        error_at(token->input, "未定義の変数です");
    }

    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->offset = offset;
    return node;
}

// 変数定義のノードを作る
Node *new_node_var(Token *token) {
    int offset = new_local_var_offset(token->name);

    Node *node = malloc(sizeof(Node));
    node->ty = ND_VAR;
    node->offset = offset;
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
            return new_node_ident(token);
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

Node *stmt();

// ブロックのパーサ
Node *block() {
    if (!consume('{')) {
        error_at(TOKEN(pos)->input, "'{'でないトークンです2");
    }

    Node *node = new_node(ND_BLOCK, NULL, NULL);
    Vector *stmts = new_vector();
    node->stmts = stmts;

    while (!consume('}')) {
        vec_push(stmts, stmt());
    }

    return node;
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

    if (consume(TK_INT)) {
        // 変数定義
        Token *id;
        if ((id = consume(TK_IDENT)) == NULL) {
            error_at(TOKEN(pos)->input, "'識別子'ではないトークンです");
        }
        if (!consume(';')) {
            error_at(TOKEN(pos)->input, "';'ではないトークンです");
        }

        return new_node_var(id);
    }

    if (next_token_is('{')) {
        return block();
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

// 関数定義のパーサ
Node *function() {
    local_var_map = new_map();
    Node *node = new_node(ND_FUNC, NULL, NULL);
    node->params = new_vector();

    if (!consume(TK_INT)) {
        error_at(TOKEN(pos)->input, "intでないトークンです");
    }

    Token *function_name;
    if ((function_name = consume(TK_IDENT)) == NULL) {
        error_at(TOKEN(pos)->input, "識別子でないトークンです");
    }

    node->name = function_name->name;

    if (!consume('(')) {
        error_at(TOKEN(pos)->input, "'('でないトークンです");
    }

    if (consume(')')) {
        // 引数無し
    } else {
        Token *param;

        if (!consume(TK_INT)) {
            error_at(TOKEN(pos)->input, "intでないトークンです");
        }


        if ((param = consume(TK_IDENT)) == NULL) {
            error_at(TOKEN(pos)->input, "識別子でないトークンです");
        }

        // TODO: パラメタ名の重複チェック
        vec_push(node->params, new_node_var(param));

        while (consume(',')) {
            if (!consume(TK_INT)) {
                error_at(TOKEN(pos)->input, "intでないトークンです");
            }

            if ((param = consume(TK_IDENT)) == NULL) {
                error_at(TOKEN(pos)->input, "識別子でないトークンです");
            }

            vec_push(node->params, new_node_var(param));
        }

        if (!consume(')')) {
            error_at(TOKEN(pos)->input, "')'でないトークンです");
        }
    }

    if (!next_token_is('{')) {
        error_at(TOKEN(pos)->input, "'{'でないトークンです1");
    }

    node->stmt = block();
    node->local_var_map = local_var_map;

    return node;
}

// 関数定義のベクター
Vector *functions;

// プログラムのパーサ
void program() {
    functions = new_vector();

    while (TOKEN(pos)->ty != TK_EOF) {
        vec_push(functions, function());
    }
}
