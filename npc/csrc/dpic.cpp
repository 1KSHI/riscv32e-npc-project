#include "include/include.h"
#include "verilated_dpi.h"

extern bool system_exit;
extern bool good_trap;

extern "C" void check_finsih(int ins,int a0zero){
  if(ins == 0x100073){
    system_exit = true;
    good_trap = a0zero;
  }
  else
    system_exit = false;
}