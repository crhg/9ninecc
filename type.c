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

