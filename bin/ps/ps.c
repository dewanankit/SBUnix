#include "sys/defs.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "user_syscall.h"

int  main(int argc, char *argv[], char *envp[]) {

	syscall_args_0(SYS_PS);
	return 0;
}
