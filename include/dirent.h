#ifndef _DIRENT_H
#define _DIRENT_H

#include <sys/defs.h>

#define NAME_MAX 255

struct dirent {
 char d_name[NAME_MAX+1];
};

typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

uint64_t openDirectory(char* filepath, uint64_t flags, uint64_t perm);
uint64_t readDirectory(uint64_t inode_no, char* buffer);
uint64_t closeDirectory(uint64_t inode_no);

#endif
