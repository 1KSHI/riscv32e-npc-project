#include "include/tb_common.h"
#include "include/include.h"

NPC_state npc_state = {NPC_RUNNING,BAD_TRAP};// check npc state
CPU_file cpu;// follow the pc reg change in npc

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
Vtop* top;

extern int is_batch_mode;

void sdb_mainloop();

int watch_dog() {
  static int count = 0;
  count ++;
  if (count > 100000) {
      printf("watch dog timeout\n");
      return 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
    sim_init();
    reset(1);
    npc_init(argc, argv);
    while (npc_state.state == NPC_RUNNING) {
        sdb_mainloop();
    }  
    if(is_batch_mode && npc_state.trap == BAD_TRAP) {
        return 1;
    }
    sim_exit();

    return 0;
}
