// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_EOF,       // 入力の終わり
    TK_EQ,        // ==
    TK_NE,        // !=
    TK_LE,        // <=
    TK_GE,        // >=
};

// トークンの型
typedef struct {
    int ty;      // トークンの型
    int val;     // tyがTK_NUMの場合、その数値
    char *input; //トークン文字列(エラーメッセージ用)
} Token;

// ノードの型を表す値
enum {
    ND_NUM = 256, // 整数のノードの型
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LE,        // <=
};

// ノードの型
typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val; // tyがND_NUMの場合のみ使う
} Node;

// 2項演算子のノードを作る
Node *new_node(int ty, Node *lhs, Node *rhs);

// 数値のノードを作る
Node *new_node_num(int val);

// 入力プログラム
extern char *user_input;

// トーク結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
extern Token tokens[];
extern int pos;

// 次のトークンが期待した値かチェックして
// 期待した値の場合だけ入力を1トークン読み進めてそのトークンを返す
// それ以外はNULLを返す
Token *consume(int ty);

// エラーを報告するための関数
// printfと同じ引数を取る
_Noreturn void error(char *fmt, ...);

// エラー箇所を報告するための関数
_Noreturn void error_at(char *loc, char *msg);

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize();

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

// コード生成
void gen(Node *node);
