#include "unistd.h"
#include "user_syscall.h"
#include "sys/defs.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

uint64_t open(char* filepath, uint64_t flags) {
	uint64_t ret_val = syscall_args_3(SYS_OPEN, (uint64_t)&filepath[0], flags, FILE_PERM);
	return ret_val;
}

uint64_t close(uint64_t inode_no) {
	uint64_t ret_val = syscall_args_1(SYS_CLOSE, inode_no);
	return ret_val;
}

uint64_t read(uint64_t inode_no, char* buffer, uint64_t buf_size) {
	uint64_t ret_val;
//	char* c = (char*) malloc(1);
	char* temp_buf = /*(char*)*/malloc(BUFFER_SIZE);
	if(buf_size < BUFFER_SIZE) {
//		printf("in first");
		ret_val = syscall_args_3(SYS_READ, inode_no, (uint64_t)&temp_buf[0], buf_size);
		strcpy(buffer, temp_buf);
//		printf("%s", buffer);
//		scanf("%s", (uint64_t)c);
	}
	else {
		while(buf_size >= BUFFER_SIZE) {
//			printf("in second");
			ret_val = syscall_args_3(SYS_READ, inode_no, (uint64_t)&temp_buf[0], BUFFER_SIZE);
			strcat(buffer, temp_buf);
//			strcpy(buffer, temp_buf);
			buf_size -= BUFFER_SIZE;
//			printf("%s", buffer);
//			scanf("%s", (uint64_t)c);
		}
		ret_val = syscall_args_3(SYS_READ, inode_no, (uint64_t)&temp_buf[0], buf_size);
		strcat(buffer, temp_buf);
	}
	return ret_val;
}

uint64_t read_line(uint64_t inode_no, char* buffer) {
	uint64_t ret_val = syscall_args_2(SYS_READLINE, inode_no, (uint64_t)&buffer[0]);
	return ret_val;
}

int read_from_terminal(char* filename, uint64_t num_bytes) {
	uint64_t ret_val = 0;
//	printf("\nfile: %s",filename);
//	while(1){}
	uint64_t fd = open(&filename[0], 0x0);
//	printf("\nfile: %s",filename);
	if(fd == FILE_ERROR) {
		printf("\nNo such file");
		return 0;
	}

	char* p = /*(char*)*/malloc(num_bytes);
	read(fd, &p[0], num_bytes);
	printf("\nReading file '%s':\n", filename);
	if(strlen(p) < 1760) {
		printf("%s", p);
	}
	else {
		int i = 0;
		char c = '\0';/* = (char)malloc(1);*/
		while(i < 1760) {
			printf("%c", p[i++]);
		}
		scanf("%c", (uint64_t)c);
		int k = 0;
		while(i < num_bytes) {
			printf("%c", p[i++]);
			k++;
			if(k == 79) {
				scanf("%c", (uint64_t)c);
				k = 0;
			}
		}
	}
	close(fd);
	return ret_val;
}

uint64_t write(uint64_t fd, /*const*/ void *buf, size_t count) {
	uint64_t ret_val = syscall_args_3(SYS_WRITE, fd, (uint64_t)buf, count);
	if(ret_val != 0) {
		printf("%s", buf);
		return FILE_ERROR;
	}
	return ret_val;
}

int write_from_terminal(char* filename) {
	uint64_t ret_val = 0;
	uint64_t fd = open(&filename[0], 0x0);
	if(fd == FILE_ERROR) {
		printf("\nNo such file");
		return -1;
	}
	char* s =/* (char*)*/malloc(500);
	printf("\nFile '%s' opened; Please type in string to write to file and press Enter:\n", filename);
	scanf("%s", (uint64_t)s);
	ret_val = write(fd, &s[0], strlen(s));
	if(ret_val == 0) {
		printf("Written in file '%s'", filename);
	}
	close(fd);
	return ret_val;
}


uint64_t chdir(char* path, char* p) {
	uint64_t ret_val = syscall_args_2(SYS_CHDIR, (uint64_t)&path[0], (uint64_t)&p[0]);
	return ret_val;
}

uint64_t getcwd(char* buf) {
	uint64_t ret_val = syscall_args_1(SYS_CWD, (uint64_t)&buf[0]);
	return ret_val;
}

int s_cat(uint64_t inode_no, char* buffer) {
	int ret_val = (int)syscall_args_2(SYS_CAT, inode_no, (uint64_t)&buffer[0]);
	return ret_val;
}


uint64_t fork() {
		uint64_t ret_val;
		__asm__ __volatile__("int $0x80;"\
							 "movq %%rax, %0;"
							 :"=r"(ret_val):"a"(SYS_FORK));
		return ret_val;
}

void yield() {
	__asm__ __volatile__("int $0x80;"::"a"(SYS_YIELD));
}

int execvpe(char *file, char * argv[], char * envp[]) {
//	__asm__ __volatile__("int $0x80"::"a"(SYS_EXECV));
	int ret_val = syscall_args_3(SYS_EXECV, (uint64_t)file, (uint64_t)argv, (uint64_t)envp);
	return ret_val;
}

void temp_execvpe() {
	__asm__ __volatile__("int $0x80"::"a"(SYS_EXECV));
}

void sleep(long time_s, char* commands) {
	syscall_args_2(SYS_SLEEP, time_s, (uint64_t)commands);
}

uint64_t getpid() {
	uint64_t ret_val = syscall_args_0(SYS_GETPID);
	return ret_val;
}

uint64_t getppid() {
	uint64_t ret_val = syscall_args_0(SYS_GETPPID);
	return ret_val;
}


void wait_pid(int pid){
	syscall_args_1(SYS_WAITID,pid);
}
