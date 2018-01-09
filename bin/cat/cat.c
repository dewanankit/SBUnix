#include "stdio.h"
#include "sys/defs.h"
#include "string.h"
#include "unistd.h"
#include "stdlib.h"

int main(int argc, char *argv[], char *envp[]){
//    char* argV[20];
//    char* arg = "/test.txt /abcd.txt";
//
//    strtok(arg, ' ', argV);
//	printf("In cat!!!!!!!!!!!!!!!!!!!!!!\n");
//	printf("%s\n",argv[0]);

    int i = 1;
    int fd = -1;
    while(argv[i] != 0) {
    	fd = -1;
    	fd = open(&(*argv[i]), 0x0);
    	if(fd == FILE_ERROR) {
    		printf("\nNo such file or directory");
    		i++;
    		continue;
    	}
    	else if(fd == FILE_ALREADY_OPEN) {
    		printf("\nFile already open");
    	}
		char* p = malloc(205);
		strcpy(p, "a");
		int completed = FILE_NOT_READ;
		int ret_val = 0;
		while((completed != FILE_READ_COMPLETELY) && (strlen(p) != 0)) {
			ret_val = s_cat(fd, &p[0]);
//			if(ret_val == 2){
//				while(1){}
//			}
//			printf("the return value was %d\n",(int)ret_val);
			printf("%s", p);
			if(ret_val == FILE_PARTIALLY_READ) {
				completed = FILE_PARTIALLY_READ;
			}
			else if(ret_val == FILE_READ_COMPLETELY) {
				break;
			}
			if(completed == FILE_NOT_READ) {
				break;
			}
		}
		close(fd);
    	i++;
    }
	return 0;
}
