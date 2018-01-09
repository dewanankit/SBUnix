#include "sys/defs.h"
#include "user_syscall.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

int  main(int argc, char *argv[], char *envp[])
{
//	int i = 0;
	/*while(i<20){
		yield();
		i++;
	}*/
	while(1){
		printf("Hello");
		yield();
	}
}
