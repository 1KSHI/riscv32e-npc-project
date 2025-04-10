#include "include/tb_common.h"


extern VerilatedContext* contextp;
extern VerilatedVcdC* tfp;
extern Vtop* top;


void step_wave(){
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void single_cycle() {
    top->clk = 0; 
    step_wave();
    top->clk = 1; 
    step_wave();
}

  
void reset(int n) {
    top->rst = 1;
    while (n -- > 0) single_cycle();
    top->rst = 0;
}

void sim_init(){
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vtop;
  contextp->traceEverOn(true);
  top->trace(tfp, 10);
  tfp->open("dump.vcd");
}

void sim_exit(){
  step_wave();
  top->final();
  delete top;
  tfp->close();
  delete contextp;
}