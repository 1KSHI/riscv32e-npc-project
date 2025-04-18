/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <macro.h>

#ifdef CONFIG_TARGET_AM
#include <klib.h>
#else
#include <assert.h>
#include <stdlib.h>
#endif

#if CONFIG_MBASE + CONFIG_MSIZE > 0x100000000ul
#define PMEM64 1
#endif

typedef MUXDEF(CONFIG_ISA64, uint64_t, uint32_t) word_t;
typedef MUXDEF(CONFIG_ISA64, int64_t, int32_t)  sword_t;
#define FMT_WORD "0x%08" PRIx32

typedef word_t vaddr_t;
typedef MUXDEF(PMEM64, uint64_t, uint32_t) paddr_t;
#define FMT_PADDR MUXDEF(PMEM64, "0x%016" PRIx64, "0x%08" PRIx32)
typedef uint16_t ioaddr_t;

#include <debug.h>

#define __GUEST_ISA__ riscv32
#define CONFIG_ISA64 0
#define CONFIG_TARGET_NATIVE_ELF 1
#define CONFIG_RT_CHECK 1
#define CONFIG_PC_RESET_OFFSET 0x0
#define CONFIG_MSIZE 0x8000000
#define CONFIG_MBASE 0x80000000
#define DIFFTEST_ON 1
#define CONFIG_ITRACE 1
#define CONFIG_DTRACE 1
#define CONFIG_MTRACE 1

#define CONFIG_DEVICE 1
#define CONFIG_HAS_TIMER 1
#define CONFIG_TIMER_GETTIMEOFDAY 1
#define CONFIG_HAS_SERIAL 1
#define CONFIG_HAS_KEYBOARD 1
#define CONFIG_HAS_VGA 0
#define CONFIG_HAS_DISK 0
#define CONFIG_HAS_AUDIO 0

#define DEVICE_BASE 0x10000000
#define MMIO_BASE   0x10000000

#define CONFIG_RTC_MMIO (DEVICE_BASE + 0x0002000)
#define CONFIG_SERIAL_MMIO (DEVICE_BASE + 0x0000000)
#define CONFIG_I8042_DATA_MMIO (DEVICE_BASE + 0x0001000)
#define CONFIG_FB_ADDR (MMIO_BASE   + 0x1000000)
#define CONFIG_VGA_CTL_MMIO (DEVICE_BASE + 0x0003000)
#define CONFIG_AUDIO_CTL_MMIO (DEVICE_BASE + 0x0004000)
#define CONFIG_SB_ADDR (MMIO_BASE   + 0x1200000)

// #define CONFIG_RTC_MMIO (0xa0000048)
// #define CONFIG_SERIAL_MMIO (0xa00003f8)
// #define CONFIG_I8042_DATA_MMIO (0xa0000060)
// #define CONFIG_FB_ADDR (0xa1000000)
// #define CONFIG_VGA_CTL_MMIO (0xa0000100)
// #define CONFIG_AUDIO_CTL_MMIO (0xa0000200)
// #define CONFIG_SB_ADDR (0xa1200000)


#define CONFIG_VGA_SHOW_SCREEN 1
#define CONFIG_VGA_SIZE_400x300 1
#define CONFIG_SDCARD_CTL_MMIO 0xa3000000
#define CONFIG_SDCARD_IMG_PATH "The path of sdcard image"

#define CONFIG_SB_SIZE 0x10000



#endif
