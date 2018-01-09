#ifndef __PCI_H
#define __PCI_H

uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot,uint8_t func,uint8_t offset);
void writeWord (uint8_t bus, uint8_t slot,uint8_t func,uint8_t offset, uint32_t);
int checkIfContainsAHCI(uint32_t bus, uint32_t sloti, uint32_t);
void mem_set2(void *start, char c, int length);
extern void find_device(int k);
extern int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf);
extern int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf);
extern void mem_set2(void *start, char c, int length);
extern int probe_port(hba_mem_t *abar);
extern int check_type(hba_port_t *port);
extern void start_cmd(hba_port_t *port);
extern void stop_cmd(hba_port_t *port);
extern void port_rebase(hba_port_t *port, int portno);


#endif
