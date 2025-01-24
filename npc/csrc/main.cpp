#include "verilated.h"
#include "verilated_vcd_c.h"
#include <Vtop.h>//user set

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vtop* top;//user set

uint32_t mem[64]={};

void step_and_dump_wave(){
  top->eval();
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

void sim_init(){
  contextp = new VerilatedContext;
  tfp = new VerilatedVcdC;
  top = new Vtop;

  contextp->traceEverOn(true);
  top->trace(tfp, 0);
  tfp->open("dump.vcd");
}

static void single_cycle() {
  top->clk = 0; 
  step_and_dump_wave();
  top->clk = 1; 
  step_and_dump_wave();
}

static void reset(int n) {
  top->rst = 1;
  while (n -- > 0) single_cycle();
  top->rst = 0;
}

static uint32_t pmem_read(uint32_t addr) {
  return mem[addr];
}

void sim_exit(){

  step_and_dump_wave();

  top->final();
  delete top;
  tfp->close();
  delete contextp;
}

int main() {
  sim_init();
  top->a=1;
  top->b=0;
  top->cin=1;
  single_cycle();
  sim_exit();

  return 0;
}

