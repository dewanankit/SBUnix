#ifndef _TARFS_H
#define _TARFS_H

#include "sys/defs.h"
#include "sys/process.h"
//#include "string.h"
//#include "util.h"
//#include "sys/vmem.h"
//#include "sys/kprintf.h"
//#include "sys/elf64.h"

uint64_t startPointerScan;
uint64_t currentPointerScan;

#define TYPE_FILE 			0
#define TYPE_DIRECTORY 		5
#define FILE_ERROR			5555
#define FILE_ALREADY_OPEN	5556

#define EXECUTEONLY			1
#define WRITEONLY 			2
#define WRITEEXECUTE 		3
#define READONLY 			4
#define READEXECUTE			5
#define READWRITE			6
#define READWRITEEXECUTE	7

#define TEXT		1
#define STACK		2
#define HEAP		3
#define DATA		4
#define NONE		5
extern char _binary_tarfs_start;
extern char _binary_tarfs_end;



typedef struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
} ustar;

typedef struct _file_structure {
	char name[100];
	uint64_t size;
	uint64_t typeflag;
	uint64_t file_address_start;
	uint64_t file_address_end;
	uint64_t inode_no;
	uint64_t parent_inode_no;
} file;


file fd[MAX_FILE];
uint64_t STDINPTR;
uint64_t STDOUTPTR;
//uint64_t STDERR;

uint64_t scanPointerStart ;
uint64_t scanPointerCurrent;


void init_tarfs();
PCB* loadExecutable(char * s);

uint64_t read_directory(uint64_t inode_no);
uint64_t open(char* file_path);
uint64_t close(uint64_t inode_no);
//uint64_t open_file(char* file_path);
uint64_t read_file(uint64_t inode_no, uint64_t size);
uint64_t cat(uint64_t inode_no, char* buffer);
uint64_t write_file(uint64_t inode_no, char* buffer);
uint64_t change_directory(char* path);
uint64_t get_current_working_dir();
void kfree(uint64_t v_addr, uint64_t* PML4);
PCB* execve(PCB* pcb, char* s, char * argv[], char * envp[]);
uint64_t read_file_per_line(uint64_t inode_no);

#endif
