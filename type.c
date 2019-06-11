#include <stdio.h>
#include <stdlib.h>
#include "9ninecc.h"

Type char_type = { .ty = CHAR, .size = 1, .alignment = 1, .incomplete = 0, };
Type int_type = { .ty = INT, .size = 4, .alignment = 4, .incomplete = 0, };

// ポインタの型
Type *pointer_of(Type *type) {
    Type *ret = malloc(sizeof(Type));
    ret->ty = PTR;
    ret->size = 8;
    ret->alignment = 8;
    ret->incomplete = 0;
    ret->ptrof = type;
    return ret;
}

// 配列の型
Type *array_of(Type *type, int len, int incomplete) {
    Type *ret = malloc(sizeof(Type));
    ret->ty = ARRAY;
    ret->ptrof = type;
    ret->len = len;
    ret->incomplete = incomplete;
    ret->alignment = type->alignment;
    ret->size = incomplete? 0: len * type->size;
    return ret;
}

// 関数の型
Type *function_of(Type *return_type, Vector *params) {
    Type *ret = malloc(sizeof(Type));
    ret->ty = FUNC;
    ret->return_type = return_type;
    ret->params = params;
    return ret;
}


// 型が等しいかどうか調べる
int type_eq(Type *x, Type *y) {
    if (x == y) {
        return 1;
    }

    if (x->ty != y->ty) {
        return 0;
    }

    if (x->ty == PTR) {
        return (type_eq(x->ptrof, y->ptrof));
    }

    return 1;
}

// 型のバイト数
int get_size_of(Type *type) {
    switch (type->ty) {
        case CHAR:
            return 1;
        case INT:
            return 4;
        case PTR:
            return 8;
        case ARRAY:
            return get_size_of(type->ptrof) * type->len;
        case STRUCT:
            return type->size;
        default:
            error("unknown type(get_size_of): %d", type->ty);
    }
}

// 型のアラインメント
int get_alignment(Type *type) {
    switch (type->ty) {
        case CHAR:
            return 1;
        case INT:
            return 4;
        case PTR:
            return 8;
        case ARRAY:
            return get_alignment(type->ptrof);
        case STRUCT:
            return type->alignment;
        default:
            error("unknown type(get_alignment): %d", type->ty);
    }
}

// tyの文字列表記
char *tyToStr(TypeId ty) {
    switch (ty) {
        case CHAR:
            return "char";
        case INT:
            return "int";
        case PTR:
            return "ptr";
        case ARRAY:
            return "array";
        case STRUCT:
            return "struct";
        default:
            error("unknown type(tyToStr): %d", ty);
    }
}
// 型の文字列表記
char *typeToStr(Type *type) {
    switch (type->ty) {
        case PTR:
            return strprintf("ptr of %s", typeToStr(type->ptrof));
        case ARRAY:
            if (type->incomplete) {
                return strprintf("array[] of %s", typeToStr(type->ptrof));
            } else {
                return strprintf("array[%d] of %s", type->len, typeToStr(type->ptrof));
            }
        default:
            return tyToStr(type->ty);
    }
}

Type *assign_type_to_lval(Node *node);

// 式に型をつける
Type *assign_type_to_expr(Node *node) {
    Type *type;
    Type *lval_type;
    Type *ltype;
    Type *rtype;

    // 左辺値として型がつけられたらそのまま返す
    lval_type = assign_type_to_lval(node);
    if (lval_type) { return lval_type; }

    // あとは右辺値
    switch (node->ty) {
        case ND_CALL:
            // TODO: 関数の型 いまのところ決め打ちでint
            node->type = &int_type;
            for (int i = 0; i < node->params->len; i++) {
                assign_type_to_expr((Node *)node->params->data[i]);
                // TODO: パラメタの型チェック
            }
            return &int_type;
        case ND_GET_PTR:
            type = assign_type_to_lval(node->ptrof);
            return node->type = pointer_of(type);
        case '=':
            lval_type = assign_type_to_lval(node->lhs);
            if (lval_type == NULL) {
                // TODO: エラー発生位置を表示したい
                error_at_node(node, "代入先が左辺値でありません");
            }
            type = assign_type_to_expr(node->rhs);
            return type;
        case '+':
            // ポインタ+整数はポインタ型になる
            ltype = assign_type_to_expr(node->lhs);
            rtype = assign_type_to_expr(node->rhs);
            if (ltype->ty == PTR && rtype->ty == INT) {
                return ltype;
            }
            if (ltype->ty == INT && rtype->ty == PTR) {
                // コード生成の都合のためrhsが整数,lhsがポインタになるように交換する
                Node *tmp;
                tmp = node->lhs;
                node->lhs = node->rhs;
                node->rhs = tmp;
                return rtype;
            }
            if (rtype->ty == PTR && ltype->ty == PTR) {
                error_at_node(node, "ポインタ同士の加算はできません");
            }
            return node->type = &int_type;
        case '-':
            // ポインタ-整数はポインタ型になる
            ltype = assign_type_to_expr(node->lhs);
            rtype = assign_type_to_expr(node->rhs);
            if (ltype->ty == PTR && rtype->ty == INT) {
                return ltype;
            }
            if (ltype->ty == INT && rtype->ty == PTR) {
                error_at_node(node, "整数からポインタは引けません");
            }
            if (ltype->ty == PTR && rtype->ty == PTR && !type_eq(ltype, rtype)) {
                error_at_node(node, "異なる型のポインタの減算はできません");
            }
                
            return node->type = &int_type;
        case '*':
        case '/':
        case ND_EQ:
        case ND_NE:
        case '<':
        case ND_LE:
            assign_type_to_expr(node->lhs);
            assign_type_to_expr(node->rhs);
            return node->type = &int_type;
        case ND_NUM:
            return node->type = &int_type;
        default:
            error_at_node(node, "unexpected node: %d", node->ty);
    }
}

// 左辺値に型をつける
// 左辺値でなければNULL
Type *assign_type_to_lval(Node *node) {
    Type *type;
    switch (node->ty) {
        case ND_LOCAL_VAR:
            return node->type = node->local_var->type;
        case ND_DEREF:
            /* print_node_pos(node); */
            /* fprintf(stderr, "assign type ND_DEREF\n"); */
            type = assign_type_to_expr(node->ptrto);
            if (type->ty != PTR) {
                error_at_node(node->ptrto, "ポインタ型ではありません");
            }
            if (type->ptrof == NULL) {
                error_at_node(node->ptrto, "不明なポインタ型");
            }
            return node->type = type->ptrof;
        default:
            return NULL;
    }
}

