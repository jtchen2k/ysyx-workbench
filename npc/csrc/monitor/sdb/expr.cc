// /*
//  * expr.cc
//  *
//  * @project: ysyx
//  * @author: Juntong Chen (dev@jtchen.io)
//  * @created: 2025-04-08 19:14:28
//  * @modified: 2025-04-08 19:14:29
//  *
//  * Copyright (c) 2025 Juntong Chen. All rights reserved.
//  */
#include "common.h"
#include "macro.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"
#include <regex.h>
#include <unordered_map>

enum TokenType {
    TK_NOTYPE = 256,
    TK_NEGATIVE,
    TK_POSITIVE,
    TK_EQ,
    TK_NEQ,
    TK_AND,
    TK_DECIMAL,
    TK_HEX,
    TK_REG,
    TK_DEREF,
    TK_GE,
    TK_LE,
    TK_GT,
    TK_LT,
};

static struct rule {
    const char *regex;
    int         token_type;
} rules[] = {
    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */
    {" +", TK_NOTYPE}, // spaces
    {"0x[0-9a-fA-F]+", TK_HEX},
    {"\\$[a-zA-Z0-9\\$]+", TK_REG},
    {"[0-9]+u?", TK_DECIMAL},
    {"\\(", '('},
    {"\\)", ')'},
    {"\\*", '*'}, // mult
    {"/", '/'},   // divide
    {"\\+", '+'}, // plus
    {"-", '-'},   // minus
    {"&&", TK_AND},
    {"!=", TK_NEQ},
    {"==", TK_EQ}, // equal
    {"<=", TK_LE},
    {">=", TK_GE},
    {"<", '<'},
    {">", '>'},
    {"!", '!'},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

void regex_init() {
    int  i;
    char error_msg[128];
    int  ret;

    for (i = 0; i < NR_REGEX; i++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            Panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
        }
    }
}

typedef struct token {
    int  type;
    char str[EXPR_TOKEN_LENGTH];
} Token;

static Token tokens[EXPR_TOKEN_SIZE] __attribute__((used)) = {};
static int   nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
    int        position = 0;
    int        i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
                char *substr_start = e + position;
                int   substr_len = pmatch.rm_eo;

                if (substr_len >= 31) {
                    Log("token length exceeds buffer size.");
                    return false;
                }

                if (nr_token >= EXPR_TOKEN_SIZE) {
                    Log("too many tokens");
                    return false;
                }

                LogTrace("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
                         rules[i].regex, position, substr_len, substr_len, substr_start);

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */
                switch (rules[i].token_type) {
                case TK_DECIMAL:
                case TK_HEX:
                    tokens[nr_token].type = rules[i].token_type;
                    strncpy(tokens[nr_token].str, substr_start, substr_len);
                    tokens[nr_token].str[substr_len] = '\0';
                    break;
                case TK_REG:
                    tokens[nr_token].type = rules[i].token_type;
                    // skip $
                    strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
                    tokens[nr_token].str[substr_len - 1] = '\0';
                    break;
                default:
                    tokens[nr_token].type = rules[i].token_type;
                }

                nr_token++;
                break;
            }
        }

        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }

    /* distinguish minus vs neg, mult vs deref */
    int last_type = TK_NOTYPE;

    // if last_type is one of the unary_prevs, then the current token is a unary
    int unary_prevs[] = {
        '+',      '-',         '*',         '/',   '(',   TK_EQ, TK_NEQ, TK_AND,
        TK_DEREF, TK_NEGATIVE, TK_POSITIVE, TK_GT, TK_LE, '>',   '<',    '!',
        TK_NOTYPE // first token
    };
    int unary_ops_map[][2] = {
        {'-', TK_NEGATIVE},
        {'+', TK_POSITIVE},
        {'*', TK_DEREF},
    };
    for (int i = 0; i < nr_token; i++) {
        Token *t = &tokens[i];
        for (int o = 0; o < ARRLEN(unary_ops_map); o++) {
            int op_type = unary_ops_map[o][0];
            int unary_type = unary_ops_map[o][1];
            if (t->type == op_type) {
                if (i == 0) {
                    t->type = unary_type;
                } else {
                    for (int j = 0; j < ARRLEN(unary_prevs); j++) {
                        if (last_type == unary_prevs[j]) {
                            t->type = unary_type;
                            break;
                        }
                    }
                }
            }
            if (t->type == unary_type)
                LogTrace("detected unary operator %c at position %d, after %c", op_type, i,
                         last_type);
        }

        if (t->type != TK_NOTYPE)
            last_type = t->type;
    }

    return true;
}

static void format_token(Token *t, char *buf) {
    switch (t->type) {
    case TK_DECIMAL:
        sprintf(buf, "%s ", t->str);
        break;
    case TK_NOTYPE:
        sprintf(buf, " ");
        break;
    case TK_EQ:
        sprintf(buf, "== ");
        break;
    default:
        sprintf(buf, "%c ", t->type);
    }
}

static void print_tokens(int p, int q) {
    for (int i = p; i <= q; i++) {
        char buf[32];
        format_token(&tokens[i], buf);
        printf("%s", buf);
    }
    putchar('\n');
}

static bool check_parentheses(int p, int q) {
    Assert(p < q && p >= 0 && q < nr_token, "bad check_parentheses call");
    char stk[EXPR_TOKEN_SIZE / 2] = {};
    int  top = 0;
    bool ret = false;
    for (int i = p; i <= q; i++) {
        if (tokens[i].type == '(') {
            if (i == p)
                ret = true;
            stk[top++] = '(';
        } else if (tokens[i].type == ')') {
            if (top == 0 || stk[--top] != '(') {
                printf("bad expression: unmatched parentheses.\n");
                return false;
            }
            if (i != q && top == 0)
                ret = false;
        }
    }
    // printf("check_parentheses(%d, %d) = %d\n\t", p, q, ret);
    // print_tokens(p, q);
    return ret;
}

static word_t eval(int p, int q, bool *success) {
    if (p > q || p < 0 || q < 0 || p >= nr_token || q >= nr_token) {
        *success = false;
        printf("unexpected eval state (%d - %d).\n", p, q);
        return 0;
    }

    /**
     * Remove leading and trailing spaces;
     */
    while (tokens[p].type == TK_NOTYPE)
        p++;
    while (tokens[q].type == TK_NOTYPE)
        q--;

    if (p > q) {
        /* Bad expression */
        *success = false;
        printf("unexpected eval state (%d - %d).\n", p, q);
        return 0;
    } else if (p == q) {
        /* Single token.
         * For now this token should be a decimal / hex / register.
         * Return the value of the number.
         */
        switch (tokens[p].type) {
        case TK_DECIMAL:
            return atoi(tokens[p].str);
        case TK_HEX:
            return strtol(tokens[p].str + 2, NULL, 16);
        case TK_REG:
            return R(tokens[p].str, success);
        default:
            *success = false;
            printf("invalid expression: unexpected token. type: %d\n", tokens[p].type);
            return 0;
        }
    } else if (check_parentheses(p, q)) {
        /* The expression is surrounded by a matched pair of parentheses.
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1, success);
    } else {
        uint32_t op = 0;
        int      op_type = TK_NOTYPE;
        int      binary_ops[] = {
            '+', '-', '*', '/', '<', '>', TK_EQ, TK_NEQ, TK_AND, TK_LE, TK_GE,
        };
        int unary_ops[] = {
            TK_NEGATIVE,
            TK_POSITIVE,
            TK_DEREF,
            '!',
        };
        std::unordered_map<int, uint8_t> precedence = {
            {TK_EQ, 150},   {TK_NEQ, 150},     {TK_AND, 150},     {TK_LE, 130},
            {TK_GE, 130},   {'>', 130},        {'<', 130},        {'+', 100},
            {'-', 100},     {'/', 50},         {'*', 50},         {'!', 20},
            {TK_DEREF, 20}, {TK_NEGATIVE, 20}, {TK_POSITIVE, 20}, {TK_NOTYPE, 0},
        };
        // stack for parentheses matching
        int stk = 0;
        for (int i = p; i <= q; i++) {
            Token t = tokens[i];
            if (t.type == '(')
                stk++;
            else if (t.type == ')')
                stk--;
            if (stk)
                continue;
            for (int b = 0; b < ARRLEN(binary_ops); b++) {
                if (t.type == binary_ops[b] && precedence[t.type] >= precedence[op_type]) {
                    op = i, op_type = t.type;
                }
            }
            for (int b = 0; b < ARRLEN(unary_ops); b++) {
                if (t.type == unary_ops[b] && precedence[t.type] > precedence[op_type]) {
                    op = i, op_type = t.type;
                }
            }
        }

        if (op_type == TK_NOTYPE) {
            *success = false;
            printf("invalid expression (%d - %d): cannot find operator.\n\t", p, q);
            print_tokens(p, q);
            return 0;
        }

        // handle unary operators
        bool is_unary = false;
        for (int j = 0; j < ARRLEN(unary_ops); j++) {
            if (op_type == unary_ops[j]) {
                is_unary = true;
                break;
            }
        }

        if (is_unary) {
            word_t val = eval(op + 1, q, success);
            switch (op_type) {
            case '!':
                return !val;
            case TK_NEGATIVE:
                return -val;
            case TK_POSITIVE:
                return val;
            case TK_DEREF:
                if (!in_pmem(val)) {
                    *success = false;
                    printf("illegal memory access: " FMT_ADDR "\n", val);
                    return 0;
                }
                return pmem_read(val, 4);
            default:
                *success = false;
                printf("unknown unary operator.\n\t");
                print_tokens(op, op);
                return 0;
            }
        } else {
            word_t val1 = eval(p, op - 1, success);
            word_t val2 = eval(op + 1, q, success);

            switch (op_type) {
            case '+':
                return val1 + val2;
            case '-':
                return val1 - val2;
            case '*':
                return val1 * val2;
            case '/':
                if (val2 == 0) {
                    // *success = false;
                    printf("warning: divided by zero - returning uint32_max.\n");
                    return UINT32_MAX; // return val1 to avoid the program crash
                }
                return val1 / val2;
            case '<':
                return val1 < val2;
            case '>':
                return val1 > val2;
            case TK_LE:
                return val1 <= val2;
            case TK_GE:
                return val1 >= val2;
            case TK_EQ:
                return val1 == val2;
            case TK_NEQ:
                return val1 != val2;
            case TK_AND:
                return val1 && val2;
            default:
                *success = false;
                printf("unknown optype.\n\t");
                print_tokens(op, op);
                return 0;
            }
        }
    }
}

word_t expr_eval(char *e, bool *success) {
    // if success is nullptr, use a temporary variable to store the result (caller don't care)
    bool success_tmp = false;
    if (success == nullptr) {
        success = &success_tmp;
    }
    if (e == NULL) {
        *success = false;
        printf("expression is NULL\n");
        return 0;
    }
    if (strlen(e) > EXPR_MAX_LENGTH) {
        printf("expression too long: %s\n", e);
        *success = false;
        return 0;
    }
    if (strlen(e) == 0) {
        *success = true;
        return 0;
    }
    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    *success = true; // set to false if any error occurs
    word_t res = eval(0, nr_token - 1, success);
    return res;
}