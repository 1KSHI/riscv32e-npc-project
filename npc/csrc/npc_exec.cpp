#include <locale.h>
#include "include/include.h"
#include "include/tb_common.h"
#include <utils.h>
#include <Vtop.h>//user set

#define MAX_INST_TO_PRINT 200
#define RINGBUF_SIZE 20

extern NPC_state npc_state;
extern CPU_state cpu;
extern TESTBENCH<Vtop> *__TB__;

extern void watch_dog();

uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

static void exec_once() {
  cpu.pc = TB(DUT(pc));
  TB(DUT(inst) = paddr_read(cpu.pc,4));
  print_regs(false);
  watch_dog();
  TB(cycles(1));
}

static void execute(uint64_t n) {
  for (;n > 0; n --) {
    exec_once();
    g_nr_guest_inst ++;
    if (npc_state.state != NPC_RUNNING) break;//stoppoint在此处触发break跳出for循环
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  printf("host time spent = " NUMBERIC_FMT " us", g_timer);
  printf("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) printf("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else printf("Finish running in less than 1 us and can not calculate the simulation frequency");
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
          npc_state.pc);
      // fall through
    case NPC_QUIT: statistic();
  }
}
