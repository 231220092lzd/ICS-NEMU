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
//实现虚拟地址的访问
word_t vaddr_ifetch(vaddr_t addr, int len) {
  uint32_t final=(paddr_t)addr;
  if(isa_mmu_check(addr,len,MEM_TYPE_IFETCH)==MMU_TRANSLATE){
    final=isa_mmu_translate(addr,len,MEM_TYPE_IFETCH);
  }
  return paddr_read(final,len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  uint32_t final=(paddr_t)addr;
  if(isa_mmu_check(addr,len,MEM_TYPE_READ)==MMU_TRANSLATE){
    final=isa_mmu_translate(addr,len,MEM_TYPE_READ);
  }
  return paddr_read(final,len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  uint32_t final=(paddr_t)addr;
  if(isa_mmu_check(addr,len,MEM_TYPE_WRITE)==MMU_TRANSLATE){
    final=isa_mmu_translate(addr,len,MEM_TYPE_WRITE);
  }
  paddr_write(final,len,data);
}
