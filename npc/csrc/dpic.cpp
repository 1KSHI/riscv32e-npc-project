#include "include/include.h"
#include "verilated_dpi.h"
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

extern "C" void check_regfile(const uint64_t* regf,int pc){
  const int REG_NUM = 32;
  cpu.pc = pc ;
  for (int i = 0; i < REG_NUM; i++) {
    cpu.reg[i] = regf[i];
  }
}

extern "C" int pmem_read(int raddr) {
  // 总是读取地址为`raddr & ~0x3u`的4字节返回
  paddr_t aligned_addr = raddr & ~0x3u; // 对地址进行对齐
  return paddr_read(aligned_addr, 4);   // 调用 paddr_read 读取 4 字节
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  // 总是往地址为`waddr & ~0x3u`的4字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
  paddr_t aligned_addr = waddr & ~0x3u; // 对地址进行对齐
  for (int i = 0; i < 4; i++) {
    if (wmask & (1 << i)) { // 检查掩码的每一位
      uint8_t byte_data = (wdata >> (i * 8)) & 0xFF; // 提取对应字节
      paddr_write(aligned_addr + i, 1, byte_data);   // 写入单字节数据
    }
  }
}