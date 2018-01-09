#include "sys/defs.h"
#include "stdio.h"
#include "stdlib.h"
#include "envio.h"

int main(int argc, char *argv[], char *envp[]){
	int a = 30;
	int b = 30;
	int c = a/(a-b);
	printf("%d",c);
	return 0;
}
