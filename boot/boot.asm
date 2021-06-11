[bits 32]
global halt

extern gdt_descriptor
extern kernel_main
extern CODE_SEG
extern DATA_SEG

section .text

STACKSIZE equ 16 * 1024

global _start:function (_start.end - _start)

_start:
    cli

    mov ebp, stack_end
    mov esp, ebp

    mov [ebxb], ebx

    call test_multiboot
    call test_cpuid
    call test_fpu

    mov eax, cr0 ; Enable FPU
    and al, ~4
    or al, 66
    mov cr0, eax
    fninit

    lgdt [gdt_descriptor] ; Load the GDT descriptor
    jmp CODE_SEG:.setcs   ; Set CS to our 32-bit flat code selector
    .setcs:
    mov ax, DATA_SEG ; Setup the segment registers with our flat data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push dword [ebxb]
    call kernel_main ; Calls the C function. The linker will know where it is placed in memory
    jmp $            ; Stay here when the kernel returns control to us (if ever)

.end:

test_multiboot:
    cmp eax, 0x2BADB002
    jne halt
    ret

test_cpuid:
    pushfd           ; Store the FLAGS-register.
    pop eax          ; Restore the A-register.
    mov ecx, eax     ; Set the C-register to the A-register.
    xor eax, 1 << 21 ; Flip the ID-bit, which is bit 21.
    push eax         ; Store the A-register.
    popfd            ; Restore the FLAGS-register.
    pushfd           ; Store the FLAGS-register.
    pop eax          ; Restore the A-register.
    push ecx         ; Store the C-register.
    popfd            ; Restore the FLAGS-register.
    xor eax, ecx     ; Do a XOR-operation on the A-register and the C-register.
    jz halt          ; The zero flag is set, no CPUID.
    ret              ; CPUID is available for use.

test_fpu:
    mov eax, 0x1
    cpuid
    test edx, 1 << 0
    jz .no_fpu
    ret

.no_fpu:
    mov eax, cr0
    or al, 4
    and al, ~2
    mov cr0, eax
    jmp halt

halt:
    cli
    hlt

.end:

section .bss

alignb 4

ebxb: resd 1
stack: resb STACKSIZE
stack_end:
