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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_DECIMAL,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // multiply
  {"/", '/'},           // divide
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_DECIMAL}, // decimal integer
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool check_parentheses(int p, int q) {
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    int count = 0;
    for (int i = p + 1; i < q; i++) {
      if (tokens[i].type == '(') count++;
      if (tokens[i].type == ')') count--;
      if (count < 0) return false;
    }
    return count == 0;
  }
  return false;
}

static int eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
    /*
      这种情况出现的时候运算符的左侧或者右侧没有数字，
      个人的解决方法是补0
      当为“+”或“-”的左侧没有数字时，补0，当为“*”或“/”的左侧没有数字时，报错
      当为“+”或“-”的右侧没有数字时，报错，当为“*”或“/”的右侧没有数字时，报错
      暂时没想到其他的情况*/
      if((tokens[p].type=='+'||tokens[p].type=='-')&&(tokens[q].type=='(')){
        return 0;
      }
      else {
        printf("Bad expression in p(%d) and q(%d), Both sides of the symbol are not full numbers\n", p, q);
        assert(0);
      }
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if (tokens[p].type == TK_DECIMAL) {
      return atoi(tokens[p].str);
    }
    else {
      /* Bad expression */
      printf("Bad expression: single token is not a number\n");
      return 0;
    }
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int parenthesis_flag = 0;
    bool sum_sub_flag = false;
    int op = -1; // the position of 主运算符 in the token expression
    for (int i = p; i <= q; i++) {
      if (tokens[i].type == '(') parenthesis_flag++;
      if (tokens[i].type == ')') parenthesis_flag--;

      if (parenthesis_flag == 0) {
        if (tokens[i].type == '+' || tokens[i].type == '-') {
          op = i;
          sum_sub_flag = true;
        }
        else if (!sum_sub_flag && (tokens[i].type == '*' || tokens[i].type == '/')) {
          op = i;
        }
      }
    }
    printf("op = %d\n", op);

    if (op == -1) {
      printf("Bad expression: no operator found\n");
      return 0;
    }

    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);
    int op_type = tokens[op].type;

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
}


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  bool negative_flag = false;
  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          //这种处理负数的方法并没有把负数当作一个整体，只是在负号前加上了一个括号，更优解法在思考中
          case '-':
            if(tokens[nr_token-1].type == '+'|| tokens[nr_token-1].type == '-' || tokens[nr_token-1].type == '*' || tokens[nr_token-1].type == '/' || nr_token == 0){
              negative_flag = true;
              break;
            } else {
              negative_flag = false;
            }
          case TK_DECIMAL:
            if(negative_flag){
              tokens[nr_token].type = '(';
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "(");
              nr_token++;

              tokens[nr_token].type = '-';
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "-");
              nr_token++;

              tokens[nr_token].type = TK_DECIMAL;
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "%.*s", substr_len, substr_start);
              nr_token++;

              tokens[nr_token].type = ')';
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), ")");
              tokens[nr_token].str[substr_len] = '\0';
              nr_token++;

              negative_flag = false;
              break;
            } 
          case '+':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_EQ:
          
            tokens[nr_token].type = rules[i].token_type;
            
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            
            nr_token++;
            break;
          default:
            printf("Unknown token type: %d\n", rules[i].token_type);
            return false;
        }

        break;
      }
    }

    //printf("tokens[%d].type = %d\n", nr_token-1, tokens[nr_token-1].type);
    //printf("tokens[%d].str = %s\n", nr_token-1, tokens[nr_token-1].str);

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  printf("nr_token = %d\n", nr_token);
  //打印所有token的str
  for(int i = 0; i < nr_token; i++){
    printf("%-*s ", 4, tokens[i].str);
  }
  printf("\n");
  for(int i = 0; i < nr_token; i++){
    printf("%-*d ", 4, i);
  }
  printf("\n");
  printf("equal to %d\n",eval(0, nr_token-1));
  
  return true;
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
}
