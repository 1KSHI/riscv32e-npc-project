#include "include/include.h"
#include "verilated_dpi.h"
extern npc_s npc_state;

extern regfile dut_reg;
extern "C" void check_finsih(int ins,int a0zero){
  if(ins == 0x100073){
    npc_state.state = NPC_QUIT;
    npc_state.trap  = a0zero?GOOD_TRAP:BAD_TRAP;
  }
  else
    npc_state.state = NPC_RUNNING;
}

extern "C" void check_regfile(const uint64_t* regf,int pc){
  const int REG_NUM = 32;
  dut_reg.pc = pc ;
  for (int i = 0; i < REG_NUM; i++) {
    dut_reg.x[i] = regf[i];
  }
}