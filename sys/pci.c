/*
References have been taken from:
	https://www-ssl.intel.com/content/www/us/en/io/serial-ata/serial-ata-ahci-spec-rev1_3.html
	wikipedia
	wiki.osdev.org/
*/
#include "sys/kprintf.h"
#include "sys/defs.h"
#include "sys/outin.h"
#include "sys/ahci.h"
#include "sys/pci.h"

//void mem_set_int(int *start, int c, int length);

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot,uint8_t func,uint8_t offset)
{
	uint32_t address;
	uint16_t tmp = 0;
	address = (uint32_t)((bus << 16)|(slot << 11)|(func << 8)|(offset & 0xfc)|((uint32_t)0x80000000));
	outl (0xCF8, address);
	tmp = (uint16_t)((inl (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}

uint32_t pciConfigReadDWord(uint8_t bus, uint8_t slot,uint8_t func,uint8_t offset)
{
        uint32_t address;
        uint32_t tmp = 0;
        address = (uint32_t)((bus << 16)|(slot << 11)|(func << 8)|(offset & 0xfc)|((uint32_t)0x80000000));
        outl (0xCF8, address);
        tmp = (uint32_t)((inl (0xCFC) >> ((offset & 2) * 8)));
        return (tmp);
}

void writeWord (uint8_t bus, uint8_t slot,uint8_t func,uint8_t offset,uint32_t value)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	address = (uint32_t)((lbus << 16)|(lslot << 11)|(lfunc << 8)|(offset & 0xfc)|((uint32_t)0x80000000));
	outl(0xCF8, address);
	outl(0xCFC,value);
}

int checkIfContainsAHCI(uint32_t bus, uint32_t slot,uint32_t fn)
{
		//kprintf("%d ",fn);
		uint16_t classSubClass = pciConfigReadWord(bus&0xFF,slot&0xFF,fn,0x0A);
		uint8_t subClass = classSubClass & 0x00FF;
		uint8_t class = classSubClass >> 8;
		if(classSubClass!=0xFFFF)
			kprintf("%x \n",classSubClass);
		if(classSubClass == 0x0106)
		{
			kprintf("\nAHCI found\n");
			kprintf("\n%d\n",fn);
		}
		if(class == 1 && subClass == 6)
		{	
			return 1;
		}
		else
		{
			return -1;
		}
		return -1;
}

void find_device(int k)
{
//	uint8_t busNeeded=0;
//	uint8_t device = 0;
	int i = 0;
	int j = 0;
	int retVal = 0;
//	int iFinal=-1;
	int slot = -1;
	int bus = -1;
	int fnc = -1;
	//kprintf("pqrs");
	//char * p = "Hello";
	//mem_set2((void *)p,'c',5);
	//kprintf("%s\n",p);
	for(i=0; i<=254;i++)
	{
		for(j=0;j<32;j++)
		{
			for(int fn = 0; fn<8; fn++)
			{
			//int p = -1;
			//retVal = checkIfContainsAHCI(i,j,fn);
				uint16_t classSubClass2 = pciConfigReadWord(i&0xFF,j&0xFF,fn,0x0A);
				if(classSubClass2 == 0x106){
					kprintf("bus %d port %d fn %d\n ",i,j,fn);
					retVal = 1;
				}
				else
					retVal = -1;
				if(retVal == 1)
				{
					slot = j;
					bus = i;
					fnc = fn;
					uint16_t vendorId  = pciConfigReadWord(bus,slot,fnc,0x00);
					uint16_t deviceId  = pciConfigReadWord(bus,slot,fnc,0x02);
					kprintf("device id - %x\n", deviceId&0xFFFF);
					kprintf("vendor id - %x\n",vendorId&0xFFFF);
					kprintf("AHCI found \n ");
			//		kprintf("values are %d %d\n", slot, bus);
				}
			}
		}
	}
	if(slot >= 0 && bus >= 0)
	{
		//updating BAR5 to feasible address
		writeWord(bus,slot,fnc,0x24,0xA6000);/*0x3EFB1000);0xA6000;*/
	 	uint16_t q1 = pciConfigReadWord(bus,slot,fnc,0x24);
		uint16_t q2 = pciConfigReadWord(bus,slot,fnc,0x26);
		uint64_t add = q2 << 16 | q1;
		if(add){}
		kprintf("updated address - %x\n",add);
		hba_mem_t * abar = (hba_mem_t *)add;
		//if(abar){}	
		int portNo = probe_port(abar);
		if(portNo){}
		kprintf("port number %d",portNo);
		//kprintf("\n cap %x\n",abar->cap);
		//volatile uint32_t * p = (uint32_t *)((uint64_t)abari+0x04);
		/*abar->ghc =  (uint32_t)(1<<31);
		abar->ghc =  (uint32_t)(1<<1);*/
		abar->ghc |= 0x1;
		abar->ghc =  abar->ghc | ((uint32_t)(1<<31) | (uint32_t)(1<<1));
		kprintf("ghc %x\n", abar->ghc);
		//abar->ghc =  (uint32_t)(1<<31);
		//abar->ghc =  (uint32_t)(1<<4);
		

		//(*abar).ghc = abar->ghc | 0x3;
		//kprintf("ghc %x",*p);
		//kprintf("addresses %x %x", *p, abar->ghc);
		

		port_rebase(&abar->ports[portNo],portNo);
		kprintf("Value of abar-pi %x\n",abar->pi);
		kprintf("Value of abar-ssts %x\n",(&abar->ports[portNo])->ssts);
		//kprintf("port number %d\n", portNo);

		//char buffer[50];
		//char * p = "Hello";		
		//char *buffer2 = buffer;
		uint32_t *buf = (uint32_t*)(uint64_t)0x4000000;
		uint8_t* buf8 = (uint8_t*)buf;
		
		/*for(int l = 0;l<100;l++)
		{*/
			for(int j=0;j<8;j++)
			{	
				buf8 = (uint8_t*)buf;
				for(int i=0;i<512;i++)
				{
					*(buf8+i) = 1;
				}
				write(&abar->ports[portNo],j,0,1,buf);
			}

		/*}*/
		kprintf("\nWritten into SATA drive at port %d", portNo);	
		
		/*kprintf("\nReading all bytes of every sector(100*8)");
		buf8 = (uint8_t*)buf;
                for(int i=0;i<512;i++)
                {
                     *(buf8+i) = 9;
                }
		for(int l = 0;l<100;l++)
		{
			for(int j = 0; j<8;j++)
			{
				//buf = buf+j*128+ l*1024;
				read(&abar->ports[portNo],l*8 +j,0,1,buf);
				buf8 = (uint8_t*)buf;
				for(int i = 0;i < 512;i++)
				{
					kprintf("%d ",*(buf8+i));
				}
				//kprintf("%d ",*(buf8+8));
				//kprintf("%d %d %d                ",l,j,*(buf8));

			}	
		}
		kprintf("\nRead all bytes of every sector(100 * 8)");*/
	
		/*for(int i=0; i<800;i++)
		{
			read(&abar->ports[portNo],i,0,1,buf);
			buf8 = (uint8_t*)buf;
			for(int j=0;j<512;j++)
			{
				kprintf("%d %d ",j,*buf8);

			}
		}*/
		
		
	}
	//iFinal = nr;
	//kprintf("Val is %d\n",retVal);
}

void mem_set2(void *start, char c, int length)
{
        unsigned char* str = start;
        int i;
        for(i=0;i<length;i++)
        {
             *str++ = c;
        }
}

/*void mem_set_int(int *start, int c, int length)
{
        unsigned int* str = start;
        int i;
        for(i=0;i<length;i++)
        {    
             *str++ = c;
        }
}
*/
