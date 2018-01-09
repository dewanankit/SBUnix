#include "sys/defs.h"
#include "user_syscall.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

int  main(int argc, char *argv[], char *envp[])
{
	while(1) {
		uint64_t pid = fork();
		if(pid == 0){
			execvpe("/bin/ls", argv, envp);
		}
		else{
			wait_pid(pid);
		}
	}
}
