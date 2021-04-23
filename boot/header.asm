MALIGN   equ 1<<0
MEMINFO  equ 1<<1
VIDINFO  equ 1<<2
FLAGS    equ MALIGN | MEMINFO | VIDINFO
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
dd 0, 0, 0, 0, 0
dd 0
dd 800, 600, 20
