#include <locale.h>
#include <common.h>
#include "include.h"
#include "tb_common.h"
#include <utils.h>
#include <Vtop.h>//user set


extern void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte); // 反汇编函数

#define MAX_INST_TO_PRINT 200
#define RINGBUF_SIZE 20

bool init_flag = false;

extern NPC_state npc_state;
extern CPU_file cpu;
extern Vtop* top;
extern void watch_dog();
char logbuf[128];
bool diff_skip    = false;
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

#if DIFFTEST_ON
  bool diff_skip_r;
#endif

static void split_asm_buf(const char *asm_buf, char *first_part, char *second_part) {
  int i = 0, j = 0;
  while (asm_buf[i] != '\0' && asm_buf[i] != 0 && asm_buf[i] != 9 && asm_buf[i] != 32) {
    first_part[i] = asm_buf[i];
    i++;
  }
  first_part[i] = '\0';
  while (asm_buf[i] == 0 || asm_buf[i] == 9 || asm_buf[i] == 32) {i++;}
  while (asm_buf[i] != '\0') {second_part[j++] = asm_buf[i++];}
  second_part[j] = '\0';
}

static void trace_and_difftest(vaddr_t pc) {
  #ifdef CONFIG_ITRACE
    log_write("%s\n", logbuf);
  #endif
  
  #ifdef CONFIG_IRINGBUF
    iring_check(cpu);
  #endif
  
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(logbuf)); }
  // IFDEF(CONFIG_DIFFTEST, difftest_step(pc, dnpc));

  #if DIFFTEST_ON
    // 1. check last cycle reg status:
    if(diff_skip_r){ //skip write or read device ins.
      diff_cpdutreg2ref();
    }
    else{
      if(!difftest_check()){
        print_regs(false);
        npc_state.state = NPC_END;
        npc_state.trap  = BAD_TRAP;
      }
    }
    // 2. nemu step and update nemu regs/mem:
    if(!diff_skip){
      difftest_step();
    }
    diff_skip_r = diff_skip;
  #endif
}

static void exec_once(vaddr_t pc) {
  top->inst = paddr_read(pc, 4);
  
  print_regs(false);
  watch_dog();

  #ifdef CONFIG_ITRACE
      char asm_buf[128];
      disassemble(asm_buf, sizeof(asm_buf), pc, (uint8_t *)&(top->inst), 4);
      char first_part[12] = {0};
      char second_part[20] = {0};
      split_asm_buf(asm_buf, first_part, second_part);
    
      uint8_t *inst_bytes = (uint8_t *)&(top->inst);
      snprintf(logbuf, sizeof(logbuf), "0x%08x: %-12s %-20s %02x %02x %02x %02x", 
               pc, first_part , second_part, inst_bytes[3], inst_bytes[2], inst_bytes[1], inst_bytes[0]);
  #endif


}

static void execute(uint64_t n) {
  for (;n > 0; n --) {
    exec_once(cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(cpu.pc);
    single_cycle();
    if (npc_state.state != NPC_RUNNING) break;
    
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  printf("\nhost time spent = " NUMBERIC_FMT " us\n", g_timer);
  printf("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) printf("\nsimulation frequency = " NUMBERIC_FMT " inst/s\n", g_nr_guest_inst * 1000000 / g_timer);
  else printf("Finish running in less than 1 us and can not calculate the simulation frequency\n");
}

void assert_fail_msg() {
  print_regs(true);
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (npc_state.state) {
    case NPC_END: case NPC_ABORT: case NPC_QUIT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: npc_state.state = NPC_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (npc_state.state) {
    case NPC_RUNNING: npc_state.state = NPC_STOP; break;

    case NPC_END: case NPC_ABORT:
      printf("npc: %s at pc = " FMT_WORD,
          (npc_state.state == NPC_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (npc_state.trap == GOOD_TRAP ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          cpu.pc);
      // fall through
    case NPC_QUIT: statistic();
  }
}
