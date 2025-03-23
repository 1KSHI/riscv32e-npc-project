#include "include/tb_common.h"
#include <Vtop.h>//user set
#include "include/include.h"

NPC_state npc_state = {NPC_RUNNING,BAD_TRAP};
CPU_state cpu;
TESTBENCH<Vtop> *__TB__;

void sdb_mainloop();

void watch_dog() {
  static int count = 0;
  count ++;
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
    while (npc_state.state == NPC_RUNNING) {
        sdb_mainloop();
    }  
    TB(~TESTBENCH());
    exit(EXIT_SUCCESS);
}
