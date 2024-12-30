/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  /* TODO: Add more token types */
  TK_DECIMAL,
};

static struct rule {
    const char *regex;
    int         token_type;
} rules[] = {
    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */
    // {" +", TK_NOTYPE},                     // spaces
    {"\\d+u?", TK_DECIMAL},
    {"\\(", '('},
    {"\\)", ')'},
    {"\\*", '*'},                          // mult
    {"/", '/'},                          // divide
    {"\\+", '+'},                          // plus
    {"-", '-'},                          // minus
    {"==", TK_EQ}, // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        if (substr_len >= 31) {
          Log("token length exceeds buffer size.");
          return false;
        }

        if (nr_token >= 32) {
            Log("too many tokens");
            return false;
        }

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_DECIMAL:
            tokens[nr_token].type = TK_DECIMAL;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
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

  return true;
}

static bool check_parentheses(int p, int q) {
  Assert(p < q, "bad check_parentheses call");
  return (tokens[p].type == '(' && tokens[q].type == ')');
}

static uint32_t eval(int p, int q, bool* success) {
  if (p > q) {
    /* Bad expression */
    Assert(0, "bad expression.");
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if (tokens[p].type != TK_DECIMAL) {
      *success = false;
      printf("invalid expression: missing decimal number.\n");
      return 0;
    }
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  else {
    uint32_t op = 0;
    int op_type = TK_NOTYPE;
    uint8_t precedence[] = {
      ['+'] = 100,
      ['-'] = 100,
      ['*'] = 50,
      ['/'] = 50,
      [TK_NOTYPE] = 0,
    };

    for (int i = p; i <= q; i++) {
      Token t = tokens[i];
      if (t.type == '+' || t.type == '-' || t.type == '*' || t.type == '/') {
        if (precedence[t.type] > precedence[op_type]) {
          op = i, op_type = t.type;
        }
      }
    }
    if (op_type == TK_NOTYPE) {
      *success = false;
      printf("invalid expression: cannot find operator.\n");
      return 0;
    }

    uint32_t val1 = eval(p, op - 1, success);
    uint32_t val2 = eval(op + 1, q, success);

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/':
        if (val2 == 0) {
          *success = false;
          printf("divided by zero.\n");
          return val1; // return val1 to avoid the program crash
        }
      default:
        *success = false;
        printf("invalid expression.\n");
        return 0;
    }
  }
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int32_t res = eval(0, nr_token - 1, success);
  return res;
}

int test_expr() {
  char buf[65536] = {};
  const char *nemu_home = getenv("NEMU_HOME");
  if (nemu_home == NULL) {
    printf("NEMU_HOME environment variable is not set.\n");
    return -1;
  }
  snprintf(buf, sizeof(buf), "%s/tools/gen-expr/input", nemu_home);
  FILE *input = fopen(buf, "r");
  if (input == NULL) {
    printf("failed to open input file %s.\n", buf);
    return -2;
  }

  int case_id = 0;
  while (fgets(buf, sizeof(buf), input) != NULL) {
    bool expr_success = true;
    if (buf[0] == '\n' || buf[0] == '\0') {
      continue;
    }
    char *expected;
    char *expression;
    expected = strtok(buf, " ");
    expression = strtok(NULL, "\n");
    uint32_t expected_val = atoi(expected);
    uint32_t result = expr(expression, &expr_success);
    if (expr_success == false) {
      printf("failed to evaluate expression #%d: %s\n", case_id, expression);
      return -100;
    }
    if (result != expected_val) {
      printf("failed to eval expression #%d: %s.\n\texpected: %u, but got: %u\n. stopping test.",
        case_id, expression, expected_val, result);
      return -101;
    }
    case_id++;
  }
  Log("test_expr() passed.");
  return 0;

}