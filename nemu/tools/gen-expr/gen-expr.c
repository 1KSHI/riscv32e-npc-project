#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
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

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    do {
      buf_pos = 0;
      paren_depth = 0;
      gen_rand_expr();
    } while (buf_pos > 128);

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

    printf("%d %s\n", result, buf);
  }
  return 0;
}