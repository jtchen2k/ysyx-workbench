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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "macro.h"
#include "utils.h"
#include "trace.h"
#include <memory/paddr.h>
#include <stdlib.h>


static int is_batch_mode = false;

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  int64_t n = args != NULL ? atoll(args) : 1;
  if (n < 0) {
    printf("Invalid argument: %s\n.", args);
    return 0;
  }
  strtok(NULL, " ");
  if (strtok(NULL, " ") != NULL) {
    printf("warning: subsequent arguments are ignored.\n");
  }
  cpu_exec(n);
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("p: missing EXPR.\n");
    return 0;
  }
  bool success = true;
  word_t res = expr(args, &success);
  if (!success) {
    printf("Failed to evaluate expression: %s\n", args);
    return 0;
  }
  printf(FMT_WORD "\n", res);
  return 0;
}

static int cmd_info (char *args) {
  if (args == NULL) {
    printf("info: missing [SUBCMD].\n");
    return 0;
  }
  char *arg1 = strtok(NULL, " ");
  if (strcmp(arg1, "r") == 0) {
    isa_reg_display();
  } else if (strcmp(arg1, "w") == 0) {
    wp_display();
  }
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("x: missing N and EXPR.\n");
    return 0;
  }
  char *arg1 = strtok(args, " ");
  char *arg2 = strtok(NULL, "\n"); // expr
  int n = atoi(arg1);
  if (arg1 == NULL || n < 0) {
    printf("invalid argument: %s\n.", arg1);
    return 0;
  }
  if (arg2 == NULL) {
    printf("x: missing EXPR.\n");
    return 0;
  }

  bool success;
  paddr_t addr = expr(arg2, &success);

  if (!success) {
    printf("Failed to evaluate expression: %s\n", arg2);
    return 0;
  }

  for (int i = 0; i < n; i++) {
    if (!in_pmem(addr)) {
      printf(ANSI_FG_RED FMT_PADDR ": illegal access\n" ANSI_NONE, addr);
    } else {
      printf(ANSI_FG_BLUE FMT_PADDR ANSI_NONE ": " FMT_WORD "\n", addr, paddr_read(addr, 4));
    }
    addr += 4;
  }
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) {
    printf("w: missing EXPR.\n");
    return 0;
  }
  WP *wp = new_wp(args);
  if (wp == NULL) {
    printf("Failed to create watchpoint for expression: %s\n", args);
  } else {
    printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
  }
  return 0;
}

static int cmd_d(char *args) {
  if (args == NULL) {
    printf("d: missing N.\n");
    return 0;
  }
  int n = atoi(args);
  if (n < 0) {
    printf("invalid argument: %s\n.", args);
    return 0;
  }
  free_wp(n);
  return 0;
}

/// instruction ring buffer
#ifdef CONFIG_ITRACE
static int cmd_ib(char *args) {
  irb_display();
  return 0;
}
#endif

/// ftrace
#ifdef CONFIG_FTRACE
static int cmd_fs(char *args) {
  printf("list of function symbols:\n");
  fsym_display();
  return 0;
}
#endif

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    /* TODO: Add more commands */
    {"si", "Step one instruction exactly. Use si[N] to step N times.", cmd_si},
    {"info", "Generic command for showing things about the program being debugged: info [SUBCMD]\n"
             "\tinfo r: Print register values.\n"
             "\tinfo w: Print information of watchpoints.", cmd_info},
    {"x", "Examine memory: x N EXPR", cmd_x},
    {"p", "Print value of expression EXPR: p EXPR. ", cmd_p},
#ifdef CONFIG_WATCHPOINT
    {"w", "Set a watchpoint for an expression: w EXPR", cmd_w},
#endif
    {"d", "Delete the watchpoint N: d N", cmd_d},
#ifdef CONFIG_ITRACE
    {"ib", "Print the iringbuf (instruction ringbuffer)", cmd_ib},
#endif
#ifdef CONFIG_FTRACE
    {"fs", "List function symbols", cmd_fs},
#endif
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }


  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
