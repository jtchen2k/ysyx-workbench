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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define BUF_SIZE 65536

// this should be enough
static char  buf[BUF_SIZE] = {};
static char  code_buf[BUF_SIZE + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static uint32_t pos = 0;

uint32_t choose(uint32_t n) { return rand() % n; }

static void gen(char c) {
  if (pos < BUF_SIZE - 1)
    buf[pos++] = c;
  else {
    printf("buffer overflow.\n");
    exit(1);
  }
}

static void gens(char *s) {
  while (*s != '\0') {
    gen(*s);
    s++;
  }
}

static void gen_space(int n) {
  while (n--)
    if (choose(2) == 0)
      gen(' ');
}

static void gen_num() {
  int n = choose(9) + 1;
  for (int i = 0; i < n; i++) {
    gen(i == 0 && n > 1 ? choose(9) + '1' : choose(10) + '0');
  }
  gen('u');
}

static char gen_rand_op() {
  char ops[] = "+-*/";
  char op = ops[choose(4)];
  gen_space(1);
  gen(op);
  gen_space(1);
  return op;
}

static void gen_rand_expr(int depth) {
  if (depth >= 20) {
    // avoid buffer overflow
    gen_num();
    return;
  }
  gen_space(1);
  switch(choose(5)) {
    case 0:
    case 1:
      gen_num();
      break;
    case 2:
      gen('('); gen_rand_expr(depth + 1); gen(')'); break;
    default: {
      gen_rand_expr(depth + 1);
      char op = gen_rand_op();
      if (op == '/') {
        gen('(');
        gen_rand_expr(depth + 1);
        gens("/2u+1u)");
      } else {
        gen_rand_expr(depth + 1);
      }
      break;
    }
  }
  gen_space(1);
}

static void init_buf() {
  pos = 0;
  buf[0] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {

    init_buf();
    gen_rand_expr(0);
    gen('\0');

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
