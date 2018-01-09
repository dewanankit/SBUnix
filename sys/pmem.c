#include "sys/defs.h"
#include "sys/pmem.h"
#include "sys/kprintf.h"
#include "sys/vmem.h"

uint64_t* MMAPAdress;
uint64_t maxPhysicalAddress = 0;
extern uint64_t* cr3;
uint64_t* allocatePage(){
	int found = 0;
	uint64_t count = 0;
	while(found ==0 && count<512){
		if((MMAPAdress[count] | 0x0ull) > 0 ){
			found=1;
		}
		else{
			count++;
		}
	}
	if(found == 1){
		found = 0;
		uint64_t count2 = 0;
		uint64_t checker = 1ull;
		while(found!=1){
			if((MMAPAdress[count] & checker)>=1){
				found=1;
			}
			else{
				checker = checker<<1;
				count2 += 1;
			}
		}
		mmapUnSetBit(count*64 + count2);
		updatePageEntriesToZero((uint64_t)(count*64ull + count2)*0x1000ul);
		return (uint64_t*)((count*64ull + count2)*0x1000ul);
	}
	kprintf("CANNOT ALLOCATE ANY MORE PAGES!");
	while(1){}
	return 0;
}

uint64_t* allocatePage2(){
	int found = 0;
	uint64_t count = 0;
	while(found ==0 && count<512){
		if((MMAPAdress[count] | 0x0ull) > 0 ){
			found=1;
		}
		else{
			count++;
		}
	}
	if(found == 1){
		found = 0;
		uint64_t count2 = 0;
		uint64_t checker = 1ull;
		while(found!=1){
			if((MMAPAdress[count] & checker)>=1){
				found=1;
			}
			else{
				checker = checker<<1;
				count2 += 1;
			}
		}
		mmapUnSetBit(count*64 + count2);
		uint64_t page = (uint64_t)(count*64ull + count2)*0x1000ul;
		//physicalToVirtual((uint64_t)page, (uint64_t)page, cr3);
		//updatePageEntriesToZero((uint64_t)page);
		//updatePageEntriesToZero((uint64_t)(count*64ull + count2)*0x1000ul);
		return (uint64_t*)page;
	}
	kprintf("CANNOT ALLOCATE ANY MORE PAGES!");
	while(1){}
	return 0;
}

void updatePageEntriesToZero(uint64_t pageAddress)
{
	uint64_t* pagePtr = (uint64_t*) pageAddress;
	int count = 0;
	int limit = PAGESIZE/8;
	while(count<limit){
		pagePtr[count] = 0;
		count+=1;
	}
}

void setAllMMAPbits(uint64_t MMAPAddr)
{
	uint64_t* pagePtr = (uint64_t*) MMAPAddr;
	int count = 0;
	int limit = PAGESIZE/8;
	while(count<limit){
		pagePtr[count] = ~0ull;
		count+=1;
	}
}

void mmapSetBit(int pageNo)
{
	MMAPAdress[pageNo/64] |= ( 1ull << ((pageNo%64)));
}

void mmapUnSetBit(int pageNo)
{
	uint64_t valUpdater =  ~(1ull << ((pageNo%64)));
	MMAPAdress[pageNo/64] =  MMAPAdress[pageNo/64] &  valUpdater;
}

void initializeMemory(uint64_t physfree)
{
	MMAPAdress = (uint64_t*) physfree;
	updatePageEntriesToZero((uint64_t)MMAPAdress);
}

void updateMMAP(uint64_t physfree,uint64_t physbase, uint64_t base, uint64_t top)
{
	uint64_t beginMemory = base;
	uint64_t endMemory = top;
	while(beginMemory < endMemory){
		if(beginMemory>physfree){
			mmapSetBit( beginMemory/ 0x1000);
		}
		beginMemory+= 0x1000;
	}
}
