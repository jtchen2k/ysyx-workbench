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

#include "sdb.h"

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].enable = false;
    wp_pool[i].hit = 0;
  }

  head = NULL;
  free_ = wp_pool;
}

// Create a new watchpoint and return it. If failed, return NULL.
WP *new_wp(char *expr) {
  if (free_ == NULL) {
      printf("No enough watchpoints.\n");
      return NULL;
  }

  // get free watchpoint, remove first from free_
  WP *wp = free_;
  free_ = free_->next;

  // insert to head of watchpoint list
  wp->next = head;
  head = wp;

  // initialize wp
  wp->enable = true;
  wp->valid = true;
  wp->hit = 0;
  strncpy(wp->expr, expr, WP_EXPR_SIZE);
  return wp;
}

void free_wp(int NO) {

  if (head == NULL) {
    printf("No watchpoints.\n");
    return;
  }

  // find the watchpoint from head list
  WP *cur = head, *prev = NULL;
  while (cur != NULL) {
    if (cur->NO == NO) {
      if (prev == NULL) {
        head = cur->next;
      } else {
        prev->next = cur->next;
      }
      cur->enable = false;
      cur->next = free_;
      free_ = cur;
      return;
    } else {
      prev = cur, cur = cur->next;
    }
  }

  printf("No watchpoint %d.\n", NO);
}


void wp_display() {
  if (head == NULL) {
    printf("No watchpoints.\n");
    return;
  }

  printf("%-4s %-4s %-4s %s\n", "No", "Enb", "Hit", "What");
  for (int i = 0; i < NR_WP; i++) {
    if (!wp_pool[i].valid) continue;
    WP *cur = &wp_pool[i];
    printf("%-4d %-4s %-4d %s\n", cur->NO, cur->enable ? "y" : "n", cur->hit, cur->expr);
    cur = cur->next;
  }
}