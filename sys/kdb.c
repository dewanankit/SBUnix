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
#include "sys/tarfs.h"
#include "sys/syscall.h"
//extern uint64_t STDINPTR;

//extern file fd[];

extern volatile uint64_t changeScanValue;
//extern volatile uint64_t keyboardEnable;
volatile int breakSleep = 0;

extern int x;

int ctrl_pressed = 0;
int shift_pressed = 0;

void printSingleCharacter(char q,int pressed);

void keyboard_handler(registers_t r)
{
	unsigned char kbdus[128] = { 0,  27, '1', '2', '3', '4', '5', '6', '7', '8',   /* 9  */
							    '9', '0', '-', '=', 0, '\t', 'q', 'w', 'e', 'r',   /* 19 */
							    't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,      /* 29 */
							    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  /* 39 */
							    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',  /* 49 */
							    'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,          /* 59 */
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                      /* 69 */
								0, 0, 0, 0, '-', 0, 0, 0, '+', 0,                  /* 79 */
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*other keys undefined  */
								};
	unsigned char scancode;
	extern char * video;
	scancode = inb(0x60);

	if(scancode == 29)
	{
		ctrl_pressed = 1;
	}
	else if(scancode == 0x9D)
	{
		ctrl_pressed = 0;
	}
	else if(scancode == 0x2A)
	{
		shift_pressed = 1;
	}
	else if(scancode == 0xAA)
	{
		shift_pressed = 0;
	}
	else if(scancode == 0x1C)
    {
		kprintf("\n");
		*((char*)scanPointerCurrent++) = '\0';
//                kprintf("%s",(char*)scanPointerStart);
		changeScanValue = 1;
		scanningAddress = scanPointerStart;
		scanPointerStart = scanPointerCurrent;
//                kprintf("%s")
		ctrl_pressed = 1;
		printSingleCharacter('M',ctrl_pressed);
		ctrl_pressed = 0;
    }
	else if(scancode == 0x0E)
	{
		if(x > 9){
			video--;
			video--;
			*video++ = ' ';
			*video++ = 0x07;
			video--;
			video--;
			ctrl_pressed = 1;
			printSingleCharacter('H',ctrl_pressed);
			x = x-2;
			*((char*)scanPointerCurrent--) = '\0';
		}
		else {
			changeScanValue = 0;
		}
	}
	else if(scancode == 0x01)
	{
		ctrl_pressed = 1;
		printSingleCharacter('[',ctrl_pressed);

	}
	else if(scancode == 0x81)
	{
		ctrl_pressed = 0;
	}

	else if(scancode == 0x8E)
	{
		ctrl_pressed = 0;
	}
	else if(scancode == 0x53)
	{
		ctrl_pressed = 1;
		printSingleCharacter('?',ctrl_pressed);
	}
	else if(scancode == 0xD3)
	{
		ctrl_pressed = 0;
	}	
	else if (scancode & 0x80)
	{
	}
	else
	{
		char p = kbdus[scancode];
		if(shift_pressed == 1)
		{
			if(p >='a' && p <= 'z')
			{
				p =  p -32;
			}
			else if(p == '1')
			{
				p = '!';
			}
			else if(p == '2')
			{
					p = '@';
			}
			else if(p == '3')
			{
					p = '#';
			}
			else if(p == '4')
			{
					p = '$';
			}
			else if(p == '5')
			{
					p = '%';
			}
			else if(p == '7')
			{
					p = '&';
			}
			else if(p == '8')
			{
					p = '*';
			}
			else if(p == '9')
			{
					p = '(';
			}
			else if(p == '0')
			{
					p = ')';
			}
			else if(p == '-')
			{
					p = '_';
			}
			else if(p == '=')
			{
					p = '+';
			}
			else if(p == '[')
			{
					p = '{';
			}
			else if(p == ']')
			{
					p = '}';
			}
			else if(p == '\\')
			{
					p = '|';
			}
			else if(p == ';')
			{
					p = ':';
			}
			else if(p == '\'')
			{
					p = '"';
			}
			else if(p == '/')
			{
					p = '?';
			}
			else if(p == ',')
			{
					p = '<';
			}
			else if(p == '.')
			{
					p = '>';
			}
		}
//		if(keyboardEnable == 1) {
			if(ctrl_pressed == 1 && p>0)
			{
					*video++ = '^';
					*video++ = 0x07;
			}
			if(p>0){
				*video++ = p;
				*video++ = 0x07;
				*((char*)scanPointerCurrent++) = p;
//        		kprintf("%c",((char*)scanPointerStart)[0]);
				printSingleCharacter(p,ctrl_pressed);
			}
//		}
//		else if(keyboardEnable == 0 && ctrl_pressed == 1 && p == 'c') {
//			breakSleep = 1;
//		}
//		else if(keyboardEnable == 0 && ctrl_pressed == 1 && p == 'z') {
//			breakSleep = 2;
//		}
	}
	outb(0x20, 0x20);
}

void printSingleCharacter(char q,int pressed)
{
        uint64_t lowerPrintAddress = (0xFFFFFFFF800B8000 + 160*24+20);
        char * p = (char*)lowerPrintAddress;
        *p++ = ' ';
        p++;
        *p++ = ' ';
        *p++ = 0x07;
        p--;
        p--;
        p--;
        p--;
        if(pressed == 1)
        {
                *p++ = '^';
                *p++ = 0x07;
        }
        *p++ = q;
        *p++ = 0x07;
        if(q != 'H') {
        	x = x+2;
        }

}

void printArrowCharacter(char q,int pressed)
{
        uint64_t lowerPrintAddress = (0xFFFFFFFF800B8000 + 160*24+20);
        char * p = (char*)lowerPrintAddress;
        *p++ = ' ';
        p++;
        *p++ = ' ';
        *p++ = 0x07;
        p--;
        p--;
        p--;
        p--;
        if(pressed == 1)
        {
                *p++ = '[';
                *p++ = 0x07;
        }
        *p++ = q;
        *p++ = 0x07;
}
