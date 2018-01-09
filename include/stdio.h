#ifndef _STDIO_H
#define _STDIO_H
#include "sys/defs.h"

static const int EOF = -1;

int putchar(int c);
int puts(const char *s);
void printf(char *format, ...);

void clrscr();

char *gets(char *s);
void scanf(char * t, uint64_t addr);

#endif
