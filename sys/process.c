#include "sys/process.h"
#include "sys/kprintf.h"
#include "sys/vmem.h"
#include <sys/gdt.h>
#include "sys/pmem.h"
#include "sys/tarfs.h"
#include "sys/string.h"
#include "sys/syscall.h"

THREAD* thread1;
THREAD* thread2;
THREAD* thread3;
extern uint64_t* PML4;
extern uint64_t* cr3;
PCB* createProcess();

extern uint64_t * virtual_PML4;

uint64_t getNewPid(){
	uint64_t retPid = pid_pp;
	pid_pp++;
	return retPid;
}

uint64_t getCurrentProcPid()
{
	return current_PCB->pid;
}
uint64_t getParentPid()
{
	return current_PCB->ppid;
}


void context_switch_old(THREAD* thready1, THREAD* thready2)
{
	//uint64_t var;

	////////////////////////////////////////////////////////////
	//save rsp
	//uint64_t localVal;
	//pushRegisters();
	//kprintf("hello\n");
	//kprintf("hello\n");

	//__asm__ __volatile__("cli");

	/*__asm__ __volatile__("movq %%rsp, %0;"\
						 "movq %1, %%rsp;"
						 "retq;"
						 :"=r"(thready1->rsp)
						 :"r"(thready2->rsp)
						);*/

	__asm__ __volatile__("movq %%rsp, %0;"
							:"=r"(thready1->rsp)
							:
							);
	//thready2->rsp+=8;
	__asm__ __volatile__("movq %0, %%rsp;"
						:"=r"(thready2->rsp)
						:
						);
	//__asm__ __volatile__ ("retq");
	/*__asm__ __volatile__("movq %%rsp, %0;"
						:"=r"(thready1->rsp)
						:
						);*/
	//kprintf("Hello\n");

	//kprintf("rsp saved value is %x\n", thready1->rsp);
	////////////////////////////////////////////////////////////
	/*__asm__ __volatile__("cli;"\
						"movq %0, %%rsp;"
						:
						:"r"(thready2->rsp));
*/
	////////////////////////////////////////////////////////////
	/*__asm__ __volatile__("lea (%%rip), %0;"\
						:"=r"(thread1->rip)
						:
						);*/
	//kprintf("%p",thread1->rip);
	/*__asm__ __volatile__(	 "pushq %%rbx;"\
							 "retq;"\
							 :
							 :"b"(thread2->rip)
							 :
							);*/
//	kprintf("var is %p",thread1->rip);
	/*__asm__ __volatile__("lea (%%rip), %%rax;"
						 "pushq %%rbx;"\
						 "retq;"\
						 :"=a"(thread1->rip)
						 :"b"(thread2->rip)
						 :
						);*/
	/*__asm__ __volatile__(
							 "pushq %%rbx;"\
							 :
							 :"b"(thready2->rip)
							 :
							);*/
	//kprintf("end of execution");
	//kprintf("back");
}

void context_switch(THREAD* old, THREAD* new)
{

	/*__asm__ __volatile__ ("movq %%rdi, %0":"=r"(old->rdi));
	__asm__ __volatile__ ("movq %%rsi, %0":"=r"(old->rsi));
	__asm__ __volatile__ ("movq %%rax, %0":"=r"(old->rax));
	__asm__ __volatile__ ("movq %%rbx, %0":"=r"(old->rbx));
	__asm__ __volatile__ ("movq %%rsp, %0":"=r"(old->rsp));
	__asm__ __volatile__ ("movq %%rbp, %0":"=r"(old->rbp));
	__asm__ __volatile__ ("movq %0, %%rsp;"	::"r"(new->rsp));
	__asm__ __volatile__ ("movq %0, %%rdi;" ::"r"(new->rdi));
	__asm__ __volatile__ ("movq %0, %%rax;" ::"r"(new->rax));
	__asm__ __volatile__ ("movq %0, %%rbx;" ::"r"(new->rbx));
	__asm__ __volatile__ ("movq %0, %%rbp;" ::"r"(new->rbp));
	__asm__ __volatile__ ("movq %0, %%rsi;" ::"r"(new->rsi));*/

	 __asm__ __volatile__ ("pushq %%rdi"::);
	 __asm__ __volatile__ ("pushq %%rsi"::);
	 __asm__ __volatile__ ("pushq %%rax"::);
	 __asm__ __volatile__ ("pushq %%rbx"::);
	 __asm__ __volatile__ ("pushq %%rbp"::);
	 __asm__ __volatile__ ("movq %%rsp, %0":"=r"(old->rsp));
	 /*__asm__ __volatile__ ("movq %0, %%rsp;" ::"r"(new->rsp) );
	 __asm__ __volatile__ ("popq %%rbp;" ::);
	 __asm__ __volatile__ ("popq %%rbx;" ::);
	 __asm__ __volatile__ ("popq %%rax;" ::);
	 __asm__ __volatile__ ("popq %%rsi;" ::);
	 __asm__ __volatile__ ("popq %%rdi;" ::);*/
	 __asm__ __volatile__("movq %0, %%rsp;"\
	 					  "popq %%rbp;"\
						  "popq %%rbx;"\
						  "popq %%rax;"\
						  "popq %%rsi;"\
						  "popq %%rdi;"
						  ::"r"(new->rsp)
	 						);
	//kprintf("hello");

	/*__asm__ __volatile__("cli;"\
						"movq %%rsp, %0;"\
						 "movq %1, %%rsp;"\
						 :"=r"(old->rsp)
						 :"r"(new->rsp)
						  );*/
	//__asm__ __volatile__("retq");
}

void context_switch2(PCB* old, PCB* new)
{
	/*__asm__ __volatile__ ("movq %%rdi, %0":"=r"(old->rdi));
	__asm__ __volatile__ ("movq %%rsi, %0":"=r"(old->rsi));
	__asm__ __volatile__ ("movq %%rax, %0":"=r"(old->rax));
	__asm__ __volatile__ ("movq %%rbx, %0":"=r"(old->rbx));
	__asm__ __volatile__ ("movq %%rsp, %0":"=r"(old->rsp));
	__asm__ __volatile__ ("movq %%rbp, %0":"=r"(old->rbp));
	__asm__ __volatile__ ("movq %0, %%rsp;"	::"r"(new->rsp));
	__asm__ __volatile__ ("movq %0, %%rdi;" ::"r"(new->rdi));
	__asm__ __volatile__ ("movq %0, %%rax;" ::"r"(new->rax));
	__asm__ __volatile__ ("movq %0, %%rbx;" ::"r"(new->rbx));
	__asm__ __volatile__ ("movq %0, %%rbp;" ::"r"(new->rbp));
	__asm__ __volatile__ ("movq %0, %%rsi;" ::"r"(new->rsi));*/

/*	 __asm__ __volatile__ ("pushq %%rdi"::);
	 __asm__ __volatile__ ("pushq %%rsi"::);
	 __asm__ __volatile__ ("pushq %%rax"::);
	 __asm__ __volatile__ ("pushq %%rbx"::);
	 __asm__ __volatile__ ("pushq %%rbp"::);*/
	 __asm__ __volatile__("movq %%rsp, %0":"=r"(old->kernelStackAddress));
//	 load_cr3(new->PML4);
	 /*__asm__ __volatile__ ("movq %0, %%rsp;" ::"r"(new->rsp) );
	 __asm__ __volatile__ ("popq %%rbp;" ::);
	 __asm__ __volatile__ ("popq %%rbx;" ::);
	 __asm__ __volatile__ ("popq %%rax;" ::);
	 __asm__ __volatile__ ("popq %%rsi;" ::);
	 __asm__ __volatile__ ("popq %%rdi;" ::);*/
	 /*__asm__ __volatile__("movq %0, %%rsp;"\
	 					  "popq %%rbp;"\
						  "popq %%rbx;"\
						  "popq %%rax;"\
						  "popq %%rsi;"\
						  "popq %%rdi;"
						  ::"r"(new->rsp)
	 						);*/
	 __asm__ __volatile__("movq %0, %%rsp" ::"r"(new->kernelStackAddress));
	 __asm__ __volatile__("retq");
//	 __asm__ __volatile("retq");
}
void switchToRing3(PCB* pcb)
{
	//__asm__ __volatile__ ("sti;");
	/*kprintf("\nbefore %p %p\n", pcb->rsp,*(uint64_t*)(pcb->rsp) );
	__asm__ __volatile__("movq %0, %%cr3;"::"r"(pcb->PML4));
	kprintf("\nafter %p %p\n", pcb->rsp,*(uint64_t*)(pcb->rsp) );*/
	load_cr3(pcb->PML4);
//	kprintf("%x",*(uint64_t*)(pcb->heap_ptr ));
	virtual_PML4 = (uint64_t*)(pcb->virtualPML4);
//	kprintf("the value is %p",pcb->rsp);
//	userPhysicalToVirtual((pcb->rsp)&0xfffffff,(pcb->rsp)&0xfffffff, (uint64_t*) virtual_PML4 );
//	pcb->rsp = (pcb->rsp)&0xfffffff;
	/*uint64_t temprsp = kmalloc();
	virtualToPhysical(temprsp, (uint64_t*)currentPML4);
	kprintf("temp rsp is %p\n",temprsp);*/
//	userPhysicalToVirtual2(temprsp&0xfffffff ,0x500000000, (uint64_t*) virtual_PML4 );
//	kprintf("pcb->rsp %p", pcb->rsp);
//	while(1){}
	set_tss_rsp((void*)(pcb->kernelStack));
	//updating all the register values to 0x23
	__asm__ volatile("mov $0x23, %%cx;"\
					 "mov %%cx, %%ds;"\
					 "mov %%cx, %%es;"\
					 "mov %%cx, %%fs;"\
					 "mov %%cx, %%gs;"
					 ::
					);
	
	//__asm__ __volatile__("movq %0, %%rsp"::"r"(pcb->rsp));//updating value of rsp to the rsp of pcb

	/*__asm__ __volatile__("pushq $0x23;");
	__asm__ __volatile__("pushq %0;"::"r"(pcb->rsp));

	__asm__ __volatile__("pushfq;");
	__asm__ __volatile__("pushq $0x2B;"::);
	__asm__ __volatile__("pushq %0;"::"r"(pcb->rip));*/
	/*
		0x20 - user data segment descriptor | 0x3 = $0x23(see gdt.c)
		0x28 - user code segment descriptor | 0x3 = $0x2B
		pushing user ds,user rsp,flags,user cs
	*/

	__asm__ __volatile__("pushq $0x23;"\
			     "pushq %0;"\
			     "pushfq;"\
				 "popq %%rbx;"\
				 "orq $0x200, %%rbx;"\
				 "pushq %%rbx;"\
			     "pushq $0x2B;"\
			     "pushq %1"
			     :
			     :"a"(pcb->rsp),"c"(pcb->rip)
			    );
	__asm__ __volatile__("iretq");
	//while(1){}

}

void function_1(){
	__asm__ __volatile__("int $0x80");
	while(1){}
}

void function_2(){
	kprintf("In 2\n");
//	while(1){}
	context_switch(thread2,thread1);
	kprintf("back in 2\n");
	context_switch(thread2,thread1);
	kprintf("Executing function2\n");

	while(1){}
}
void function_3(){
	kprintf("In 3\n");
	//kprintf("Executing function2\n");
	context_switch(thread2,thread1);
	while(1);
	kprintf("back in 2.1\n");

	//kprintf("back in 2.1\n");
	//kprintf("5");
	//context_switch(thread2,thread1);
	while(1){}
}




PCB* createProcess()
{
	//creating PCB block
	PCB* pcb = (PCB*)kmalloc();


	pcb->next = pcb;
	pcb->pid = getNewPid();
	//creating kernelStack to be used for context switching
	pcb->kernelStack = (uint64_t*)((uint64_t)kmalloc() +0x1000 - 0x8);
//	kprintf("kernel Stack : %p",(pcb->kernelStack));

	//	Creating PML4 table for PCB
	uint64_t page = (uint64_t)kmalloc();
	pcb->PML4 = ((uint64_t)page) & 0xffffff;		//saving physical address of PML4
	//kprintf("pcb->pml4 : %p\n",pcb->PML4);
	pcb->virtualPML4 = page;				//saving virtual address of PML4
	updatePageEntriesToZero((uint64_t)(pcb->virtualPML4 )); //setting all values of page table to zero
	((uint64_t*)page)[510] = (pcb->PML4) | 0x7ull; // for self-refrencing
	//((uint64_t*)(pcb->virtualPML4))[511] = virtual_PML4[511];
	((uint64_t*)page)[511]= virtual_PML4[511];

//	pcb->virtualPML4  = (uint64_t)((((((((0x1FEull << 9ull ) | 0x1FEull) << 9ull) | 0x1FEull ) << 9ull ) | 0x1FEull) <<12ull) | 0xFFFF000000000000ull);
//	kprintf ("PML4 updated to %p",*(uint64_t*)(pcb->virtualPML4));
//	while(1){}


	//setting up the mm_struct
	mm_struct* mem = (mm_struct*) kmalloc();
	mem->firstVMA = NULL;
	pcb->mem  = mem;

	//	allocate file structure
	int i = 0;
	while(strlen(fd[i].name) != 0) {
//		strcpy(pcb->pcb_fd[i].name, "");
//		pcb->pcb_fd[i].typeflag = 0x9999ull;
		pcb->pcb_fd[i].inode_no = 0x9999ull;
//		pcb->pcb_fd[i].parent_inode_no = 0x9999ull;
		pcb->pcb_fd[i].read_offset = 0x0ull;
		pcb->pcb_fd[i].open_flag = 0;
		i++;
	}

//	strcpy(pcb->current_working_dir.name, fd[3].name);
//	pcb->current_working_dir.typeflag = fd[3].typeflag;
	pcb->curr_inode_no = fd[3].inode_no;
//	pcb->current_working_dir.parent_inode_no = fd[3].parent_inode_no;
//	pcb->current_working_dir.read_offset = 0x0ull;
//	pcb->current_working_dir.open_flag = 0;

	//allocate heap memory
//	pcb->heap_mem = (uint64_t)(0x8000000ull);
//	uint64_t physAddr = (uint64_t)kmalloc();
//	physAddr &= 0xFFFFFFFull;
//	clear_screen();
//	userPhysicalToVirtual(physAddr, pcb->heap_mem, (uint64_t*)(pcb->virtualPML4));
//	load_cr3(pcb->PML4);
//	pcb->heap_ptr = (uint64_t*)(pcb->heap_mem);

//	uint64_t stackVirtualAddress = (uint64_t)(0x60000000ull);
//	pcb->stack = (uint64_t*)kmalloc();
//	pcb->stack = (uint64_t*)((uint64_t)pcb->stack+ (0x1000) - 0x40);
//	userPhysicalToVirtual((uint64_t)(pcb->stack) & 0xfffffff, stackVirtualAddress, (uint64_t*)(pcb->virtualPML4));
//	pcb->rsp = (stackVirtualAddress + 0x1000ull - 0x20);
//	kprintf("n%x\n",pcb->rsp);
//	while(1){}
	/*uint64_t* temp = (uint64_t*)kmalloc();
	stackVirtualAddress = (uint64_t)(0x60000000ull);
	userPhysicalToVirtual((uint64_t)(temp) & 0xfffffff, stackVirtualAddress, (uint64_t*)pcb->virtualPML4);*/
//	while(1){}
//	current_PCB = pcb;
	return pcb;
}

uint64_t malloc(uint64_t size) {
//	kprintf("\nmalloc begin addr: %p", current_PCB->heap_ptr);
	uint64_t ret_addr = (uint64_t)current_PCB->heap_ptr;
//	kprintf("\nthe size is %d\n", size);
	current_PCB->heap_ptr = (uint64_t*)((uint64_t)(current_PCB->heap_ptr) + size);
//	kprintf("\nmalloc end addr: %p", current_PCB->heap_ptr);
	return ret_addr;
}

uint64_t getpid() {
	uint64_t ret_val = current_PCB->pid;
	return ret_val;
}

void addToZombieList(PCB *temp){
	deletePageTables(temp);
	if(zombieList == NULL){
		zombieList = temp;
		temp->next=NULL;
	}
	else{
		PCB* current = zombieList;
		while(current->next != NULL){
			current = current->next;
		}
		current->next = temp;
		temp->next=NULL;
	}

}


THREAD* createThread()
{
	THREAD* thread = (THREAD*)kmalloc();
	thread->stack = (uint64_t*)kmalloc();
	thread->stack = (uint64_t*)((uint64_t)thread->stack+ (0x1000 - 0x10));
	thread->rsp = (uint64_t)(thread->stack);
	return thread;
}


void copyFileDescriptors(PCB *child){

	child->curr_inode_no = current_PCB->curr_inode_no;
	int i = 0;
//	strcpy(child->current_working_dir.name, current_PCB->current_working_dir.name);
//	child->current_working_dir.typeflag = current_PCB->current_working_dir.typeflag;
//	child->current_working_dir.inode_no = current_PCB->current_working_dir.inode_no;
//	child->current_working_dir.parent_inode_no = current_PCB->current_working_dir.parent_inode_no;
//	child->current_working_dir.read_offset = current_PCB->current_working_dir.read_offset;
//	child->current_working_dir.open_flag = current_PCB->current_working_dir.open_flag ;
	while(strlen(fd[i].name) != 0) {
//			strcpy(child->pcb_fd[i].name, current_PCB->pcb_fd[i].name);
//			child->pcb_fd[i].typeflag = current_PCB->pcb_fd[i].typeflag;
			child->pcb_fd[i].inode_no = current_PCB->pcb_fd[i].inode_no;
//			child->pcb_fd[i].parent_inode_no = current_PCB->pcb_fd[i].parent_inode_no;
			child->pcb_fd[i].read_offset = current_PCB->pcb_fd[i].read_offset;
			child->pcb_fd[i].open_flag = current_PCB->pcb_fd[i].open_flag ;
			i++;
		}
}


uint64_t createChildProcess()
{
//	create a child structure
	PCB* child = (PCB*)kmalloc();

//	child->current_working_dir = current_PCB->current_working_dir;
//	create its page tables
//	while(1){}
	child->virtualPML4 = kmalloc();
	child->PML4 = (child->virtualPML4) & 0xfffffff;
	((uint64_t*)(child->virtualPML4))[510] = child->PML4;
	((uint64_t*)(child->virtualPML4))[511] = ((uint64_t*)currentPML4)[511];
//	pageWalk((uint64_t*)(child->virtualPML4));

//	copy page tables
	copyParentPageTablesToChild((uint64_t*)(current_PCB->virtualPML4), (uint64_t*)(child->virtualPML4));
//	pageWalk((uint64_t*)(child->virtualPML4));

//	copy VMA structure
	mm_struct* mem = (mm_struct*) kmalloc();
	mem->firstVMA = NULL;
	child->mem  = mem;

	copyVMAStructure(child, current_PCB);
//	kprintf("calling walk vma for child");
//	walkVMA(child);
//	pageWalk((uint64_t*)(child->virtualPML4));

//	create child kernel Stack
//	child->kstack
	child->kernelStack = (uint64_t*)((uint64_t)kmalloc() +0x1000 - 0x8);
//	kprintf("kernel stack : %p\n",child->kernelStack );
	__asm__ __volatile__ ("movq %0, %%cr3"::"r"((child->virtualPML4)&0xfffffff));
//	*(child->kernelStack) =35;
	memcpy((char*)((uint64_t)(current_PCB->kernelStack)-0x1000),
		   (char*)((uint64_t)(child->kernelStack)-0x1000),
		   0x1000 );
//	pageWalk((uint64_t*)(child->virtualPML4));
//	kprintf("here in hild");
	child->pid = getNewPid();
	child->ppid = current_PCB->pid;
	PCB* temp = current_PCB->next;
	current_PCB->next = child;
	child->next  = temp;
//	(current_PCB->kernelStack)[511] = 0x234;
	copyFileDescriptors(child);
//	pageWalk((uint64_t*)(child->virtualPML4));
	child->kernelStackAddress = (((uint64_t)(child->kernelStack) & ~(0xfff)) | (globalAddress & 0xfff));
	current_PCB->time_slice = (current_PCB->time_slice)/2;
	child->time_slice = current_PCB->time_slice;
	child->time_slice_left = child->time_slice;
	__asm__ __volatile__("movq %0, %%cr3"::"r"((current_PCB->virtualPML4)&0xfffffff));
//	kprintf(" child->kernelStackAddress %p\n",child->kernelStackAddress);
	/*__asm__ volatile("mov $0x23, %%bx;"\
														 "mov %%bx, %%ds;"\
														 "mov %%bx, %%es;"\
														 "mov %%bx, %%fs;"\
														 "mov %%bx, %%gs;"
														 ::
														);*/
//	kprintf("the value being returned is %d\n",child->pid);
	return child->pid;
//	__asm__ __volatile__ ("movq $95, %rax");
//	__asm__ __volatile__("movq %%rsp, %0":"=r"(child->rsp):);
	/*__asm__ __volatile__("movq %%rsp, %0"::"r"(child->rsp));*/
//	load_cr3(child->PML4);

//
//	virtual_PML4 = (uint64_t*)(child->virtualPML4);
//	0x00400175
//	kprintf("the value was Oh my fg %p\n",*((uint64_t*)(child->virtualPML4)));
//	virtualToPhysical(0x00400175, (uint64_t*)(child->virtualPML4));
//	kprintf("back");
//	kprintf("%p",*virtual_PML4);
//	while(1){}
//	load_cr3(child->PML4);
//	kprintf("child->kernelStack : %p",*(child->kernelStack));

}
