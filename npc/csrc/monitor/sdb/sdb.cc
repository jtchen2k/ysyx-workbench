/*
 * sdb.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-08 17:37:02
 * @modified: 2025-04-08 21:36:28
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

#define SDB_STOP 0xff
#define SDB_CONTINUE 0
#define SDB_INVALID 0xee

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

int cmd_h(char *args);

int cmd_q(char *args) { return SDB_STOP; }

int cmd_c(char *args) {
    core_exec(-1);
    return SDB_CONTINUE;
}

int cmd_info(char *args) {
    printf("[cycle = %8lu, pc = 0x%08x, inst = 0x%08x] \n", g_context->time(),
           g_core->io_pc, g_core->io_inst);
    char *subcmd = strtok(args, " ");
    if (subcmd == nullptr) {
        return SDB_CONTINUE;
    }
    if (strcmp(subcmd, "r") == 0) {
        print_registers();
        return SDB_CONTINUE;
    }
    printf("unknown subcommand: %s\n", subcmd);
    return SDB_INVALID;
}

int cmd_x(char *args) {
    printf("todo\n");
    return SDB_CONTINUE;
}

int cmd_p(char *args) {
    printf("todo\n");
    return SDB_CONTINUE;
}

int cmd_si(char *args) {
    char   *nstr = strtok(args, " ");
    int64_t n = nstr ? atoll(nstr) : 1;
    if (n < 0) {
        printf("invalid argument: %s\n", args);
        return SDB_INVALID;
    }
    core_exec(n);
    return SDB_CONTINUE;
}

static struct {
    const char *name;
    const char *shortcut;
    const char *description;
    int (*handler)(char *);
} sdb_commands[] = {
    {"help", "h", "show this help message", cmd_h},
    {"continue", "c", "continue the execution of the program", cmd_c},
    {"print", "p", "print expression. usage: p EXPR", cmd_p},
    {"step", "si", "step instructions. usage: si <n>, where n defaults to 1",
     cmd_si},
    {"scan", "x", "scan memory. usage: x n EXPR", cmd_x},
    {
        "info", "i",
        "display core information. usage: info <SUBCMD>\n"
        "\t\tinfo r: Print register values.",
        cmd_info
        // "\tinfo w: Print information of watchpoints."
        //
    },
    {"quit", "q", "quit npc", cmd_q},
};
static int NR_CMD = ARRLEN(sdb_commands);

int cmd_h(char *args) {
    for (int i = 0; i < NR_CMD; i++) {
        printf(ANSI_BOLD "%10s" ANSI_NONE " (%s) %s\n", sdb_commands[i].name,
               sdb_commands[i].shortcut, sdb_commands[i].description);
    }
    return 0;
}

void sdb_mainloop() {

    if (g_args->batch) {
        core_exec(-1);
        return;
    }

    for (char *line; (line = rl_gets()) != nullptr;) {
        char *cmd = strtok(line, " ");
        int   i, ret;
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(cmd, sdb_commands[i].name) == 0 ||
                strcmp(cmd, sdb_commands[i].shortcut) == 0) {
                ret = sdb_commands[i].handler(strtok(nullptr, " "));
                if (ret == SDB_STOP) {
                    goto end;
                }
                break;
            }
        }
        if (i == NR_CMD) {
            printf("unknown command: %s. type h for help.\n", cmd);
        }

        // while (g_context->time() < CONFIG_MAX_INST) {
        //     switch (g_core_state->state) {
        //     case CORE_STATE_RUNNING:
        //         core_exec(1);
        //         break;
        //     case CORE_STATE_QUIT:
        //         core_stop();
        //         goto end;
        //     case CORE_STATE_STOP:
        //         Panic("unexpected core state: %d", g_core_state->state);
        //     }
        // }

        // LogWarn("core reached maximum instruction limit: %d",
        // CONFIG_MAX_INST);
    }
end:
    return;
}

void sdb_init() {
    // init_regex();
}