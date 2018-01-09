#include "user_syscall.h"
#include "sys/defs.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

char* octalconvert(long unsigned n, char* str) {
	static char string[20];
	char* ptr = &string[19];
	*ptr = '\0';
	while(n>0) {
		int k = n & 7;
		//int rem = k%10;
		//int needed = k/10;
		char q;
		q = '0'+k;
		//printf("%c\n",q);
		ptr--;
		*ptr = q;
		n = n >> 3;
	}
	str = ptr;
	return str;
}

char* printPointer(long unsigned int n, char* str) {
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	while(n>0) {
		int k = n & 15;
		int rem = k%10;
		int needed = k/10;
		char q;
		if(needed > 0) {
			q = 65 + rem;
		}
		else {
			q = '0' + rem;
		}
		//printf("%c\n",q);
		ptr--;
		*ptr = q;
		n = n >> 4;
	}
	str = ptr;
	return str;
}

char* hexaconvert(long unsigned n, char* str) {
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	if(n == 0) {
		*--ptr = '0';
	}
	while(n>0) {
		int k = n & 15;
		int rem = k%10;
		int needed = k/10;
		char q;
		if(needed > 0) {
			q = 65 + rem;
		}
		else {
			q = '0' + rem;
		}
		//printf("%c\n",q);
		ptr--;
		*ptr = q;
		n = n >> 4;
	}
//	ptr--;
//	*ptr = 'x';
//	ptr--;
//	*ptr = '0';
	str = ptr;
	return str;
}

char* addressconvert(unsigned long int n, char* str) {
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	while(n>0) {
			int k = n & 15;
			int rem = k%10;
			int needed = k/10;
			char q;
			if(needed > 0) {
					q = 65 + rem;
			}
			else {
					q = '0' + rem;
			}
			//printf("%c\n",q);
			ptr--;
			*ptr = q;
			n = n >> 4;
	}
	ptr--;
	*ptr = 'x';
	ptr--;
	*ptr = '0';
	str = ptr;
	return str;
}

char* decaconvert(int d,char* str) {
	static char string[30];
	char * ptr = &string[29];
	*ptr = '\0';
	if(d == 0) {
		*--ptr = '0';
	}
	while(d>0) {
		int current = d%10;
		*--ptr = '0' + current;
		d = d/10;
	}
	str = ptr;
	return str;
}

void printf(char *fmt, ...) {
	char *str = fmt;
	unsigned int i;
	long unsigned int pointeraddress;
	va_list arg;
	va_start(arg, fmt);

	char string[2048];
	int count = 0;

	while(*str != '\0') {
		if(*str != '%' && *str != '\\' && *str != '\n'){
			string[count++] = *str++;
			continue;
		}
		else if(*str == '%'){
			str++;
			char* str1 = '\0';
			switch(*str){
				case 'c':
					string[count++] = va_arg(arg, int);
					str++;
					break;
				case 'd':
					i = va_arg(arg, int);
					if(i<0){
						string[count++] = '-';
					}
					str1 = decaconvert(i,str);
					while(*str1 != '\0')
					{
						string[count++] = *str1++;
					}
					break;
				case 'x':
					i = va_arg(arg, unsigned int);
					str1  = hexaconvert(i, str);
					while(*str1 != '\0')
					{
						string[count++] = *str1++;
					}
					break;
				case 's':
					str1 = va_arg(arg, char*);
					while(*str1 != '\0')
					{
						string[count++] = *str1++;
					}
					break;
				case 'p':
					pointeraddress  = va_arg(arg,unsigned long);
					str1 = addressconvert(pointeraddress,str1);
					while(*str1 != '\0')
					{
						string[count++] = *str1++;
					}
					break;
			}
			str++;
		}
		else if(*str == '\n')
		{
			str++;
			string[count++] = '\n';
		}
		else if(*str == '\r')
		{
			str++;
			string[count++] = '\r';
		}
	}
	string[count++] = '\0';
	va_end(arg);

//	uint64_t length = (uint64_t) strlen(string);
	syscall_args_2(SYS_WRITE, STDOUT, (long)string);
}

void clrscr() {
	syscall_args_0(SYS_CLRSCR);
}

void scanf(char * t, uint64_t addr){
	syscall_args_2(SYS_READ, STDIN, addr);
//	__asm__ __volatile__("int $0x80"::"a"(0x03), "b"(0x0),"c"(addr));
}

char *gets(char *s) {
	scanf("%s", (uint64_t)s);
	return &s[0];
}

int putchar(int c) {
	printf("%c", c);
	return 0;
}

int puts(const char *s) {
	printf("%s", s);
	return 0;
}
