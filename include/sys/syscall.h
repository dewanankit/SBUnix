#ifndef _SYSSYSCALL_H
#define _SYSSYSCALL_H

//void syscall_read(registers_t reading);
#include "sys/irq.h"
uint64_t globalAddress;
volatile int scanLock;
volatile uint64_t scanningAddress;
uint64_t syscall_fork(registers_t);
void syscall_read(registers_t r);
void syscall_write(registers_t r);
void syscall_open(registers_t r);
void syscall_malloc(registers_t r);
void syscall_getdents(registers_t r);
void syscall_getcwd(registers_t r);
void syscall_chdir(registers_t r);
uint64_t syscall_cat(registers_t r);
void syscall_close(registers_t r);
void syscall_getpid(registers_t r);
void syscall_sleep(registers_t r);
void syscall_yield();
void scan(registers_t r);
void syscall_readline(registers_t r);
void sys_exit(registers_t r);
void ps();
void syscall_waitpid(registers_t r);
void syscall_kill(registers_t r);
#endif
