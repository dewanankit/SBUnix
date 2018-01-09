#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/pci.h>
#include "sys/pmem.h"
#include "sys/vmem.h"
#include "sys/process.h"
#include "sys/tarfs.h"
#include "sys/string.h"
#include "sys/timer.h"

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void printValues(uint64_t physfree)
{
	kprintf("printValues is %x\n",physfree);
	uint64_t * addr = (uint64_t*) 0x000000000003F000;
	kprintf("The value is %x %x  %x\n",addr[0],*(addr+1),addr[2]);
	kprintf("The address are %x %x",addr, addr+1);
	//paging(physfree);
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
//	doHandlePageFault = 0;
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
//  kprintf("physfree before calling %x\n",(uint64_t)physfree);
  initializeMemory((uint64_t)physfree);
  uint64_t max_diff = 0;
  uint64_t max_start = 0;
  uint64_t max_end = 0;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
   //kprintf("Hello\n");
   if (smap->type == 1 /* memory */ && smap->length != 0) {
//      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
	   if(smap->length > max_diff){
		   max_diff = smap->length;
		   max_start = smap->base;
		   max_end = smap->base + smap->length;
	   }
      updateMMAP((uint64_t)physfree,(uint64_t)physbase, smap->base, smap->base + smap->length);
    }
  }
  kprintf("%x %x",max_start, max_end);
//  while(1){}
  init_paging((uint64_t)physfree,(uint64_t)physbase, max_start, max_end);

  uint64_t vAddress = kmalloc();

  timer_phase(10);
  init_idt();
  seconds = 0;

  __asm__ __volatile__("sti");

  virtualToPhysical(vAddress, (uint64_t*)currentPML4);
  kprintf("back\n");

  init_tarfs();

  init = loadExecutable("/bin/init");
  init->ppid = init->pid;
  current_PCB = init;
  strcpy(init->p_name,"init");
  init->state = RUNNING;
  kprintf("\nCalling switch function");

  switchToRing3(init);
 __asm__ __volatile__ ("sti");

  while(1){};
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();

  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );

  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
