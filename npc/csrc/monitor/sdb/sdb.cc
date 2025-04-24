/*
 * sdb.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 17:37:02
 * @modified: 2025-04-24 16:07:36
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "common.h"
#include "core.h"
#include "macro.h"
#include "mem.h"
#include "monitor.h"
#include "utils.h"

#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <string>
#include <vector>

#define SDB_STOP 0xff
#define SDB_CONTINUE 0
#define SDB_INVALID 0xee

struct CommandInfo {
    const char *name;
    const char *shortcut;
    const char *description;
    int (*handler)(char *args);
};

static char *rl_gets() {
    static char *line_read = nullptr;
    if (line_read) {
        free(line_read);
        line_read = nullptr;
    }
    line_read = readline("(npc) ");
    if (line_read && *line_read) {
        add_history(line_read);
    }
    return line_read;
}

static int cmd_h(char *args);

static int cmd_q([[maybe_unused]] char *args) {
    printf("bye.\n");
    return SDB_STOP;
}

static int cmd_c([[maybe_unused]] char *args) {
    core_exec(-1);
    return SDB_CONTINUE;
}

static int cmd_i(char *args) {
    char *subcmd = strtok(args, " ");
    if (subcmd == nullptr) {
        printf("[cycle = %8lu, pc = 0x%08x] \n", g_vcontext->time(), R(PC));
        return SDB_CONTINUE;
    }
    if (strcmp(subcmd, "r") == 0) {
        reg_display();
        return SDB_CONTINUE;
    }
    if (strcmp(subcmd, "w") == 0) {
        wp_display();
        return SDB_CONTINUE;
    }
#ifdef CONFIG_ITRACE
    if (strcmp(subcmd, "i") == 0) {
        itrace_display();
        return SDB_CONTINUE;
    }
#endif
    printf("unknown subcommand: %s\n", subcmd);
    return SDB_INVALID;
}

static char print_formats[] = "duxb";
static void fmtprint(word_t val, char fmt) {
    switch (fmt) {
    case 'd':
        printf("%d", (int32_t)val);
        break;
    case 'u':
        printf("%u", (uint32_t)val);
        break;
    case 'b':
        for (int i = 31; i >= 0; i--) {
            printf("%d", (val >> i) & 1);
            if (i % 4 == 0) {
                printf(" ");
            }
        }
        break;
    default:
        printf(FMT_WORD, (uint32_t)val);
        break;
    }
}

/// get the printing format (if applicable).
/// return nullptr if print format is invalid. otherwise, return the rest of the
/// command line arguments.
static char *get_format(char *args, char *fmt) {
    if (strlen(args) > 0 && args[0] == '/') {
        args++;
        if (strchr(print_formats, args[0]) == nullptr) {
            printf("invalid print format: %c\n", args[0]);
            return nullptr;
        }
        *fmt = args[0];
        args++;
    }
    return args;
}

static int cmd_x(char *args) {
    char fmt = 'x';
    args = get_format(args, &fmt);
    if (!args) {
        return SDB_INVALID;
    }
    char *nstr = strtok(args, " ");
    if (nstr == nullptr) {
        printf("missing n. usage: x n EXPR\n");
        return SDB_INVALID;
    }
    int  n = atoi(nstr);
    bool reverse = n < 0;
    bool success = false;
    if (reverse) {
        n = -n;
    }
    char *e = strtok(nullptr, "");
    if (e == nullptr) {
        printf("missing EXPR. usage: x n EXPR\n");
        return SDB_INVALID;
    }
    word_t addr = expr_eval(e, &success);
    if (!success) {
        printf("failed to evaluate expression: %s\n", e);
        return SDB_INVALID;
    }
    for (int i = 0; i < n; i++) {
        if (!in_pmem(addr)) {
            printf(ANSI_LG_RED "invalid memory address: " ANSI_NONE FMT_ADDR "\n", addr);
            return SDB_INVALID;
        }
        word_t val = paddr_read(addr, 4);
        printf(ANSI_BOLD FMT_ADDR ANSI_NONE ": ", addr);
        fmtprint(val, fmt);
        printf("\n");
        addr += (reverse ? -4 : 4);
    }
    return SDB_CONTINUE;
}

static int cmd_p(char *args) {
    if (args == nullptr) {
        printf("missing EXPR\n");
        return SDB_INVALID;
    }
    char fmt = 'x';
    args = get_format(args, &fmt);
    if (!args) {
        return SDB_INVALID;
    }
    bool   success = false;
    word_t res = expr_eval(args, &success);
    if (!success) {
        printf("failed to evaluate expression: %s\n", args);
        return SDB_INVALID;
    }
    fmtprint(res, fmt);
    printf("\n");
    return SDB_CONTINUE;
}

static int cmd_si(char *args) {
    int64_t n = args != nullptr ? atoll(args) : 1;
    if (n < 0) {
        printf("invalid argument: %s\n", args);
        return SDB_INVALID;
    }
    core_exec(n);
    return SDB_CONTINUE;
}

static int cmd_w(char *args) {
    if (args == nullptr) {
        printf("missing EXPR\n");
        return SDB_INVALID;
    }
    bool success = false;
    wp_add(args, &success);
    return success ? SDB_CONTINUE : SDB_INVALID;
}

static int cmd_d(char *args) {
    if (args == nullptr) {
        printf("missing ID\n");
        return SDB_INVALID;
    }
    int  id = atoi(strtok(args, " "));
    bool success = false;
    wp_remove(id, &success);
    return success ? SDB_CONTINUE : SDB_INVALID;
}

static std::vector<CommandInfo> sdb_commands{
    {"continue", "c", "continue the execution of the program", cmd_c},
    {"print", "p",
     "print expression. usage: p EXPR, p/<format> EXPR, where <format> is one "
     "of:\n"
     "\t\td: signed decimal, u: unsigned decimal, x: hexadecimal, b: binary\n"
     "\t\tthe default format is hexadecimal.",
     cmd_p},
    {"step", "si", "step instructions. usage: si <n>, where n defaults to 1", cmd_si},
    {"scan", "x",
     "scan memory. usage: x n EXPR, x/<format> n EXPR\n"
     "\t\t<format> works the same as the print command.",
     cmd_x},
    {"watch", "w", "set watchpoint. usage: w EXPR", cmd_w},
    {"unwatch", "d", "remove watchpoint. usage: d ID", cmd_d},
    {"info", "i",
     "display runtime information. usage: info <SUBCMD>\n"
     "\t\ti r: print register values.\n"
#ifdef CONFIG_ITRACE
     "\t\ti i: print instruction ring buffer\n"
#endif
     "\t\ti w: print watchpoints.",
     cmd_i},
    {"help", "h", "show this help message", cmd_h},
    {"quit", "q", "quit npc", cmd_q},
};

static int cmd_h([[maybe_unused]] char *args) {
    printf("\nList of available commands:\n\n");
    for (uint32_t i = 0; i < sdb_commands.size(); i++) {
        printf(ANSI_BOLD "%10s" ANSI_NONE " (%s) %s\n", sdb_commands[i].name,
               sdb_commands[i].shortcut, sdb_commands[i].description);
    }
    printf("\n");
    return 0;
}

void exit_reporter() {
    IFDEF(CONFIG_ITRACE, itrace_display());
    reg_display();
}

void sdb_mainloop() {

    if (g_args->batch) {
        core_exec(-1);
        return;
    }

    for (char *line; (line = rl_gets()) != nullptr;) {
        char *cmd = strtok(line, " ");
        if (cmd == nullptr) {
            continue;
        }
        char *args = strtok(nullptr, "");

        // support command with slash (for print)
        char *slash = nullptr;
        if ((slash = strchr(cmd, '/')) != nullptr) {
            char *t = new char[strlen(cmd)];
            strncpy(t, cmd, slash - cmd);
            t[slash - cmd] = '\0';
            cmd = t;
            if (args != nullptr)
                *(args - 1) = ' ';
            args = slash;
        }
        uint32_t i = 0;
        int32_t  ret = SDB_CONTINUE;
        for (i = 0; i < sdb_commands.size(); i++) {
            if (strcmp(cmd, sdb_commands[i].name) == 0 ||
                strcmp(cmd, sdb_commands[i].shortcut) == 0) {
                Assert(sdb_commands[i].handler != nullptr, "command %s has no handler", cmd);
                ret = sdb_commands[i].handler(args);
                if (ret == SDB_STOP) {
                    goto end;
                }
                break;
            }
        }
        if (i == sdb_commands.size()) {
            printf("unknown command: %s. type h for help.\n", cmd);
        }
    }
end:
    return;
}

// clear line when ctrl+c instead of exiting
static void handle_sigint(int sig) {
    if (sig == SIGINT) {
        static uint64_t last_sigint = 0;
        uint64_t now = get_time();
        if (now - last_sigint < 0.5e6) {
            LogInfo("SIGINT received again. exiting.");
            exit(monitor_exit());
        }
        last_sigint = now;
        if (g_core_context->state == CORE_STATE_RUNNING) {
            g_core_context->state = CORE_STATE_STOP;
        }
        rl_replace_line("", 0);
        rl_crlf();
        rl_on_new_line();
        rl_redisplay();
    }
}
static void signal_init() {
    signal(SIGINT, handle_sigint);
}

void sdb_init() {
    regex_init();
    wp_init();
    signal_init();
    disasm_init();
    LogDebug("sdb fully initialized.");
}