#include "sys/defs.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"
#include "dirent.h"

int main(int argc, char *argV[], char *envp[])
{
//	printf("in ls\n");
//	printf("%s",argV[0]);
//	while(1){}
//	printf("yielding");
//	yield();
	char* filepath = /*(char*)*/malloc(100);
	int i = 1;
	int fd = -1;
	if(argV[i] == 0) {
		getcwd(&filepath[0]);
//		printf("\nafter pwd: %s", filepath);

//		chdir(filepath, &temp[0]);

		fd = openDirectory(&filepath[0], 0x0, FILE_PERM);
//		printf("\nfd: %d", fd);

		if(fd == FILE_ERROR) {
			printf("\nNo such file or directory");
			return -1;
		}
		char* p = /*(char*)*/malloc(500);
		char* b = /*(char*)*/malloc(500);
		readDirectory(fd, &p[0]);
		strcpy2(b, p, '/');
		printf("\n%s", b);
		closeDirectory(fd);
	}
	else {
		while(argV[i] != 0) {
			fd = -1;
			fd = openDirectory(&(*argV[i]), 0x0, FILE_PERM);
			if(fd == FILE_ERROR) {
				printf("\nNo such file or directory");
				i++;
				continue;
			}
			char* p = /*(char*)*/malloc(500);
			char* b = /*(char*)*/malloc(500);
			readDirectory(fd, &p[0]);
			strcpy2(b, p, '/');
			if(strlen(p) == 0) {
				printf("\n%s\n%s\n", argV[i], b);
			}
			else {
				printf("\n%s:\n%s\n", argV[i], b);
			}
			i++;
			closeDirectory(fd);
		}
	}

//	while(1){};

	return 0;
}
