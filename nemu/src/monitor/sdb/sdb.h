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

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

#define EXPR_TOKEN_SIZE 4096
#define EXPR_TOKEN_LENGTH 32

#define WP_EXPR_SIZE 512
#define NR_WP 32

typedef struct watchpoint {
  int                NO;
  struct watchpoint *next;
  bool               enable; // if enabled
  bool               valid;  // if the watchpoint is added by user
  int                hit;
  char               expr[WP_EXPR_SIZE];
  word_t             value;
} WP;

/** expr.c **/
void   init_regex();
word_t expr(char *e, bool *success);

/** watchpoint.c */
void init_wp_pool();
WP  *new_wp(char *expr);
void free_wp(int NO);
void wp_display();
void wp_update(int *hit);

#endif