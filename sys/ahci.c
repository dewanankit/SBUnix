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

int find_cmdslot(hba_port_t *port)
{
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i<32; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("\nCannot find free command list entry");
	return -1;
}

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf)
{
	port->is_rwc = (uint32_t)-1;
	int spin = 0;
	int slot = find_cmdslot(port);
	//kprintf("\nValue is %d\n",slot);
	if(slot){}
	int i=0;
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*) port->clb;
	cmdheader = cmdheader +  slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = 0;
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	mem_set2(cmdtbl, 0, sizeof(hba_cmd_tbl_t)+(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
	for ( i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;
		count -= 16;
	}
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;
	cmdtbl->prdt_entry[i].i = 1;
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	if(cmdfis){}
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = 0x25;
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
	cmdfis->count = count;
        //kprintf("I am here %s now\n",buf);
	while ((port->tfd & ( ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
	port->ci = 1<<slot;
	while (1)
	{
		if ((port->ci & (1<<slot)) == 0)
			break;
		if (port->is_rwc & HBA_PxIS_TFES)
		{
			kprintf("Disk read error\n");
			return -1;
		}
	}
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Disk read error\n");
		return -1;
	}
	return 1;
}

int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf)
{
	/*kprintf("early sctl value %x\n", port->sctl);
	kprintf("early ssts value %x\n", port->ssts);
	port->sctl = port->sctl | 1;
	kprintf("later sctl value %x\n", port->sctl);
	kprintf("later ssts value %x",port->ssts);*/
	port->is_rwc =(uint32_t)-1;
	port->cmd |= 0x1;
	port->cmd = (1<<3);
	port->cmd |= (0x1);

	int spin = 0;
	int slot = find_cmdslot(port);
	//kprintf("\nValue is %d\n",slot);
	//kprintf("cmd %x\n",port->cmd);
	if(slot){}
	int i=0;
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*) port->clb;
	cmdheader = cmdheader +  slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = 1;
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	mem_set2(cmdtbl, 0, sizeof(hba_cmd_tbl_t)+(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
	for ( i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;
		count -= 16;
	}
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;
	cmdtbl->prdt_entry[i].i = 1;
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
	if(cmdfis){}
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = 0x35;
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
	cmdfis->count = count;
	//kprintf("ssts value 2 %x",port->ssts);
	while ((port->tfd & ( ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
	port->ci = 1<<slot;
	while (1)
	{
		if ((port->ci & (1<<slot)) == 0)
			break;
		if (port->is_rwc & HBA_PxIS_TFES)
		{
			kprintf("\nDisk read error");
			return -1;
		}
	}
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("\nDisk read error");
		return -1;
	}
	return 1;
}

int probe_port(hba_mem_t *abar)
{
	uint32_t pi = abar->pi;
	int i = 0;
	//int cnt = 0;
	int retValue = -1;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
//			kprintf("%x             ",dt);
			if (dt == AHCI_DEV_SATA)
			{
				retValue = i;
				kprintf("SATA drive found at port %d\n", i);
			}
			else
			{
				//kprintf("No drive found at port %d\n", i);
			}
		}
		pi >>= 1;
		i++;
	}
	kprintf("RetValue %d\n",retValue);
	return retValue;
}

int check_type(hba_port_t *port)
{
	switch (port->sig)
	{
		case AHCI_DEV_SATA:
			return AHCI_DEV_SATA;
		case AHCI_DEV_SATAPI:
			return AHCI_DEV_SATAPI;
		case AHCI_DEV_SEMB:
			return AHCI_DEV_SEMB;
		case AHCI_DEV_PM:
			return AHCI_DEV_PM;
		default:
			return 0;
	}
}

void start_cmd(hba_port_t *port)
{
	while (port->cmd & HBA_PxCMD_CR);
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void stop_cmd(hba_port_t *port)
{
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;
	while(1)
	{
		kprintf("port cmd value - %x", port->cmd);
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
	kprintf("port cmd value - %x", port->cmd);
	port->cmd &= ~HBA_PxCMD_FRE;
}

void port_rebase(hba_port_t *port, int portno)
{
	//kprintf("In rebase\n");
	port->cmd &= ~1;
	for(int j=0;j<10000;j++){
			for(long i = 0; i < 10000;i++)
			{}
	}
	port->sctl |= 0x1;
	for(int j=0;j<10000;j++){
		for(long i = 0; i < 10000;i++)
		{}
	}
	while(port->sctl & 0x1){
		kprintf("stuck\n");
		for(int j=0;j<10000;j++){
				for(long i = 0; i < 10000;i++)
				{}
			}
	}
	//port->sctl &= ~0x1;
	port->cmd |= 0x1;
	port->cmd = (1<<3);
	for(int j=0;j<10000;j++){
		for(long i = 0; i < 10000;i++)
			{}
	}
	port->cmd |= (0x1);
	kprintf("tfd  %x ssts %x sctl %x  cmd %x\n",port->tfd,port->ssts,port->sctl,port->cmd);
	stop_cmd(port);
	//kprintf("port no : %d\n",portno);
	port->clb = AHCI_BASE + (portno<<10);
	mem_set2((void*)(port->clb), 0, 1024);
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	mem_set2((void*)(port->fb), 0, 256);
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		mem_set2((void*)cmdheader[i].ctba, 0, 256);
	}
	start_cmd(port);
	//kprintf("end of rebase");
}

