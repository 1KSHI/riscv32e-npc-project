#include "include/include.h"
#include <common.h>
#include <getopt.h>
char *img_file = NULL;
static char *log_file = NULL;
static char *diff_so_file = NULL;



extern void init_disasm(); // 初始化反汇编器
static int parse_args(int argc, char *argv[]);
static long load_img(char *img_file);
void init_sdb();

void npc_init(int argc, char *argv[]) {
  // Parse arguments.
  parse_args(argc, argv);

  init_disasm();

  // /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img(img_file);

  init_log(log_file);

  init_sdb();

  #if  DIFFTEST_ON
  // Initialize differential testing.
  difftest_init(diff_so_file, img_size);
  #endif

}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"img"      , required_argument, NULL, 'i'},
    {"diff"     , required_argument, NULL, 'd'},
    {"log"      , required_argument, NULL, 'l'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-d:i:l", table, NULL)) != -1) {
    switch (o) {
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'i': img_file = optarg; break;
    }
  }
  return 0;
}

extern uint8_t pmem[CONFIG_MSIZE];  // from paddr.c, use for load_img.
static long load_img(char *img_file) {
  if (img_file == NULL) {
    printf("No image is given. Use the default build-in image.\n");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  if(fp == NULL){
    printf("Can not open '%s'\n", img_file);
    assert(0); 
  }

  fseek(fp, 0, SEEK_END); // move cur to end.
  long size = ftell(fp);
  printf("\n------- image info -------\n\n");
  printf("The image is %s\n", img_file);
  printf("The size  is %ld\n", size);
  printf("\n------- memory info ------\n\n");
  fseek(fp, 0, SEEK_SET);
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  for(uint32_t i=0;i<size;i=i+4){
    printf("0x%08x, 0x%08x\n",PMEM_START+i,paddr_read(PMEM_START+i,4));
  }
  printf("\n-------- reg info --------\n\n");

  fclose(fp);
  return size;
}
