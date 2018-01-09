#include "sys/defs.h"
#include "user_syscall.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

int  main(int argc, char *argv[], char *envp[])
{
	char * s = malloc(100);
	printf("enter input:");
	scanf("%s",(uint64_t)s);
	printf("the scanned value was %s", s);
	return 0;
}
