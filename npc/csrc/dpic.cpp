#include "include/include.h"
#include "verilated_dpi.h"
extern NPC_state npc_state;

extern CPU_state cpu;
extern "C" void check_finsih(int ins,int a0zero){
  if(ins == 0x100073){
    npc_state.state = NPC_END;
    npc_state.trap  = a0zero?GOOD_TRAP:BAD_TRAP;
  }
  else
    npc_state.state = NPC_RUNNING;
}

extern "C" void check_regfile(const uint64_t* regf,int pc){
  const int REG_NUM = 32;
  cpu.pc = pc ;
  for (int i = 0; i < REG_NUM; i++) {
    cpu.reg[i] = regf[i];
  }
}