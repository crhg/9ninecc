#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9ninecc.h"

// グローバル変数のマップ
// 名前->型
Map *global_var_map;

// グローバル変数を登録する
void new_global_var(char *name, Type *type) {
    map_put(global_var_map, name, type);
}

// ローカル変数のマップ
Map *local_var_map;

// ローカル変数を新しく登録する
LocalVar *new_local_var(char *name, Type *type) {
    LocalVar *local_var = malloc(sizeof(LocalVar));
    local_var->type = type;
    map_put(local_var_map, name, local_var);
    return local_var;
}

// 文字列リテラルのベクター
Vector *strings;

// 新しい文字列リテラルを登録しインデックス番号を返す
int new_string(char *string) {
    int r = strings->len;

    vec_push(strings, string);

    return r;
}

// 式の値が配列型だったらポインタ型に書き換える
// 暗黙の配列からポインタへの型変換に使う
Node *conv_a_to_p(Node *node) {
    assert_at_node(node, node->type != NULL, "ノードに型がありません(conv_a_to_p)");

    if (node->type->ty == ARRAY) {
        // XXX: Type構造体は共有されているのでnode->type->tyを直接書き換えてはいけない
        // TODO: 毎回新しく割り当てるのももったいないような気もする
        node->type = pointer_of(node->type->ptrof);
    }

    return node;
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
    node->type = &int_type;
    return node;
}

// 変数参照のノードを作る
Node *new_node_var(Token *token) {
    Node *node;

    char *name = token->name;
    LocalVar *local_var = LOCAL_VAR(name);
    if (local_var != NULL) {
        node = new_node(ND_LOCAL_VAR, token);
        node->local_var = local_var;
        node->type = local_var->type;
        node->variable_type = local_var->type;
        return node;
    }

    Type *type = (Type *)map_get(global_var_map, name);
    if (type != NULL) {
        node = new_node(ND_GLOBAL_VAR, token);
        node->type = type;
        node->variable_type = type;
        return node;
    }

    error_at(token->input, "未定義の変数です");
}

// 足し算のノードを作る
Node *new_node_add(Node *lhs, Node *rhs, Token *token) {
    lhs = conv_a_to_p(lhs);
    rhs = conv_a_to_p(rhs);

    if (rhs->type->ty == PTR) {
        Node *tmp;

        tmp = rhs;
        rhs = lhs;
        lhs = tmp;
    }

    if (rhs->type->ty == PTR) {
        error_at_token(token, "加算できない型の組み合わせです");
    }

    Node *node = new_node_binop('+', lhs, rhs, token);
    node->type = lhs->type;

    return node;
}

Node *new_node_ptr(Node *pt, Token *token) {
    pt = conv_a_to_p(pt);

    if (pt->type->ty != PTR) {
        error_at_node(pt, "ポインタ型でありません");
    }

    Node *node = new_node(ND_DEREF, token);
    node->ptrto = pt;
    node->type = pt->type->ptrof;

    return node;
}

// lvalueか判定
int is_lvalue(Node * node) {
    if (node->ty == ND_LOCAL_VAR || node->ty == ND_GLOBAL_VAR) {
        return node->type->ty != ARRAY;
    }

    if (node->ty == ND_DEREF) {
        return 1;
    }

    return 0;
}


Node *expr();

//- <term> ::=
//-     '(' <expr> ')'
//-   | NUM
//-   | STRING
//-   | IDENT ('(' (<expr> (',' <expr>)* )? ')' )?

Node *term() {
    Node *node;
    Token *token;

    if (consume('(')) {
        Node *node_expr = expr();
        if (!consume(')'))
            error_at(TOKEN(pos)->input, "開きカッコに対応する閉じカッコがありません");
        return node_expr;
    }

    if ((token = consume(TK_NUM)) != NULL)
        return new_node_num(token->val, token);

    if ((token = consume(TK_STRING)) != NULL) {
        int index = new_string(token->str);
        Node *stringNode = new_node(ND_STRING, token);
        stringNode->str_index = index;
        stringNode->type = pointer_of(&char_type);
        return stringNode;
    }

    if ((token = consume(TK_IDENT)) != NULL) {
        if (!consume('(')) {
            // ただの変数参照
            return new_node_var(token);
        } else {
            // カッコがあれば関数呼び出し
            node = new_node(ND_CALL, token);
            Vector *params = new_vector();
            node->name = token->name;
            node->type = &int_type;
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

//- <array_term>> ::= <term> ('[' <expr> ']')*
Node *array_term() {
    Node *node = term();

    Token *token;
    while ((token = consume('['))) {
        Node *e = expr();

        if (!consume(']')) {
            error_at(token->input, "']'がありません");
        }

        node = new_node_ptr(new_node_add(node, e, token), token);
    }

    return node;
}

//- <pointer_term> = <array_term> | ('*' | '&') <pointer_term>
Node *pointer_term() {
    Token *token;
    if ((token = consume('*'))) {
        return new_node_ptr(pointer_term(), token);
    }

    if ((token = consume('&'))) {
        Node *pt;
        pt = pointer_term();
        if (pt->type->ty == ARRAY) {
            return conv_a_to_p(pt);
        }

        if (!is_lvalue(pt)) {
            error_at_node(pt, "lvalueでありません: %d", pt->ty);
        }

        Node *node = new_node(ND_GET_PTR, token);
        node->ptrof = pt;
        node->type = pointer_of(pt->type);
        return node;
    }

    return array_term();
}

//- <unary> ::= sizeof <unary>
//-           | ('+'|'-') <pointer_term>
Node *unary() {
    Token *token;
    if ((token = consume(TK_SIZEOF))) {
        Node *node_unary = unary();
        return new_node_num(get_size_of(node_unary->type), token);
    }

    if (consume('+'))
        return pointer_term();

    if ((token = consume('-'))) {
        Node *node_term = conv_a_to_p(pointer_term());
        if (node_term->type->ty == PTR) {
            error_at_node(node_term, "ポインターを負にすることはできません");
        }

        Node *node = new_node_binop('-', new_node_num(0, token), node_term, token);
        node->type = &int_type;
        return node;
    }
    return pointer_term();
}

//- <mul> ::= <unary> (('*'|'/') <unary>)*
Node *mul() {
    Node *node = unary();

    Node *rhs;
    Token *token;
    for (;;) {
        if ((token = consume('*'))) {
            node = conv_a_to_p(node);
            rhs = conv_a_to_p(unary());
            if (node->type->ty == PTR) {
                error_at_node(node, "ポインタの掛け算はできません");
            }
            if (rhs->type->ty == PTR) {
                error_at_node(rhs, "ポインタの掛け算はできません");
            }

            node = new_node_binop('*', node, rhs, token);
            node->type = &int_type;
        } else if ((token = consume('/'))) {
            node = conv_a_to_p(node);
            rhs = conv_a_to_p(unary());
            if (node->type->ty == PTR) {
                error_at_node(node, "ポインタの割り算はできません");
            }
            if (rhs->type->ty == PTR) {
                error_at_node(rhs, "ポインタの割り算はできません");
            }
            node = new_node_binop('/', node, rhs, token);
            node->type = &int_type;
        } else {
            return node;
        }
    }
}

//- <add> ::= <mul> (('+'|'-') <mul>)*
Node *add() {
    Node *node = mul();

    Token *token;
    Node *lhs;
    Node *rhs;
    for (;;) {
        if ((token = consume('+'))) {
            lhs = node;
            rhs = mul();
            node = new_node_add(lhs, rhs, token);
        } else if ((token = consume('-'))) {
            lhs = conv_a_to_p(node);
            rhs = conv_a_to_p(mul());
            node = new_node_binop('-', lhs, rhs, token);

            // XXX: INTとPTRしかないのでこの判定
            if (lhs->type->ty == PTR && type_eq(lhs->type, rhs->type)) {
                node->type = &int_type;
            } else if (rhs->type->ty == INT || rhs->type->ty == CHAR) {
                node->type = lhs->type;
            } else {
                error_at_token(token, "減算できない型の組み合わせです");
            }
        } else {
            return node;
        }
    }
}

//- <relational> ::= <add> (('<'|LE|'>'|GE) <add>)*
Node *relational() {
    Node *node = add();

    Token *token;
    for (;;) {
        if ((token = consume('<'))) {
            node = new_node_binop('<', conv_a_to_p(node), conv_a_to_p(add()), token);
            node->type = &int_type;
        } else if ((token = consume(TK_LE))) {
            node = new_node_binop(ND_LE, conv_a_to_p(node), conv_a_to_p(add()), token);
            node->type = &int_type;
        } else if ((token =consume('>'))) {
            node = new_node_binop('<', conv_a_to_p(add()), conv_a_to_p(node), token);
            node->type = &int_type;
        } else if ((token =consume(TK_GE))) {
            node = new_node_binop(ND_LE, conv_a_to_p(add()), conv_a_to_p(node), token);
            node->type = &int_type;
        } else {
            return node;
        }
    }
}

//- <equality> ::= <relational>> ((EQ|NE) <relational>)*
Node *equality() {
    Node *node = relational();

    Token *token;
    for (;;) {
        if ((token = consume(TK_EQ))) {
            node = new_node_binop(ND_EQ, conv_a_to_p(node), conv_a_to_p(relational()), token);
            node->type = &int_type;
        } else if ((token = consume(TK_NE))) {
            node = new_node_binop(ND_NE, conv_a_to_p(node), conv_a_to_p(relational()), token);
            node->type = &int_type;
        } else {
            return node;
        }
    }
}

//- <assign> ::= <equality> ('=' <assign>)*
Node *assign() {
    Node *node = equality();

    Type *type = node->type;

    Token *token;
    if ((token = consume('='))) {
        if (!is_lvalue(node)) {
            error_at_node(node, "lvalueでありません");
        }

        node = new_node_binop('=', node, conv_a_to_p(assign()), token);
        // XXX: 代入式の型は左辺の型にしといたけど要確認
        node->type = type;
    }
    return node;
}

//- <expr> ::= <assign>
Node *expr() {
    Node *expr = assign();
    /* assign_type_to_expr(expr); */
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

// 定数式の値を求める
int eval_constant_expr(Node *node) {
    switch (node->ty) {
        case ND_NUM:
            return node->val;
        case '+':
            return eval_constant_expr(node->lhs) + eval_constant_expr(node->rhs);
        case '-':
            return eval_constant_expr(node->lhs) - eval_constant_expr(node->rhs);
        case '*':
            return eval_constant_expr(node->lhs) * eval_constant_expr(node->rhs);
        case '/':
            return eval_constant_expr(node->lhs) / eval_constant_expr(node->rhs);
        // TODO: 演算子のバリエーション
        default:
            error_at_node(node, "定数式ではありません");
    }
}

//- <ptr_ident> :: = IDENT ('[' <expr> ']')? | '*' <ptr_ident>
Node *ptr_ident(Type *type) {
    Node *node;

    Token *token;
    if ((token = consume('*'))) {
        node = new_node(ND_DEREF, token);
        node->ptrto = ptr_ident(pointer_of(type));
        node->type = type;
        return node;
    } 

    Token *id;
    if ((id = consume(TK_IDENT)) != NULL) {
        if (consume('[')) {
            Node *size_expr = expr();
            if (!consume(']')) {
                error_at(TOKEN(pos)->input, "']'でないトークンです");
            }

            type = array_of(type, eval_constant_expr(size_expr), 0);
        }

        node = new_node(ND_IDENT, id);
        node->name = id->name;
        node->type = type;
        node->local_var = new_local_var(id->name, type);
        return node;
    }

    error_at(TOKEN(pos)->input, "'*'でも'識別子'でもないトークンです");
}

LocalVar *find_local_var(Node *node) {
    for (;;) {
        if (node->ty == ND_IDENT) {
            return node->local_var;
        }
        node = node->ptrto;
    }
}

//- <type_spec> ::= int | char
// 見つからなければnullを返す
Type *type_spec() {
    if (consume(TK_INT)) {
        return &int_type;
    }

    if (consume(TK_CHAR)) {
        return &char_type;
    }

    return NULL;
}

Declarator *declarator(Type *type);
void declaration_rest(Type *type, Declarator *decl, Vector *vec);

//- <local_var_def> ::=
//-     <type_spec> <declarator> <declaration_rest>
// <type_spec>は既に読まれた状態で呼ばれる。結果のtypeはパラメタでもらう
Node *local_var_def(Type *type) {
    Declarator *decl = declarator(type);
    Vector *vec = new_vector();

    declaration_rest(type, decl, vec);

    Vector *local_vars = new_vector();
    for (int i = 0; i < vec->len; i++) {
        Declarator *d = ((DeclInit *) vec->data[i])->decl;
        vec_push(local_vars, new_local_var(d->id->name, d->type));
    }

    Node *node = new_node(ND_LOCAL_VAR_DEF, decl->id); // XXX: とりらえず最初の識別子トークンで代表
    node->decl_inits = vec;
    node->local_vars = local_vars;

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
    Type *type;

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

    type = type_spec();
    if (type != NULL) {
        node = local_var_def(type);
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

// マップの中のローカル変数にオフセットを割り当てる
void allocate_local_var(Map *map) {
    int n = LOCAL_VAR_NUM;

    int offset = 0;
    for (int i = 0; i < n; i++) {
        LocalVar *var = LOCAL_VAR_AT(i);
        int size = get_size_of(var->type);
        int alignment = get_alignment(var->type);

        offset += size;
        if (offset % alignment != 0) {
            offset += (size - offset % alignment);
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

    if (type->ty == ARRAY) {
        printf("array[%d] of ", type->array_size);
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

//          (http://port70.net/~nsz/c/c11/n1570.html#6.7.6)
//          declarator:
//                 pointeropt direct-declarator
//          direct-declarator:
//                  identifier
//                  ( declarator )
//                  direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
//                  direct-declarator [ static type-qualifier-listopt assignment-expression ]
//                  direct-declarator [ type-qualifier-list static assignment-expression ]
//                  direct-declarator [ type-qualifier-listopt * ]
//                  direct-declarator ( parameter-type-list )
//                  direct-declarator ( identifier-listopt )
//          pointer:
//                 * type-qualifier-listopt
//                 * type-qualifier-listopt pointer
//          type-qualifier-list:
//                 type-qualifier
//                 type-qualifier-list type-qualifier
//          parameter-type-list:
//                parameter-list
//                parameter-list , ...
//          parameter-list:
//                parameter-declaration
//                parameter-list , parameter-declaration
//          parameter-declaration:
//                declaration-specifiers declarator
//                declaration-specifiers abstract-declaratoropt
//          identifier-list:
//                 identifier
//                 identifier-list , identifier
Declarator *declarator(Type *type);

//- <param_decl> ::= <type_spec> <declarator>
Declarator *param_decl() {
    Type *type = type_spec();
    if (type == NULL) {
        error_at(TOKEN(pos)->input, "intまたはcharでないトークンです(param_decl)");
    }

    return declarator(type);
}

//- <direct_declarator_rest> ::=
//-     ε
//-   | '[' <expr>? ']' <direct_declarator_rest>
//-   | '(' (<param_decl> (,<param_decl>)*)? ')'
Type *direct_declarator_rest(Type *type) {
    Token *token;
    if ((token = consume('['))) {
        size_t size;
        if (next_token_is(']')) {
            size = 0;
        } else {
            Node *e = expr();
            size = eval_constant_expr(e);

        }

        if (!consume(']')) {
            error_at(TOKEN(pos)->input, "']'でないトークンです(direct_declarator)");
        }

        Type *array_type = array_of(direct_declarator_rest(type), size, 0);
        array_type->token = token;
        return array_type;
    }

    if ((token = consume('('))) {
        // TODO: パラメタリストは型を書かずidだけの古い形式もあるが対応していない
        // TODO: ...には対応していない
        // TODO: 関数型の定義だけなら仮引数名は省略できるが対応していない
        Vector *params = new_vector();
        if (!next_token_is(')')) {
            vec_push(params, param_decl());
            while (consume(',')) {
                vec_push(params, param_decl());
            }
        }

        if (!consume(')')) {
            error_at(TOKEN(pos)->input, "')'でないトークンです(direct_declarator)");
        }

        Type *function_type = function_of(direct_declarator_rest(type), params);
        function_type->token = token;
        return function_type;
    }

    return type;
}

//- <direct_declarator> ::- (<ident>|'(' <declarator> ')')<direct_declarator_rest>
// 左再帰を回避するため変換した。
Declarator *direct_declarator(Type *type) {
    Token *id;
    Declarator *decl;

    if (consume('(')) {
        decl = declarator(NULL);
        if (!consume(')')) {
            error_at(TOKEN(pos)->input, "'('でないトークンです(direct_declarator)");
        }
    } else if ((id = consume(TK_IDENT)) != NULL) {
        decl = malloc(sizeof(Declarator));
        decl->id = id;
    } else {
        error_at(TOKEN(pos)->input, "識別子でも'('でもないトークンです(direct_declarator)");
    }


    decl->type = direct_declarator_rest(type);
    return decl;
}

//- <declarator> ::= '*' <declarator> | <direct_declarator>
Declarator *declarator(Type *type) {
    Token *token;
    if ((token = consume('*'))) {
        Declarator *decl = declarator(pointer_of(type));
        type->token = token;
        return decl;
    }

    return direct_declarator(type);
}

int get_initializer_size(Initializer *init, int level) {
    if (init == NULL) { return -1; }

    if (init->ty == INITIALIZER_TYPE_EXPR && init->expr->ty ==ND_STRING && level == 0) {
        return strlen(init->expr->token->str) + 1;
    }

    if (init->ty == INITIALIZER_TYPE_LIST) {
        if (level == 0) {
            return init->list->len;
        }

        int ret = 0;
        for (int i = 0; i < init->list->len; i++) {
            int size = get_initializer_size(init->list->data[i], level - 1);
            if (size < 0) { return -1; }
            if (size > ret) {
                ret = size;
            }
        }

        return ret;
    }

    return -1;
}

void determine_array_size(Type *type, Initializer *init) {
    int level = 0;
    while (type->ty == ARRAY && type->incomplete_size) {
        int size = get_initializer_size(init, level);
        if (size < 0) {
            error("配列の要素数が確定しません");
        }
        type->array_size = size;
        type->incomplete_size = 0;

        type = type->ptrof;
        level++;
    }
}

Node *global_var_def(Declarator *decl, Initializer *init) {
    if (decl->type->ty == FUNC) {
        error_at_token(decl->id, "関数型の変数は定義できません");
    }

    new_global_var(decl->id->name, decl->type);

    Node *node = new_node(ND_GLOBAL_VAR_DEF, decl->id);
    node->type = decl->type;
    node->initializer = init;

    determine_array_size(decl->type, init);

    return node;
}

Node *init_declarator_global(Declarator *decl, Initializer *init) {
    // いまのところはグローバル変数の定義だけ
    // そのうちtypedefとかも
    return global_var_def(decl, init);
}

//- <top_level> ::=
//-     <type_spec> <declarator> <block>
//-   | <type_spec> <declarator> ('=' <initializer>)?  (',' <declarator> ('=' <initializer>)? )* ';'
//
//     *配列サイズは定数式でなければならない
//     *パラメタはintかポインタのみ許される
//
//          (http://port70.net/~nsz/c/c11/n1570.html#6.9)
//          translation-unit:
//                  external-declaration
//                  translation-unit external-declaration
//          external-declaration:
//                 function-definition
//                 declaration
//
//          (http://port70.net/~nsz/c/c11/n1570.html#6.9.1)
//          function-definition:
//                 declaration-specifiers declarator declaration-listopt compound-statement
//          declaration-list: # 関数のパラメタの型の古い書き方 int f(x) int x; {...} みたいな奴
//                 declaration
//                 declaration-list declaration
//
//          (http://port70.net/~nsz/c/c11/n1570.html#6.7)
//          declaration:
//                 declaration-specifiers init-declarator-listopt ;
//                 static_assert-declaration
//          declaration-specifiers:
//                 storage-class-specifier declaration-specifiersopt # typedef, extern, static, auto, register, _Thread_local
//                 type-specifier declaration-specifiersopt # void, char, int, struct..., union..., enum..., ...
//                 type-qualifier declaration-specifiersopt # const, restrict, volatile, _Atomic
//                 function-specifier declaration-specifiersopt # include, _Noreturn
//                 alignment-specifier declaration-specifiersopt # _Alignas (...)
//          init-declarator-list:
//                  init-declarator
//                  init-declarator-list , init-declarator
//          init-declarator:
//                  declarator
//                  declarator = initializer

// 関数のパラメタとして使える型かどうか
int is_allowed_as_param(Type *type) {
    switch (type->ty) {
        case CHAR:
        case INT:
        case PTR:
            return 1;
        default:
            return 0;
    }
}

void register_param_as_local_var(Vector *params) {
    for (int i = 0; i < params->len; i++) {
        Declarator *decl = (Declarator *)params->data[i];

        if (!is_allowed_as_param(decl->type)) {
            error_at_token(decl->id, "パラメタとして許されない型です: %s", typeToStr(decl->type));
        }

        // ローカル変数として登録してリンクを張る
        decl->local_var = new_local_var(decl->id->name, decl->type);
    }
}

//- <function_definition> ::= <block>
Node *function_definition(Declarator *decl) {
    Node *node = new_node(ND_FUNC, decl->id);
    node->name = decl->id->name;
    node->type = decl->type;

    local_var_map = new_map();
    register_param_as_local_var(decl->type->params);
    printf("params:\n");
    dump_local_var(local_var_map);

    node->stmt = block();
    node->local_var_map = local_var_map;

    allocate_local_var(local_var_map);
    dump_local_var(local_var_map);

    printf("# function %s parsed\n", decl->id->name);

    return node;
}

//- <initializer> ::=
//-     <expr>
//-   | '{' (<initializer> (',' <initializer>)* ','? '}'
Initializer *initializer() {
    Initializer *ret;

    ret = malloc(sizeof(Initializer));

    if (consume('{')) {
        ret->ty = INITIALIZER_TYPE_LIST;
        Vector *list = new_vector();
        for (;;) {
            if (consume('}')) {
                break;
            }

            vec_push(list, initializer());

            if (consume('}')) {
                break;
            }

            if (!consume(',')) {
                error_at(TOKEN(pos)->input, "','でも'}'でもないトークンです");
            }
        }
        ret->list = list;
        return ret;
    }

    ret->ty = INITIALIZER_TYPE_EXPR;
    ret->expr = expr();
    return ret;
}


DeclInit *decl_init(Declarator *decl, Initializer *init) {
    DeclInit *ret = malloc(sizeof(DeclInit));
    ret->decl = decl;
    ret->init = init;
    return ret;
}

// 変数定義の残りの部分
//- <declaration_rest> ::=
//-     ('=' <initializer>)? ( ';'| ',' <declarator> <declaration_rest>)
// vecに結果を追加していく。
void declaration_rest(Type *type, Declarator *decl, Vector *vec) {
    if (consume('=')) {
        Initializer *init = initializer();
        vec_push(vec, decl_init(decl, init));
    } else {
        vec_push(vec, decl_init(decl, NULL));
    }

    if (consume(';')) {
        return;
    }

    if (!consume(',')) {
        error_at(TOKEN(pos)->input, "','でも';'でもないトークンです");
    }

    declaration_rest(type, declarator(type), vec);
}


//- <top_level> ::=
//     <type_spec> <declarator> <function_definition> # <function_definition>は'{'で開始する
//   | <type_spec> <declarator> ('=' <initializer>)? (',' <declarator> ('=' <initializer>)?)* ';'
void top_level(Vector *top_levels) {
    Type *type;
    Declarator *decl;

    type = type_spec();
    if (type == NULL) {
        error_at(TOKEN(pos)->input, "intまたはcharでないトークンです(top_level)");
    }

    decl = declarator(type);

    if (next_token_is('{')) {
        vec_push(top_levels, function_definition(decl));
        return;
    }

    Vector *decl_inits = new_vector();
    declaration_rest(type, decl, decl_inits);
    for (int i = 0; i < decl_inits->len; i++) {
        DeclInit *decl_init = (DeclInit *)decl_inits->data[i];
        vec_push(top_levels, init_declarator_global(decl_init->decl, decl_init->init));
    }
}


//- <program> ::= <top_level>* EOF
Node *program() {
    Vector *top_levels = new_vector();
    global_var_map = new_map();
    strings = new_vector();

    while (TOKEN(pos)->ty != TK_EOF) {
        top_level(top_levels);
    }

    Node *node = new_node(ND_PROGRAM, NULL);
    node->top_levels = top_levels;
    node->strings = strings;
    return node;
}
