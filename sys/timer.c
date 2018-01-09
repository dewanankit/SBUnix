/* References taken from 
   wiki.osdev.org/
   osdever.net
   wikipedia
   www.jamesmolloy.co.uk/
*/

#include "sys/defs.h"
#include "sys/kprintf.h"
#include "sys/outin.h"
#include "sys/irq.h"
#include "sys/timer.h"
#include "sys/tarfs.h"
#include "sys/vmem.h"
#include "sys/syscall.h"

extern volatile uint64_t changeScanValue;

unsigned long timer_ticks = 0;
//unsigned long seconds = 0;
volatile int sleep_t = 0;
extern volatile uint64_t breakSleep;

void timer_install();
void timer_phase(int hz);
void timer_handler(registers_t r);
char * decaconvert2(int d,char * str);
volatile uint64_t timerTicks = 0;

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void timer_handler(registers_t r){
	if(current_PCB!=NULL && current_PCB->next!=NULL){
		timerTicks++;
		seconds = timerTicks/15;
		current_PCB->time_slice_left -=1;
//		kprintf("%d",seconds);

		if(current_PCB->time_slice_left <=0)
		{
			syscall_yield();
		}
//		kprintf("in timer");
	}
	outb(0x20, 0x20);
}

/*
void timer_handler(registers_t r)
{
	timer_ticks++;
    int n = 85;
	if(timer_ticks%n == 0)
	{
        	char * str1 = ((void*)0);
	        int time = timer_ticks/n;
	        unsigned long seconds = 0;
	        unsigned long minutes = 0;
	        unsigned long hours = 0;
	        str1 = decaconvert2(time,str1);
       		extern char * video;
		char * temp = video;
		uint64_t address = (0xFFFFFFFF800B8000 + 160*24+50);
		video = (char *)address;
    		char * p = "Time since boot : ";

		while(*p !='\0')
    		{
			*video++ = *p++;
        		*video++ = 0x07;
    		}
    		video++;
    		video++;
    		video++;
    		video++;
    		seconds = time%60;

    		time = time/60;
    		minutes = time%60;
    		hours = time/60;
    		str1 = decaconvert2(hours,str1);
    		if(hours ==0)
    		{
          		*video++ = '0';
          		*video++ = 0x07;
          		*video++ = '0';
          		*video++ = 0x07;
    		}
    		else if(hours<10)
   		{
          		*video++ = '0';
          		*video++ = 0x07;
   		}
		while(*str1 != '\0')
    		{
        		*video++ = *str1++;
        		*video++ = 0x07;
    		}
    		*video++ = ' ';
    		*video++ = 0x07;
    		*video++ = ':';
    		*video++ = 0x07;
    		*video++ = ' ';
    		*video++ = 0x07;

    		str1 = decaconvert2(minutes,str1);
    		if(minutes ==0)
    		{
          		*video++ = '0';
          		*video++ = 0x07;
          		*video++ = '0';
          		*video++ = 0x07;
    		}
    		else if(minutes<10)
   		{
          		*video++ = '0';
          		*video++ = 0x07;
   		}
    		while(*str1 != '\0')
    		{
        		*video++ = *str1++;
        		*video++ = 0x07;
    		}
    		*video++ = ' ';
    		*video++ = 0x07;
    		*video++ = ':';
    		*video++ = 0x07;
    		*video++ = ' ';
    		*video++ = 0x07;

    		if(seconds ==0)
    		{
          		*video++ = '0';
          		*video++ = 0x07;
          		*video++ = '0';
          		*video++ = 0x07;
    		}
		else if(seconds<10)
   		{
          		*video++ = '0';
          		*video++ = 0x07;
   		}
    		str1 = decaconvert2(seconds,str1);
    		while(*str1 != '\0')
    		{
        		*video++ = *str1++;
        		*video++ = 0x07;
    		}
    		video = temp;
	}
	if(sleep_t != 0) {
		sleep_t--;
	}
	outb(0x20, 0x20);
}
*/
void timer_install()
{
//    irq_install_handler(0, timer_handler);
}

char * decaconvert2(int d,char * str)
{
        static char string[30];
        char * ptr = &string[29];
        *ptr = '\0';
        while(d>0)
        {
                int current = d%10;
                *--ptr = '0' + current;
                d = d/10;
        }
        str = ptr;
        return str;
}

void memcpy2(char* src, char* dest, int size){
	char* tempSrc = src;
	char* tempDest = dest;
	int i = 0;
	while(i<size){
		*tempDest = *tempSrc;
		if(*tempSrc =='\0'){
			*tempDest = '\n';
		}
		i++;
		tempSrc++;
		tempDest++;
	}
	*tempDest= '\0';
}
void sleep(int s, char* string) {
	current_PCB->sleepSeconds = s;
	current_PCB->ticksLastRecorded = seconds;
	while((seconds - current_PCB->ticksLastRecorded)<current_PCB->sleepSeconds){
		syscall_yield();
	}
//	uint64_t scanningAddressInSleep = scanPointerStart;

	/*while(sleep_t != 0 && breakSleep != 1 && breakSleep != 2){
		syscall_yield();
	}*/
//	changeScanValue = 1;
//	kprintf("hello");
/*	uint64_t endAddress = scanPointerCurrent;
	scanPointerStart = scanPointerCurrent;
//	*(char*(endAddress+1)) = '@';
//	kprintf("%p  %p",scanningAddressInSleep,endAddress );
	memcpy2((char*)scanningAddressInSleep,string,(int) (endAddress-scanningAddressInSleep));*/
//	kprintf("%s\n",string);
//	if(breakSleep == 2) {
//		kprintf("Stopped    sleep %d", s);
//	}
//	breakSleep = 0;
}
