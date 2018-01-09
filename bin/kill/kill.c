#include "sys/defs.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "util.h"
#include "user_syscall.h"

int  main(int argc, char *argv[], char *envp[]) {

	syscall_args_1(SYS_KILL, atoi(argv[2]));
	return 0;
}
