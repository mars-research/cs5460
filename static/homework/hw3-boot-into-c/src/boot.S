#include "asm.h"
#include "mmu.h"

.global start

.code32
.section .text
start:
    movw $0x0248, 0xb8000 # H
    movw $0x0265, 0xb8002 # e
    movw $0x026c, 0xb8004 # l
    movw $0x026c, 0xb8006 # l
    movw $0x026f, 0xb8008 # o
    movw $0x022c, 0xb800a # ,
    movw $0x0220, 0xb800c # (space)
    movw $0x0277, 0xb800e # w
    movw $0x026f, 0xb8010 # o
    movw $0x0272, 0xb8012 # r
    movw $0x026c, 0xb8014 # l
    movw $0x0264, 0xb8016 # d
    movw $0x0221, 0xb8018 # !

    mov $p3_table, %eax
    or $(PTE_P | PTE_W), %eax
    mov %eax, p4_table

    mov $p2_table, %eax
    or $(PTE_P | PTE_W), %eax
    mov %eax, p3_table

    mov $(PTE_P | PTE_W | PTE_PS), %eax
    mov %eax, p2_table

    mov p4_table, %eax
    mov %eax, %cr3

    mov %cr4, %eax
    or $(CR4_PAE), %eax
    mov %eax, %cr4

    mov $(EFER_MSR), %ecx
    rdmsr
    or $(EFER_MSR_LME), %eax
    wrmsr

    mov %cr0, %eax
    or $(CR0_PG), %eax
    mov %eax, %cr0

    lgdt gdt64desc
    ljmp $(SEG_KCODE<<3), $start64

.code64
start64:
    movabs $(stack + 4096), %rsp
    call main

.section .bss
.align 4096
/* Reserve memory for the 3 page tables (4KB each) */
p4_table: .skip 4096
p3_table: .skip 4096
p2_table: .skip 4096

.section .data
.comm stack, 4096

.align 8
gdt64:
  SEG_NULLASM                             # null seg
  SEG64_ASM(STA_X|STA_R, SEG64_CODE)      # code seg
  SEG64_ASM(STA_W, SEG64_OTHER)           # data seg

gdt64desc:
  .word   (gdt64desc - gdt64 - 1)         # sizeof(gdt64) - 1
  .quad   gdt64                           # address gdt64
