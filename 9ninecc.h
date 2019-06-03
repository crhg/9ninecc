#include <stdarg.h>

// 可変長ベクタ
typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

// マップ
typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_IDENT,     // 識別子
    TK_EOF,       // 入力の終わり
    TK_RETURN,    // return
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_INT,
    TK_SIZEOF,
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
};

// トークンの型
typedef struct {
    int ty;      // トークンの型
    int val;     // tyがTK_NUMの場合、その数値
    char *name;  // tyがTK_IDENTの場合、その名前
    char *input; //トークン文字列(エラーメッセージ用)
} Token;

// ID種別
typedef enum IdType {
    ID_LOCAL_VAR,
} IdType;

// 型
typedef struct Type {
    enum { INT, PTR, ARRAY } ty;
    struct Type *ptrof;
    int array_size;
} Type;
extern Type int_type;
Type *pointer_of(Type *type);
Type *array_of(Type *type, int size);
int type_eq(Type *x, Type *y);
int get_size_of(Type *type);
int get_alignment(Type *type);

// ローカル変数
typedef struct LocalVar {
    Type *type;
    int offset;
} LocalVar;

// ノードの型を表す値
typedef enum NodeType {
    ND_NUM = 256, // 整数のノードの型
    ND_IDENT,     // 識別子
    ND_LOCAL_VAR,
    ND_GLOBAL_VAR,
    ND_PTR,
    ND_PTR_OF,
    ND_RETURN,    // return
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_EXPR,      // <式>; 文
    ND_BLOCK,
    ND_CALL,      // 関数呼び出し
    ND_FUNC,      // 関数定義
    ND_LOCAL_VAR_DEF,
    ND_GLOBAL_VAR_DEF,
    ND_PROGRAM,
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
} NodeType;

// ノードの型
typedef struct Node {
    int ty;
    Token *token;      // 位置表示のためのトークン
    struct Node *lhs;
    struct Node *rhs;
    struct Node *cond; // ND_IF, ND_WHILE, ND_FORのときの条件式
    struct Node *stmt; // ND_IF, ND_WHILE, ND_FORのときの実行文
    struct Node *else_stmt; // ND_IFのときのelse文
    struct Node *init; // ND_FORのときの１つ目の式
    struct Node *next; // ND_FORのときの３つ目の式
    Vector *stmts;     // ND_BLOCKの時のstmtのベクタ
    int val;           // tyがND_NUMの場合のみ使う
    LocalVar *local_var;
    char *name;        // tyがND_CALLの時に使う識別子名
    Vector *params;    // tyがND_CALLの時に使うパラメタの式のベクタ
    Map *local_var_map; // 関数定義のローカル変数マップ
    struct Node *ptrto;       // tyがND_PTRの時に使う
    struct Node *ptrof;       // tyがND_PTR_OFの時に使う
    Type *type;          // 式であるときその値の型(暗黙の配列ポインタ変換で変化する)
    Type *variable_type; // 変数参照のとき変数自体の型(変わらない)
    Vector *top_levels; // ND_PROGRAMのときのトップレベルのベクタ
} Node;

// 入力プログラム
extern char *user_input;

// トークン列を保存するベクター
extern Vector *token_vector;

// 指定された位置のトークンを返すマクロ
#define TOKEN(pos) ((Token *)(token_vector->data[pos]))

// トークンの読み出し位置
extern int pos;

// 次のトークンが期待した値かチェックして
// 期待した値の場合だけ入力を1トークン読み進めてそのトークンを返す
// それ以外はNULLを返す
Token *consume(int ty);

int next_token_is(int ty);

// エラーを報告するための関数
// printfと同じ引数を取る
_Noreturn void error(char *fmt, ...);
_Noreturn void verror(char *fmt, va_list args);

// エラー箇所を報告するための関数
_Noreturn void error_at(char *loc, char *fmt, ...);
_Noreturn void verror_at(char *loc, char *fmt, va_list args);
_Noreturn void error_at_token(Token *token, char *fmt, ...);
_Noreturn void verror_at_token(Token *token, char *fmt, va_list args);
_Noreturn void error_at_node(Node *node, char *fmt, ...);
void assert_at_node(Node *node, int cond, char *fmt, ...);

void print_loc(char *loc);
void print_token_pos(Token *token);
void print_node_pos(Node *node);

// user_inputが指している文字列を
// トークンに分割して保存する
void tokenize();

// 式に型をつける
Type *assign_type_to_expr(Node *node);

// パーサ
Node *program();
extern Map *local_var_map;
#define LOCAL_VAR(name) ((LocalVar *)map_get(local_var_map, name))
#define LOCAL_VAR_AT(i) ((LocalVar *)map->vals->data[i])
#define LOCAL_VAR_NUM (map->keys->len)
int get_local_var_offset(char *name);
int new_local_var_offset(char *name);
int get_size_of(Type *type);

// コード生成
void gen(Node *node);

// その他
int is_alnum(char c);

// テスト
void runtest();
