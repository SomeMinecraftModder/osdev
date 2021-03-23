MALIGN   equ 1<<0
MEMINFO  equ 1<<1
FLAGS    equ MALIGN | MEMINFO
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
