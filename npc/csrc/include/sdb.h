#ifndef __SDB_H__
#define __SDB_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[32];
  uint64_t last_value;
  int state;//0 is watchpoint, 1 is stoppoint
  /* TODO: Add more members if necessary */

} WP;

WP* new_wp();



bool watchpoint_delete(int NO);
void watchpoint_display();
void check_watchpoints(vaddr_t pc);
word_t expr(char *e, bool *success);

#endif
