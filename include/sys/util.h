#ifndef __UTIL_H
#define __UTIL_H

#include "sys/defs.h"

uint64_t atoi(char* str);
uint64_t oct_to_dec(char* oct);
uint64_t pow(uint64_t x, int e);
void memset(char* startAddress,  char setter, int numberOfBytes);
int charToInt(char chan_num);

#endif
