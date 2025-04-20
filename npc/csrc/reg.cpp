#include "include/include.h"

extern CPU_file cpu;
extern int is_batch_mode;
const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csrs[] = {
  "mstatus", "mtvec", "mepc", "mcause"
};

bool checkregs(CPU_file *ref, CPU_file *cpu) {
  if(ref->pc != cpu->pc){
    printf("difftest error: ");
    printf("next reg pc is diff: ref = 0x%08x, cpu = 0x%08x\n",ref->pc,cpu->pc);
    return false;
  } 
  for (int i = 0; i < ARRLEN(regs); i++) {
    if(ref->reg[i] != cpu->reg[i]){
      printf("difftest error at nextpc = 0x%08x, ",cpu->pc);
      printf("reg %s is diff: ref = 0x%08x, cpu = 0x%08x\n",regs[i],ref->reg[i],cpu->reg[i]);
      return false;
    }
  }
  // for (int i = 0; i < ARRLEN(csrs); i++) {
  //   if(ref->csr[i] != cpu->csr[i]){
  //     printf("difftest error at nextpc = 0x%08x, ",cpu->pc);
  //     printf("csr %s is diff: ref = 0x%08x, cpu = 0x%08x\n",csrs[i],ref->csr[i],cpu->csr[i]);
  //     return false;
  //   }
  // }
  return true;
}

void print_regs(bool all){
  //printf("cpu      pc = 0x%08x\n",cpu.pc);
  for (int i = 0; i < ARRLEN(regs); i++) {
    if( !is_batch_mode && cpu.reg[i]!=0){
      printf("reg %3s | cpu = 0x%08x\n",regs[i],cpu.reg[i]);
    }
  }
}

void log_regs(bool all){
  //printf("cpu      pc = 0x%08x\n",cpu.pc);
  for (int i = 0; i < ARRLEN(regs); i++) {
    if( cpu.reg[i]!=0){
      log_write("reg %3s | cpu = 0x%08x\n",regs[i],cpu.reg[i]);
    }
  }
}

void print_one_regs(char *reg) {
  for (int i = 0; i < sizeof(regs) / sizeof(regs[0]); i++) {
    if(strcmp(regs[i], reg) == 0) {
      printf("%s\t0x%08x\n", regs[i], cpu.reg[i]);
      return;
    }
  }
  printf("Unknown register name: %s\n", reg);
}

word_t reg_str2val(const char *s, bool *success) {
  for (int i = 0; i < sizeof(regs) / sizeof(regs[0]); i++) {
    if (strcmp(regs[i], s) == 0) {
      *success = true;
      return cpu.reg[i];
    }
  }
  *success = false;
  return 0;
}