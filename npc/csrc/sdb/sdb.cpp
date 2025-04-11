#include <readline/readline.h>
#include <readline/history.h>
#include "include.h"
#include "sdb.h"
#include <memory/paddr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_batch_mode = false;
extern NPC_state npc_state;
void init_regex();
void init_wp_pool();


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(npc) ");

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
    arg = strtok(NULL, " ");
    if(arg == NULL){
      print_regs(true);
    }else{
      print_one_regs(arg);
    }
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
    
  }

  WP *wp = new_wp();
  if (wp == NULL) {
      printf("No enough space for a new watchpoint\n");
      return 0;
  }

  char *arg = strtok(args, "==");
  printf("arg:%s\n",arg);

  if(strcmp(arg, "&pc") == 0) {//断点判断
    printf("pc\n");
    arg = strtok(NULL, "==");
    printf("arg:%s\n",arg);
    wp->state = true;
  }else{
    wp->state = false;
  }

  strncpy(wp->expr, arg, sizeof(wp->expr) - 1);
  wp->expr[sizeof(wp->expr) - 1] = '\0';
  wp->last_value = expr(arg, NULL);
  wp->state?printf("Stoppoint %d: %s\n", wp->NO, wp->expr):printf("Watchpoint %d: %s\n", wp->NO, wp->expr);

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

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { 
          npc_state.state = NPC_QUIT;
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
