[SECTION .trampoline]
[BITS 16]
extern start32          ;Our 32-bit Start

trampoline_code:
;First clear interrupts just incase
cli
xor ax, ax
mov ds, ax

lock inc dword [0x5000]
;Next, lets load our GDT
lgdt [0x7000]
;;Enable 32-bit
mov eax, cr0
or  eax, 1
mov cr0, eax

[BITS 32]
    jmp 0x08:0x6000

;Just hang for now
hlt

