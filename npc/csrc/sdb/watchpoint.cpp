#include "sdb.h"
#include "include.h"
#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
extern NPC_state npc_state;

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

void check_watchpoints(vaddr_t pc) {
  WP *wp = head;
  while (wp != NULL) {
    bool success = true;
    uint64_t new_value = expr(wp->expr+wp->state, &success);
    // printf("expr = %s\n", wp->expr);
    // printf("new_value = %016lx\n", new_value);
    // printf("last_value = %016lx\n", wp->last_value);
    // printf("success = %d\n", success);
    // printf("pc = "FMT_WORD"\n", pc);
    if (wp->state==0 && success && new_value != wp->last_value) {
      printf("Watchpoint %d triggered: %s at pc=%8x\n", wp->NO, wp->expr, pc-4);
      printf("Old value = 0x%08lx, New value = 0x%08lx\n", wp->last_value, new_value);
      wp->last_value = new_value;
      npc_state.state = NPC_STOP;
    }else if(wp->state==1 && (pc == new_value)){
      printf("Stoppoint %d triggered: %s at pc=%8x\n", wp->NO, wp->expr, pc);
      npc_state.state = NPC_STOP;
    }
    wp = wp->next;
  }
}

void watchpoint_display() {
  WP *wp = head;
  while (wp != NULL) {
    wp->state?printf("Stoppoint %d: %s\n", wp->NO, wp->expr):printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
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
