#include <sys/kprintf.h>
#include "stdarg.h"
#include "sys/defs.h"

volatile int x = 1;
int y = 0;
//volatile char *video = (volatile char*)(0xFFFFFFFF800B8000);
volatile char *videoMemLocation = (volatile char*)(0xFFFFFFFF800B8000);
volatile char *video = (volatile char*)(0xFFFFFFFF800B8000);
//volatile char *video = (volatile char*)(0xB8000);
char * octalconvert(long unsigned n, char * str);
char * printPointer(long unsigned int n, char * str);
char * hexaconvert(long unsigned n, char * str);
char * addressconvert(unsigned long int n, char * str);
void kprintf(/*const*/ char *fmt, ...);

void clear_screen(){

	int i = 0;

	while(i<45){
		kprintf("\n");
		i++;
	}
	uint64_t address = (0xFFFFFFFF800B8000);
	video =(char *) (address);
	x=0;
	y=0;
}

void kprintf(/*const*/ char *fmt, ...)
{
	char color = 0x07;
	char *str = fmt;
	unsigned int i;
	long unsigned int pointeraddress;
	va_list arg;
	va_start(arg, fmt);
	while(*str != '\0'){
		//char color = 0x07;
        	if(x>160)
        	{
                	x=1;
                	//*video++ = 'S';
                	//*video++ = color;
                	y=y+1;
        	}
		if(y>=22)
		{
		//	int i,j;
		//	char * video1;
		//	char * video2;
			uint64_t add = (0xFFFFFFFF800B8000 + 160*23);
			char * pt = (char *)add;
			*pt++ = ' ';
			*pt++ = color;
			y--;
			x = 1;
			uint64_t startAddress = (0xFFFFFFFF800B8000);
			char * t =(char *) startAddress;
			if(t){}
			uint64_t endAddress =  (0xFFFFFFFF800B8000 + (160*23));
			char * end = (char *) endAddress;
			if(end){}
			//*vidieo++ = 'q';
			//*video++ = color;
			while(t < end)
			{
				*(t) = *(t+160);
				t++;
			}
			uint64_t address = (0xFFFFFFFF800B8000 + 160 * y);
			video = (char*) address;
			/*
			for(i=1;i<=24;i++)
			{
				for(j=1;j<=160;j++)
				{
					uint64_t address;
				}
			}
			*/
		}

//		*video++ = *str++;
//		*video++ = 0x07;
		if(*str != '%' && *str != '\\' && *str != '\n'){
			*video++ = *str++;
			*video++ = color;
			x=x+2;
		}
		else if(*str == '%'){
		
			str++;
			char * str1 = NULL;
			switch(*str){
				case 'c': i = va_arg(arg, int);
					  *video++ = i;
					  *video++ = color;
					  x=x+2;
					  break;
				case 'd': i = va_arg(arg, int);
					  if(i<0){
					  	i = -i;
						*video++ = '-';
						*video++ = color;
						x=x+2;
					  }
					  str1 = decaconvert(i,str);
					  while(*str1 != '\0')
					  {
					  	*video++ = *str1++;
					  	*video++ = color;
						x=x+2;
					  }
					  break;
				case 'x': i = va_arg(arg, unsigned int);
					  str1  = hexaconvert(i, str);
					  while(*str1 != '\0')
					  {
					  	*video++ = *str1++;
						*video++ = color;
						x=x+2;
					  }
					  break;
				case 's': str1 = va_arg(arg, char *);
					  while(*str1 != '\0')
					  {
						if(*str1 == '\n')
						{
							str1++;
							x = 1;
							y = y+1;
							uint64_t address = (0xFFFFFFFF800B8000 + 160 * y);

							video =(char *) (address);
						}
						else{
							*video++ = *str1++;
							/*__asm__ __volatile__("movq %0,%%rax;"\
												"movq %%rax, %1"
												:"=r"(*video):"r"(*str));
							uint64_t a = *str1;
							__asm__ __volatile__("movq %0, %%rdx;"::"r"(a):"%rdx");
							__asm__ __volatile__ ("movq  %%rdx, %0": "=r"  (video)::"%rdx");*/

//							video++;
//							str1++;
							*video++ = color;
							x=x+2;
						}
						if(x>160)
						        	{
						                	x=1;
						                	//*video++ = 'S';
						                	//*video++ = color;
						                	y=y+1;
						        	}
						if(y>=22)
								{
								//	int i,j;
								//	char * video1;
								//	char * video2;
									uint64_t add = (0xFFFFFFFF800B8000 + 160*23);
									char * pt = (char *)add;
									*pt++ = ' ';
									*pt++ = color;
									y--;
									x = 1;
									uint64_t startAddress = (0xFFFFFFFF800B8000);
									char * t =(char *) startAddress;
									if(t){}
									uint64_t endAddress =  (0xFFFFFFFF800B8000 + (160*23));
									char * end = (char *) endAddress;
									if(end){}
									//*vidieo++ = 'q';
									//*video++ = color;
									while(t < end)
									{
										*(t) = *(t+160);
										t++;
									}
									uint64_t address = (0xFFFFFFFF800B8000 + 160 * y);
									video = (char*) address;
									/*
									for(i=1;i<=24;i++)
									{
										for(j=1;j<=160;j++)
										{
											uint64_t address;
										}
									}
									*/
								}
					}
					  break;
				case 'p': pointeraddress  = va_arg(arg,unsigned long);
					  str1 = addressconvert(pointeraddress,str1);
					  while(*str1 != '\0')
					  {
					  	*video++ = *str1++;
						*video++ = color;
						x=x+2;
					  }
					  break;
			}
			str++;
		}
		else if(*str == '\n')
		{
			str++;
			x = 1;
			y = y+1;
			uint64_t address = (0xFFFFFFFF800B8000 + 160 * y);
			
			video =(char *) (address);
		}
		else if(*str == '\r')
		{
			x=1;
			str++;
			uint64_t address = (0xFFFFFFFF800B8000 + 0x100 * y);
			video =(char *) (address);
		}

	}
	va_end(arg);
}

char * addressconvert(unsigned long n, char * str)
{
	static char string[20];
        char * ptr = &string[19];
        *ptr = '\0';
        if(n == 0)
		{
			*--ptr = '0';
		}
        while(n>0)
        {
                int k = n & 15;
                int rem = k%10;
                int needed = k/10;
                char q;
                if(needed > 0)
                {
                        q = 65 + rem;
                }
                else
                {
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

char * hexaconvert(long unsigned n, char * str)
{
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	if(n == 0)
	{
		*--ptr = '0';
	}
	while(n>0)
	{
		int k = n & 15;
		int rem = k%10;
		int needed = k/10;
		char q;
		if(needed > 0)
		{
			q = 65 + rem;
		}
		else
		{
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


char * octalconvert(long unsigned n, char * str)
{
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	while(n>0)
	{
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


char * printPointer(long unsigned int n, char * str)
{
	static char string[20];
	char * ptr = &string[19];
	*ptr = '\0';
	while(n>0)
	{
		int k = n & 15;
		int rem = k%10;
		int needed = k/10;
		char q;
		if(needed > 0)
		{
			q = 65 + rem;
		}
		else
		{
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


char * decaconvert(int d,char * str)
{
	static char string[30];
	char * ptr = &string[29];
	*ptr = '\0';
	if(d == 0)
	{
		*--ptr = '0';
	}
	while(d>0)
	{
		int current = d%10;
		*--ptr = '0' + current;
		d = d/10;
	}
	str = ptr;
	return str;
}


/*char * printPointer(long unsigned int n, char * str)
{
	char string[30];
	char * ptr = &string[29];
	*ptr = '\0';
	while(n>0)
	{
		int k = n & 15;
		int rem = k%10;
		int needed = k/10;
		char q;
		if(needed > 0)
		{
			q = 65 + rem;
		}
		else
		{
			q = '0' + rem;
		}
		printf("%c\n",q);
		//printf("%c\n",q);
		ptr--;
		*ptr = q;
		n = n >> 4;
	}
	str = ptr;
	return str;
}*/
