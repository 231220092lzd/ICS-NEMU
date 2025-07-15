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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expression[20000];
  int value_before;
  int value_new;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){ 
  if(head==NULL){ 
    head=free_;
    free_=free_->next;
    head->next=NULL;
  }else{
    if(free_==NULL){
      printf("NO MORE FREE WP\n");      
      return NULL;
    }
    WP* current=free_;
    free_=free_->next;
    current->next=head;
    head=current;
  }
  return head;
}

void free_wp(WP *wp){ 
  WP* current=head;
  if(current->NO==wp->NO){ 
    head=head->next;
    current->next=free_;
    free_=current;
    return;
  }
  while(wp->NO!=current->next->NO){ 
    current=current->next;
    if(current->next==NULL){ 
      printf("NO WP TO FREE\n");
      return;
    }
  }
  WP* middle=current->next;
  current->next=middle->next;
  middle->next=free_;
  free_=middle;
}

void set_wp(char* args){
  bool success=false;
  uint32_t final=expr(args,&success);
  if(success==true&&new_wp()!=NULL){
    strcpy(head->expression,args);  
    head->value_before=final;
    head->value_new=final;
  }else{ 
    printf("cant expr when setting point\n");
    return;
  }

}

void delete_wp(int n){ 
  WP* current=head;
  if(head==NULL){ 
    printf("no watchpoint");
  }
  while(current->NO!=n){ 
    current=current->next;
    if(current==NULL){ 
      printf("no watchpoint %d to delete\n",n);
    }
  }
  free_wp(current);
}


void print_wp(){
  WP* current=head;
  if(head==NULL){ 
    printf("no watchpoint\n");
  }
  while(current!=NULL){ 
    printf("watchpoint:%d,expr:%s,value_before:%u,value_new:%u\n",current->NO,current->expression,current->value_before,current->value_new);
    current=current->next;
  }

}

void diff(){
  WP* current=head;
  while(current!=NULL){ 
    bool success=true;      
    int final=expr(current->expression,&success);
    if(success!=true){ 
      printf("fail when check point");
      assert(0);
    }
    current->value_new=final;
    if(final!=current->value_before){ 
      printf("value_before:%u,value_new%u\n",current->value_before,final);
      current->value_before=current->value_new;
      nemu_state.state=NEMU_STOP;
      return;
    }
    current=current->next;
  }
  return;
} 
 

/* TODO: Implement the functionality of watchpoint */

