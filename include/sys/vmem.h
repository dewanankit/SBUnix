#ifndef __VMEM_H
#define __VMEM_H

#include "sys/defs.h"
#include "sys/process.h"
//Present bit
#define PG_P  0x1ull
//Read/Write bit
#define PG_R  0x2ull
//User/Supervisor bit
#define PG_U  0x4ull
//Write Through bit
#define PG_W  0x8ull
//Cache disabled bit
#define PG_C  0x16ull
//Accessed bit
#define PG_A  0x32ull
//Dirty bit
#define PG_D  0x64ull
//Global bit
#define PG_G  0x256ull
#define PG_COW  1ull<<9
//#define VMEM_START 0XFFFFFFFFF0000000ull
#define PMEM_START 0X0ull
uint64_t * virtual_PML4;
//volatile int doHandlePageFault;

uint64_t currentPML4;
void walkVMA(PCB* pcb);
void init_paging(uint64_t, uint64_t,uint64_t,uint64_t);
void copyParentPageTablesToChild(uint64_t* parentPML4, uint64_t* childPML4);
int getPLM4Offset(uint64_t p);
int getPDTEOffset(uint64_t p);
int getPDTOffset(uint64_t p);
int getPageTableOffset(uint64_t p);
void identityPaging(uint64_t bottom, uint64_t top);
uint64_t * virtualizeKernMem(uint64_t physfree,uint64_t physbase);
uint64_t physicalToVirtual(uint64_t pAddress,uint64_t vAddress, uint64_t * PML4);
void createAndInsertVMA(PCB* pcb, uint64_t startAddress, uint64_t endAddress,int flags, int type);

void copyVMAStructure(PCB* child, PCB* parent);
uint64_t  kmalloc();
void deletePageTables(PCB* pcb);
void load_cr3(uint64_t PML4);
uint64_t virtualToPhysical(uint64_t vAddress, uint64_t * PML4);
void userPhysicalToVirtual(uint64_t pAddress,uint64_t vAddress, uint64_t *PML4);
void memcpy(char* addrSrc, char* addrDest, int size);
void pageWalk(uint64_t* pml4);
void userPhysicalToVirtual2(uint64_t pAddress,uint64_t vAddress, uint64_t *PML4);
#endif
