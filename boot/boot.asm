global loader
global stack_ptr

extern kernel_main
extern gdt_descriptor
extern CODE_SEG
extern DATA_SEG 

section .bootstrap_stack nobits write align=16
align 16
stack_bottom:
resb 32768
stack_top:

section .text
[bits 32]

STACKSIZE equ 0x4000

global _start:function (_start.end - _start)

_start:
    mov esp, stack+STACKSIZE ; Point ESP to the start of the stack.

    call test_multiboot
    call test_cpuid

    push eax
    push ebx
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
    cli
    mov esp, stack_top
    mov ebp, 0x90000 ; Update the stack right at the top of the free space
    mov esp, ebp

    call kernel_main ; Calls the C function. The linker will know where it is placed in memory
    jmp $ ; Stay here when the kernel returns control to us (if ever)

.end:

test_multiboot:
    cmp eax, 0x2BADB002
    jne .no_multiboot
    ret

.no_multiboot:
    mov al, "0"
    jmp error

test_cpuid:
    pushfd               ; Store the FLAGS-register.
    pop eax              ; Restore the A-register.
    mov ecx, eax         ; Set the C-register to the A-register.
    xor eax, 1 << 21     ; Flip the ID-bit, which is bit 21.
    push eax             ; Store the A-register.
    popfd                ; Restore the FLAGS-register.
    pushfd               ; Store the FLAGS-register.
    pop eax              ; Restore the A-register.
    push ecx             ; Store the C-register.
    popfd                ; Restore the FLAGS-register.
    xor eax, ecx         ; Do a XOR-operation on the A-register and the C-register.
    jz .no_cpuid         ; The zero flag is set, no CPUID.
    ret                  ; CPUID is available for use.

.no_cpuid:
    mov al, "1"
    jmp error

; Prints `ERR: ` and the given error code to screen and hangs.
; parameter: error code (in ascii) in al
error:
    mov dword [0xB8000], 0x4F524F45
    mov dword [0xB8004], 0x4F3A4F52
    mov dword [0xB8008], 0x4F204F20
    mov byte  [0xB800A], al
    hlt

global dis_ints
global enb_ints
global int_halt
global halt

enb_ints:
    sti

dis_ints:
    cli

int_halt:
    hlt

halt:
    cli
    hlt

.Lhang:
    jmp halt

.end:

section .bss
align 4
stack:
  resb STACKSIZE
stack_ptr:

section .kend
global end_of_kernel
end_of_kernel:
