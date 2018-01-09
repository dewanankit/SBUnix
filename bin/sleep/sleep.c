#include "sys/defs.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "util.h"
#include "stdlib.h"

int main(int argc, char *argV[], char *envp[])
{
	char * commands = malloc(100);
	long sleep_time_seconds = (long)atoi(argV[1]);
	sleep(sleep_time_seconds,commands);
//	printf("Back in string\n");
//	printf("commands found : %s",commands);

//	char* command[50];
//
//	int num_commands = strtok(commands, '\n', command);
////	printf("\nnumber= %d", num_commands);
////	char* temp = malloc(100);
//	int i = 0;
//	while(i<num_commands) {
////		printf("\nHi");
//		shebang(command[i],/* argc, argv,*/ envp);
//		i++;
//	}

	return 0;
}
