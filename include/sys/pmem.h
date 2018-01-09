#ifndef __PMEM_H
#define __PMEM_H

#include "sys/defs.h"

void initializeMemory(uint64_t physfree);
void updateMMAP(uint64_t physfree,uint64_t physbase,uint64_t base, uint64_t top);
void updatePageEntriesToZero(uint64_t pageAddress);
uint64_t* allocatePage();
uint64_t* allocatePage2();
void mmapSetBit(int pageNo);
void mmapUnSetBit(int pageNo);
#endif
