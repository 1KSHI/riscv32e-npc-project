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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "../../isa/riscv32/local-include/reg.h"
#include "sdb.h"
#include <memory/paddr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_batch_mode = false;

//value of the expression test
static char buf[65536] = {};
static char code_buf[65536 + 128] = {};
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int buf_pos = 0;
static int paren_depth = 0;

void init_regex();
void init_wp_pool();

static int choose(int n) {
  return rand() % n;
}

static void gen_num() {
  int num = rand() % 100;
  buf_pos += sprintf(buf + buf_pos, "%d", num);
}

static void gen(char c) {
  buf[buf_pos++] = c;
  buf[buf_pos] = '\0';
}

static void gen_rand_op() {
  char ops[] = "+-*/";
  char op = ops[choose(4)];
  gen(op);
}

static void gen_rand_expr() {
  if (paren_depth > 10) {
    gen_num();
    return;
  }

  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: 
      gen('('); 
      paren_depth++;
      gen_rand_expr(); 
      gen(')'); 
      paren_depth--;
      break;
    default: 
      gen_rand_expr(); 
      gen_rand_op(); 
      if (buf[buf_pos - 1] == '/') {
        int num;
        do {
          num = rand() % 100;
        } while (num == 0); // 确保除数不为零
        buf_pos += sprintf(buf + buf_pos, "%d", num);
      } else {
        gen_rand_expr();
      }
      break;
  }
}

static void gen_rand_expr_test(int loop) {
  for (int i = 0; i < loop; i ++) {
    do {
      buf_pos = 0;
      paren_depth = 0;
      gen_rand_expr();
    } while (buf_pos > 32);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);
    bool success = true;
    int result_cal = expr(buf, &success);
    
    if(result_cal==result)printf("success   ");
    else printf("fail      ");
    printf("%s\n",buf);
  }
  
}


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    cpu_exec(1);
  } else {
    cpu_exec(atoi(arg));
  }
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("Unknown command 'info' without arguments\n");
    return 0;
  } else if (strcmp(arg, "r") == 0) {
    isa_reg_display();
  } else if (strcmp(arg, "w") == 0) {
    watchpoint_display();
  } else {
    printf("Unknown command 'info %s'\n", arg);
  }
  return 0;
}


static int cmd_x(char *args) {
    char *arg = strtok(args, " ");
    int N = atoi(arg);

    arg = strtok(NULL, " ");
    printf("arg:%s\n",arg);
    long unsigned int addr = (unsigned int)strtol(arg, NULL, 0);
    printf("addr:%lx\n",addr);
    static int i;
    for(i=0;i<N;i++){
    printf("%lx:%08x\n",addr,paddr_read(addr,4));
    addr += 4;
    }

    return 0;
}


static int cmd_p(char *args) {
    char *arg = strtok(args, " ");
    
    word_t result = 0;
    if (arg == NULL) {
      printf("Unknown command 'e' without arguments\n");
      return 0;
    } else if(strcmp(arg, "test") == 0) {
      arg = strtok(NULL, " ");
      int seed = time(0);
      srand(seed);
      int loop = 1;
      if (arg != NULL) {
        sscanf(arg, "%d", &loop);
      }
      printf("Test expression %d times\n", loop);
      gen_rand_expr_test(loop); 
    } else {
      bool success = true;
      result = expr(arg, &success);
      if (success) {
      // printf("————————————————————\n");
      printf("Dec = %d\n",result);
      printf("Hex = %08x\n",result);
      } else {
        printf("Invalid expression\n");
      }
    }

    return 0;
}

static int cmd_w(char *args) {
  if(args == NULL) {
      printf("No expression\n");
      return 0;
  }

  WP *wp = new_wp();
  if (wp == NULL) {
      printf("No enough space for a new watchpoint\n");
      return 0;
  }

  strncpy(wp->expr, args, sizeof(wp->expr) - 1);
  wp->expr[sizeof(wp->expr) - 1] = '\0';
  wp->last_value = expr(args, NULL);
  printf("Watchpoint %d: %s\n", wp->NO, wp->expr);

  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("Unknown command 'd' without arguments\n");
    return 0;
  }
  int NO = atoi(arg);
  if(watchpoint_delete(NO)){
    printf("Watchpoint %d deleted\n", NO);
  } else {
    printf("No watchpoint numbered %d\n", NO);
  }
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute N instructions in a single step", cmd_si },
  { "info", "Print the state of the program", cmd_info },
  { "x", "Scan Memory", cmd_x },
  { "p", "Evaluate the value of an expression", cmd_p },
  { "w", "Set a watchpoint", cmd_w },
  { "d", "Delete a watchpoint", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { 
          nemu_state.state = NEMU_QUIT;
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
