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
#define IRQ_TIMER 0x80000007
word_t isa_raise_intr(word_t NO, vaddr_t epc) {//mtvec:773 mepc833 mecause834
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mcause=NO;
  if(NO==IRQ_TIMER){
    cpu.mepc=epc;
  }else{
    cpu.mepc=epc+4;
  }
  if((cpu.mstatus>>3)&0x1){
    cpu.mstatus|=0x00000080;
  }else{
    cpu.mstatus&=0xFFFFFF7F;
  }
  cpu.mstatus&=0xFFFFFFF7;
  return cpu.mtvec;
}

word_t isa_query_intr() {
  if(((cpu.mstatus>>3)&0x1)&&cpu.INTR==true){
    cpu.INTR=false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
