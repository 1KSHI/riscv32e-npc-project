#include "include/include.h"
#include "verilated_dpi.h"
extern Vtop* top;
extern NPC_state npc_state;
extern bool diff_skip;
extern bool log_enable();
// MMIO 映射表
typedef struct {
  const char* name;
  paddr_t start;
  paddr_t end;
} mmio_map_t;

mmio_map_t mmio_maps[] = {
  {"serial"    , 0x10000000, 0x10000007},
  {"rtc"       , 0x10002000, 0x10002007},
  {"vgactl"    , 0x10003000, 0x10003007},
  {"vmem"      , 0x11000000, 0x110752ff},
  {"keyboard"  , 0x10001000, 0x10001003},
  {"audio"     , 0x10004000, 0x10004017},
  {"audio-sbuf", 0x11200000, 0x1120ffff}
};

// mmio_map_t mmio_maps[] = {
//   {"serial"  , 0xa00003f8, 0xa00003ff},
//   {"rtc"     , 0xa0000048, 0xa000004f},
//   {"vgactl"  , 0xa0000100, 0xa0000107},
//   {"vmem"    , 0xa1000000, 0xa10752ff},
//   {"keyboard", 0xa0000060, 0xa0000063},
// };




enum device_type{
  SERIAL,
  RTC,
  VGACTL,
  VMEM,
  KEYBOARD,
  AUDIO,
  AUDIO_SBUF
};


#define NUM_MMIO_MAPS (sizeof(mmio_maps) / sizeof(mmio_map_t))

enum device_type check_mmio_device(paddr_t addr) {
  for (int i = 0; i < NUM_MMIO_MAPS; i++) {
    if (addr >= mmio_maps[i].start && addr <= mmio_maps[i].end) {
      return static_cast<device_type>(i); // 返回对应的设备类型
    }
  }
  return static_cast<device_type>(-1); // 如果地址不属于任何 MMIO 范围，返回无效值
}

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
  enum device_type dev = check_mmio_device(raddr);
  uint64_t data;
  if (dev != static_cast<device_type>(-1)) {
    switch (dev) {
      case SERIAL:break;
      case RTC:data = paddr_read(raddr, 4);break;
      case VGACTL:break;
      case VMEM:break;
      case KEYBOARD:paddr_read(raddr, 4);break;
      case AUDIO:break;
      case AUDIO_SBUF:break;
      default:printf("Unknown device type\n");break;
    }
    #if CONFIG_DTRACE
    log_write("read  device: %s, addr = %08x, data = %02x\n", mmio_maps[dev].name, raddr, data);
    #endif
    diff_skip = true;
  }else{
    paddr_t aligned_addr = raddr & ~0x3u;
    paddr_t data_1 = paddr_read(aligned_addr, 4);
    paddr_t data_2 = paddr_read(aligned_addr + 4, 4);
    data = (data_2 << 32) | data_1;
  }
  //printf("对齐前地址 = %08x, 对齐后地址 = %08x, data = %016x\n", raddr,aligned_addr, data);
  return data;
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  paddr_t aligned_addr = waddr & ~0x3u;
  int count = 0;
  enum device_type dev = check_mmio_device(waddr);
  if (dev != static_cast<device_type>(-1)) {
    switch (dev) {
      case SERIAL:break;
      case RTC:break;
      case VGACTL:break;
      case VMEM:break;
      case KEYBOARD:break;
      case AUDIO:break;
      case AUDIO_SBUF:break;
      default:printf("Unknown device type\n");break;
    }
    #if CONFIG_DTRACE
    log_write("write device: %s, addr = %08x, data = %02x\n", mmio_maps[dev].name, waddr, wdata);
    #endif
    diff_skip = true;
  }
  for (int i = 0; i < 8; i++) {
    if (wmask & (1 << i)) { 
      uint8_t byte_data = (wdata >> (count * 8)) & 0xFF;
      paddr_write(aligned_addr + i, 1, byte_data);
      count++;
    }
  }
  //printf("对齐前地址 = %08x, 对齐后地址 = %x, wdata = %x, wmask = %x\n", waddr, aligned_addr, wdata, wmask);
}