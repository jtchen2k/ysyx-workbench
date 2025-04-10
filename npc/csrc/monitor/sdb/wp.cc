/*
 * wp.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-09 00:05:49
 * @modified: 2025-04-10 23:03:50
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "core.h"
#include "monitor.h"

#define WP_MAX_NUM 32

struct wp_t {
    char    *expr;
    word_t   val;
    uint32_t hit;
    wp_t    *next;
    bool     enabled;
};

static wp_t  wp_pool[WP_MAX_NUM];
static wp_t *wp_free = nullptr;
static int   wp_count = 0;

void wp_init() {
    wp_free = wp_pool;
    for (int i = 0; i < WP_MAX_NUM; i++) {
        wp_pool[i].next = i == WP_MAX_NUM - 1 ? nullptr : wp_pool + i + 1;
        wp_pool[i].enabled = false;
        wp_pool[i].expr = nullptr;
        wp_pool[i].val = 0;
        wp_pool[i].hit = 0;
    }
    wp_count = 0;
}

void wp_add(char *expr, bool *success) {
    *success = false;
    word_t res = expr_eval(expr, success);
    if (!*success) {
        printf("unable to add watchpoint. invalid expression: %s\n", expr);
        return;
    }
    if (wp_free == nullptr) {
        printf("you have used all %d watchpoints\n", WP_MAX_NUM);
        return;
    }
    wp_t *wp = wp_free;
    wp_free = wp_free->next;
    wp->expr = strdup(expr);
    wp->val = res;
    wp->hit = 0;
    wp->enabled = true;
    *success = true;
    wp_count++;
    printf("added watchpoint %d: %s\n", (int)(wp - wp_pool + 1), expr);
}

void wp_remove(int id, bool *success) {
    *success = false;
    if (id < 1 || id > WP_MAX_NUM) {
        printf("invalid watchpoint id: %d\n", id);
        return;
    }
    if (!wp_pool[id - 1].enabled) {
        printf("cannot find watchpoint %d\n", id);
        return;
    }
    wp_t *wp = wp_pool + id - 1;
    wp->enabled = false;
    wp->next = wp_free;
    wp_free = wp;
    *success = true;
    wp_count--;
    printf("removed watchpoint %d\n", id);
}

void wp_display() {
    if (wp_count == 0) {
        printf("no watchpoints\n");
        return;
    }
    printf("%-4s%-10s  %-7s %s\n", "id", "value", "hit", "expr");
    for (int i = 0; i < WP_MAX_NUM; i++) {
        if (!wp_pool[i].enabled)
            continue;
        printf("%-4d0x%08x  %-7d %s\n", i + 1, wp_pool[i].val, wp_pool[i].hit,
               wp_pool[i].expr);
    }
}

void wp_eval() {
    for (int i = 0; i < WP_MAX_NUM; i++) {
        if (!wp_pool[i].enabled || !wp_pool[i].expr) {
            continue;
        }
        word_t res = expr_eval(wp_pool[i].expr, nullptr);
        if (res != wp_pool[i].val) {
            wp_pool[i].hit++;
            printf(ANSI_BOLD "hit" ANSI_NONE " watchpoint %d: %s (0x%08x -> 0x%08x)\n",
                   i + 1, wp_pool[i].expr, wp_pool[i].val, res);
            wp_pool[i].val = res;
            g_core_state->state = CORE_STATE_STOP;
        }
    }
}