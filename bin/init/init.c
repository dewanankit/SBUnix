#include "sys/defs.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

int  main(int argc, char *argv[], char *envp[])
{
	while(1){
		int pid = fork();
		printf("\nExecuting init");
		if(pid==0){
			execvpe("/bin/sbush",argv,envp);
		}
		else{
			wait_pid(pid);
//			printf("back here");
		}
	}
/*	int pid = fork();
	printf("\nExecuting init");

	if(pid==0){
		execvpe("/bin/sbush",argv,envp);
	}
	else{
		while(1){
//			printf("back in init\n");
			yield();
		}
	}*/
}
