#include "sys/vmem.h"
#include "sys/pmem.h"
#include "sys/defs.h"
#include "sys/kprintf.h"
#include "sys/process.h"
#include "sys/irq.h"
#include "sys/outin.h"
#include "sys/tarfs.h"
#include "sys/syscall.h"

extern uint64_t* MMAPAdress;
extern char kernmem, physbase;
extern char* videoMemLocation;
int pageTableUpdated = 0;

extern uint64_t maxPhysicalAddress;
uint64_t * cr3;
uint64_t * virtual_PML4;


void memcpy(char* addrSrc, char* addrDest, int size)
{
	int i = 0;
	/*for(;i<size;i++){
		addrDest[i] = addrSrc[i];
	}*/
	while(i<size){
		*addrDest = *addrSrc;
		i++;
		addrDest+=1;
		addrSrc+=1;
		/*if(i>4093){
			kprintf("greater\n");
		}*/
	}
}
uint64_t physicalToVirtual(uint64_t pAddress,uint64_t vAddress, uint64_t * PML4)
{
	uint64_t* PDTE;
	uint64_t* PDT;
	uint64_t* PT;
	int PML4Offset = getPLM4Offset((uint64_t)vAddress);
	int PDTEOffset = getPDTEOffset((uint64_t)vAddress);
	int PDTOffset = getPDTOffset((uint64_t)vAddress);
	int PTOffset = getPageTableOffset((uint64_t)vAddress);
	uint64_t PDTEVal = PML4[PML4Offset];
	if((PDTEVal & 0x1)>0){
		PDTE =  (uint64_t*)((uint64_t)PDTEVal & ~(0xFFF));
	}
	else{
		//kprintf("PDTE not found\n");
		PDTE = allocatePage();
		//physicalToVirtual((uint64_t)PDTE,(uint64_t)PDTE, PML4);
		PML4[PML4Offset] = (uint64_t)((uint64_t)PDTE | 0x3ull);
		physicalToVirtual((uint64_t)PDTE,(uint64_t)PDTE, PML4);
	}
	uint64_t PDTVal = PDTE[PDTEOffset];
	if((PDTVal & 0x1)>0){
		PDT =  (uint64_t*)((uint64_t)PDTVal & ~(0xFFF));
	}
	else{
		//
		PDT = allocatePage();
		PDTE[PDTEOffset] = (uint64_t)((uint64_t)PDT | 0x3ull);
		physicalToVirtual((uint64_t)PDT,(uint64_t)PDT, PML4);
	}
	uint64_t PTVal = PDT[PDTOffset];
	if((PTVal & 0x1)>0){
		PT =  (uint64_t*)((uint64_t)PTVal & ~(0xFFF));
	}
	else{
		PT = allocatePage();
		PDT[PDTOffset] = (uint64_t)((uint64_t)PT | 0x3ull);
		physicalToVirtual((uint64_t)PT,(uint64_t)PT, PML4);
	}
	PT[PTOffset] = (uint64_t)(pAddress | 0x3ull);
	return vAddress;
}


uint64_t virtualToPhysical(uint64_t vAddress, uint64_t * PML4)
{
	int PML4Offset = getPLM4Offset((uint64_t)vAddress);
	int PDTEOffset = getPDTEOffset((uint64_t)vAddress);
	int PDTOffset = getPDTOffset((uint64_t)vAddress);
	int PTOffset = getPageTableOffset((uint64_t)vAddress);
//	kprintf("the value of address : %p\n",vAddress);
//	kprintf("%d %d %d %d\n",PML4Offset,PDTEOffset, PDTOffset ,PTOffset);
/*---------------------------------------------------------------------------------------*/
	uint64_t PDTE_val = (uint64_t)PML4[PML4Offset];
	uint64_t* PDTE;
	if((PDTE_val & PG_P)==0){
//		kprintf("Page fault\n");
		return 0;
	}
	else{
		PDTE = (uint64_t*)((PDTE_val& ~(0xFFF)) | 0xffffffff80000000);
//		kprintf("PML4 wrks fine\n");
	}
/*---------------------------------------------------------------------------------------*/
	uint64_t* PDT ;
	uint64_t PDT_val = (uint64_t)PDTE[PDTEOffset] ;
	if((PDT_val & PG_P)==0){
//		kprintf("Page fault\n");
		return 0;
	}
	else{
		PDT = (uint64_t*)((PDT_val& ~(0xFFF)) | 0xffffffff80000000);
//		kprintf("PDTE wrks fine\n");
	}
/*---------------------------------------------------------------------------------------*/
	uint64_t* PT;
	uint64_t PT_val = (uint64_t)PDT[PDTOffset];
	if((PT_val & PG_P)==0){
//		kprintf("Page fault\n");
		return 0;
	}
	else{
		PT = (uint64_t*)((PT_val& ~(0xFFF)) | 0xffffffff80000000);
//		kprintf("PDT wrks fine\n");
	}
/*---------------------------------------------------------------------------------------*/
	uint64_t* address = (uint64_t*)((uint64_t)PT[PTOffset]);
	if(((uint64_t)address  & PG_P)==0){
//		kprintf("PT Page fault\n");
		return 0;
	}
	else{
//		kprintf("Phsical address is %p and virtual address is %p\n",address,vAddress);
		return (uint64_t)address;
	}
}






void load_cr3(uint64_t PML4)
{
	currentPML4 = PML4 | 0xffffffff80000000ull;
	__asm volatile("movq %0, %%cr3":: "b"(PML4));
}

/*
void identityPaging(uint64_t bottom, uint64_t top)
{
	uint64_t * PML4 = cr3;
	while(bottom<0x60DA000){
		physicalToVirtual(bottom,bottom| 0xffffffff80000000ull ,PML4);
		bottom+=0x1000ul;
	}
	//kprintf("fffffffff\n");
	//virtualToPhysical((uint64_t)0x20E000ul, PML4);
}
*/
void identityPagingInKernel(uint64_t bottom, uint64_t top)
{
	uint64_t * PML4 = cr3;
	while(bottom<0x60DA000){
		physicalToVirtual(bottom,(uint64_t) bottom | 0xffffffff80000000ull ,PML4);
		bottom+=0x1000ul;
	}
	//kprintf("fffffffff\n");
	//virtualToPhysical((uint64_t)0x20E000ul, PML4);
}

/*uint64_t createAndInsertVMA(PCB* pcb, uint64_t startAddress, uint64_t endAddress,int flags, int type ){
	struct VMA* newVMA = (struct VMA*)kmalloc();
	struct VMA* vma = pcb->firstVMA;
	newVMA->size = size;
	if(vma == NULL){
		newVMA->startAddress = USER_VIRTUAL_ADDRESS;
		pcb->firstVMA = newVMA;
	}
	else{
		uint64_t left = USER_VIRTUAL_ADDRESS;
		uint64_t right = vma->startAddress;
		while((right-left)<size && (vma->next)!=NULL){
			left = (vma->startAddress) + (vma->size);
			vma = vma->next;
			right = vma->size;
		}
		if(vma->next == NULL){
			newVMA->next = NULL;
			vma->next = newVMA;
			newVMA->startAddress = vma->startAddress + vma->size;
		}
		else{
			newVMA->next = vma->next;
			vma->next = newVMA;
			newVMA->startAddress = vma->startAddress + vma->size;
		}
	}
	return newVMA->startAddress;
}*/

void createAndInsertVMA(PCB* pcb, uint64_t startAddress, uint64_t endAddress,int flags, int type ){
	VMA* newVMA = (VMA*)kmalloc();
	newVMA->next = NULL;
	newVMA->start = startAddress;
	newVMA->end = endAddress;
	newVMA->flags = flags;

	newVMA->type = type;
	if(type == TEXT){
//		kprintf("TEXT WAS LOADED\n");
	}
	else{
	if(type == DATA){
//			kprintf("DATA WAS LOADED\n");
		}
	}
	mm_struct * mem  = pcb->mem;
	if(mem->firstVMA == NULL){
		mem->firstVMA = newVMA;
		if(mem){}
	}
	else{
		VMA* current = mem->firstVMA;
		while(current->next != NULL){
			current = current->next;
		}
		current->next = newVMA;
	}
	return;
}



void walkVMA(PCB* pcb){
	mm_struct * mem  = pcb->mem;
	if(mem->firstVMA == NULL){
//		kprintf("empty VMA");
		return;
	}
	VMA* current = mem->firstVMA;
	while(current!=NULL){
//		kprintf("VMA Values: start %p  end %p  flags %x tpe  %x \n",current->start, current->end, current->flags, current->type);
		current = current->next;
	}
	return;
}

void copyVMAStructure(PCB* child, PCB* parent){
	mm_struct * parentMem  = parent->mem;
	mm_struct * childMem  = child->mem;

	VMA* currentParentVMA = parentMem->firstVMA;

	VMA* newVMA = (VMA*)kmalloc();
	newVMA->start = currentParentVMA->start;
	newVMA->end = currentParentVMA->end;
	newVMA->flags = currentParentVMA->flags;
	newVMA->type = currentParentVMA->type;
	childMem->firstVMA = newVMA;
	VMA* last = newVMA;
	currentParentVMA = currentParentVMA->next;

	while(currentParentVMA!=NULL)
	{
		VMA* newVMA = (VMA*)kmalloc();
		newVMA->next = NULL;
		last->next = newVMA;
		last = newVMA;
		newVMA->start = currentParentVMA->start;
		newVMA->end = currentParentVMA->end;
		newVMA->flags = currentParentVMA->flags;
		newVMA->type = currentParentVMA->type;
		currentParentVMA = currentParentVMA->next;
	}


}

/*
void init_paging(uint64_t physbase, uint64_t physfree)
{
	uint64_t * PML4 = virtualizeKernMem(physbase,physfree);

	physicalToVirtual((uint64_t)videoMemLocation,(uint64_t)videoMemLocation+0xffffffff80000000, PML4);
	physicalToVirtual((uint64_t)MMAPAdress,(uint64_t)MMAPAdress | 0xffffffff80000000, PML4);
	MMAPAdress = (uint64_t*)(((uint64_t)MMAPAdress) | 0xffffffff80000000ull);
	kprintf("MMAP address %p\n",MMAPAdress);
	physicalToVirtual((uint64_t)PML4,(uint64_t)PML4,PML4);

	virtualToPhysical((uint64_t)PML4, PML4);
	identityPaging(0,0);
	//identityPagingInKernel(0,0);
//	kprintf("mkc");
	//virtualToPhysical((uint64_t)0x20E000ul, PML4);
//	kprintf("PML4 value is %p\n",PML4);
	//kprintf("Hello");

	load_cr3((uint64_t)PML4);
	kprintf("over here");
	kprintf("PML4 value is %p\n",((uint64_t*)PML4)[511]);
	//kprintf("\nthe fin %p\n",*(uint64_t*)((uint64_t)page|0xffffffff81000000));

	//kprintf("Hello");
}
*/


int getPLM4Offset(uint64_t p){
	return ((p>>39) & 0x1FF);
}


int getPDTEOffset(uint64_t p){
	return ((p>>30) & 0x1FF);
}


int getPDTOffset(uint64_t p){
	return ((p>>21) & 0x1FF);
}


int getPageTableOffset(uint64_t p){
	return ((p>>12) & 0x1FF);
}


void deletePageTables(PCB* pcb)
{
	uint64_t* pml4 = (uint64_t*)(pcb->virtualPML4);
	int pml4Index = 0;
	for(pml4Index = 0; pml4Index <=	 510;pml4Index++){
		if((pml4[pml4Index] & PG_P)>0){
			uint64_t* pdte = (uint64_t*)((pml4[pml4Index] & ~0xFFF) | 0xffffffff80000000);
			int pdteIndex = 0;
			for(pdteIndex=0; pdteIndex<=511; pdteIndex++){
				if((pdte[pdteIndex] &PG_P)>0)
				{
					uint64_t* pdt = (uint64_t*)((pdte[pdteIndex] & ~0xFFF) | 0xffffffff80000000);
					int pdtIndex = 0;
					for(pdtIndex=0; pdtIndex<=511; pdtIndex++){
						if((pdt[pdtIndex] &PG_P)>0)
						{
							uint64_t* pt = (uint64_t*)((pdt[pdtIndex] & ~0xFFF) | 0xffffffff80000000);
							int ptIndex = 0;
							for(ptIndex=0;ptIndex<=511;ptIndex++)
							{
								if((pt[ptIndex] & PG_P)>0)
								{
									uint64_t value = pt[ptIndex];
									if((value & PG_R)>0)
									{
//										kprintf("page :%p\t",value & (0xffff000));
										value = value & (0xfffffff);
										mmapSetBit(value/0x1000);
									}
								}
							}
							uint64_t value = (uint64_t)pt & 0xfffffff;
							if((value & PG_R)>0)
							{
								mmapSetBit(value/0x1000);
//								kprintf("%p\t",value & (0xfffffff));
							}
						}
					}
					uint64_t value = (uint64_t)pdt & 0xfffffff;
					if((value & PG_R)>0)
					{
						mmapSetBit(value/0x1000);
//						kprintf("%p\t",value & (0xfffffff));
					}

				}
			}
			uint64_t value = (uint64_t)pdte & 0xfffffff;
			if((value & PG_R)>0)
			{
				mmapSetBit(value/0x1000);
//				kprintf("%p\t",value & (0xfffffff));
			}
		}
	}
	uint64_t value = (uint64_t)pml4 & 0xfffffff;
	if((value & PG_R)>0)
	{
		mmapSetBit(value/0x1000);
//		kprintf("%p\t",value & (0xfffffff));
	}
}

uint64_t kmalloc()
{
		uint64_t page = (uint64_t)allocatePage2();
//		userPhysicalToVirtual((uint64_t)page, (uint64_t)page | 0xffffffff80000000ull, virtual_PML4);
		page = page | 0xffffffff80000000ull;
		updatePageEntriesToZero((uint64_t)page);
//		kprintf("%p     ",page);
		return page ;
}


void userPhysicalToVirtual(uint64_t pAddress,uint64_t vAddress, uint64_t *PML4)
{

	uint64_t* PDTE;
//	uint64_t* PDTE
	uint64_t* PDT;
	uint64_t* PT;
	int PML4Offset = getPLM4Offset((uint64_t)vAddress);
	int PDTEOffset = getPDTEOffset((uint64_t)vAddress);
	int PDTOffset = getPDTOffset((uint64_t)vAddress);
	int PTOffset = getPageTableOffset((uint64_t)vAddress);

	uint64_t PDTEVal = PML4[PML4Offset]  | 0xffffffff80000000;
	if((PDTEVal & 0x1)>0)
	{
		PDTE =  (uint64_t*)((uint64_t)PDTEVal & ~(0xFFF));
	}
	else{
		PDTE = (uint64_t*)kmalloc();
//		PDTE[100] = 35;
		PML4[PML4Offset] = ((uint64_t)PDTE & 0xfffffff) | 0x7ull;
//		PDTE[510] = ((uint64_t)PDTE & 0xfffffff) | 0x7ull;
	}
	/*if(PML4Offset <= 256 )
		PDTE = (uint64_t*) ((((((((PML4Offset << 9ull ) | 0x1FEull) << 9ull) | 0x1FEull ) << 9ull ) | 0x1FEull) <<12ull) | 0x000000000000ull);
	else
		PDTE = (uint64_t*) ((((((((PML4Offset << 9ull ) | 0x1FEull) << 9ull) | 0x1FEull ) << 9ull ) | 0x1FEull) <<12ull) | 0xFFFF000000000000ull);
*/
	uint64_t PDTVal = PDTE[PDTEOffset] | 0xffffffff80000000;
	if((PDTVal & 0x1)>0){
		PDT =  (uint64_t*)((uint64_t)PDTVal & ~(0xFFF));
	}
	else{
		PDT = (uint64_t*)kmalloc();
		PDTE[PDTEOffset] = ((uint64_t)PDT & 0xfffffff) | 0x7ull;
//		PDT[510] = ((uint64_t)PDT & 0xfffffff) | 0x7ull;
	}
	/*if(PML4Offset <= 256 )
		PDT = (uint64_t*) ((((((((PML4Offset << 9ull ) | PDTEOffset) << 9ull) | 0x1FEull ) << 9ull ) | 0x1FEull) <<12ull) | 0x000000000000ull);
	else
		PDT = (uint64_t*) ((((((((PML4Offset << 9ull ) | PDTEOffset) << 9ull) | 0x1FEull ) << 9ull ) | 0x1FEull) <<12ull) | 0xFFFF000000000000ull);
*/
	uint64_t PTVal = PDT[PDTOffset] | 0xffffffff80000000;
	if((PTVal & 0x1)>0){
		PT =  (uint64_t*)((uint64_t)PTVal & ~(0xFFF));
	}
	else{
		PT = (uint64_t*)kmalloc();
		PDT[PDTOffset] = ((uint64_t)PT & 0xfffffff) | 0x7ull;
//		PT[510] = ((uint64_t)PT & 0xfffffff) | 0x7ull;
	}
	/*if(PML4Offset <= 256 )
		PT = (uint64_t*) ((((((((PML4Offset << 9ull ) | PDTEOffset) << 9ull) | PDTOffset ) << 9ull ) | 0x1FEull) <<12ull) | 0x000000000000ull);
	else
		PT = (uint64_t*) ((((((((PML4Offset << 9ull ) | PDTEOffset) << 9ull) | PDTOffset ) << 9ull ) | 0x1FEull) <<12ull) | 0xFFFF000000000000ull);
*/
	PT[PTOffset] = (uint64_t)(pAddress | 0x7ull);
	//kprintf("\n\n%p\n",PT[510]);
}



void pageWalk(uint64_t* pml4)
{
	int count = 0;
	int pml4Index = 0;
	for(pml4Index = 0; pml4Index <= 510;pml4Index++){
		if((pml4[pml4Index] & PG_P)>0){
//			kprintf("PMl4 %p ",pml4[pml4Index]);
//			kprintf("%d   ",pml4Index);
			uint64_t* pdte = (uint64_t*)((pml4[pml4Index] & ~0xFFF) | 0xffffffff80000000);
			int pdteIndex = 0;
			for(pdteIndex=0; pdteIndex<=511; pdteIndex++){
				if((pdte[pdteIndex] &PG_P)>0)
				{
//					kprintf("PDTE %p ",pdte[pdteIndex]);
//					kprintf("%d    ",pdteIndex);
					uint64_t* pdt = (uint64_t*)((pdte[pdteIndex] & ~0xFFF) | 0xffffffff80000000);
					int pdtIndex = 0;
					for(pdtIndex=0; pdtIndex<=511; pdtIndex++){
						if((pdt[pdtIndex] &PG_P)>0)
						{
//							kprintf("PDT %p ",pdt[pdtIndex]);
//							kprintf("%d   ",pdtIndex);
							uint64_t* pt = (uint64_t*)((pdt[pdtIndex] & ~0xFFF) | 0xffffffff80000000);
							int ptIndex = 0;
							for(ptIndex=0;ptIndex<=511;ptIndex++)
							{
								if((pt[ptIndex] &PG_P)>0)
								{
//									kprintf("The value at the address is %p %d %d %d %d\n" , pt[ptIndex],pml4Index, pdteIndex, pdtIndex, ptIndex);
									count++;
//									return pt[ptIndex];
//									kprintf("%d %d %d %d\n",pml4Index, pdteIndex, pdtIndex, ptIndex);
//									uint64_t* pt = (uint64_t*)((pdt[pdtIndex] & ~0xFFF) | 0xffffffff80000000);
								}
							}
						}
					}
				}
			}
		}
	}
//	kprintf("count is : %d\n", count);
}




void pageFault_handler_code(registers_t r){
//	kprintf("in page fault");
	/*kprintf("allocating a page");
	uint64_t address;
	if(doHandlePageFault ==1){
		__asm__ __volatile__(
							"movq %%cr2, %0;"\
							:"=r"(address)
							:
							);
		uint64_t newPageAddress = address & ~(0xFFF);
		uint64_t page = (uint64_t)allocatePage2();
		kprintf("%p",page);
		userPhysicalToVirtual((uint64_t)page,newPageAddress, virtual_PML4);
//		kprintf("%p", *(uint64_t*)address);
//		kprintf("In page fault handler %p, %p\n", address, newPageAddress);
	}
	outb(0x20, 0x20);
	return;*/
//	pageWalk((uint64_t*)(current_PCB->virtualPML4));
	volatile uint64_t address;
		__asm__ __volatile__(
							"movq %%cr2, %0;"\
							:"=r"(address)
							:
							);
//		kprintf("Page Fault at %p pid is: %d\n",address,current_PCB->pid);

	PCB* pcb = current_PCB;
	mm_struct * mem = pcb->mem;
	VMA* currentVMA = mem->firstVMA;
	while(currentVMA!=NULL && !(address >= currentVMA->start && address<currentVMA->end))
	{
		currentVMA = currentVMA->next;
	}
	if(currentVMA != NULL){
		/*uint64_t newPageAddress = address & ~(0xFFF);
		uint64_t page = (uint64_t)allocatePage2();
		kprintf("%p",page);
		userPhysicalToVirtual((uint64_t)page,newPageAddress, virtual_PML4);
//		kprintf("%p", *(uint64_t*)address);
		kprintf("In page fault handler %p, %p\n", address, newPageAddress);
		outb(0x20, 0x20);
		return;*/
		uint64_t pageAlignedAddress = address & ~(0xFFF);
//		kprintf("newPageAddress : %p\n",pageAlignedAddress);
		uint64_t pt_entry = virtualToPhysical(pageAlignedAddress,(uint64_t*)(pcb->virtualPML4));
//		kprintf("pt_entry : %p\n",pt_entry);
		uint64_t currentCR3;
		__asm__ __volatile__("movq %%cr3, %0":"=b"(currentCR3):);
		uint64_t virtualPML4 = currentCR3 | 0xffffffff80000000ull;
		if((pt_entry & PG_P) == 0x0)
		{
			uint64_t page = kmalloc();
			userPhysicalToVirtual((uint64_t)page & 0xfffffff,pageAlignedAddress, (uint64_t*)virtualPML4);
//			kprintf("the value  updated is %p", *(uint64_t*)pageAlignedAddress);
//			kprintf("page allocated\n");
		}
		else
		{
			if((pt_entry & PG_R) == 0 )
			{
				if((pt_entry & PG_COW) > 0 )
				{
					if(currentVMA->type == STACK || currentVMA->type == HEAP || currentVMA->type == DATA)
					{
						uint64_t newPage = kmalloc();
						uint64_t newPagePhysAddress = newPage & 0xfffffff;
						memcpy((char*)pageAlignedAddress, (char*)newPage,0x1000);
						userPhysicalToVirtual(newPagePhysAddress,pageAlignedAddress,(uint64_t*)virtualPML4);
						__asm__ __volatile__ ("movq %0, %%cr3" ::"r"(virtualPML4&0xfffffff));
//						kprintf("allocating memory");
//						pageWalk((uint64_t*)virtualPML4);
					}
					else{
						kprintf("\nTrying to access data section!!!!!!!!!!");
						sys_exit(r);
//						__asm__ __volatile__ ("");
					}
				}
				else{
					kprintf("\nSEGMENTATION FAULT!!");
//					__asm__ __volatile__("hlt");
					sys_exit(r);
				}
			}
		}

	}
	else{
		kprintf("\nSEGMENTATION FAULT ON ACCESSING ADDRESSS %p\n",address);
//		kprintf("halting");
		sys_exit(r);
//		__asm__ __volatile__("hlt");
	}
	outb(0x20, 0x20);
	//while(1){}
}


void identityPaging(uint64_t bottom, uint64_t top)
{
	uint64_t * PML4 = cr3;
	while(bottom<top){
		userPhysicalToVirtual2(bottom,bottom| 0xffffffff80000000 ,PML4);
		bottom+=0x1000ul;
	}
}


void init_paging(uint64_t physbase, uint64_t physfree, uint64_t max_start, uint64_t max_end){
	uint64_t * PML4 =virtualizeKernMem(physbase,physfree);
	userPhysicalToVirtual2((uint64_t)0xb8000,(uint64_t)0xb8000 | 0xffffffff80000000, PML4);
	virtualToPhysical((uint64_t)0xb8000 | 0xffffffff80000000, PML4);
	identityPaging(0,max_end);
	userPhysicalToVirtual((uint64_t)MMAPAdress,(uint64_t)MMAPAdress | 0xffffffff80000000, PML4);
	MMAPAdress = (uint64_t*)((uint64_t)MMAPAdress | 0xffffffff80000000);
	//virtualToPhysical((uint64_t)MMAPAdress | 0xffffffff80000000, PML4);
//	kprintf("%p\n",(uint64_t)(videoMemLocation) | 0xffffffff80000000);
//	physicalToVirtual((uint64_t)PML4,(uint64_t)PML4,PML4);
	load_cr3((uint64_t)PML4);
	virtual_PML4 = (uint64_t*)((uint64_t)PML4 | 0xffffffff80000000ull);
//	kprintf("\n \n\nthe value is %p\n\n\n\n\n ",PML4);
//	kprintf("PML4 value is %p %p\n",virtual_PML4, ((uint64_t*)virtual_PML4)[510]);
//	kprintf("3");
	if(PML4){}
}



uint64_t * virtualizeKernMem(uint64_t physfree,uint64_t physbase)
{

	uint64_t * PML4 = allocatePage();
//	PML4[510] = (uint64_t)PML4 | 0x3ull;
	cr3 = PML4;
	uint64_t * PDTE = allocatePage();
//	PDTE[510] = (uint64_t)PDTE | 0x3ull;
	uint64_t * PDT  = allocatePage();
//	PDT[510] = (uint64_t)PDT | 0x3ull;
	uint64_t * PT   = allocatePage();
//	PT[510] = (uint64_t)PT | 0x3ull;
	int PML4Offset = getPLM4Offset((uint64_t)&kernmem);
	int PDTEOffset = getPDTEOffset((uint64_t)&kernmem);
	int PDTOffset = getPDTOffset((uint64_t)&kernmem);
	int PTOffset = getPageTableOffset((uint64_t)&kernmem);
	PML4[PML4Offset] = (uint64_t)PDTE | 0x3ull;
	PDTE[PDTEOffset] = (uint64_t)PDT | 0x3ull;
	PDT[PDTOffset] = (uint64_t)PT | 0x3ull;

	uint64_t kernelStart = (uint64_t)physbase;
	uint64_t kernelEnd = (uint64_t)physfree;
	while(kernelStart <kernelEnd){

		PT[PTOffset] = ((uint64_t)kernelStart & 0xfffffff)| 0x3ull;
		kernelStart+=0x1000;
		PTOffset+=1;
	}

	userPhysicalToVirtual((uint64_t)PML4,(uint64_t)PML4 | 0xffffffff80000000ull,PML4);

	userPhysicalToVirtual((uint64_t)PDTE,(uint64_t)PDTE | 0xffffffff80000000ull,PML4);
//	virtualToPhysical((uint64_t)PML4| 0xffffffff80000000ull, PML4);
	userPhysicalToVirtual((uint64_t)PDT,(uint64_t)PDT | 0xffffffff80000000ull,PML4);
//	virtualToPhysical((uint64_t)PDTE| 0xffffffff80000000ull, PML4);
	userPhysicalToVirtual((uint64_t)PT,(uint64_t)PT | 0xffffffff80000000ull,PML4);
	return PML4;
}




void userPhysicalToVirtual2(uint64_t pAddress,uint64_t vAddress, uint64_t *PML4)
{
	uint64_t* PDTE;
//	uint64_t* PDTE
	uint64_t* PDT;
	uint64_t* PT;
	int PML4Offset = getPLM4Offset((uint64_t)vAddress);
	int PDTEOffset = getPDTEOffset((uint64_t)vAddress);
	int PDTOffset = getPDTOffset((uint64_t)vAddress);
	int PTOffset = getPageTableOffset((uint64_t)vAddress);

	uint64_t PDTEVal = PML4[PML4Offset];
	if((PDTEVal & 0x1)>0)
	{
		PDTE =  (uint64_t*)((uint64_t)PDTEVal & ~(0xFFF));
	}
	else{
		PDTE = (uint64_t*)allocatePage();
//		PDTE[100] = 35;
		PML4[PML4Offset] = ((uint64_t)PDTE & 0xfffffff) | 0x3ull;
//		PDTE[510] = ((uint64_t)PDTE & 0xfffffff) | 0x3ll;

	}
	uint64_t PDTVal = PDTE[PDTEOffset];
	if((PDTVal & 0x1)>0){
		PDT =  (uint64_t*)((uint64_t)PDTVal & ~(0xFFF));
	}
	else{
		PDT = (uint64_t*)allocatePage();
		PDTE[PDTEOffset] = ((uint64_t)PDT & 0xfffffff) | 0x3ull;
//		PDT[510] = ((uint64_t)PDT & 0xfffffff) | 0x3ull;
	}
	uint64_t PTVal = PDT[PDTOffset];
	if((PTVal & 0x1)>0){
		PT =  (uint64_t*)((uint64_t)PTVal & ~(0xFFF));
	}
	else{
		PT = (uint64_t*)allocatePage();
		PDT[PDTOffset] = ((uint64_t)PT & 0xfffffff) | 0x3ull;
//		PT[510] = ((uint64_t)PT & 0xfffffff) | 0x3ull;
	}
	PT[PTOffset] = (uint64_t)(pAddress | 0x3ull);
	//kprintf("\n\n%p\n",PT[510]);
}



uint64_t gettingVirtualAddressFromOffsets(uint64_t PML4Index,uint64_t pdteIndex, uint64_t pdtIndex, uint64_t ptIndex){
	uint64_t virtualAddress = (((((((PML4Index<<9)|pdteIndex)<<9)|pdtIndex)<<9)|ptIndex)<<12);
	if(PML4Index > 255){
		virtualAddress |= 0xFFFF000000000000;
	}
	return virtualAddress;
}


void copyParentPageTablesToChild(uint64_t* parentPML4, uint64_t* childPML4)
{
	int pml4Index = 0;
//	volatile uint64_t* childPML4Copy;
//	childPML4Copy = childPML4;
//	kprintf("\n\n%p\n\n\n", *childPML4);
	for(pml4Index =0;pml4Index < 510; pml4Index++)
	{
		if((parentPML4[pml4Index] & PG_P )>0){
			uint64_t* childPDTE = (uint64_t*)kmalloc();
//			kprintf("%p\n", childPDTE);
			childPML4[pml4Index] = ((uint64_t)childPDTE & 0xfffffff)| 0x7;
//			kprintf("%p\n", childPML4[pml4Index]);
//			childPDTE[510] = ((uint64_t)childPDTE & 0xfffffff) | 0x7;
//			kprintf("%p\n",childPDTE[510]);
			uint64_t* parentpdte = (uint64_t*)(((uint64_t)parentPML4[pml4Index] & ~(0xfff)) | 0xffffffff80000000ull);
//			kprintf("%p\n", parentpdte);
//			kprintf("\nPML4 index is OMG %p\n", parentpdte);
			if(parentpdte){}
			int pdteIndex = 0;
			for(pdteIndex = 0; pdteIndex <= 511; pdteIndex++){
				if((parentpdte[pdteIndex] & PG_P )>0){
					uint64_t* childPDT = (uint64_t*)kmalloc();
					childPDTE[pdteIndex] = ((uint64_t)childPDT & 0xfffffff) | 0x7;
//					childPDT[510] = ((uint64_t)childPDT & 0xfffffff) | 0x7;
//					kprintf("%p",childPDTE[pdteIndex]);
					uint64_t* parentpdt = (uint64_t*)(((uint64_t)parentpdte[pdteIndex] & ~(0xfff)) | 0xffffffff80000000ull);
//					kprintf("%p", parentpdt);
					int pdtIndex = 0;
					for(pdtIndex = 0; pdtIndex <= 511; pdtIndex++){
						if((parentpdt[pdtIndex] & PG_P )>0){
							uint64_t* childPT = (uint64_t*)kmalloc();
							childPDT[pdtIndex] = ((uint64_t)childPT &0xfffffff) | 0x7;
//							childPT[510] = ((uint64_t)childPT & 0xfffffff) | 0x7;
//							kprintf("%p",childPDT[pdtIndex]);
							uint64_t* parentpt = (uint64_t*)(((uint64_t)parentpdt[pdtIndex] & ~(0xfff)) | 0xffffffff80000000ull);
							int ptIndex = 0;
							for(ptIndex = 0; ptIndex <= 511; ptIndex++){
								if((parentpt[ptIndex] & PG_P )>0){
									parentpt[ptIndex] = parentpt[ptIndex] & ~(0xfff);
									parentpt[ptIndex] = parentpt[ptIndex] | 0x5ull | PG_COW ;
//									parentpt[ptIndex] = (parentpt[ptIndex] & ~(PG_R)) | PG_COW);
									childPT[ptIndex] = parentpt[ptIndex] ;
//									parentpt[ptIndex] = ((parentpt[ptIndex] & ~(PG_R)) | PG_COW);
//									parentp[ptIndex] = ((parentpt[ptIndex] & ~(0x2) | PG_COW));
//									parentpt[ptIndex] =  (parentpt[ptIndex] & (~0x2)) | PG_COW;
//									kprintf("%p ",childPT[ptIndex]);
//									kprintf("%p        ", parentpt[ptIndex]);
//									uint64_t* parentpt = (uint64_t*)(((uint64_t)parentpdt[pdtIndex] & ~(0xfff)) | 0xffffffff80000000ull);
								}
							}
						}
					}
				}
			}
		}
	}
}
