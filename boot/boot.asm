[bits 32]
global stack_ptr
global dis_ints
global enb_ints
global int_halt
global halt

extern gdt_descriptor
extern kernel_main
extern CODE_SEG
extern DATA_SEG 

section .bootstrap_stack nobits write
align 16
stack_bottom:
resb 32768
stack_top:

section .text

STACKSIZE equ 4 * 1024

global _start:function (_start.end - _start)

_start:
    cli

    finit ; Enable FPU

    mov esp, stack + STACKSIZE ; Point ESP to the start of the stack.

    mov [ebxb], ebx

    call test_multiboot
    call test_cpuid

    lgdt [gdt_descriptor] ; Load the GDT descriptor
    jmp CODE_SEG:.setcs ; Set CS to our 32-bit flat code selector
    .setcs:
    mov ax, DATA_SEG ; Setup the segment registers with our flat data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, stack_top
    mov ebp, 0x90000 ; Update the stack right at the top of the free space
    mov esp, ebp

    push dword [ebxb]
    call kernel_main ; Calls the C function. The linker will know where it is placed in memory
    jmp $ ; Stay here when the kernel returns control to us (if ever)

.end:

test_multiboot:
    cmp eax, 0x2BADB002
    jne halt
    ret

test_cpuid:
    pushfd ; Store the FLAGS-register.
    pop eax ; Restore the A-register.
    mov ecx, eax ; Set the C-register to the A-register.
    xor eax, 1 << 21 ; Flip the ID-bit, which is bit 21.
    push eax ; Store the A-register.
    popfd ; Restore the FLAGS-register.
    pushfd ; Store the FLAGS-register.
    pop eax ; Restore the A-register.
    push ecx ; Store the C-register.
    popfd ; Restore the FLAGS-register.
    xor eax, ecx ; Do a XOR-operation on the A-register and the C-register.
    jz halt ; The zero flag is set, no CPUID.
    ret ; CPUID is available for use.

enb_ints:
    sti

dis_ints:
    cli

int_halt:
    hlt

halt:
    cli
    hlt

.end:

section .bss
ebxb resw 1
alignb 4
stack:
    resb STACKSIZE
stack_ptr:
