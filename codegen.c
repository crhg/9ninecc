#include <stdio.h>
#include "9ninecc.h"

// ラベル番号
int label_seq = 0;


// スタック位置
int stack_ptr = 0;

// スタックにsizeバイトをpushしたことを記録する
void stack_push(int size) {
    stack_ptr += size;
}

// スタックにsizeバイトをpopしたことを記録する
void stack_pop(int size) {
    stack_ptr -= size;
}

// スタック調整
// 関数呼び出し前にスタックが16バイト境界になるように調整する
// callが戻り番地を8バイト積むのでその分も考慮
// 調整量を返す
// param_stack_sizeはスタック渡しするパラメタのサイズ合計
int adjust_stack(int param_stack_size) {
    int adjust = 16 - (stack_ptr + param_stack_size + 8) % 16;
    if (adjust != 0) {
        printf("  sub rsp, %d\n", adjust);
        stack_push(adjust);
    }
    return adjust;;
}

// 関数呼び出し後のスタックの回復
// sizeはスタック調整量+スタック渡しのパラメタのサイズ合計
void restore_stack(int size) {
    if (size != 0) {
        printf("  add rsp, %d\n", size);
        stack_pop(size);
    }
}

typedef enum Reg { RAX, RDI, RSI, RDX, RCX, RBP, RSP, RBX, R8, R9, R10, R11, R12, R13, R14, R15 } Reg;
typedef enum RegSize { SIZE64, SIZE32, SIZE16, SIZE8 } RegSize;

char *reg_table[][4] = {
    {"rax", "eax", "ax", "al"},
    {"rdi", "edi", "di", "dil"},
    {"rsi", "esi", "si", "sil"},
    {"rdx", "edx", "dx", "dl"},
    {"rcx", "ecx", "cx", "cl"},
    {"rbp", "ebp", "bp", "bpl"},
    {"rsp", "esp", "sp", "spl"},
    {"rbx", "ebx", "bx", "bl"},
    {"r8", "r8d", "r8w", "r8b"}, 
    {"r9", "r9d", "r9w", "r9b"}, 
    {"r10", "r10d", "r10w", "r10b"}, 
    {"r11", "r11d", "r11w", "r11b"}, 
    {"r12", "r12d", "r12w", "r12b"}, 
    {"r13", "r13d", "r13w", "r13b"}, 
    {"r14", "r14d", "r14w", "r14b"}, 
    {"r15", "r15d", "r15w", "r15b"}, 
};

// レジスタ名の選択
char * select_reg(int ty, Reg reg) {
    RegSize reg_size;
    switch (ty) {
        case PTR:
            reg_size = SIZE64;
            break;
        case INT:
            reg_size = SIZE32;
            break;
        default:
            error("不明な型: %d", ty);
    }

    return reg_table[reg][reg_size];
}

// 左辺値のコード生成
// アドレスをスタックトップにプッシュする
// 型を返す
int  gen_lval(Node *node) {
    if (node->ty != ND_LOCAL_VAR)
        error_at_node(node, "代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->local_var->offset);
    printf("  push rax\n");
    stack_push(8);

    return node->local_var->type->ty;
}

// コード生成
void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        stack_push(8);
        return;
    }

    if (node->ty == ND_LOCAL_VAR) {
        // 変数の読み出し
        // アドレスを求めて間接参照で読み出す
        gen_lval(node);
        printf("  pop rax\n");
        switch (node->local_var->type->ty) {
            case INT:
                printf("  mov eax, [rax]\n");
                break;
            case PTR:
                printf("  mov rax, [rax]\n");
                break;
            default:
                error_at_node(node, "unknown type(codegen): %d", node->local_var->type->ty);
        }
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_LOCAL_VAR_DEF) {
        // 変数定義
        // 今のところ何もしない
        return;
    }

    if (node->ty == ND_PTR) {
        printf("# ND_PTR\n");
        gen(node->ptrto);
        printf("# ND_PTR: ptrto compiled\n");
        printf("  pop rax\n");
        if (node->type == NULL) {
            error_at_node(node, "ND_PTR: type is NULL\n");
        }

        switch (node->type->ty) {
            case INT:
                printf("  mov eax, [rax]\n");
                break;
            case PTR:
                printf("  mov rax, [rax]\n");
                break;
            default:
                error_at_node(node, "unknown type at ND_PTR: %d\n", node->type->ty);
        }
        printf("  push rax\n");

        // popしてpushなのでスタック増減はない
        return;
    }

    if (node->ty == ND_PTR_OF) {
        gen_lval(node->ptrof);
        return;
    }

    if (node->ty == ND_CALL) {
        // 関数呼び出し

        // スタック渡しするパラメタのサイズを計算
        int param_stack_size = (node->params->len > 6)? (node->params->len - 6) * 8: 0;

        // スタックのアラインメント調整
        int adjusted = adjust_stack(param_stack_size);

        // パラメタを計算(右から左)
        for (int i = node->params->len - 1; i >= 0; i--) {
            gen(node->params->data[i]);
        }

        // レジスタ渡しするパラメタの処理
        if (node->params->len >= 1) {
            printf("  pop rdi\n");
            stack_pop(8);
        }
        if (node->params->len >= 2) {
            printf("  pop rsi\n");
            stack_pop(8);
        }
        if (node->params->len >= 3) {
            printf("  pop rdx\n");
            stack_pop(8);
        }
        if (node->params->len >= 4) {
            printf("  pop rcx\n");
            stack_pop(8);
        }
        if (node->params->len >= 5) {
            printf("  pop r8\n");
            stack_pop(8);
        }
        if (node->params->len >= 6) {
            printf("  pop r9\n");
            stack_pop(8);
        }

        // 呼び出し
        printf("  call %s\n", node->name);

        // スタック回復
        restore_stack(adjusted + param_stack_size);

        // 戻り値をスタックに積む
        printf("  push rax\n");
        stack_push(8);
        return;
    }

    if (node->ty == '=') {
        // 代入
        int ty = gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");

        switch (ty) {
            case INT:
                printf("  mov [rax], edi\n");
                break;
            case PTR:
                printf("  mov [rax], rdi\n");
                break;
            default:
                error("不明な型: %d", ty);
        }

        printf("  push rdi\n"); // 全体の値は右辺の計算結果
        stack_pop(8);
        return;
    }

    if (node->ty == ND_EXPR) {
        stack_ptr = 0;
        gen(node->lhs);
        printf("  pop rax\n");
        stack_pop(8);
        return;
    }

    if (node->ty == ND_RETURN) {
        stack_ptr = 0;
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    if (node->ty == ND_IF) {
        stack_ptr = 0;
        printf("# if!!\n");
        gen(node->cond);
        int seq = label_seq++;

        if (node->else_stmt == NULL) {
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", seq);
            gen(node->stmt);
            printf(".Lend%d:\n", seq);
        } else {
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lelse%d\n", seq);
            gen(node->stmt);
            printf("  jmp .Lend%d\n", seq);
            printf(".Lelse%d:\n", seq);
            gen(node->else_stmt);
            printf(".Lend%d:\n", seq);
        }
        return;
    }

    if (node->ty == ND_WHILE) {
        printf("# while!!\n");
        int seq = label_seq++;

        printf(".Lbegin%d:\n", seq);
        stack_ptr = 0;
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", seq);
        gen(node->stmt);
        printf("  jmp .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);

        return;
    }

    if (node->ty == ND_FOR) {
        int seq = label_seq++;

        if (node->init != NULL) {
            stack_ptr = 0;
            gen(node->init);
            printf("  pop rax\n");
        }
        printf(".Lbegin%d:\n", seq);
        if (node->cond != NULL) {
            stack_ptr = 0;
            gen(node->cond);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", seq);
        }
        gen(node->stmt);
        if (node->next != NULL) {
            stack_ptr = 0;
            gen(node->next);
            printf("  pop rax\n");
        }
        printf("  jmp .Lbegin%d\n", seq);
        printf(".Lend%d:\n", seq);

        return;
    }

    if (node->ty == ND_BLOCK) {
        for(int i = 0; i < node->stmts->len; i++) {
            gen(node->stmts->data[i]);
        }
        return;
    }

    if (node->ty == ND_FUNC) {
        printf(".global %s\n", node->name);
        printf("%s:\n", node->name);

        // プロローグ
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, %d\n", node->local_var_map->keys->len * 8);

        // rbp + 0: 以前のrbp
        // rbp + 8: 戻り番地
        // rbp + 16: 第7引数
        // rbp + 24: 第8引数
        // ...

        // 引数の値をローカル変数にコピーする
        for (int i = 0; i < node->params->len; i++) {
            // XXX: とりあえずr10は使って良さそうなので使ってみたが...
            LocalVar *param = ((Node *)node->params->data[i])->local_var;
            int ty = param->type->ty;
            printf("  mov r10, rbp\n");
            printf("  sub r10, %d\n", param->offset);
            Reg reg;
            if (i == 0) {
                reg = RDI;
            } else if (i == 1) {
                reg = RSI;
            } else if (i == 2) {
                reg = RDX;
            } else if (i == 3) {
                reg = RCX;
            } else if (i == 4) {
                reg = R8;
            } else if (i == 5) {
                reg = R9;
            } else {
                printf("  mov rax, rbp\n");
                printf("  add rax, %d\n", (i-6) * 8 + 16);
                printf("  mov rax, [rax]\n");
                reg = RAX;
            }
            printf("  mov [r10], %s\n", select_reg(ty, reg));
        }

        gen(node->stmt);

        // エピローグ
        // 最後の式の結果はraxに残っているのでそれが返り値になる
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    // 以下2項演算子
    if (node->lhs == NULL || node->rhs == NULL) {
        error("たぶん2項演算子ではないノード: %d", node->ty);
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
    case '*':
        printf("  imul rdi\n");
        break;
    case '/':
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case '<':
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    default:
        error("知らないノード種別: %d\n", node->ty);
        break;
    }

    printf("  push rax\n");
    stack_pop(8);
}
