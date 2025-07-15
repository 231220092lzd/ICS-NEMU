/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
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
#include <memory/paddr.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
int findmain(int p,int q);
bool check_parentheses(int p,int q);
uint32_t eval(int p,int q);
struct mystack{ 
	char stack[1000];
	int top;
};
enum {
  TK_NOTYPE = 256, TK_EQ=0, INT_NUMBER=1,AND=2,TK_NEQ=3,REG=4,HEX=5,DEREF=6,MINUS=7

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"\\&\\&",AND},      
  {"\\!\\=",TK_NEQ},      
  {"\\$+[a-zA-Z]*[0-9]*",REG},      
  {"0x[0-9a-f]+",HEX},      
  {"\\)",')' },	
  {"\\(",'(' },	
  {"[0-9]+",INT_NUMBER}, //int 
  {"\\*",'*' },         //multiplication
  {"\\/",'/' },		//division
  {"\\-",'-' },           //minus
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\=\\=", TK_EQ},        // equal
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

static Token tokens[20000] __attribute__((used)) = {};//记得改回大小
static int nr_token __attribute__((used))  = 0;
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
       // char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

       /* Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);*/

        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {		
	        case 256:break;
          case INT_NUMBER:case HEX:tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
            tokens[nr_token++].str[substr_len]='\0';
            break;
          case REG: tokens[nr_token].type=rules[i].token_type;
            strncpy(tokens[nr_token].str,&e[position-substr_len+1],substr_len-1);
            tokens[nr_token++].str[substr_len-1]='\0';
            break;
          default:tokens[nr_token++].type=rules[i].token_type;break;	
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }else{ 
    *success=true;
  }
  /* TODO: Insert codes to evaluate the expression. */
  for(int i=0;i<nr_token;i++){
    if(i==0){
      if(tokens[i].type=='*'){ 
        tokens[i].type=DEREF;
      }else if(tokens[i].type=='-'){ 
        tokens[i].type=MINUS;
      }        
      continue;
    }      
    if((tokens[i-1].type!=HEX&&tokens[i-1].type!=REG&&tokens[i-1].type!=INT_NUMBER&&tokens[i-1].type!=')')){ 
        if(tokens[i].type=='*'){ 
            tokens[i].type=DEREF;
        }else if(tokens[i].type=='-'){ 
            tokens[i].type=MINUS;
        }
    }
  }
  return eval(0,nr_token-1);
}

uint32_t eval(int p,int q){
	if(p>q){
      assert(0);
	}else if(p==q){
	  uint32_t final;
    switch(tokens[p].type){ 
            case INT_NUMBER:sscanf(tokens[p].str,"%u",&final);break;
            case HEX:sscanf(tokens[p].str,"%x",&final);break;
            case REG:bool tsuccess=true;final=isa_reg_str2val(tokens[p].str,&tsuccess);
                     if(tsuccess==false){ 
                        printf("cant read reg");
                        assert(0);
                     }
                     break;      
            default:printf("token type wrong");assert(0);
    }
		return final;
	}else if(check_parentheses(p,q)==true){      
		return eval(p+1,q-1);
	}else if(p==q-1){
    if(tokens[p].type==DEREF){ 
      paddr_t final;      
      sscanf(tokens[q].str,"%x",&final);
      uint32_t output=paddr_read(final,4);
      return output;
    }else if(tokens[p].type==MINUS){ 
      uint32_t final;
      sscanf(tokens[q].str,"%u",&final);
      return (uint32_t)-final;
    }
  }else{      
    int op=findmain(p,q);
    if(op==0){ 
      if(tokens[0].type==DEREF){ 
        return (uint32_t)paddr_read((paddr_t)eval(p+1,q),4);
      }else if(tokens[0].type==MINUS){ 
        return (uint32_t)-eval(p+1,q);
      }
    }
		uint32_t val1=eval(p,op-1);
		uint32_t val2=eval(op+1,q);
    switch(tokens[op].type){ 
		    case '+':return val1+val2;
		    case '-':return val1-val2;
		    case '*':return val1*val2;
	      case '/':if(val2==0){     
                    assert(0);
                    return 0;
                 }
                 return val1/val2;
        case TK_EQ:if(val1==val2){ 
                    return 1;
                  }else{ 
                    return 0;  
                  }
        case TK_NEQ:if(val1==val2){ 
                      return 0;
                    }else{ 
                      return 1;
                    }
        case AND:return val1&&val2;
	  	  default:assert(0);
		}
	}
  return 0;
}

int findmain(int p,int q){
  int inbrackets=0;
	int position=-1;
 	int current=DEREF;
	for(int i=q;i>=p;i--){
		switch(tokens[i].type){ 
			case '+':case '-':if(inbrackets==0){ 
				     	  	if(current=='/'||current=='*'||current==DEREF||current==MINUS){ 
                    current='+';
                    position=i;
                  }
			     	 	  }
                break;
			case '*':case '/':if(inbrackets==0){
                if(current==DEREF||current==MINUS){  
						        position=i;
                    current=tokens[i].type;
			          }
              }  
              break;
      case ')':inbrackets++;break;
      case '(':inbrackets--;break;
      case TK_EQ:case TK_NEQ:if(inbrackets==0){ 
                    if(current!=TK_EQ&&current!=TK_NEQ){ 
                      position=i;
                      current=TK_EQ;
                    }
                 }
               break;
      case AND:if(inbrackets==0){ 
                  return i;
               }
      case DEREF:case MINUS:if(inbrackets==0){ 
                    if(current==DEREF||current==MINUS){ 
                      position=i;
                      current=tokens[i].type;
                    }
                 }
                 break;
      default:break;
		}
	}
	return position;
}

bool check_parentheses(int p,int q){
  struct mystack curstack={"",0};
  if((char)tokens[p].type!='('||(char)tokens[q].type!=')')return false;
	for(int i=p+1;i<=q-1;i++){	
		if((char)tokens[i].type=='('){
			if(curstack.top==0){ 
	       			curstack.stack[curstack.top++]='(';
				continue;	
	       		}
			if(curstack.stack[curstack.top-1]=='('){ 
				curstack.stack[curstack.top++]='(';
			}
		}else if((char)tokens[i].type==')'){
		        if(curstack.top==0){ 
				curstack.stack[curstack.top++]=')';
				continue;
			}	
			if(curstack.stack[curstack.top-1]==')'){
				curstack.stack[curstack.top++]=')';
			}else{ 
				curstack.top--;
			}
		}else{ 
			continue;
		}
	}
	if(curstack.top!=0){ 
		  return false;
	}else{ 
		  return true;
	}
}

