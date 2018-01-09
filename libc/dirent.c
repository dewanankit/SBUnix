#include "dirent.h"
#include "user_syscall.h"

uint64_t openDirectory(char* filepath, uint64_t flags, uint64_t perm) {
	uint64_t ret_val = syscall_args_3(SYS_OPEN, (uint64_t)&filepath[0], flags, perm);
	return ret_val;
}

uint64_t readDirectory(uint64_t inode_no, char* buffer) {
	uint64_t ret_val = syscall_args_2(SYS_GETDENTS, inode_no, (uint64_t)&buffer[0]);
	return ret_val;
}

uint64_t closeDirectory(uint64_t inode_no) {
	uint64_t ret_val = syscall_args_1(SYS_CLOSE, inode_no);
	return ret_val;
}
