#include "include/tb_common.h"
#include "include/include.h"

NPC_state npc_state = {NPC_RUNNING,BAD_TRAP};
CPU_file cpu;
VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
Vtop* top;

void sdb_mainloop();

void watch_dog() {
  static int count = 0;
  count ++;
  if (count > 100) {
      printf("watch dog timeout\n");
      sim_exit();
  }
}

int main(int argc, char *argv[]) {
    sim_init();
    reset(1);
    npc_init(argc, argv);
    while (npc_state.state == NPC_RUNNING) {
        sdb_mainloop();
    }  
    sim_exit();

    return 0;
}
