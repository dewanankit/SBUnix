#ifndef __IRQ_H
#define __IRQ_H

typedef struct registers
{
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, rax;
        uint64_t rip, cs, eflags, rsp, ss;
} registers_t;



void irq_handler(registers_t  regs);
void irq_install_handler(int irq, void (*handler)(registers_t *r));
void irq_uninstall_handler(int irq);
extern void timer_install();
extern void timer_phase(int);
extern void timer_handler(registers_t r);
extern void keyboard_handler(registers_t r);
extern void syscall_handler(registers_t r);
extern void pageFault_handler_code(registers_t r);
void divideByZero_handler_code(registers_t regs);
#endif
