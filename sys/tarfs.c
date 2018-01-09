#include "sys/tarfs.h"
#include "sys/process.h"
#include "sys/string.h"
#include "sys/util.h"
#include "sys/vmem.h"
#include "sys/kprintf.h"
#include "sys/elf64.h"
#include "sys/pmem.h"
#include "sys/syscall.h"

extern uint64_t* PML4;
extern uint64_t* cr3;
extern uint64_t* virtual_PML4;

file create_file_structure(char* name, uint64_t inode_no, uint64_t typeflag, uint64_t parent_inode_no) {
	file a;
	strcpy(a.name, name);
	a.size = 0x1000;
	a.typeflag = typeflag;
	a.file_address_start = (uint64_t) kmalloc();
	a.file_address_end = a.file_address_start + 0x1000;
	a.inode_no = inode_no;
	a.parent_inode_no = parent_inode_no;
	return a;
}

uint64_t get_parent_inode_no(char* file_name) {
	char file_path[100], temp[100];
	strcpy(file_path, file_name);
//	kprintf("%s", file_path);
	char* files[100];
	uint64_t num_files = strtok(file_path, '/', files);
	uint64_t i = 0;
	strcpy(temp, "/");
//	kprintf("%d", num_files);
//		while(1){};
	while(i < num_files-1) {
		strcat(temp, files[i]);
		strcat(temp, "/");
		i++;
	}

	uint64_t parent_node = 3;
	while((strcmp(temp, fd[parent_node].name) != 0)) {
		parent_node++;
	}
	return parent_node;
}

void init_tarfs() {

	fd[0] = create_file_structure("STDIN", 0, TYPE_FILE, 0);
	STDINPTR = fd[0].file_address_start;
	scanPointerStart = STDINPTR;
	scanPointerCurrent = STDINPTR;


	fd[1] = create_file_structure("STDOUT", 1, TYPE_FILE, 1);
	STDOUTPTR = fd[1].file_address_start;

	fd[2] = create_file_structure("STDERR", 2, TYPE_FILE, 2);
	fd[3] = create_file_structure("/", 3, TYPE_DIRECTORY, 3);

	uint64_t curr_addr = 0;
	int i = 4;
	char t[100] = "/";

	ustar* ustar_var = (ustar*) (&_binary_tarfs_start + curr_addr);
	while(strlen(ustar_var->name) != 0) {
		strcpy(fd[i].name, ustar_var->name);
		strcpy(fd[i].name, strcat(t, fd[i].name));
		strcpy(t, "/");
		fd[i].size = oct_to_dec(ustar_var->size);
		fd[i].typeflag = atoi(ustar_var->typeflag);
		fd[i].file_address_start = (uint64_t)&_binary_tarfs_start + curr_addr + 512;
		fd[i].file_address_end = fd[i].file_address_start + ((((fd[i].size)+511)/512)+1)*512;
		fd[i].inode_no = (uint64_t) i;
		fd[i].parent_inode_no = get_parent_inode_no(fd[i].name);
		curr_addr += (((fd[i].size+511)/512)+1)*512;
		ustar_var = (ustar*) (&_binary_tarfs_start + curr_addr);
		i++;
		int j = i-1;
		kprintf("File name: %s, inode: %d, parentinode: %d, size: %d\n", fd[j].name, fd[j].inode_no, fd[j].parent_inode_no, fd[j].size );
	}
}

uint64_t get_file_start_address(char* fileName) {
	int i = 3;
	while((strcmp(fileName,fd[i].name)) != 0){
//		kprintf("Not same %s %s \n",fileName,fd[i].name );
		i++;
	}
//	kprintf("hello\n");
//	kprintf("%s\n",((Elf64_Ehdr*)tarfs_main[i].file_address_header)[2] );
//	kprintf("Same found %s %s \n",fileName,fd[i].name );
	return fd[i].file_address_start;
}




void kfree(uint64_t vAddress, uint64_t* PML4)
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
		return;
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
		return;
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
		return;
	}
	else{
		PT = (uint64_t*)((PT_val& ~(0xFFF)) | 0xffffffff80000000);
//		kprintf("PDT wrks fine\n");
	}
/*---------------------------------------------------------------------------------------*/
	uint64_t address = PT[PTOffset];
	if((address  & PG_P)==0 || (address  & PG_R) == 0){
		if(PML4Offset < 511)
			PT[PTOffset] = 0x0;
	}
	else{
		mmapSetBit(address/0x1000);
		if(PML4Offset < 511)
			PT[PTOffset] = 0x0;
	}
	uint64_t oldCr3 = 0;
	/*__asm__ __volatile__("movq %%cr3, %0"::"r"(oldCr3));
	__asm__ __volatile__("movq %0, %%cr3":"=r"(PML4):);
	__asm__ __volatile__("movq %%cr3, %0"::"r"(oldCr3));*/
	__asm__ __volatile__ ("movq %%cr3, %0":"=r"(oldCr3):);
	__asm__ __volatile__ ("movq %0, %%cr3"::"r"((uint64_t)PML4 & 0xfffffff));
	__asm__ __volatile__ ("movq %0, %%cr3"::"r"(oldCr3));
	if(oldCr3){}
}

void deleteVMAentries(PCB *pcb){
	mm_struct * mem  = pcb->mem;
	VMA* currentVMA = mem->firstVMA;
	while(currentVMA!= NULL){
		uint64_t startAddr = ((currentVMA->start) & ~(0xfff));
		uint64_t endAddr = (currentVMA->end);
		while(startAddr < endAddr){
			kfree(startAddr, (uint64_t*)(pcb->virtualPML4));
			startAddr +=0x1000;
		}
		VMA* vmaToDelete = currentVMA;
		currentVMA = currentVMA->next;
		kfree((uint64_t)vmaToDelete,(uint64_t*)(pcb->virtualPML4));
	}
	mem->firstVMA = NULL;
}


PCB* execve(PCB* pcb, char* s, char * argv[], char * envp[])
{
	char** tokenArrays = (char**)kmalloc();
	if(tokenArrays){}
//	pcb->state = READY;
	char * temp = (char *)kmalloc();
	strcpy(temp, s);
	int n = strtok(temp,'/',tokenArrays);
	strcpy(pcb->p_name,tokenArrays[n-1]);
//	kprintf("%s",tokenArrays[n-1]);
	kfree((uint64_t)tokenArrays,(uint64_t*)(current_PCB->virtualPML4));
	kfree((uint64_t)temp,(uint64_t*)(current_PCB->virtualPML4));
//	strcpy(pcb->p_name, tokenArray[n-1]);
//	kfree((uint64_t)tokenArray, (uint64_t*)(current_PCB->virtualPML4));
//	kprintf("in execve");
	uint64_t fileStartAddress = get_file_start_address(s);
	Elf64_Ehdr* elf64_ehdr = (Elf64_Ehdr*)fileStartAddress;
	char TEMP_ENVP[10][30];
	int w = 0;
//	if(envp){
//		int w = 0;
		while(envp[w] != 0) {
			strcpy(TEMP_ENVP[w], envp[w]);
			w++;
		}
//	}
	int z = 0;
	char TEMP_ARGV[10][30];
	int TEMP_ARGC = 0;
//	if(argv){
//		int z = 1;
		while(argv[z] != 0) {
			strcpy(TEMP_ARGV[z], argv[z]);
			z++;
		}

		TEMP_ARGC = z;
//	}
	deleteVMAentries(pcb);
	__asm__  __volatile__ ("movq %0, %%cr3"::"r"((uint64_t)(pcb->PML4)));
	char* e_ident = (char*)&(elf64_ehdr->e_ident);
	if(*(e_ident) == '\x7F' && *(e_ident+1) == 'E' && *(e_ident+2) == 'L' && *(e_ident+3) == 'F' ) {
//		kprintf("in elf");

		Elf64_Phdr * elf64_phdr = (Elf64_Phdr * )((uint64_t)elf64_ehdr + elf64_ehdr->e_phoff);
		int numberOfProgHeaders = elf64_ehdr->e_phnum;
		int i = 0;
		uint64_t startAddr;
		uint64_t size;
		while(i<numberOfProgHeaders) {
			if((elf64_phdr->p_type) == 1 ) {
				startAddr = elf64_phdr->p_vaddr;
				size = elf64_phdr->p_memsz;
				load_cr3(pcb->PML4);
				virtual_PML4 =(uint64_t*)( pcb->virtualPML4);
				uint64_t type = 0;
				if(elf64_phdr->p_flags == READEXECUTE){
					type = TEXT;
				}
				else if(elf64_phdr->p_flags == READWRITE){
					type = DATA;
				}
//				uint64_t retAd = kmalloc();
//				virtualToPhysical(retAd, (uint64_t*)(pcb->virtualPML4));
//				if(retAd){}
//				while(1){}
				createAndInsertVMA(pcb, elf64_phdr->p_vaddr, elf64_phdr->p_vaddr + elf64_phdr->p_memsz,elf64_phdr->p_flags, type);
//				uint64_t virtualPML4 = pcb->virtualPML4;
//				kprintf("virtualPML4 %p\n",virtualPML4);
				cr3 = (uint64_t*)(pcb->PML4);

//				handled by page fault handler
				memcpy((char*)((uint64_t)elf64_ehdr + (uint64_t)(elf64_phdr->p_offset)),(char*) startAddr, elf64_phdr->p_filesz);
				memset((char*)(startAddr+elf64_phdr->p_filesz), 0, size -  elf64_phdr->p_filesz);
//				kprintf("copied");
//				kprintf("loaded code\n");


			}
			elf64_phdr+=1;
			i=i+1;
		}
//		while(1){}
//		creating user stack
		uint64_t stackVirtualAddressStart = (uint64_t)(0x10000000ull);
		uint64_t stackVirtualAddressEnd = (uint64_t)(0x10000000ull + 0x10000ull);
		uint64_t type = STACK;
		uint64_t flags  = READWRITE;
		createAndInsertVMA(pcb, stackVirtualAddressStart, stackVirtualAddressEnd,flags, type);
		pcb->rsp = (stackVirtualAddressEnd - 0x20);
//		creating user heap
		uint64_t heapVirtualAddressStart = (uint64_t)(0x80000000ull);
		uint64_t heapVirtualAddressEnd = (uint64_t)(0x80000000ull + 0x10000ull);
		type = HEAP;
		flags  = READWRITE;
		createAndInsertVMA(pcb, heapVirtualAddressStart, heapVirtualAddressEnd,flags, type);
		(pcb->heap_mem) = heapVirtualAddressStart;
		pcb->heap_ptr = (uint64_t*)(pcb->heap_mem);


//		walkVMA(pcb);
		pcb->stack = (uint64_t*)kmalloc();

		pcb->rip = elf64_ehdr->e_entry;
		uint64_t oldCr3 = 0;
		__asm__ __volatile__ ("movq %%cr3, %0":"=r"(oldCr3):);
		__asm__ __volatile__ ("movq %0, %%cr3"::"r"((uint64_t)(pcb->PML4) & 0xfffffff));
		if(oldCr3){}


		int ARGC = TEMP_ARGC;

		char **ARGV = (char**)malloc(sizeof(char*)*10);
		int k = 0;
		while(k < z) {
			ARGV[k] = (char*)malloc(sizeof(char) * 30);
			strcpy(ARGV[k], TEMP_ARGV[k]);
			k++;
		}

		char **ENVP = (char**)malloc(sizeof(char*) * 10);
		int q = 0;
		while(q < w) {
			ENVP[q] = (char*)malloc(sizeof(char) * 30);
			strcpy(ENVP[q], TEMP_ENVP[q]);
			q++;
		}

		__asm__ __volatile__ ("movq %0, %%cr3"::"r"(oldCr3));
		*(uint64_t*)(pcb->rsp) = (uint64_t)ARGC;
		*(uint64_t*)(pcb->rsp+8) = (uint64_t)ARGV;
		*(uint64_t*)(pcb->rsp+16) = (uint64_t)ENVP;

		return pcb;
	}
	else{
		kprintf("\nNot an executable file");
		return NULL;
	}
}




PCB* loadExecutable(char* s) {
	clear_screen();
	scanLock = 0;
//	kprintf("In load");
	uint64_t fileStartAddress = get_file_start_address(s);
	Elf64_Ehdr* elf64_ehdr = (Elf64_Ehdr*)fileStartAddress;
//	doHandlePageFault = 1;
	//kprintf("the value is %s\n", header->e_ident);
	char* e_ident = (char*)&(elf64_ehdr->e_ident);
	if(e_ident[0] == '\x7F' && (str_contains_substr2(e_ident, "ELF", 1, 3) == 1)) {
		PCB* pcb = createProcess();
		current_PCB = pcb;
		Elf64_Phdr * elf64_phdr = (Elf64_Phdr * )((uint64_t)elf64_ehdr + elf64_ehdr->e_phoff);
		int numberOfProgHeaders = elf64_ehdr->e_phnum;
		int i = 0;
		uint64_t startAddr;
		uint64_t size;
		while(i<numberOfProgHeaders) {
			if((elf64_phdr->p_type) == 1 ) {
				startAddr = elf64_phdr->p_vaddr;
				size = elf64_phdr->p_memsz;
				load_cr3(pcb->PML4);
				virtual_PML4 =(uint64_t*)( pcb->virtualPML4);
				uint64_t type = 0;
				if(elf64_phdr->p_flags == READEXECUTE){
					type = TEXT;
				}
				else if(elf64_phdr->p_flags == READWRITE){
					type = DATA;
				}
				createAndInsertVMA(pcb, elf64_phdr->p_vaddr, elf64_phdr->p_vaddr + elf64_phdr->p_memsz,elf64_phdr->p_flags, type);
//				kprintf("Ypo\n");
				//*(uint64_t*)(0x500000000)=100;
//				virtualToPhysical(pcb->virtualPML4);
//				kprintf("pcb->virtualPML4 %p\n", pcb->virtualPML4);
//				uint64_t virtualPML4 = pcb->virtualPML4;
//				kprintf("virtualPML4 %p\n",virtualPML4);
				cr3 = (uint64_t*)(pcb->PML4);
//				kprintf("cr3 value is %p\n",);
//				kprintf("Ypo\n");
//				kprintf("the value of vaddr was %p\n",(elf64_phdr->p_vaddr));
//				kprintf("elf64_phdr->p_filesz is %p\n", elf64_phdr->p_filesz);
//				uint64_t addr = (elf64_phdr->p_vaddr);
//				uint64_t size2 = 0;

				/*while(size<elf64_phdr->p_filesz){
					kprintf("setting value\n");
					kprintf("vaddr %p\n", virtual_PML4);
					*(uint64_t*)(addr) = 0x0;
//					userPhysicalToVirtual(tempPage & 0xfffffff,addr,virtual_PML4);
					addr+=0x1000;
					size2+=0x1000;
				}*/
//				kprintf("out\n");
//				handled by page fault handler
				memcpy((char *)((uint64_t )elf64_ehdr +  (uint64_t)(elf64_phdr->p_offset) ),(char*) startAddr,  elf64_phdr-> p_filesz);
				memset((char*)(startAddr+ elf64_phdr->p_filesz), 0, size -  elf64_phdr-> p_filesz);
//				kprintf("after memcopy\n");
//				kprintf("YO %p\n",*(uint64_t*)(elf64_phdr->p_vaddr));
			}
			elf64_phdr+=1;
			i=i+1;
//			kprintf("The value of i is %x",i);
		}
//		creating user stack
		uint64_t stackVirtualAddressStart = (uint64_t)(0x10000000ull);
		uint64_t stackVirtualAddressEnd = (uint64_t)(0x10000000ull + 0x10000ull);
		uint64_t type = STACK;
		uint64_t flags  = READWRITE;
		createAndInsertVMA(pcb, stackVirtualAddressStart, stackVirtualAddressEnd,flags, type);
		pcb->rsp = (stackVirtualAddressEnd - 0x20);
		pcb->time_slice = 100;
		pcb->time_slice_left = pcb->time_slice;
//		creating user heap
		uint64_t heapVirtualAddressStart = (uint64_t)(0x80000000ull);
		uint64_t heapVirtualAddressEnd = (uint64_t)(0x80000000ull + 0x10000ull);
		type = HEAP;
		flags  = READWRITE;
		createAndInsertVMA(pcb, heapVirtualAddressStart, heapVirtualAddressEnd,flags, type);
		(pcb->heap_mem) = heapVirtualAddressStart;
		pcb->heap_ptr = (uint64_t*)(pcb->heap_mem);


//		walkVMA(pcb);
		pcb->stack = (uint64_t*)kmalloc();
		//kprintf("pcb mem%p\n",pcb->mem);
		//		doHandlePageFault = 0;
		//		kprintf("returning i");
		pcb->rip = elf64_ehdr->e_entry;
		//		kprintf("\npcb->rip is %p",pcb->rip);
				//while(1){}
		uint64_t oldCr3 = 0;
		__asm__ __volatile__ ("movq %%cr3, %0":"=r"(oldCr3):);
		__asm__ __volatile__ ("movq %0, %%cr3"::"r"((uint64_t)(pcb->PML4) & 0xfffffff));
		if(oldCr3){}

		int ARGC = 1;

		char **ARGV = (char**)malloc(sizeof(char*)*10);
		ARGV[0] = (char*)malloc(sizeof(char) * 10);
		ARGV[1] = (char*)malloc(sizeof(char) * 10);
		strcpy(ARGV[0],"Zero");

		char **E = (char**)malloc(sizeof(char*)*10);
		E[0] = (char*)malloc(sizeof(char) * 20);
		strcpy(E[0], "$PATH=/ ");
		E[1] = (char*)malloc(sizeof(char) * 20);
		strcpy(E[1], "$HOME=/ ");
		E[2] = (char*)malloc(sizeof(char) * 20);
		strcpy(E[2], "$USER=root ");
		E[3] = (char*)malloc(sizeof(char) * 20);
		strcpy(E[3], "$PS1=>>> ");

		__asm__ __volatile__ ("movq %0, %%cr3"::"r"(oldCr3));
		*(uint64_t*)(pcb->rsp) = (uint64_t)ARGC;
		*(uint64_t*)(pcb->rsp+8) = (uint64_t)ARGV;
		*(uint64_t*)(pcb->rsp+16) = (uint64_t)E;

		//		(char*) a = "2";
		//		__asm__ volatile("pushq %0"::"r"((char*)a[0]));


		/*		a[2] = (char*)malloc(sizeof(char) * 10);
				strcpy(a[1],"First");
		//		__asm__ __volatile("pushq %0"::"r"((char*)a[1]));
				kprintf("%s",a[1]);
				strcpy(a[2],"Second");
		//		__asm__ __volatile("pushq %0"::"r"((char*)a[2]));
				kprintf("%s",a[2]);

		//		__asm__ __volatile__("movq %0, %%rsp"::"r"((uint64_t)a));

				kprintf("the address would habebeen %p\n\n\n\n\n\n\n",pcb->rsp+8);*/
//		__asm__ __volatile__ ("movq %0, %%cr3"::"r"(oldCr3));
//		*(uint64_t*)(pcb->rsp+8) = (uint64_t)a;//		(char**)(pcb->rsp+16) = a;
		return pcb;
	}
	else{
		kprintf("\nNot an elf file");
		return NULL;
	}
}

uint64_t get_absolute_path(char* file_path, uint64_t current_inode_no, uint64_t mode) {
	typedef struct _cwd {
		uint64_t inode_no;
		uint64_t parent_inode_no;
		char name[100];
	} C;

	C cwd;
	cwd.inode_no = current_inode_no;
	cwd.parent_inode_no = fd[current_inode_no].parent_inode_no;
	strcpy(cwd.name, fd[current_inode_no].name);

	char* temp = (char*)kmalloc();
//	kprintf("\nin get begin: %s", &file_path[0]);
//	while(1){}
//	char temp[100];
	char path[100];
	strcpy(path, file_path);
	char* files[100];
	uint64_t num_files = strtok(path, '/', files);
//	file_p cwd = current_PCB->current_working_dir;
//	kprintf("\nin get begin inode: %d", inode);
//	strcpy(temp, strcat(temp, "/"));
	uint64_t inode = cwd.inode_no;
	uint64_t parent_inode = cwd.parent_inode_no;
	if(mode == 1) {
		strcpy(temp, "/");
	}
	else {
		strcpy(temp, fd[inode].name);
	}
	int k = 0;
	file fd_temp;
	while(k < num_files && cwd.inode_no != 0x9999ull && cwd.parent_inode_no != 0x9999ull) {
		inode = cwd.inode_no;
		parent_inode = cwd.parent_inode_no;
		if((strcmp( &(*files[k]), "..") != 0) && (strcmp(&(*files[k]), ".") != 0)) {
			strcpy(temp, strcat(temp, &(*files[k])));
			if(k == num_files-1) {
				int i = 3;
				while(strlen(fd[i].name) != 0) {
					if(strncmp(temp, fd[i].name, strlen(temp)) == 0) {
						if(fd[i].typeflag == TYPE_DIRECTORY) {
							strcpy(temp, strcat(temp, "/"));
							break;
						}
					}
					i++;
				}
				break;
			}
			else {
				strcpy(temp, strcat(temp, "/"));
				int i = 3;
				while(strlen(fd[i].name) != 0) {
					if(strcmp(temp, fd[i].name) == 0) {
						cwd.inode_no = fd[i].inode_no;
						cwd.parent_inode_no = fd[i].parent_inode_no;
						break;
					}
					else {
						cwd.inode_no = 0x9999ull;
						cwd.parent_inode_no = 0x9999ull;
					}
					i++;
				}
			}

		}
		else if((strcmp( &(*files[k]), ".") == 0) && (k == 0)) {
			strcpy(temp, fd[inode].name);

		}
		else if((strcmp( &(*files[k]), "..") == 0)) {
			strcpy(temp, fd[parent_inode].name);
			fd_temp = fd[parent_inode];
			cwd.inode_no = fd_temp.inode_no;
			cwd.parent_inode_no = fd_temp.parent_inode_no;
		}
		k++;
	}
//	kprintf("\nin get end: %s", &temp[0]);
	int j = 3;
	uint64_t flag_inode = 0x9999ull;
	while(strlen(fd[j].name) != 0) {
		if(strcmp(temp, fd[j].name) == 0) {
			flag_inode = fd[j].inode_no;
		}
		j++;
	}
	if(flag_inode == 0x9999ull) {
		strcpy(temp, "Nooo");
	}
	kfree((uint64_t)temp,(uint64_t*)(current_PCB->virtualPML4));
//	return &temp[0];
//	kprintf("in get: %s", &temp[0]);
	return flag_inode;
}

uint64_t open(char* file_path) {
	uint64_t cwd = current_PCB->curr_inode_no;
//	kprintf("\n in kernel: %s %d", file_path, cwd);
	uint64_t curr_inode_no = get_absolute_path(&file_path[0], cwd, 1);
//	kprintf("\n in kernel: %d", curr_inode_no);
	if(curr_inode_no == 0x9999ull) {
		return FILE_ERROR;
	}
//	strcpy(current_PCB->pcb_fd[curr_inode_no].name, fd[curr_inode_no].name);
//	current_PCB->pcb_fd[curr_inode_no].typeflag = fd[curr_inode_no].typeflag;
	current_PCB->pcb_fd[curr_inode_no].inode_no = fd[curr_inode_no].inode_no;
//	current_PCB->pcb_fd[curr_inode_no].parent_inode_no = fd[curr_inode_no].parent_inode_no;
	current_PCB->pcb_fd[curr_inode_no].read_offset = 0x0ull;
	if(fd[curr_inode_no].typeflag == TYPE_DIRECTORY) {
		current_PCB->pcb_fd[curr_inode_no].open_flag = 1;
	}
	else if(fd[curr_inode_no].typeflag == TYPE_FILE) {
		if(current_PCB->pcb_fd[curr_inode_no].open_flag == 1) {
			return FILE_ALREADY_OPEN;
		}
		else {
			current_PCB->pcb_fd[curr_inode_no].open_flag = 1;
		}
	}
	return curr_inode_no;
}

uint64_t read_directory(uint64_t inode_no) {
	char* buf = (char*)kmalloc();
	int i = 3, length = strlen(fd[inode_no].name);
	if((inode_no < 3) || (strlen(fd[inode_no].name) == 0)) {
		strcpy(buf, "No such file or directory");
		return ((uint64_t)(&buf[0]));
	}
	if(fd[inode_no].typeflag != TYPE_DIRECTORY) {
		strcpy(buf, fd[inode_no].name+length);
		return ((uint64_t)(&buf[0]));
	}
	while(strlen(fd[i].name) != 0) {
		if(fd[i].parent_inode_no == inode_no && fd[i].inode_no != 3) {
			strcat(buf, fd[i].name+length);
			strcat(buf, "   ");
		}
		i++;
	}
	if(strlen(buf) == 0) {
		strcpy(buf, "");
	}
	return ((uint64_t)(&buf[0]));
}

uint64_t read_file(uint64_t inode_no, uint64_t size) {
	char* buffer = (char*)kmalloc();
	if((inode_no < 3) || (strlen(fd[inode_no].name) == 0)) {
		strcpy(buffer, "No such file or directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(fd[inode_no].typeflag == TYPE_DIRECTORY) {
		strcpy(buffer, "Is a directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(current_PCB->pcb_fd[inode_no].open_flag == 0) {
		strcpy(buffer, "File is not open");
		return ((uint64_t)(&buffer[0]));
	}
//	kprintf("\n%x %x %x\n", fd[inode_no].file_address_start, current_PCB->pcb_fd[inode_no].read_offset, fd[inode_no].file_address_end);
	if((fd[inode_no].file_address_start)+(current_PCB->pcb_fd[inode_no].read_offset) > (fd[inode_no].file_address_end)) {
//		kprintf("\nShould not be in here...tarfs structure wrongly created");
		strcpy(buffer, "");
		return ((uint64_t)(&buffer[0]));
	}
	char* file_ptr = (char*)((fd[inode_no].file_address_start) + (uint64_t)(current_PCB->pcb_fd[inode_no].read_offset));
	int i = 0;
	while(i < size && file_ptr[i]!='\0') {
		buffer[i] = file_ptr[i];
		i++;
	}
//	kprintf("hi\n");
	current_PCB->pcb_fd[inode_no].read_offset = (uint64_t)&file_ptr[i] - (fd[inode_no].file_address_start);
//	kprintf("\n%x \n", current_PCB->pcb_fd[inode_no].read_offset);
//	uint64_t file_ptr = fd_temp.file_address_start;
//	buffer = (char*) file_ptr;
	return ((uint64_t)(&buffer[0]));
}

uint64_t read_file_per_line(uint64_t inode_no) {

	char* buffer = (char*)kmalloc();
	if((inode_no < 3) || (strlen(fd[inode_no].name) == 0)) {
		strcpy(buffer, "No such file or directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(fd[inode_no].typeflag == TYPE_DIRECTORY) {
		strcpy(buffer, "Is a directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(current_PCB->pcb_fd[inode_no].open_flag == 0) {
		strcpy(buffer, "File is not open");
		return ((uint64_t)(&buffer[0]));
	}
//	kprintf("\n%x %x %x\n", fd[inode_no].file_address_start, current_PCB->pcb_fd[inode_no].read_offset, fd[inode_no].file_address_end);
	if((fd[inode_no].file_address_start)+(current_PCB->pcb_fd[inode_no].read_offset) > (fd[inode_no].file_address_end)) {
//		kprintf("\nShould not be in here...tarfs structure wrongly created");
		strcpy(buffer, "");
		return ((uint64_t)(&buffer[0]));
	}
	char* file_ptr = (char*)((fd[inode_no].file_address_start) + (uint64_t)(current_PCB->pcb_fd[inode_no].read_offset));
	int i = 0;
	while(file_ptr[i]!='\n' && file_ptr[i]!='\0') {
		buffer[i] = file_ptr[i];
		i++;
	}
//	kprintf("%s\n", buffer);
	current_PCB->pcb_fd[inode_no].read_offset = (uint64_t)&file_ptr[i+1] - (fd[inode_no].file_address_start);
//	kprintf("\n%x \n", current_PCB->pcb_fd[inode_no].read_offset);
//	uint64_t file_ptr = fd_temp.file_address_start;
//	buffer = (char*) file_ptr;
	return ((uint64_t)(&buffer[0]));
}

uint64_t cat(uint64_t inode_no, char* buffer) {
//	char* buffer = (char*) kmalloc();
//	kprintf("inode %d\n", inode_no);
	if((inode_no < 3) || (strlen(fd[inode_no].name) == 0)) {
		strcpy(buffer, "No such file or directory");
		return FILE_NOT_READ;
	}
	if(fd[inode_no].typeflag == TYPE_DIRECTORY) {
		strcpy(buffer, "Is a directory");
		return FILE_NOT_READ;
	}
	if(current_PCB->pcb_fd[inode_no].open_flag == 0) {
		strcpy(buffer, "File is not open");
		return FILE_NOT_READ;
	}

	if((fd[inode_no].file_address_start)+(current_PCB->pcb_fd[inode_no].read_offset) >= (fd[inode_no].file_address_end)) {
//		kprintf("\nShould not be in here...tarfs structure wrongly created");
		strcpy(buffer, "");
		return FILE_READ_COMPLETELY;
	}
	char* file_ptr = (char*)((fd[inode_no].file_address_start) + (uint64_t)(current_PCB->pcb_fd[inode_no].read_offset));
	int i = 0;
	while(i < BUFFER_SIZE/* && file_ptr[i]!='\0'*/) {
		buffer[i] = file_ptr[i];
		i++;
	}
//	kprintf("hi\n");
	current_PCB->pcb_fd[inode_no].read_offset = (uint64_t)&file_ptr[i] - (fd[inode_no].file_address_start);
//	kprintf("\n%x \n", current_PCB->pcb_fd[inode_no].read_offset);
//	uint64_t file_ptr = fd_temp.file_address_start;
//	buffer = (char*) file_ptr;
	return FILE_PARTIALLY_READ;
}

uint64_t write_file(uint64_t inode_no, char* buffer) {
	if((inode_no < 3) || (strlen(fd[inode_no].name) == 0)) {
		strcpy(buffer, "No such file or directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(fd[inode_no].typeflag == TYPE_DIRECTORY) {
		strcpy(buffer, "Is a directory");
		return ((uint64_t)(&buffer[0]));
	}
	if(current_PCB->pcb_fd[inode_no].open_flag == 0) {
		strcpy(buffer, "File is not open");
		return ((uint64_t)(&buffer[0]));
	}
	uint64_t file_ptr = fd[inode_no].file_address_start;
	char* temp_buf = (char*) kmalloc();
	temp_buf = (char*) file_ptr;
	int z = 0;
	uint64_t buf_len = strlen(buffer);
	while(z < buf_len){
		temp_buf[z] = buffer[z];
		z++;
	}
	temp_buf[z] = '\0';
	return 0;
}

uint64_t close(uint64_t inode_no) {
	current_PCB->pcb_fd[inode_no].read_offset = 0x0ull;
	current_PCB->pcb_fd[inode_no].open_flag = 0;
	return 0;
}

uint64_t get_current_working_dir() {
	char* buffer = (char*) kmalloc();
	strcpy(buffer, fd[current_PCB->curr_inode_no].name);
//	kprintf("\nin kern: %s", buffer);
	return ((uint64_t)buffer);
}

uint64_t change_directory(char* path) {
	char* buffer = (char*) kmalloc();

	if((strcmp(path, "/") == 0) || (strcmp(path, "~") == 0)) {
		current_PCB->curr_inode_no = 3;
		strcpy(buffer, fd[current_PCB->curr_inode_no].name);
//		kprintf("nooo %s", path);
		return ((uint64_t)&buffer[0]);
	}
	uint64_t temp_cwd = current_PCB->curr_inode_no;
	uint64_t curr_inode_no = get_absolute_path(&path[0], temp_cwd, 0);
//	kprintf("\nIn kernel %d", curr_inode_no);
	if(curr_inode_no == 0x9999ull) {
		strcpy(buffer, "No such file or directory");
		return ((uint64_t)&buffer[0]);
	}
	if(fd[curr_inode_no].typeflag == TYPE_FILE) {
		strcpy(buffer, "Not a directory");
		return ((uint64_t)&buffer[0]);
	}
	current_PCB->curr_inode_no = curr_inode_no;
	strcpy(buffer, fd[current_PCB->curr_inode_no].name);
//	strcpy(buffer, current_PCB->current_working_dir.name);
	return ((uint64_t)&buffer[0]);
}
