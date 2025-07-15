#include "common.h"
#include <inttypes.h>
#include <string.h>
typedef struct{
  uint8_t data[64];
  uint32_t Note;//标记位
  bool valid_bit;
  bool dirty_bit; 
}BLOCK;
BLOCK* mycache;//块内地址6
uint32_t Block_per_group;//每组有多少行
uint32_t group_number;//一共有多少组
uint32_t group_width;//组号的长度
void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;

void cycle_increase(int n) { cycle_cnt += n; }

// TODO: implement the following functions

uint32_t cache_read(uintptr_t addr) {
  uint32_t adr=(uint32_t)addr& (~0x3);
  uint32_t block_addr=adr<<(32-BLOCK_WIDTH)>>(32-BLOCK_WIDTH);//取出块内地址 OK
  uint32_t group=adr<<(32-BLOCK_WIDTH-group_width)>>(32-group_width);//取出组号 OK
  uint32_t Note_addr=adr>>(group_width+BLOCK_WIDTH);//取出标记位 OK
  for(int i=0;i<Block_per_group;i++){
    if(mycache[group*Block_per_group+i].valid_bit==true&&mycache[group*Block_per_group+i].Note==Note_addr){
      uint32_t final=0;
      /*for(int j=0;j<4;j++){
        final|=mycache[group*Block_per_group+i].data[block_addr+3-j]<<(j*8);
      }*/
     memcpy(&final,(uint8_t*)(mycache[group*Block_per_group+i].data+block_addr),4);
      return final;
    }
  }//循环结束没有则缺损
  for(int i=0;i<Block_per_group;i++){
    if(mycache[group*Block_per_group+i].valid_bit==false||i==Block_per_group-1){
      if(mycache[group*Block_per_group+i].dirty_bit==true){
        mem_write((mycache[group*Block_per_group+i].Note<<group_width)|group,mycache[group*Block_per_group+i].data);//写回
        mycache[group*Block_per_group+i].dirty_bit=false;
      }
      mem_read(adr>>BLOCK_WIDTH,mycache[group*Block_per_group+i].data);
      /*printf("adr: %x,group: %x\n",adr,(adr>>6)&0x3f);
      for(int j=0;j<8;j++){
        for(int k=0;k<8;k++){
          printf("%x ",mycache[group*Block_per_group+i].data[j*8+k]);
        }
        printf("\n");
      }
      printf("\n");*/
      mycache[group*Block_per_group+i].valid_bit=true;
      mycache[group*Block_per_group+i].Note=Note_addr;
      uint32_t final=0;
      memcpy(&final,(uint8_t*)(mycache[group*Block_per_group+i].data+block_addr),4);
      //printf("final:%x\n",final);
      //printf("%x %x %x %x\n",mycache[group*Block_per_group+i].data[block_addr],mycache[group*Block_per_group+i].data[block_addr+1],mycache[group*Block_per_group+i].data[block_addr+2],mycache[group*Block_per_group+i].data[block_addr+3]);
      return final;
    }
  }
  printf("not here\n");
  assert(0);
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uint32_t adr=(uint32_t)addr& (~0x3);
  uint32_t block_addr=adr<<(32-BLOCK_WIDTH)>>(32-BLOCK_WIDTH);//取出块内地址 OK
  uint32_t group=adr<<(32-BLOCK_WIDTH-group_width)>>(32-group_width);//取出组号 OK
  uint32_t Note_addr=adr>>(group_width+BLOCK_WIDTH);//取出标记位 OK
  for(int i=0;i<Block_per_group;i++){//命中，修改即可
    if(mycache[group*Block_per_group+i].valid_bit==true&&mycache[group*Block_per_group+i].Note==Note_addr){
      uint32_t* write_p=(uint32_t*)(mycache[group*Block_per_group+i].data+block_addr);
      *write_p=((*write_p)&(~wmask))|(data&wmask);
      mycache[group*Block_per_group+i].dirty_bit=true;
      return;
    }
  }
  //没有命中
  for(int i=0;i<Block_per_group;i++){//找空的，没有就替换最后一个
    if((mycache[group*Block_per_group+i].valid_bit==false)||(i==Block_per_group-1)){
      if(mycache[group*Block_per_group+i].dirty_bit==true&&(mycache[group*Block_per_group+i].valid_bit==true)){
        mem_write((mycache[group*Block_per_group+i].Note<<group_width)|group,mycache[group*Block_per_group+i].data);//写回
      }
      mem_read(adr>>BLOCK_WIDTH,mycache[group*Block_per_group+i].data);
      mycache[group*Block_per_group+i].valid_bit=true;
      mycache[group*Block_per_group+i].Note=Note_addr;
      mycache[group*Block_per_group+i].dirty_bit=true;
      uint32_t* write_p=(uint32_t*)(mycache[group*Block_per_group+i].data+block_addr);
      *write_p=((*write_p)&(~wmask))|(data&wmask);
      return;
    }
  }
}

void init_cache(int total_size_width, int associativity_width) {
  mycache=(BLOCK*)malloc(sizeof(BLOCK)*exp2(total_size_width));
  for(int i=0;i<exp2(total_size_width);i++){
    mycache[i].valid_bit=0;
    mycache[i].dirty_bit=0;
    mycache[i].Note=0;
  }
  Block_per_group=exp2(associativity_width);
  group_width=(total_size_width-6)-associativity_width;
  group_number=exp2(group_width);
}

void display_statistic(void) {
}
