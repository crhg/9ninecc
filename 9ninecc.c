#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
    TK_NUM = 256, // 整数トークン
    TK_EOF,       // 入力の終わり
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
};

// ノードの型
typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val; // tyがND_NUMの場合のみ使う
} Node;

// 2項演算子のノードを作る
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

// 入力プログラム
char *user_input;

// トーク結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];


int pos = 0;

// 次のトークンが期待した値かチェックして
// 期待した値の場合だけ入力を1トークン読み進めてそのトークンを返す
// それ以外はNULLを返す
Token *consume(int ty) {
    if (tokens[pos].ty != ty)
        return NULL;

    return &tokens[pos++];
}

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *msg) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}

// user_inputが指している文字列を
// トークンに分割してtokensに保存する
void tokenize() {
    char *p = user_input;

    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

// 式のパーサ
Node *expr() {
    Token *num;
    if ((num = consume(TK_NUM)) == NULL)
        error_at(tokens[pos].input, "数でありません");

    Node *node = new_node_num(num->val);

    for (;;) {
        if (consume('+'))
            node = new_node('+', node, expr());
        else if (consume('-'))
            node = new_node('-', node, expr());
        else
            return node;
    }
}

// コード生成
void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    default:
        error("知らないノード種別: %d\n", node->ty);
        break;
    }

    printf("  push rax\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    tokenize();
    Node *node = expr();

    if (!consume(TK_EOF))
        error_at(tokens[pos].input, "EOFでありません");

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // コード生成
    gen(node);

    // スタックトップに残っている式全体の値を
    // RAXにロードして関数からの返値にする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
