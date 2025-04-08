#ifndef __TB_COM__
#define __TB_COM__

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <Vtop.h>


void step_wave();
void single_cycle();
void reset(int n);
void sim_init();
void sim_exit();


#endif