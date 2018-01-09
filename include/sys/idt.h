#ifndef __IDT_H_
#define __IDT_H

#define SELECTOR_VAL    0x08
#define TYPE_ATTRIB_VAL 0x8E
#define TYPE_ATTRIB_VAL_RING3 0xEE
typedef struct idt_entry_struct
{
        uint16_t offset_low;
        uint16_t selector;
        uint8_t reserved;
        uint8_t flg;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t reserved2;
}__attribute__ ((packed)) idt_struct;

typedef struct idt_ptr_struct
{
        uint16_t limit;
        uint64_t offset;
}__attribute__ ((packed)) idt_ptr;


void init_idt();
void mem_set(void *, char,int);
void idt_set(uint8_t, uint64_t,uint16_t,uint8_t);
void irq_remap(void);

#endif
