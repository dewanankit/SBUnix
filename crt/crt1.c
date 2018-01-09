//#include "stdlib.h"
//#include <sys/defs.h>
//#include "envio.h"
//
//void _start(void) {
//	uint64_t *rsp = 0;
//
//	// call main() and exit() here
//	uint64_t * argc;
//	char **argv = NULL;
//	//char **envp = NULL;
//	__asm__ __volatile__("movq %%rsp, %0;" : "=a" (rsp)	 : :  "cc","memory");
//
//	argc = ((uint64_t *)rsp + 1);
//	argv = ((char **)rsp + 0x2);
//	if(*argc > 1)
//		global_env = argv + (*argc - 1) + 0x2;
//	else
//		global_env = argv + 0x2;
//	int ret_value;
//	ret_value = main(*argc, argv, global_env);
//	exit(ret_value);
//}


#include "stdlib.h"
#include <sys/defs.h>
#include "envio.h"
#include "stdio.h"
void _start(void) {
	uint64_t *rsp = 0;
	// call main() and exit() here
	int argc;
	char **argv = NULL;
	char **envp = NULL;
	__asm__ __volatile__("movq %%rsp, %0;" : "=a" (rsp)	 : :  "cc","memory");

//	argc = ((uint64_t*)rsp + 0x1);
	uint64_t heapAddress1;
	heapAddress1 = *(uint64_t*)((char **)rsp + 0x1);
	argc = (int)(heapAddress1);

//	argc = ((uint64_t *)rsp + 0x1);
	uint64_t heapAddress2;
	heapAddress2 = *(uint64_t*)((char **)rsp + 0x2);
	argv = (char**)(heapAddress2);

	uint64_t heapAddress3;
	heapAddress3 = *(uint64_t*)((char **)rsp + 0x3);
	envp = (char**)(heapAddress3);

//	envp = argv;
//	while(1){}
//	global_env = argv;
//	while(1){}
//	if(*argc > 1)
//		global_env = argv + (*argc - 1) + 0x2;
//	else
//		global_env = argv + 0x2;
	int ret_value;
//	while(1){}
	ret_value = main(argc, argv, envp);
	exit(ret_value);
}
