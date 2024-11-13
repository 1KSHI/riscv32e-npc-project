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
#include <cpu/cpu.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (free_ == NULL) {
    return NULL;
  }

  WP *wp = free_;
  free_ = free_->next;

  wp->next = head;
  head = wp;

  return wp;
}

void free_wp(WP *wp) {
  if (wp == NULL) {
    return;
  }

  WP **cur = &head;
  while (*cur != NULL && *cur != wp) {
    cur = &(*cur)->next;
  }

  if (*cur == wp) {
    *cur = wp->next;
    wp->next = free_;
    free_ = wp;
  }
}

void check_watchpoints() {
  WP *wp = head;
  while (wp != NULL) {
    bool success = true;
    uint64_t new_value = expr(wp->expr, &success);
    // printf("expr = %s\n", wp->expr);
    // printf("new_value = %16lx\n", new_value);
    // printf("last_value = %16lx\n", wp->last_value);
    // printf("success = %d\n", success);
    if (success && new_value != wp->last_value) {
      printf("Watchpoint %d triggered: %s\n", wp->NO, wp->expr);
      printf("Old value = %16lx, New value = %lu\n", wp->last_value, new_value);
      wp->last_value = new_value;
      nemu_state.state = NEMU_STOP;
    }
    wp = wp->next;
  }
}

void watchpoint_display() {
  WP *wp = head;
  while (wp != NULL) {
    printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
    wp = wp->next;
  }
}

bool watchpoint_delete(int NO) {
  WP *wp = head;
  while (wp != NULL) {
    if (wp->NO == NO) {
      free_wp(wp);
      return true;
    }
    wp = wp->next;
  }
  return false;
}
