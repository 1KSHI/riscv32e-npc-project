#include "include/tb_common.h"
#include <Vtop.h>//user set
#include "include/include.h"

bool system_exit  = false;
bool good_trap    = false;
int count         = 0;
TESTBENCH<Vtop> *__TB__;
//inst_type
//R-type
//000000000010 00001 000 00000 0010011

uint32_t mem[64]={0x00208093,0x00308093,0x00408093,0x00508093,0x00308093,0x00408093,0x00508093,0x00100073};

static uint32_t pmem_read(uint32_t addr) {
  return mem[addr];
}

static void watch_dog() {
    count++;
    if (count > 100) {
        printf("watch dog timeout\n");
        TB(~TESTBENCH());
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    __TB__ = new TESTBENCH<Vtop>(argc, argv);
    TB(sim_init());
    TB(DUT(clk)=0);
    TB(sim_reset());
    npc_init(argc, argv);
    while (!system_exit) {
        TB(DUT(inst) = paddr_read(TB(DUT(pc)),4));
        print_regs();
        watch_dog();
        TB(cycles(1));
    }  
    switch(good_trap){
    case 1: printf("\n----------EBREAK: HIT !! GOOD !! TRAP!!---------------\n\n"); break;
    case 0: printf("\n----------EBREAK: HIT !! BAD  !! TRAP!!---------------\n\n"); break;
    }
    TB(~TESTBENCH());
    exit(EXIT_SUCCESS);
}
