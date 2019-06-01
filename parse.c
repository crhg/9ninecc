#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "9ninecc.h"

// ローカル変数のマップ
Map *local_var_map;

// ローカル変数を新しく登録する
LocalVar *new_local_var(char *name) {
    LocalVar *local_var = malloc(sizeof(LocalVar));
    map_put(local_var_map, name, local_var);
    return local_var;
}
// ノードを作る
Node *new_node(int ty, Token *token) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->token = token;
    return node;
}

// 2項演算子のノードを作る
Node *new_node_binop(int ty, Node *lhs, Node *rhs, Token *token) {
    Node *node = new_node(ty, token);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値のノードを作る
Node *new_node_num(int val, Token *token) {
    Node *node = new_node(ND_NUM, token);
    node->val = val;
    return node;
}

// 変数参照のノードを作る
Node *new_node_var(Token *token) {
    LocalVar *local_var = LOCAL_VAR(token->name);
    if (local_var != NULL) {
        Node *node = new_node(ND_LOCAL_VAR, token);
        node->local_var = local_var;
        return node;
    }

    error_at(token->input, "未定義の変数です");
}

Node *expr();

//- <term> ::=
//-     '(' <expr> ')'
//-   | NUM
//-   | IDENT ('(' (<expr> (',' <expr>)* )? ')' )?
//-   | * <term>
//-   | & <term>
Node *term() {
    Token *token;

    if (consume('(')) {
        Node *node = expr();
        if (!consume(')'))
            error_at(TOKEN(pos)->input, "開きカッコに対応する閉じカッコがありません");
        return node;
    }

    if ((token = consume('*'))) {
        Node *node = new_node(ND_PTR, token);
        node->ptrto = term();
        return node;
    }

    if ((token = consume('&'))) {
        Node *node = new_node(ND_PTR_OF, token);
        node->ptrof = term();
        return node;
    }

    if ((token = consume(TK_NUM)) != NULL)
        return new_node_num(token->val, token);

    if ((token = consume(TK_IDENT)) != NULL) {
        if (!consume('(')) {
            // ただの変数参照
            return new_node_var(token);
        } else {
            // カッコがあれば関数呼び出し
            Node *node = new_node(ND_CALL, token);
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

//- <unary> ::= ('+'|'-') <term>
Node *unary() {
    Token *token;
    if (consume('+'))
        return term();
    if ((token = consume('-')))
        return new_node_binop('-', new_node_num(0, token), term(), token);
    return term();
}

//- <mul> ::= <unary> (('*'|'/') <unary>)*
Node *mul() {
    Node *node = unary();

    Token *token;
    for (;;) {
        if ((token = consume('*')))
            node = new_node_binop('*', node, unary(), token);
        else if ((token = consume('/')))
            node = new_node_binop('/', node, unary(), token);
        else
            return node;
    }
}

//- <add> ::= <mul> (('+'|'-') <mul>)*
Node *add() {
    Node *node = mul();

    Token *token;
    for (;;) {
        if ((token = consume('+')))
            node = new_node_binop('+', node, mul(), token);
        else if ((token = consume('-')))
            node = new_node_binop('-', node, mul(), token);
        else
            return node;
    }
}

//- <relational> ::= <add> (('<'|LE|'>'|GE) <add>)*
Node *relational() {
    Node *node = add();

    Token *token;
    for (;;) {
        if ((token = consume('<')))
            node = new_node_binop('<', node, add(), token);
        else if ((token = consume(TK_LE)))
            node = new_node_binop(ND_LE, node, add(), token);
        else if ((token =consume('>')))
            node = new_node_binop('<', add(), node, token);
        else if ((token =consume(TK_GE)))
            node = new_node_binop(ND_LE, add(), node, token);
        else
            return node;
    }
}

//- <equality> ::= <relational>> ((EQ|NE) <relational>)*
Node *equality() {
    Node *node = relational();

    Token *token;
    for (;;) {
        if ((token = consume(TK_EQ)))
            node = new_node_binop(ND_EQ, node, relational(), token);
        else if ((token = consume(TK_NE)))
            node = new_node_binop(ND_NE, node, relational(), token);
        else
            return node;
    }
}

//- <assign> ::= <equality> ('=' <assign>)*
Node *assign() {
    Node *node = equality();

    Token *token;
    if ((token = consume('=')))
        node = new_node_binop('=', node, assign(), token);
    return node;
}

//- <expr> ::= <assign>
Node *expr() {
    Node *expr = assign();
    assign_type_to_expr(expr);
    return expr;
}

Node *stmt();

//- <block> ::= '{' <stmt>* '}'
Node *block() {
    if (!consume('{')) {
        error_at(TOKEN(pos)->input, "'{'でないトークンです2");
    }

    Node *node = new_node(ND_BLOCK, NULL);
    Vector *stmts = new_vector();
    node->stmts = stmts;

    while (!consume('}')) {
        vec_push(stmts, stmt());
    }

    return node;
}

//- <ptr_ident> :: = IDENT || '*' <ptr_ident>
Node *ptr_ident() {
    Node *node;

    Token *token;
    if ((token = consume('*'))) {
        node = new_node(ND_PTR, token);
        node->ptrto = ptr_ident();
        return node;
    } 

    Token *id;
    if ((id = consume(TK_IDENT)) != NULL) {
        node = new_node(ND_IDENT, id);
        node->name = id->name;
        return node;
    }

    error_at(TOKEN(pos)->input, "'*'でも'識別子'でもないトークンです");
}

// ローカル変数を作成する
LocalVar * create_local_var(Node *node, Type *type) {
    if (node->ty == ND_IDENT) {
        LocalVar *local_var = new_local_var(node->name);
        local_var->type = type;
        return local_var;
    }

    if (node->ty == ND_PTR) {
        Type *new_type = malloc(sizeof(Type));
        new_type->ty = PTR;
        new_type->ptrof = type;
        return create_local_var(node->ptrto, new_type);
    }

    error("IDENTでも*でもないノード");
}

//- <local_var_def> ::= int <ptr_ident>
Node *local_var_def() {
    // 変数定義
    if (!consume(TK_INT)) {
        error_at(TOKEN(pos)->input, "intではないトークンです");
    }

    Node *node_ptr_ident = ptr_ident();

    Type *type = malloc(sizeof(Type));
    type->ty = INT;
    LocalVar *local_var = create_local_var(node_ptr_ident, type);

    Node *node = new_node(ND_LOCAL_VAR_DEF, NULL);
    node->local_var = local_var;
    return node;
}

//- <stmt> ::=
//-      if '(' <expr> ')' <stmt> (else <stmt>)?
//-    | while '(' <expr> ')' <stmt>
//-    | for '(' <expr>? ';' <expr>? ';' <expr>? ')' <stmt>
//-    | <local_var_def> ';'
//-    | <block>
//-    | return <expr> ';'
//-    | <expr> ';'
Node *stmt() {
    Node *node;

    if (consume(TK_IF)) {
        node = new_node(ND_IF, NULL);

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
        node = new_node(ND_WHILE, NULL);

        if (!consume('('))
            error_at(TOKEN(pos)->input, "'('ではないトークンです");

        node->cond = expr();

        if (!consume(')'))
            error_at(TOKEN(pos)->input, "')'ではないトークンです");

        node->stmt = stmt();

        return node;
    }

    if (consume(TK_FOR)) {
        node = new_node(ND_FOR, NULL);

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

    if (next_token_is(TK_INT)) {
        node = local_var_def();
        if (!consume(';')) {
            error_at(TOKEN(pos)->input, "';'ではないトークンです");
        }
        return node;
    }

    if (next_token_is('{')) {
        return block();
    }

    if (consume(TK_RETURN)) {
        node = new_node_binop(ND_RETURN, expr(), NULL, NULL);
    } else {
        node = new_node_binop(ND_EXPR, expr(), NULL, NULL);
    }

    if (!consume(';'))
        error_at(TOKEN(pos)->input, "';'ではないトークンです");
    return node;
}

// 型のバイト数
int get_size_of(int type) {
    switch (type) {
        case INT:
            return 4;
        case PTR:
            return 8;
        default:
            error("unknown type(get_size_of): %d", type);
    }
}

// マップの中のローカル変数にオフセットを割り当てる
void allocate_local_var(Map *map) {
    int n = LOCAL_VAR_NUM;

    int offset = 0;
    for (int i = 0; i < n; i++) {
        LocalVar *var = LOCAL_VAR_AT(i);
        int size = get_size_of(var->type->ty);

        offset += size;
        if (offset % size != 0) {
            offset += (size - offset % size);
        }

        var->offset = offset;
    }
}

void dump_type(Type *type) {
    if (type == NULL) {
        printf("NULL?");
        return;
    }

    if (type->ty == INT) {
        printf("int");
        return;
    }

    if (type->ty == PTR) {
        printf("ptr of ");
        dump_type(type->ptrof);
        return;
    }

    printf("ty=%d?", type->ty);
}

// ローカル変数をデバッグ出力
void dump_local_var(Map *map) {
    printf("# local variables\n");

    for (int i = 0; i < map->keys->len; i++) {
        LocalVar *var = (LocalVar *)map->vals->data[i];
        printf("#   name=%s, offset=%d, type=", (char *)map->keys->data[i], var->offset);
        dump_type(var->type);
        printf("\n");
    }

    printf("# end\n");
}


//- <function> ::= int IDENT '(' (<local_var_def> (',' <local_var_def>)*)? ')' <block>
Node *function() {
    local_var_map = new_map();
    Node *node = new_node(ND_FUNC, NULL);
    node->params = new_vector();

    if (!consume(TK_INT)) {
        error_at(TOKEN(pos)->input, "intでないトークンです");
    }

    Token *function_name;
    if ((function_name = consume(TK_IDENT)) == NULL) {
        error_at(TOKEN(pos)->input, "識別子でないトークンです");
    }

    node->token = function_name;

    node->name = function_name->name;

    if (!consume('(')) {
        error_at(TOKEN(pos)->input, "'('でないトークンです");
    }

    if (consume(')')) {
        // 引数無し
    } else {
        vec_push(node->params, local_var_def());

        while (consume(',')) {
            vec_push(node->params, local_var_def());
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

    allocate_local_var(local_var_map);

    dump_local_var(local_var_map);

    return node;
}

// 関数定義のベクター
Vector *functions;

//- <program> ::= <function>* EOF
void program() {
    functions = new_vector();

    while (TOKEN(pos)->ty != TK_EOF) {
        vec_push(functions, function());
    }
}

