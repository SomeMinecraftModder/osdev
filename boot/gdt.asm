[bits 32]
global gdt_descriptor
global CODE_SEG
global DATA_SEG

; GDT - Global Descriptor Table
gdt_start:
gdt_null: ; Entry 1: Null entry must be included first (error check)
    dd 0x0 ; double word = 4 bytes = 32 bits
    dd 0x0

gdt_code: ; Entry 2: Code segment descriptor
    ; Structure:
    ; Segment Base Address (base) = 0x0
    ; Segment Limit (limit) = 0xFFFFF
    dw 0xFFFF ; Limit bits 0-15
    dw 0x0    ; Base bits 0-15
    db 0x0    ; Base bits 16-23
    ; Flag Set 1:
        ; Segment Present: 0B1
        ; Descriptor Privilege level: 0x00 (ring 0)
        ; Descriptor Type: 0B1 (code/data)
    ; Flag Set 2: Type Field
        ; Code: 0B1 (this is a code segment)
        ; Conforming: 0B0 (Code w/ lower privilege may not call this)
        ; Readable: 0B1 (Readable or execute only? Readable means we can read code constants)
        ; Accessed: 0B0 (Used for debugging and virtual memory. CPU sets bit when accessing segment)
    db 10011010B ; Flag set 1 and 2
    ; Flag Set 3
        ; Granularity: 0B1 (Set to 1 multiplies limit by 4K. Shift 0xFFFFF 3 bytes left, allowing to span full 32G of memory)
        ; 32-bit default: 0B1
        ; 64-bit segment: 0B0
        ; AVL: 0B0
    db 11001111B ; Flag set 3 and limit bits 16-19
    db 0x0       ; Base bits 24-31

gdt_data:
    ; Same except for code flag:
    ; Code: 0B0
    dw 0xFFFF    ; Limit bits 0-15
    dw 0x0       ; Base bits 0-15
    db 0x0       ; Base bits 16-23
    db 10010010B ; Flag set 1 and 2
    db 11001111B ; 2nd flags and limit bits 16-19
    db 0x0       ; Base bits 24-31

gdt_end: ; Needed to calculate GDT size for inclusion in GDT descriptor

; GDT Descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size of GDT, always less one
    dd gdt_start

; Define constants
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
