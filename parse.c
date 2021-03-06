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

// 構造体名のマップ。名前→型
Map *struct_map;

void register_struct(char *name, Type *type) {
    map_put(struct_map, name, type);
}

Type *get_struct(char *name) {
    return map_get(struct_map, name);
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

    Node *node = new_node_binop(ND_ADD, lhs, rhs, token);
    node->type = lhs->type;

    return node;
}

Node *new_node_deref(Node *pt, Token *token) {
    pt = conv_a_to_p(pt);

    if (pt->type->ty != PTR) {
        error_at_node(pt, "ポインタ型でありません");
    }

    Node *node = new_node(ND_DEREF, token);
    node->ptrto = pt;
    node->type = pt->type->ptrof;

    return node;
}

int is_lvalue(Node * node);
Node *new_node_get_ptr(Node *pt, Token *token) {
    if (!is_lvalue(pt)) {
        error_at_node(pt, "lvalueでありません: %d", pt->ty);
    }

    Node *node = new_node(ND_GET_PTR, token);
    node->ptrof = pt;
    node->type = pointer_of(pt->type);
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

    if (node->ty == ND_ARROW) {
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

    if (consume(TK_LPAR)) {
        Node *node_expr = expr();
        if (!consume(TK_RPAR))
            error_at_here("開きカッコに対応する閉じカッコがありません");
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
        if (!consume(TK_LPAR)) {
            // ただの変数参照
            return new_node_var(token);
        } else {
            // カッコがあれば関数呼び出し
            node = new_node(ND_CALL, token);
            Vector *params = new_vector();
            node->name = token->name;
            node->type = &int_type;
            node->params = params;

            if (consume(TK_RPAR)) {
               return node;
            }

            vec_push(params, expr());

            while (consume(TK_COMMA)) {
                vec_push(params, expr());
            }

            if (!consume(TK_RPAR))
                error_at_here("閉じカッコがありません");

            return node;
        }
    }

    error_at_here("数値でも開きカッコでも識別子でもないトークンです");
}

Node *new_node_arrow(Node *term, Token *field_name, Field *field, Token *token);

//- <array_term>> ::= <term> ('[' <expr> ']' | '->' <ident> | '.' <ident> ))*
Node *array_term() {
    Node *node = term();

    Token *token;
    for (;;) {
        if ((token = consume(TK_LBRACKET))) {
            Node *e = expr();

            if (!consume(TK_RBRACKET)) {
                error_at(token->input, "']'がありません");
            }

            node = new_node_deref(new_node_add(node, e, token), token);

            continue;
        }

        if ((token = consume(TK_ARROW))) {
            assert_at_node(node, node->type != NULL, "型が不明です");
            if (!(node->type->ty == PTR && (node->type->ptrof->ty == STRUCT || node->type->ptrof->ty == UNION))) {
                error_at_token(token, "左辺がstructまたはunionへのポインタでない");
            }

            Token *field_name = consume(TK_IDENT);
            Field *field = map_get(node->type->ptrof->fields, field_name->name);
            if (field == NULL) {
                error_at_token(field_name, "存在しないフィールド名です");
            }

            node = new_node_arrow(node, field_name, field, token);
            continue;
        }

        if ((token = consume(TK_DOT))) {
            assert_at_node(node, node->type != NULL, "型が不明です");
            if (!(node->type->ty == STRUCT || node->type->ty == UNION)) {
                error_at_token(token, "左辺がstructまたはunionでない");
            }

            Token *field_name = consume(TK_IDENT);
            Field *field = map_get(node->type->fields, field_name->name);
            if (field == NULL) {
                error_at_token(field_name, "存在しないフィールド名です");
            }

            // x.y を (&x)->y として扱う
            node = new_node_arrow(new_node_get_ptr(node, token), field_name, field, token);
            continue;
        }

        break;
    }

    return node;
}

Node *new_node_arrow(Node *term, Token *field_name, Field *field, Token *token) {
    Node *arrow = new_node(ND_ARROW, token);
    arrow->term = term;
    arrow->field_name = field_name;
    arrow->field = field;
    arrow->type = field->type;

    // x->yのlvalしか定義してないので一般のx->yは*(&(x->y))に変形しておく
    term = new_node_deref(new_node_get_ptr(arrow, token), token);
    return term;
}

//- <pointer_term> = <array_term> | ('*' | '&') <pointer_term>
Node *pointer_term() {
    Token *token;
    if ((token = consume(TK_AST))) {
        return new_node_deref(pointer_term(), token);
    }

    if ((token = consume(TK_AMP))) {
        return new_node_get_ptr(pointer_term(), token);
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

    if (consume(TK_PLUS))
        return pointer_term();

    if ((token = consume(TK_MINUS))) {
        Node *node_term = conv_a_to_p(pointer_term());
        if (node_term->type->ty == PTR) {
            error_at_node(node_term, "ポインターを負にすることはできません");
        }

        Node *node = new_node_binop(ND_SUB, new_node_num(0, token), node_term, token);
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
        if ((token = consume(TK_AST))) {
            node = conv_a_to_p(node);
            rhs = conv_a_to_p(unary());
            if (node->type->ty == PTR) {
                error_at_node(node, "ポインタの掛け算はできません");
            }
            if (rhs->type->ty == PTR) {
                error_at_node(rhs, "ポインタの掛け算はできません");
            }

            node = new_node_binop(ND_MUL, node, rhs, token);
            node->type = &int_type;
        } else if ((token = consume(TK_SLASH))) {
            node = conv_a_to_p(node);
            rhs = conv_a_to_p(unary());
            if (node->type->ty == PTR) {
                error_at_node(node, "ポインタの割り算はできません");
            }
            if (rhs->type->ty == PTR) {
                error_at_node(rhs, "ポインタの割り算はできません");
            }
            node = new_node_binop(ND_DIV, node, rhs, token);
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
        if ((token = consume(TK_PLUS))) {
            lhs = node;
            rhs = mul();
            node = new_node_add(lhs, rhs, token);
        } else if ((token = consume(TK_MINUS))) {
            lhs = conv_a_to_p(node);
            rhs = conv_a_to_p(mul());
            node = new_node_binop(ND_SUB, lhs, rhs, token);

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
        if ((token = consume(TK_LT))) {
            node = new_node_binop(ND_LT, conv_a_to_p(node), conv_a_to_p(add()), token);
            node->type = &int_type;
        } else if ((token = consume(TK_LE))) {
            node = new_node_binop(ND_LE, conv_a_to_p(node), conv_a_to_p(add()), token);
            node->type = &int_type;
        } else if ((token =consume(TK_GT))) {
            node = new_node_binop(ND_LT, conv_a_to_p(add()), conv_a_to_p(node), token);
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
    if ((token = consume(TK_ASSIGN))) {
        if (!is_lvalue(node)) {
            error_at_node(node, "lvalueでありません");
        }

        node = new_node_binop(ND_ASSIGN, node, conv_a_to_p(assign()), token);
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
    if (!consume(TK_LBRACE)) {
        error_at_here("'{'でないトークンです2");
    }

    Node *node = new_node(ND_BLOCK, NULL);
    Vector *stmts = new_vector();
    node->stmts = stmts;

    while (!consume(TK_RBRACE)) {
        vec_push(stmts, stmt());
    }

    return node;
}

// 定数式の値を求める
int eval_constant_expr(Node *node) {
    switch (node->ty) {
        case ND_NUM:
            return node->val;
        case ND_ADD:
            return eval_constant_expr(node->lhs) + eval_constant_expr(node->rhs);
        case ND_SUB:
            return eval_constant_expr(node->lhs) - eval_constant_expr(node->rhs);
        case ND_MUL:
            return eval_constant_expr(node->lhs) * eval_constant_expr(node->rhs);
        case ND_DIV:
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
    if ((token = consume(TK_AST))) {
        node = new_node(ND_DEREF, token);
        node->ptrto = ptr_ident(pointer_of(type));
        node->type = type;
        return node;
    } 

    Token *id;
    if ((id = consume(TK_IDENT)) != NULL) {
        if (consume(TK_LBRACKET)) {
            Node *len_expr = expr();
            if (!consume(TK_RBRACKET)) {
                error_at_here("']'でないトークンです");
            }

            type = array_of(type, eval_constant_expr(len_expr), 0);
        }

        node = new_node(ND_IDENT, id);
        node->name = id->name;
        node->type = type;
        node->local_var = new_local_var(id->name, type);
        return node;
    }

    error_at_here("'*'でも'識別子'でもないトークンです");
}

//- <type_spec> ::= int | char
// 見つからなければnullを返す
Declarator *declarator(Type *type);
void add_field(Type *type, Declarator *decl);

Type *make_incomplete_struct_or_union_type(TypeId ty, Token *token);

Type *struct_or_union(TypeId ty, Token *token);

Type *type_spec() {
    Token *token;


    if (consume(TK_INT)) {
        return &int_type;
    }

    if (consume(TK_CHAR)) {
        return &char_type;
    }
    
    if ((token = consume(TK_STRUCT)) || (token = consume(TK_UNION))) {
        TypeId ty = (token->ty == TK_STRUCT) ? STRUCT : UNION;

        return struct_or_union(ty, token);
    }

    return NULL;
}

Type *struct_or_union(TypeId ty, Token *token) {
    Type *type = NULL;
    Token *id;

    if ((id = consume(TK_IDENT))) {
        type = get_struct(id->name);
        if (type == NULL) {
            type = make_incomplete_struct_or_union_type(ty, token);
            register_struct(id->name, type);
        } else if (ty != type->ty) {
            error_at_token(id, "二重定義です");
        }
    }

    if (!consume(TK_LBRACE)) {
        if (type != NULL) {
            return type;
        }
        error_at_here("'{'がありません"); // 名前がなければ {..} が必要
    }

    // 無名の場合
    if (type == NULL) {
        type = make_incomplete_struct_or_union_type(ty, token);
    }

    if (!type->incomplete) {
        error_at_token(id, "二重定義です");
    }

    while (!consume(TK_RBRACE)) {
        Type *field_type = type_spec();
        if (field_type == NULL) {
            error_at_here("フィールドの型がありません");
        }

        Declarator *decl;
        decl = declarator(field_type);
        add_field(type, decl);

        while (consume(TK_COMMA)) {
            decl = declarator(field_type);
            add_field(type, decl);
        }

        if (!consume(TK_SEMI)) {
            error_at_here("';'がありません");
        }
    }

    type->incomplete = 0;

    return type;
}

// 空の不完全なstruct型を作る
Type *make_incomplete_struct_or_union_type(TypeId ty, Token *token) {
    Type *type = malloc(sizeof(Type));
    type->ty = ty;
    type->token = token;
    type->fields = new_map();
    type->alignment = 1;
    type->size = 0;
    type->incomplete = 1;
    type->next_offset = 0;
    return type;
}

// 構造体型にフィールドを追加します
void add_field(Type *type, Declarator *decl) {
    if (map_get(type->fields, decl->id->name)) {
        error_at_token(decl->id, "二重定義です");
    }

    int alignment = get_alignment(decl->type);
    type->alignment = max(type->alignment, alignment);
    
    
    int offset;
    int size = get_size_of(decl->type);

    if (type->ty == STRUCT) {
        offset = round_up(type->next_offset, alignment);

        type->next_offset = offset + size;
        type->size = round_up(type->next_offset, type->alignment);
    } else {
        offset = 0;
        type->size = max(
                round_up(type->size, type->alignment),
                round_up(size, type->alignment)
                );
    }

    Field *field = malloc(sizeof(Field));
    field->type = decl->type;
    field->offset = offset;
    map_put(type->fields, decl->id->name, field);
}

void declaration_rest(Type *type, Declarator *decl, Vector *vec);
void determine_array_size(Declarator *decl, Initializer *init);

//- <local_var_def> ::=
//-     <type_spec> <declarator> <declaration_rest>
// <type_spec>は既に読まれた状態で呼ばれる。結果のtypeはパラメタでもらう
Node *local_var_def(Type *type) {
    Token *token;
    if ((token = consume(TK_SEMI))) {
        return new_node(ND_EMPTY, token); // 定義する変数がない場合: 空文を返しておく
    }

    Declarator *decl = declarator(type);
    Vector *vec = new_vector(); // (DeclInit *)のベクター

    declaration_rest(type, decl, vec);

    Vector *local_vars = new_vector();
    for (int i = 0; i < vec->len; i++) {
        DeclInit *decl_init = (DeclInit *) vec->data[i];
        Declarator *d = decl_init->decl;
        determine_array_size(d, decl_init->init);
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

        if (!consume(TK_LPAR))
            error_at_here("'('ではないトークンです");

        node->cond = expr();

        if (!consume(TK_RPAR))
            error_at_here("')'ではないトークンです");

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

        if (!consume(TK_LPAR))
            error_at_here("'('ではないトークンです");

        node->cond = expr();

        if (!consume(TK_RPAR))
            error_at_here("')'ではないトークンです");

        node->stmt = stmt();

        return node;
    }

    if (consume(TK_FOR)) {
        node = new_node(ND_FOR, NULL);

        if (!consume(TK_LPAR))
            error_at_here("'('ではないトークンです");

        if (consume(TK_SEMI)) {
            node->init = NULL;
        } else {
            node->init = expr();
            if (!consume(TK_SEMI))
                error_at_here("';'ではないトークンです");
        }

        if (consume(TK_SEMI)) {
            node->cond = NULL;
        } else {
            node->cond = expr();
            if (!consume(TK_SEMI))
                error_at_here("';'ではないトークンです");
        }

        if (consume(TK_RPAR)) {
            node->next = NULL;
        } else {
            node->next = expr();
            if (!consume(TK_RPAR))
                error_at_here("')'ではないトークンです");
        }

        node->stmt = stmt();

        return node;
    }

    type = type_spec();
    if (type != NULL) {
        node = local_var_def(type);
        return node;
    }

    if (next_token_is(TK_LBRACE)) {
        return block();
    }

    if (consume(TK_RETURN)) {
        node = new_node_binop(ND_RETURN, expr(), NULL, NULL);
        if (!consume(TK_SEMI))
            error_at_here("';'ではないトークンです");
        return node;
    }

    Token *token;
    if ((token = consume(TK_SEMI))) {
        return new_node(ND_EMPTY, token);
    }

    // 残りは <expr> ';'のみ
    node = new_node_binop(ND_EXPR, expr(), NULL, NULL);
    if (!consume(TK_SEMI))
        error_at_here("';'ではないトークンです");
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

        offset = round_up(offset + size, alignment);

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
        printf("array[%d] of ", type->len);
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


//- <param_decl> ::= <type_spec> <declarator>
Declarator *param_decl() {
    Type *type = type_spec();
    if (type == NULL) {
        error_at_here("intまたはcharでないトークンです(param_decl)");
    }

    return declarator(type);
}

//- <direct_declarator_rest> ::=
//-     ε
//-   | '[' <expr>? ']' <direct_declarator_rest>
//-   | '(' (<param_decl> (,<param_decl>)*)? ')'
Type *direct_declarator_rest(Type *type) {
    Token *token;
    char incomplete_len;
    if ((token = consume(TK_LBRACKET))) {
        size_t len;
        if (next_token_is(TK_RBRACKET)) {
            len = 0;
            incomplete_len = 1;
        } else {
            Node *e = expr();
            len = eval_constant_expr(e);
            incomplete_len = 0;
        }

        if (!consume(TK_RBRACKET)) {
            error_at_here("']'でないトークンです(direct_declarator)");
        }

        Type *element_type = direct_declarator_rest(type);
        if (element_type->ty == FUNC) {
            error_at_token(token, "関数型の配列は作れません");
        }
        if (element_type->incomplete) {
            warn_at_token(token, "未確定な型の配列は作れません");
            error_at_token(element_type->token, "%s", typeToStr(element_type));
        }

        Type *array_type = array_of(element_type, len, incomplete_len);
        array_type->token = token;
        return array_type;
    }

    if ((token = consume(TK_LPAR))) {
        // TODO: パラメタリストは型を書かずidだけの古い形式もあるが対応していない
        // TODO: ...には対応していない
        // TODO: 関数型の定義だけなら仮引数名は省略できるが対応していない
        Vector *params = new_vector();
        if (!next_token_is(TK_RPAR)) {
            vec_push(params, param_decl());
            while (consume(TK_COMMA)) {
                vec_push(params, param_decl());
            }
        }

        if (!consume(TK_RPAR)) {
            error_at_here("')'でないトークンです(direct_declarator)");
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

    if (consume(TK_LPAR)) {
        decl = declarator(NULL);
        if (!consume(TK_RPAR)) {
            error_at_here("')'でないトークンです(direct_declarator)");
        }
    } else if ((id = consume(TK_IDENT)) != NULL) {
        decl = malloc(sizeof(Declarator));
        decl->id = id;
    } else {
        error_at_here("識別子でも'('でもないトークンです(direct_declarator)");
    }


    decl->type = direct_declarator_rest(type);
    return decl;
}

//- <declarator> ::= '*' <declarator> | <direct_declarator>
Declarator *declarator(Type *type) {
    Token *token;
    if ((token = consume(TK_AST))) {
        Declarator *decl = declarator(pointer_of(type));
        type->token = token;
        return decl;
    }

    return direct_declarator(type);
}

// もし配列型で長さが未確定の時にinitializerで確定できれば確定させます。
// 確定できなければエラーです。
void determine_array_size(Declarator *decl, Initializer *init) {
    Type *type = decl->type;

    if (!(type->ty == ARRAY && type->incomplete)) {
        return;
    }
    
    if (init == NULL) {
        error_at_token(decl->id, "初期値がないので配列の長さを確定できません");
    }

    int len;
    switch (init->ty) {
        case INITIALIZER_TYPE_EXPR:
            if (init->expr->ty != ND_STRING) {
                error_at_node(init->expr, "文字列リテラル以外の式は配列の初期化に使えません");
            }
            len = strlen(init->expr->token->str) + 1;
            break;
        case INITIALIZER_TYPE_LIST:
            len = init->list->len;
            if (len == 0) {
                error_at_token(decl->id, "配列サイズが省略されているのに初期値リストが空");
            }
            break;
        default:
            error_at_token(decl->id, "予期しないinitializer type: %d", init->ty);
    }

    type->len = len;
    type->size = len * type->ptrof->size;
    type->incomplete = 0;
}

Node *global_var_def(Declarator *decl, Initializer *init) {
    if (decl->type->ty == FUNC) {
        error_at_token(decl->id, "関数型の変数は定義できません");
    }

    new_global_var(decl->id->name, decl->type);

    Node *node = new_node(ND_GLOBAL_VAR_DEF, decl->id);
    node->type = decl->type;
    node->initializer = init;

    determine_array_size(decl, init);

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
//-   | '{' (<designation>? <initializer> (',' <designation>? <initializer>)* ','? '}'
// 当面structとunionは.field形式の<designation>必須とする
Initializer *struct_initializer(Type *type);
Initializer *union_initializer(Type *type);
Initializer *array_initializer(Type *type);

Initializer *initializer(Type *type) {
    Token *token;

    if ((token = consume(TK_LBRACE))) {
        switch (type->ty) {
            case ARRAY:
                return array_initializer(type);
            case STRUCT:
                return struct_initializer(type);
            case UNION:
                return union_initializer(type);
            default:
                error_at_token(token, "リスト形式で初期化できない型です");
        }
    }

    Initializer *ret = malloc(sizeof(Initializer));
    ret->ty = INITIALIZER_TYPE_EXPR;
    ret->expr = expr();

    if (is_scalar_type(type)) {
        return ret;
    }

    if (type->ty == ARRAY && type->ptrof->ty == CHAR && ret->expr->ty == ND_STRING) {
        return ret;
    }

    error_at_node(ret->expr, "式では初期化できない型です");
}

// TODO: [ <constant-expression> ] = 形式の<designation>対応
Initializer *array_initializer(Type *type) {
    Initializer *ret = malloc(sizeof(Initializer));

    ret->ty = INITIALIZER_TYPE_LIST;
    Vector *list = new_vector();

    for (;;) {
        if (consume(TK_RBRACE)) {
            break;
        }

        vec_push(list, initializer(type->ptrof));

        if (consume(TK_RBRACE)) {
            break;
        }

        if (!consume(TK_COMMA)) {
            error_at_here("','でも'}'でもないトークンです");
        }
    }
    ret->list = list;
    return ret;
}

Initializer *struct_initializer(Type *type) {
    Initializer *ret = malloc(sizeof(Initializer));

    ret->ty = INITIALIZER_TYPE_MAP;
    Map *map = new_map(); // フィールド名→Initializer
    ret->map = map;

    for (;;) {
        if (consume(TK_RBRACE)) {
            break;
        }

        if (!consume(TK_DOT)) {
            error_at_here("'.'がありません(フィールド名なしの構造体の初期化は未実装)");
        }

        Token *identifier;
        if (!(identifier = consume(TK_IDENT))) {
            error_at_here("フィールドの識別子がありません");
        }


        if (!consume(TK_ASSIGN)) {
            error_at_here("'='がありません");
        }

        if (map_get(map, identifier->name)) {
            error_at_token(identifier, "二重の初期化です");
        }

        Field *field = map_get(type->fields, identifier->name);

        if (field == NULL) {
            error_at_token(identifier, "存在しないフィールド名です");
        }

        map_put(map,identifier->name, initializer(field->type));

        if (consume(TK_RBRACE)) {
            break;
        }

        if (!consume(TK_COMMA)) {
            error_at_here("','でも'}'でもないトークンです");
        }
    }

    return ret;
}

Initializer *union_initializer(Type *type) {
    Initializer *ret = malloc(sizeof(Initializer));

    ret->ty = INITIALIZER_TYPE_MAP;
    Map *map = new_map(); // フィールド名→Initializer
    ret->map = map;


    if (consume(TK_RBRACE)) {
        return ret;
    }

    if (!consume(TK_DOT)) {
        error_at_here("'.'がありません(フィールド名なしの共用体の初期化は未実装)");
    }

    Token *identifier;
    if (!(identifier = consume(TK_IDENT))) {
        error_at_here("フィールドの識別子がありません");
    }

    if (!consume(TK_ASSIGN)) {
        error_at_here("'='がありません");
    }

    Field *field = map_get(type->fields, identifier->name);

    if (field == NULL) {
        error_at_token(identifier, "存在しないフィールド名です");
    }

    map_put(map, identifier->name, initializer(field->type));

    consume(TK_COMMA);

    if (!consume(TK_RBRACE)) {
        error_at_here("'}'でないトークンです");

    }

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
    if (consume(TK_ASSIGN)) {
        Initializer *init = initializer(decl->type);
        vec_push(vec, decl_init(decl, init));
    } else {
        vec_push(vec, decl_init(decl, NULL));
    }

    if (consume(TK_SEMI)) {
        return;
    }

    if (!consume(TK_COMMA)) {
        error_at_here("','でも';'でもないトークンです");
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
        error_at_here("intまたはcharでないトークンです(top_level)");
    }

    if (consume(TK_SEMI)) {
        return;
    }

    decl = declarator(type);

    if (next_token_is(TK_LBRACE)) {
        vec_push(top_levels, function_definition(decl));
        return;
    }

    Vector *decl_inits = new_vector();
    declaration_rest(decl->type, decl, decl_inits);
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
    struct_map = new_map();

    while (TOKEN(pos)->ty != TK_EOF) {
        top_level(top_levels);
    }

    Node *node = new_node(ND_PROGRAM, NULL);
    node->top_levels = top_levels;
    node->strings = strings;
    return node;
}

char *binopToStr(char *op, Node *node);

char *nodeVectorToStr(const Vector *vec, char *sep);

char *nodeToStr(Node *node) {
    switch (node->ty) {
        case ND_NUM:
            return strprintf("(NUM %d)", node->val);
        case ND_ADD:
            return binopToStr("+", node);
        case ND_SUB:
            return binopToStr("-", node);
        case ND_MUL:
            return binopToStr("*", node);
        case ND_DIV:
            return binopToStr("/", node);
        case ND_LT:
            return binopToStr("<", node);
        case ND_ASSIGN:
            return binopToStr("=", node);
        case ND_IDENT:
            return strprintf("(ID %s)", node->token->name);
        case ND_LOCAL_VAR:
            return strprintf("(LV %s %s)", typeToStr(node->type), node->token->name);
        case ND_GLOBAL_VAR:
            return strprintf("(GV %s %s)", typeToStr(node->type), node->token->name);
        case ND_CALL: {
            return strprintf("(CALL %s %s)", node->name, nodeVectorToStr(node->params, " "));
        }
        case ND_EQ:
            return binopToStr("==", node);
        case ND_NE:
            return binopToStr("!=", node);
        case ND_LE:
            return binopToStr("<=", node);
        case ND_STRING:
            return strprintf("\"%s\"", node->token->str);
        case ND_ARROW:
            return strprintf("(-> %s %s %s)", typeToStr(node->type), nodeToStr(node->term), node->field_name->name);
        case ND_DEREF:
            return strprintf("(DEREF %s %s)", typeToStr(node->type), nodeToStr(node->ptrto));
        case ND_GET_PTR:
            return strprintf("(GET_PTR %s %s)", typeToStr(node->type), nodeToStr(node->ptrof));
        case ND_EMPTY:
            return "(EMPTY)";
        case ND_RETURN:
            return strprintf("(RETURN %s)", nodeToStr(node->lhs));
        case ND_IF:
            return strprintf("(IF %s %s %s)",
                    nodeToStr(node->cond),
                    nodeToStr(node->stmt),
                    node->else_stmt ? nodeToStr(node->else_stmt) : ""
                    );
        case ND_WHILE:
            return strprintf("(WHILE %s %s)", nodeToStr(node->cond), nodeToStr(node->stmt));
        case ND_FOR:
            return strprintf("(FOR %s %s %s %s)",
                    node->init ? nodeToStr(node->init) : "()",
                    node->cond ? nodeToStr(node->cond) : "()",
                    node->next ? nodeToStr(node->next) : "()",
                    nodeToStr(node->stmt)
                    );
        case ND_EXPR:
            return strprintf("(EXPR %s)", nodeToStr(node->lhs));
        case ND_PROGRAM: {
            return nodeVectorToStr(node->top_levels, "\n# ");
        }
        case ND_FUNC:
            return strprintf("(FUNC %s %s)", node->name, nodeToStr(node->stmt));
        case ND_BLOCK: {
            return nodeVectorToStr(node->stmts, " ");
        }
        case ND_LOCAL_VAR_DEF:
            return "(ND_LOCAL_VAR_DEF)";
        case ND_GLOBAL_VAR_DEF:
            return "(ND_GLOBAL_VAR_DEF)";
        default:
            error("nodeToStr: not implemented: %d", node->ty);
    }
}

typedef struct StringBuf {
    char *buf;
    size_t len;
    size_t allocated;
} StringBuf;

StringBuf *new_string_buf() {
    StringBuf *ret = malloc(sizeof(StringBuf));
    ret->len = 0;
    ret->allocated = 16;
    ret->buf = malloc(ret->allocated);
    return ret;
}

void string_buf_add(StringBuf *buf, char *s) {
    size_t new_len = buf->len + strlen(s);

    if (buf->allocated < new_len + 1) {
        while (buf->allocated < new_len + 1) {
            buf->allocated *= 2;
        }
        buf->buf = realloc(buf->buf, buf->allocated);
    }

    strcpy(buf->buf + buf->len, s);
    buf->len = new_len;
}

char *binopToStr(char *op, Node *node) {
    return strprintf("(%s %s %s)", op, nodeToStr(node->lhs), nodeToStr(node->rhs));
}

char *nodeVectorToStr(const Vector *vec, char *sep) {
    StringBuf *buf = new_string_buf();
    for (int i = 0; i < vec->len; i++) {
        string_buf_add(buf, nodeToStr(vec->data[i]));
        string_buf_add(buf, sep);
    }
    return buf->buf;
}

