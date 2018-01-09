#ifndef __PROCESS_H
#define __PROCESS_H

#include "sys/defs.h"
//#include "sys/tarfs.h"

#define USER_VIRTUAL_ADDRESS 0x4000000000000


typedef struct vma VMA;
struct vma {
	VMA* next;
	uint64_t start;
	uint64_t end;
	uint64_t flags;
	uint64_t type;
};

typedef struct mm_struct mm_struct;
typedef struct mm_struct {
	VMA* firstVMA;
} mm_struct;

typedef struct __pcb_file_structure {
//	char name[100];
//	uint64_t size;
//	uint64_t typeflag;
	uint64_t inode_no;
//	uint64_t parent_inode_no;
	uint64_t read_offset;
	uint8_t open_flag;
//	uint64_t ref_open_count;
} file_p;
typedef struct _PCB  PCB;
struct _PCB {
   char p_name[20];
   uint64_t* stack;
   uint64_t* kernelStack;
   uint64_t kernelStackAddress;
   //char* kern;
   uint64_t pid;
   uint64_t time_slice;
   uint64_t time_slice_left;
   uint64_t sleepSeconds;
   uint64_t ticksLastRecorded;
   uint64_t ppid;
   uint64_t parentpid;
   uint64_t rsp;
   uint64_t rip;
   uint64_t PML4;
   uint64_t virtualPML4;
   int waitPid;
   int children[20];
   mm_struct* mem;
   uint64_t curr_inode_no;
   enum {RUNNING, READY, SLEEPIN, ZOMBIE, KILLED} state;
   int exit_status;
   file_p pcb_fd[MAX_FILE];
   PCB * next;
   uint64_t heap_mem;
   uint64_t* heap_ptr;
};


PCB* zombieList;
PCB* current_PCB;
typedef struct _THREAD {
   uint64_t* stack;
   uint64_t rsp;
   uint64_t rip;
   uint64_t rdi;
   uint64_t rax;
   uint64_t rbx;
   uint64_t rbp;
   uint64_t rsi;
   //enum {RUNNIN, READY, SLEEPIN, ZOMBIE} state;
   int exit_status;
} THREAD;


void addToZombieList(PCB *temp);
struct VMA{
	uint64_t startAddress;
	uint64_t size;
	int fd;
	struct VMA* next;
};
uint64_t pid_pp;
void context_switch2(PCB* old, PCB* new);
PCB* init_process();
PCB* init;
THREAD* createThread();
PCB* createProcess();
uint64_t createChildProcess();
void switchToRing3(PCB* pcb);
uint64_t malloc(uint64_t size);
uint64_t getpid();
uint64_t getNewPid();
uint64_t getCurrentProcPid();
uint64_t getParentPid();

#endif
