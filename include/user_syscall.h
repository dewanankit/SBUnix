#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "sys/defs.h"

extern uint64_t syscall_args_0(uint64_t syscall);
extern uint64_t syscall_args_1(uint64_t syscall, uint64_t arg_1);
extern uint64_t syscall_args_2(uint64_t syscall, uint64_t arg_1, uint64_t arg_2);
extern uint64_t syscall_args_3(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3);
extern uint64_t syscall_args_4(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4);
extern uint64_t syscall_args_5(uint64_t syscall, uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4, uint64_t arg_5);
extern uint64_t _execve_ (uint64_t syscall, char* file, char** str, char** count);
#endif
