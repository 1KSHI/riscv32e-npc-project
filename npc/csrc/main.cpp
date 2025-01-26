#include "tb_common.h"
#include <Vysyx_24110026_top.h>//user set

TESTBENCH<Vysyx_24110026_top> *__TB__;
int main(int argc, char *argv[]) {
    __TB__ = new TESTBENCH<Vysyx_24110026_top>(argc, argv);
    TB(sim_init());
    TB(sim_reset());
    TB(DUT(inst) = 0x006101B3);
    TB(cycles(10));

    TB(~TESTBENCH());
    exit(EXIT_SUCCESS);
}

// VerilatedContext* contextp = NULL;
// VerilatedVcdC* tfp = NULL;

// static Vysyx_24110026_top* top;//user set

// uint32_t mem[64]={};

// void step_and_dump_wave(){
//   top->eval();
//   contextp->timeInc(1);
//   tfp->dump(contextp->time());
// }

// void sim_init(){
//   contextp = new VerilatedContext;
//   tfp = new VerilatedVcdC;
//   top = new Vysyx_24110026_top;

//   contextp->traceEverOn(true);
//   top->trace(tfp, 5);
//   tfp->open("dump.vcd");
// }

// static void single_cycle() {
//   top->clk = 0; 
//   step_and_dump_wave();
//   top->clk = 1; 
//   step_and_dump_wave();
// }

// static void reset(int n) {
//   top->rst = 1;
//   while (n -- > 0) single_cycle();
//   top->rst = 0;
// }

// static uint32_t pmem_read(uint32_t addr) {
//   return mem[addr];
// }

// void sim_exit(){

//   step_and_dump_wave();

//   top->final();
//   delete top;
//   tfp->close();
//   delete contextp;
// }

// int main() {
//   sim_init();
//   single_cycle();
//   single_cycle();
//   top->inst = 0x00000064;
//   single_cycle();
//   single_cycle();
//   single_cycle();
//   single_cycle();
//   sim_exit();

//   return 0;
// }

