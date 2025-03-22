#include "include/include.h"
#include "verilated_dpi.h"

extern bool system_exit;
extern bool good_trap;
extern regfile dut_reg;
extern "C" void check_finsih(int ins,int a0zero){
  if(ins == 0x100073){
    system_exit = true;
    good_trap = a0zero;
  }
  else
    system_exit = false;
}

extern "C" void check_regfile(const uint64_t* regf,int pc){
  const int REG_NUM = 32;
  dut_reg.pc = pc ;
  for (int i = 0; i < REG_NUM; i++) {
    dut_reg.x[i] = regf[i];
  }
}