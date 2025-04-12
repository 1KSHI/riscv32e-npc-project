#include "include/include.h"
#include "verilated_dpi.h"
extern Vtop* top;
extern NPC_state npc_state;

extern CPU_file cpu;
extern "C" void check_finsih(int ins,int a0zero){
  if(ins == 0x100073){
    npc_state.state = NPC_END;
    npc_state.trap  = a0zero?GOOD_TRAP:BAD_TRAP;
  }
  else
    npc_state.state = NPC_RUNNING;
}

extern "C" void check_regfile(const uint64_t* regf,int pc,int inst){
  const int REG_NUM = 32;
  cpu.pc = pc ;
  cpu.inst = inst;
  for (int i = 0; i < REG_NUM; i++) {
    cpu.reg[i] = regf[i];
  }
}

extern "C" long pmem_read(int raddr){
  paddr_t aligned_addr = raddr & ~0x3u;
  paddr_t data_1 = paddr_read(aligned_addr, 4);
  paddr_t data_2 = paddr_read(aligned_addr + 4, 4);
  uint64_t data = (data_2 << 32) | data_1;
  //printf("对齐前地址 = %08x, 对齐后地址 = %08x, data = %016x\n", raddr,aligned_addr, data);
  return data;
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  paddr_t aligned_addr = waddr & ~0x3u;
  int count = 0;
  //printf("对齐前地址 = %08x, 对齐后地址 = %x, wdata = %x, wmask = %x\n", waddr, aligned_addr, wdata, wmask);
  for (int i = 0; i < 8; i++) {
    if (wmask & (1 << i)) { 
      uint8_t byte_data = (wdata >> (count * 8)) & 0xFF;
      paddr_write(aligned_addr + i, 1, byte_data);
      //printf("aligned_addr = %x, byte_data = %x\n", aligned_addr + i, byte_data);
      count++;
    }
  }
}