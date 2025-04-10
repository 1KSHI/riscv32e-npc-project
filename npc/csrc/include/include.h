#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "Vtop.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory/paddr.h>
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

//#define DIFFTEST_ON  1
//#define DUMPWAVE_ON  1

#define INST_START    0x80000000 // use for difftest reg copy.
#define PMEM_START    0x80000000 // use for difftest mem copy.
#define PMEM_END      0x87ffffff
#define PMEM_MSIZE    (PMEM_END+1-PMEM_START)

#define SERIAL_PORT   0x10000000 // 0x1000_0000 ~ 0x1000_0fff

typedef struct {
  uint32_t reg[32];
  uint32_t pc;
  uint32_t inst;
  uint32_t csr[4];
} CPU_file;


enum npc_state_t{
  NPC_QUIT = 0,
  NPC_RUNNING,
  NPC_STOP,
  NPC_END,
  NPC_ABORT
};

enum npc_trap_t{
  GOOD_TRAP = 0,
  BAD_TRAP
};

typedef struct {
  npc_state_t state;
  npc_trap_t trap;
  word_t pc;
} NPC_state;

void npc_init(int argc, char *argv[]);
void print_regs(bool all);
void cpu_exec(uint64_t n);
void print_one_regs(char *reg);
bool checkregs(CPU_file *ref, CPU_file *cpu);
word_t reg_str2val(const char *s, bool *success);
void init_log(const char *log_file); 

#ifdef DIFFTEST_ON
void difftest_init(char *ref_so_file, long img_size);
bool difftest_check();
void difftest_step();
void diff_cpdutreg2ref();
#endif

#endif