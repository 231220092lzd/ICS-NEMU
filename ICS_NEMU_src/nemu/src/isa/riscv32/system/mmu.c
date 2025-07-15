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
#include <memory/vaddr.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  //务必用assert检查页目录项和页表项
  //satp中存放的是页表基址(低22位)
  uint32_t page_offset=BITS(vaddr,11,0);//偏移
  uint32_t page_number_two=BITS(vaddr,21,12);//二级页表
  uint32_t page_number_one=BITS(vaddr,31,22);//一级页表

  uint32_t  first_page_word=paddr_read((cpu.satp<<12)+(page_number_one<<2),4);
  if((first_page_word&0x1)==0){
    printf("first_page_word:0x%x\n",(cpu.satp<<12)+(page_number_one<<2));
    printf("地址转换type:%d,取指0,读1,写2\n",type);
    printf("缺页\n");
    printf(" vaddr:0x%x first_page_word:%d\n",vaddr,first_page_word);
    printf("pc:%x\n",cpu.pc);
    assert(0);  
  }//一级页表有效位
  uint32_t second_page_addr=(BITS(first_page_word,31,12)<<12)+(page_number_two<<2);
  uint32_t  second_page_word=paddr_read(second_page_addr,4);
  assert((second_page_word&0x1)!=0);

  if(type==MEM_TYPE_IFETCH){
    assert(((BITS(second_page_word,3,3)&0x1)!=0));
  }
  if(type==MEM_TYPE_WRITE){
    assert(((BITS(second_page_word,2,2)&0x1)!=0));
  }
  if(type==MEM_TYPE_READ){
    assert((BITS(second_page_word,1,1)&0x1)!=0);
  }
  paddr_t final_addr=(BITS(second_page_word,31,12)<<12)+page_offset;
  /*if(final_addr!=vaddr){
    printf("final:%d != vaddr:%d type:%d\n",final_addr,vaddr,type);
  }*/
  //assert(final_addr==vaddr);
  return final_addr;
}
