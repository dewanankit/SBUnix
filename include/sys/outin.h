#ifndef __OUTIN_H
#define __OUTIN_H

#include<sys/defs.h>

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);
extern void outl(uint16_t port, uint32_t val);
extern uint32_t inl(uint16_t port);


#endif

