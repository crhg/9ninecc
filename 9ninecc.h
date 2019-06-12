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
typedef enum TokenId {
    // 1文字記号は値を文字コードで合わせる
    TK_AMP = '&',
    TK_LPAR = '(',
    TK_RPAR = ')',
    TK_AST = '*',
    TK_PLUS = '+',
    TK_COMMA = ',',
    TK_MINUS = '-',
    TK_DOT = '.',
    TK_SLASH = '/',
    TK_SEMI = ';',
    TK_LT = '<',
    TK_ASSIGN = '=',
    TK_GT = '>',
    TK_LBRACKET = '[',
    TK_RBRACKET = ']',
    TK_LBRACE = '{',
    TK_RBRACE = '}',

    TK_EOF = 256,       // 入力の終わり

    // 2文字記号
    TK_NE, // !=
    TK_ARROW, // ->
    TK_LE, // <=
    TK_EQ, // ==
    TK_GE, // >=

    // 予約語
    TK_RETURN,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_CHAR,
    TK_INT,
    TK_SIZEOF,
    TK_STRUCT,

    // それ以外
    TK_NUM,       // 整数トークン
    TK_IDENT,     // 識別子
    TK_STRING,    // 文字列リテラル
} TokenId;

// トークンの型
typedef struct {
    TokenId ty;      // トークンの型
    int val;     // tyがTK_NUMの場合、その数値
    char *name;  // tyがTK_IDENTの場合、その名前
    char *str;   // tyがTK_STRINGの場合、その文字列

    char *input; //トークン文字列(エラーメッセージ用)
} Token;

// ID種別
typedef enum IdType {
    ID_LOCAL_VAR,
} IdType;

// 型
typedef enum TypeId {CHAR, INT, PTR, ARRAY, FUNC, STRUCT} TypeId;
typedef struct Type {
    TypeId ty;

    int size;
    int alignment;

    char incomplete; // 型が未確定であることを示すフラグ
                     // 数値型やポインタ型は常に偽(確定している)
                     // 配列: 長さが決まっていない, 構造体: 名前のみ決まっている
                     // 関数型については意味が無い(多分)

    struct Type *ptrof; // PTRとARRAYのとき

    int len; // ARRAYのときの長さ(要素数)


    struct Type *return_type; // FUNCのとき戻り値の型を示す
    Vector *params; // FUNCのときパラメタのベクター。要素はDeclarator

    Map *fields;     // STRUCTのときフィールドのマップ,フィールド名→(Field *)
    int next_offset; // STRUCTのとき次のオフセット

    Token *token; // 宣言のときのみエラー表示用にトークンを保持する
} Type;

// 構造体のフィールド情報
typedef struct Field {
    Type *type;
    int offset;
} Field;

extern Type int_type;
extern Type char_type;
Type *pointer_of(Type *type);
Type *array_of(Type *type, int len, int incomplete);
Type *function_of(Type *type, Vector *params);
int type_eq(Type *x, Type *y);
int get_size_of(Type *type);
int get_alignment(Type *type);
char *tyToStr(TypeId ty);
char *typeToStr(Type *type);

// ローカル変数
typedef struct LocalVar {
    Type *type;
    int offset;
} LocalVar;

// ノードの型を表す値
typedef enum NodeType {
    ND_ADD = '+',
    ND_SUB = '-',
    ND_MUL = '*',
    ND_DIV = '/',
    ND_LT = '<',
    ND_ASSIGN = '=',

    // 式
    ND_NUM = 256,   // 整数

    ND_IDENT,       // 識別子
    ND_LOCAL_VAR,
    ND_GLOBAL_VAR,

    ND_CALL,        // 関数呼び出し
    ND_EQ,          // ==
    ND_NE,          // !=
    ND_LE,          // <=
    ND_STRING,      // 文字列リテラル

    ND_ARROW,       // ->

    ND_DEREF,       // *x
    ND_GET_PTR,     // &x

    // 文
    ND_EMPTY,       // 空文: ;
    ND_RETURN,      // return
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_EXPR,        // 式文: <式>;
    ND_BLOCK,

    ND_FUNC,        // 関数定義
    ND_LOCAL_VAR_DEF,
    ND_GLOBAL_VAR_DEF,
    ND_PROGRAM,
} NodeType;


struct Initializer;

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
    struct Node *ptrto;       // tyがND_DEREFの時に使う
    struct Node *ptrof;       // tyがND_GET_PTRの時に使う
    Type *type;          // 式であるときその値の型(暗黙の配列ポインタ変換で変化する)
    Type *variable_type; // 変数参照のとき変数自体の型(変わらない)
    Vector *top_levels; // ND_PROGRAMのときのトップレベルのベクタ
    Vector *strings;    // ND_PROGRAMのときのストリングリテラルのベクタ
    int str_index;      // ND_STRINGのときの文字列リテラルの通し番号
    struct Initializer *initializer; // 変数定義のときの初期化子
    Vector *decl_inits; // ND_LOCAL_VAR_DEFのときのDeclaratorのベクタ
    Vector *local_vars;   // ND_LOCAL_VAR_DEFのときのLocalVarのベクタ

    struct Node *term;  // ND_ARROWのときにtermが入る
    Field *field;       // ND_ARROWのときに(Field *)が入る
    Token *field_name;  // ND_ARROWのときに識別子が入る
} Node;

typedef struct Declarator {
    Type *type;
    Token *id;
    LocalVar *local_var;
} Declarator;

typedef enum InitializerType {
    INITIALIZER_TYPE_EXPR,
    INITIALIZER_TYPE_LIST,
} InitializerType;

typedef struct Initializer {
    InitializerType ty;
    Node *expr;
    Vector *list;
} Initializer;

typedef struct DeclInit {
    Declarator *decl;
    Initializer *init;
} DeclInit;

// 入力プログラムのファイル名
extern char *filename;

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
void warn(char *fmt, ...);

// エラー箇所を報告するための関数
_Noreturn void error_at(char *loc, char *fmt, ...);
void warn_at(char *loc, char *fmt, ...);
_Noreturn void error_at_here(char *fmt, ...);
void warn_at_here(char *fmt, ...);
_Noreturn void error_at_token(Token *token, char *fmt, ...);
void warn_at_token(Token *token, char *fmt, ...);
_Noreturn void error_at_node(Node *node, char *fmt, ...);
void warn_at_node(Node *node, char *fmt, ...);
void assert_at_node(Node *node, int cond, char *fmt, ...);

void print_loc(char *loc);
void print_token_pos(Token *token);
void print_node_pos(Node *node);

// ファイルを読み込む
char *read_file(char *path);

// 文字列領域を確保するsprintf
char *strprintf(char *fmt, ...);

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

// コード生成
void gen(Node *node);

// その他
int is_alpha(char c);
int is_alnum(char c);
int round_up(int n, int m);
int max(int n, int m);

// テスト
void runtest();
