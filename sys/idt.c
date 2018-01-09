/* References taken from 
   wiki.osdev.org/
   osdever.net
   wikipedia
   www.jamesmolloy.co.uk/
*/

#include <sys/defs.h>
#include "sys/idt.h"
#include "sys/outin.h"

idt_struct idt_segment[256];
idt_ptr idt_p;

extern void _x86_64_asm_lidt(idt_ptr *);
extern void isr0();
extern void irq0();
extern void irq1();
extern void pageFault();
extern void syscalls();
extern void divideByZero();
//extern void int_syscall_stub();

void init_idt()
{
	idt_p.limit = sizeof(idt_struct) * 256 - 1;
	idt_p.offset = (uint64_t)&idt_segment;
	irq_remap();
	mem_set(&idt_segment,0,sizeof(idt_struct) * 256);
	idt_set(0, (uint64_t)&divideByZero, SELECTOR_VAL, TYPE_ATTRIB_VAL);
	idt_set(14, (uint64_t)&pageFault, SELECTOR_VAL, TYPE_ATTRIB_VAL);
	idt_set(32, (uint64_t)&irq0, SELECTOR_VAL, TYPE_ATTRIB_VAL);
	idt_set(33, (uint64_t)&irq1, SELECTOR_VAL, TYPE_ATTRIB_VAL);
	idt_set(128, (uint64_t)&syscalls, SELECTOR_VAL, TYPE_ATTRIB_VAL_RING3);
	_x86_64_asm_lidt(&idt_p);
}

void irq_remap(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void mem_set(void *start, char c, int length)
{
        unsigned char* str = start;
	int i;
	for(i=0;i<length;i++)
	{
	     *str++ = c;
	}
}

void idt_set(uint8_t num, uint64_t offset, uint16_t sel, uint8_t flg)
{
        idt_segment[num].offset_low     = offset & 0xFFFF;
        idt_segment[num].selector       = sel;
        idt_segment[num].reserved       = 0;
        idt_segment[num].flg            = flg;
        idt_segment[num].offset_mid     = (offset>>16) & 0xFFFF;
        idt_segment[num].offset_high    = (offset>>32) & 0xFFFFFFFF;
        idt_segment[num].reserved2      = 0;
}
