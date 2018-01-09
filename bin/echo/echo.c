#include "sys/defs.h"
#include "stdio.h"
#include "stdlib.h"
#include "envio.h"

int main(int argc, char *argv[], char *envp[])
{
//	printf("\n%s   %s\n", envp[2], envp[3]);
//	while(1){}
	int i = 1;
//	int j;
	printf("\n");
	while(i < argc) {
		if(argv[i][0] != '$') {
			printf("%s  ", argv[i]);
		}
		else {
			getenv(argv[i], envp);
		}
		i++;
//		printf("\n");
	}

//	while(1){}

	return 0;
}
