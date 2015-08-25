; Copyright (c) 2010, ZineOS development team
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; Redistributions of source code must retain the above copyright notice, this
; list of conditions and the following disclaimer. Redistributions in binary
; form must reproduce the above copyright notice, this list of conditions and
; the following disclaimer in the documentation and/or other materials provided
; with the distribution. Neither the name of the ZineOS nor the names of its
; contributors may be used to endorse or promote products derived from this
; software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.

[BITS 32]
%include "src/inc/defines.asm"

;===============================================================================
; External symbols
;===============================================================================
EXTERN code, data_end, end
EXTERN _kmain, _printf, _boot
EXTERN _pml4_table, _kernel_address
EXTERN _g_system_params
;-------------------------------------------------------------------------------
; Multiboot header
;-------------------------------------------------------------------------------
[SECTION .text]

ALIGN 4
multiboot:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	dd multiboot
	dd code
	dd data_end
	dd end
	dd start32


;===============================================================================
; Macros
;===============================================================================
%macro print 1+
    [SECTION .data]
    %%string: db %1, 0xA, 0
    [SECTION .text]
    push dword 0
    push dword %%string
    call _printf
    add esp, 8
%endmacro



;===============================================================================
; loader code
;===============================================================================
; the kernel entry point
GLOBAL start32
start32:
    ; setup stack
    mov ESP, (stack + STACK_SIZE)

    ; store multiboot on stack
    push ebx            ; magic number
    push eax            ; multimoot info block

    ; Do most basic setup
    call _boot;
    print "Boot start"
    print "============"

    ; Check for existance of CPUID
    pushfd              ; Store the FLAGS-register.
    pop eax             ; Restore the A-register.
    mov ecx, eax        ; Set the C-register to the A-register.
    xor eax, 1 << 21    ; Flip the ID-bit, which is bit 21.
    push eax            ; Store the A-register.
    popfd               ; Restore the FLAGS-register.
    pushfd              ; Store the FLAGS-register.
    pop eax             ; Restore the A-register.
    push ecx            ; Store the C-register.
    popfd               ; Restore the FLAGS-register.
    xor eax, ecx        ; Do a XOR-operation on the A-register and the C-register.
    jz .NoCPUID         ; The zero flag is set, no CPUID.

    ; check if extended functions exist in cpuid
    mov eax, 0x80000000 ; Set the A-register to 0x80000000.
    cpuid               ; CPU identification.
    cmp eax, 0x80000001 ; Compare the A-register with 0x80000001.
    jb .NoLongMode      ; It is less, there is no long mode.

    ; Check if long mode is supported
    mov eax, 0x80000001 ; Set the A-register to 0x80000001.
    cpuid               ; CPU identification.
    test edx, 1 << 29   ; Test if the LM-bit, is set in the D-register.
    jz .NoLongMode      ; They aren't, there is no long mode.

	; call main
	; arguments are already on the stack!
	call _kmain

	;0 = error
	cmp eax, 0
	je .KernelNotLoaded
    mov [KernelAddress], eax

	; disable interrupts
	cli

    ;Let our AP's know they can go
	inc byte [KernelReady]

	; Build the page tables
;	call BuildPageTables

    .DoAPEntry:
	; enable 64-bit page translation table entries
	; by setting CR4.PAE = 1.
	; Paging is not enabled until long mode.
	mov eax, cr4
	bts eax, 5
	mov cr4, eax

	; Create long mode page table and init CR3 to
	; point to the base of the PML4 page table
	mov eax, [_pml4_table]
	mov cr3, eax

	; Enable Long mode and SYSCALL / SYSRET instructions
	mov ecx, 0xC0000080
	rdmsr
	bts eax, 8
	bts eax, 0
	wrmsr

	mov ebx, [KernelAddress]
    mov edx, _g_system_params

	; Load the 32 bit GDT
	lgdt [GdtTablePtr]

	; enable paging to activate long mode
	mov eax, cr0
	bts eax, 31
	mov cr0, eax

	; Jump into 64bit code (the kernel entry point)
	jmp CS_KERNEL:.to64
	.to64:

    jmp ebx

[bits 32]
    ; NoCPUID
    ;---------------------------------------------------------------------------
    .NoCPUID:
        print "No CPUID"
        cli
        hlt

    ; Long mode not supported
    ;---------------------------------------------------------------------------
    .NoLongMode:
        print "Long mode not supported"
        cli
        hlt

    ; Kernel not loaded
    ;---------------------------------------------------------------------------
    .KernelNotLoaded:
        ;print "-----------------"
        ;print "Kernel not loaded"
        cli
        hlt
        nop



 ;This is where our AP's enter
global ApplicationProcessor
ApplicationProcessor:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    .again:
    cmp byte [KernelReady], 0
    je .again

    jmp start32.DoAPEntry

    cli
    hlt

[SECTION .data]
;-------------------------------------------------------------------------------
; GDT tables
;-------------------------------------------------------------------------------
global GdtTablePtr
ALIGN 0x1000
GdtTablePtr:
	dw GdtTableEnd - GdtTable - 1
	dd GdtTable, 0
GdtTable:
	dq 0x0000000000000000		; Null Descriptor
	dq 0x00af9a000000ffff, 0	; CS_KERNEL
	dq 0x00af93000000ffff, 0	; DS_KERNEL
	dq 0, 0
GdtTableEnd:

;Tells our AP's when to go to kernel
KernelReady:
    db  0
KernelAddress:
    dd  0

[SECTION .bss]

;-------------------------------------------------------------------------------
; Stack
;-------------------------------------------------------------------------------
ALIGN 0x4
stack: resb STACK_SIZE
