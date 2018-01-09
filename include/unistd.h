#ifndef _UNISTD_H
#define _UNISTD_H
#include "sys/defs.h"

uint64_t open(char* filepath, uint64_t flags);
uint64_t close(uint64_t inode_no);
int read_from_terminal(char* filename, uint64_t num_bytes);
uint64_t read(uint64_t inode_no, char* buffer, uint64_t buf_size);
uint64_t read_line(uint64_t inode_no, char* buffer);
int write_from_terminal(char* filename);
uint64_t write(uint64_t fd, /*const*/ void *buf, size_t count);

uint64_t chdir(char* path, char* p);
uint64_t getcwd(char* buf);
int s_cat(uint64_t inode_no, char* buffer);

uint64_t fork();
void yield();
int execvpe(char *file, char * argv[], char * envp[]);
pid_t wait(int *status);
int waitpid(int pid, int *status);
void wait_pid(int pid);
void sleep(long time_s, char*);
uint64_t getpid(void);
uint64_t getppid(void);

//// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
//off_t lseek(int fd, off_t offset, int whence);
//int mkdir(const char *pathname, mode_t mode);

//// OPTIONAL: implement for ``signals and pipes (+10 pts)''
//int pipe(int pipefd[2]);

#endif
