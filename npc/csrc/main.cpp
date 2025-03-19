#include "include/tb_common.h"
#include <Vysyx_24110026_top.h>//user set
#include "include/include.h"

TESTBENCH<Vysyx_24110026_top> *__TB__;
//inst_type
//R-type
//000000000010 00001 000 00000 0010011

uint32_t mem[64]={0x00208093,0x00308093,0x00408093,0x00508093,0x00308093,0x00408093,0x00508093,0x00100073};

static uint32_t pmem_read(uint32_t addr) {
  return mem[addr];
}

extern "C" void ebreak() {
    TB(~TESTBENCH());
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    __TB__ = new TESTBENCH<Vysyx_24110026_top>(argc, argv);
    TB(sim_init());
    TB(sim_reset());
    npc_init(argc, argv);
    for(int i=0;i<10;i++){
        TB(DUT(inst) = pmem_read((TB(DUT(pc))-0x80000000)/4));
        TB(cycles(1));
    }
    TB(cycles(10));

    
    //exit(EXIT_SUCCESS);
}
