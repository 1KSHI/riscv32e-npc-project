#include <regex.h>
#include <memory/paddr.h>
enum {
  TK_NOTYPE = 256, TK_HEX,TK_NEGATIVE,TK_DECIMAL,TK_EQ,TK_NEQ,TK_AND,TK_REG,DEREF

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
  {"0x[0-9a-fA-F]+", TK_HEX}, // hexadecimal number
  {"-[0-9]+", TK_NEGATIVE}, // decimal integer
  {"[0-9]+", TK_DECIMAL}, // decimal integer
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"\\*", '*'},         // multiply
  {"/", '/'},           // divide
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"\\$[$a-zA-Z0-9]+", TK_REG}, // register name
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

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;



static bool check_parentheses(int p, int q) {
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    int count = 0;
    for (int i = p + 1; i < q; i++) {
      if (tokens[i].type == '(') count++;
      else if (tokens[i].type == ')') count--;
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
      暂时没想到其他的情况
    */
    if((tokens[p].type=='+'||tokens[p].type=='-')||(tokens[q].type=='(')){
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
    switch (tokens[p].type) {
      case TK_DECIMAL:
      case TK_NEGATIVE:
        return atoi(tokens[p].str);
      case TK_HEX:
        return (unsigned int)strtol(tokens[p].str, NULL, 0);
      case TK_REG: {
        bool success = true;
        word_t result = isa_reg_str2val(tokens[p].str + 1, &success);
        if (success) {
          return result;
        } else {
          printf("Bad expression: register name not found\n");
          return 0;
        }
      }
      default:
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
    int op = -1; // the position of 主运算符 in the token expression
    for (int i = p; i <= q; i++) {
      if (tokens[i].type == '(') parenthesis_flag++;
      if (tokens[i].type == ')') parenthesis_flag--;

      if (parenthesis_flag == 0) {
        switch (tokens[i].type) {
          case TK_EQ:
          case TK_AND:
          case TK_NEQ:
            op = i;
            break;
          case '+':
          case '-':
            if (op == -1 || (tokens[op].type != TK_EQ && tokens[op].type != TK_AND && tokens[op].type != TK_NEQ)) {
              op = i;
            }
            break;
          case '*':
          case '/':
            if (op == -1 || (tokens[op].type != TK_EQ && tokens[op].type != TK_AND && tokens[op].type != TK_NEQ && tokens[op].type != '+' && tokens[op].type != '-')) {
              op = i;
            }
            break;
          case DEREF:
            if (op == -1) {
              op = i;
            }
            break;
        }
      }
    }

    if (op == -1) {
      printf("Bad expression: no operator found\n");
      return 0;
    }

    int val1 = (tokens[op].type != DEREF) ? eval(p, op - 1) : 0;
    int val2 = eval(op + 1, q);
    int op_type = tokens[op].type;

    switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case DEREF: return paddr_read(val2, 4);
      default: assert(0);
    }
  }
}


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  //bool negative_flag = false;
  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          
          //这种处理负数的方法并没有把负数当作一个整体，只是在负号前加上了一个括号，更优解法在思考中（11月11日）
          //原来可以通过调整re编译的优先级来直接识别出负数，害（11月12日）
          case TK_NEGATIVE:
            if(tokens[nr_token-1].type == TK_DECIMAL || tokens[nr_token-1].type == TK_HEX || tokens[nr_token-1].type == TK_NEGATIVE || tokens[nr_token-1].type == ')'){
              tokens[nr_token].type = '+';
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "+");
              nr_token++;
              tokens[nr_token].type = TK_NEGATIVE;
              snprintf(tokens[nr_token].str, sizeof(tokens[nr_token].str), "%.*s", substr_len, substr_start);
              nr_token++;
              break;
            }
          case TK_DECIMAL:
          case '-':
          case '+':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_EQ:
          case TK_HEX:
          case TK_NEQ:
          case TK_AND:
          case TK_REG:
          
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
  // printf("nr_token = %d\n", nr_token);
  // //打印所有token的str
  // for(int i = 0; i < nr_token; i++){
  //   printf("%-*s ", 4, tokens[i].str);
  // }
  // printf("\n");
  // for(int i = 0; i < nr_token; i++){
  //   printf("%-*d ", 4, i);
  // }
  // printf("\n");
  // printf("————————————————————\n");
  
  return true;
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for (int i = 0; i < nr_token; i ++) {
    if(tokens[i].type == '*' && 
        ( i == 0 || 
          (
          tokens[i - 1].type != TK_HEX && 
          tokens[i - 1].type != TK_NEGATIVE && 
          tokens[i - 1].type != TK_DECIMAL && 
          tokens[i - 1].type != ')' && 
          tokens[i - 1].type != TK_REG
          )
        ) 
      ) {
      tokens[i].type = DEREF;
    }
  }

  return eval(0, nr_token-1);
}
