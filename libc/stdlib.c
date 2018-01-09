#include "sys/defs.h"
#include "user_syscall.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

void exit(int status){
/*      long syscall_nbr = 60;
        long exit_status = 42;
         __asm__ __volatile__ ( "movq %0, %%rax\n\t"
                                "movq %1, %%rdi\n\t"
                                "syscall"
                                :
                                : "m" (syscall_nbr), "m" (exit_status)
                                : "rax", "rdi");*/
//        __asm__ __volatile__ ("int $0x80" : : "a" (0x01), "b" (status) : "memory");
//	printf("\nExecuting Exit");
	__asm__ __volatile__ ("int $0x80" : : "a" (SYS_EXIT): "memory");

//      __asm__ __volatile__ ( "int $0x80" :  :"a" (0), "b"(status): "memory");
}

void *malloc(size_t size) {
	void* ret_val = (void*)syscall_args_1(SYS_MALLOC, size);
	return ret_val;
}


void shebang(char* command, char* temp, char *envp[]) {

	//Implement free for this
	char* tokens[20];
	if(strlen(command) == 0) {
		return;
	}
	uint64_t n = strtok(command, ' ', tokens);

	int amp_flag = 0;
	int i = 0;
	while(i<n) {
		if(strcmp(tokens[i], "&") == 0) {
			amp_flag = 1;
			tokens[i] = 0;
		}
		i++;
	}
//	printf("\nAmpersand flag = %d", amp_flag);

	if(strcmp(tokens[0], "pwd") == 0) {
		getcwd(&temp[0]);
		printf("%s", temp);
	}
	else if(strcmp(tokens[0], "cd") == 0) {
		if(tokens[1] == 0) {
//				strcpy(&(*tokens[1]), "/");
			char* t2 = /*(char*)*/ malloc(5);
			strcpy(t2, "/");
			chdir(&t2[0], &temp[0]);
			if(temp[0] != '/') {
				printf("\n%s", temp);
			}
			return;
		}
		chdir(tokens[1], &temp[0]);
		if(temp[0] != '/') {
			printf("\n%s", temp);
		}
	}
	else if(strcmp(tokens[0], "read") == 0) {
		if((tokens[1] == 0) || (tokens[2] == 0)) {
			printf("\nIncorrect syntax; Use: read <file name> <bytes to read>");
			return;
		}
		read_from_terminal(tokens[1], atoi(tokens[2]));
	}
	else if(strcmp(tokens[0], "write") == 0) {
		if(tokens[1] == 0) {
			printf("\nIncorrect syntax; Use: write <file name>");
			return;
		}
		write_from_terminal(tokens[1]);
	}
	else if(strcmp(tokens[0], "ps") == 0) {
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/ps", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "clear") == 0) {
		clrscr();
	}
	else if(strcmp(tokens[0], "ls") == 0) {
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/ls", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
//				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "cat") == 0) {
		if((tokens[1] == 0)) {
			printf("\nUse file name to read; Use: cat <file name> <file name>");
			return;
		}
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/cat", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
//				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "echo") == 0) {
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/echo", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
//				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "kill") == 0) {
		if (tokens[1] == 0) {
			printf("\nIncorrect syntax; Use: kill -9 <pid> OR kill <pid>");
			return;
		}
		else if(tokens[1][0] != '-') {
			strcpy(tokens[2], tokens[1]);
			strcpy(tokens[1], "-9");
//			printf("1:%s 2:%s", tokens[1], tokens[2]);
		}
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/kill", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
//				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "sleep") == 0) {
		if((tokens[1] == 0)) {
			printf("\nEnter sleep time in seconds; Use: sleep <time in seconds>");
			return;
		}
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/sleep", tokens, envp);
		}
		else{
			if(amp_flag == 0) {
				wait_pid(pid);
			}
			else {
//				yield();
			}
		}
	}
	else if(strcmp(tokens[0], "test1") == 0){
			uint64_t pid = fork();
			if(pid == 0){
				execvpe("/bin/preemptiveTest1", tokens, envp);
			}
			else{
//				yield();
		}
	}
	else if(strcmp(tokens[0], "test2") == 0){
			uint64_t pid = fork();
			if(pid == 0){
				execvpe("/bin/preemptiveTest2", tokens, envp);
			}
			else{
				yield();
		}
	}
	else if(strcmp(tokens[0], "test3") == 0){
			uint64_t pid = fork();
			if(pid == 0){
				execvpe("/bin/divideByZero", tokens, envp);
			}
			else{
				wait_pid(pid);
		}
	}
	else {

			uint64_t fd = open(tokens[0], 0x0);
			if(fd == FILE_ERROR) {
				printf("\n%s is Invalid command, syntax or file", tokens[0]);
				return;
			}

			int flag = 0;

	//		char* temp = malloc(100);
	//		char* temp_buf = malloc(100);

			read_line(fd, temp);
			if(strcmp(temp, "Is a directory") == 0) {
				printf("\nIs a directory");
				return;
			}
			if(temp[0] == '#' && temp[1] == '!') {
				flag = 3;
				char* temp2 = malloc(100);
				while(flag == 3) {
		//			flag = 0;
	//				char* temp_buf2 = malloc(BUFFER_SIZE);
					read_line(fd, &temp[0]);
	//				printf("%s", temp);
	//				printf("%s", temp_buf2);
					if(strlen(temp) == 0) {
						flag = 0;
						continue;
					}
					strcpy2(temp2, temp, '\r');
					shebang(temp2, temp, envp);
					memset(temp2, 0, 100);
				}
			}
			else if (temp[0] != '#' || temp[1] != '!'){
				printf("\nNot an executable script file");
				return;
			}
			close(fd);
		}
	if(n){;}
	if(temp){;}
	if(amp_flag){;}

	memset(temp, 0, 100);

	return;
}
