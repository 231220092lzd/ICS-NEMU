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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int position=0;
static void gen_num(){
  int number=rand()%99+1;
  int middle[2]={-1,-1};
  for(int i=1;i>=0;i--){ 
    middle[i]=number%10;
    number/=10;
    if(number==0){ 
      break;
    }
  }
  for(int i=0;i<=1;i++){
    if(middle[i]!=-1){ 
      buf[position++]='0'+middle[i];
    }
  }
  buf[position++]='u';
}
static void gen(char c){ 
  buf[position++]=c;

}
static void gen_rand_op(){
  char c;
  switch(rand()%4){ 
    case 0:c='+';break;
    case 1:c='-';break;
    case 2:c='*';break;
    case 3:c='/';break;
    default:c='+';
  }
  buf[position++]=c;
}

static void gen_rand_expr() {
  int myseed;
  if(position>100){ 
    myseed=0;
  }else{ 
    myseed=rand()%3;
  }
  switch(myseed){
    case 0:gen_num();
           buf[position]='\0';
           break;
    case 1:gen('(');gen_rand_expr();gen(')');
           buf[position]='\0';
           break;
    default:gen_rand_expr();gen_rand_op();
            if(buf[position-1]=='/'){ 
              gen_num();
            }else{ 
              gen_rand_expr();
            }
            buf[position]='\0';
  }
}


int main(int argc, char *argv[]) {
  int seed=time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    position=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    uint32_t result;
    ret = fscanf(fp, "%u", &result);
    pclose(fp);

    printf("%s %u\n", buf, result);
  }
  return 0;
}
