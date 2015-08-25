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

[BITS 64]
%include "src/inc/defines.asm"


;===============================================================================
; External symbols
;===============================================================================
EXTERN _kmain
EXTERN _g_cpu_count
EXTERN _gp_system_params
;===============================================================================
; Kernel code
;===============================================================================
[SECTION .text]
GLOBAL start64
start64:
    mov rax, start_again
    jmp rax
start_again:

	; Initialize the 64 bit stack pointer.
	mov rsp, stack + STACK_SIZE

    mov rax, 1
    lock xadd [rel _g_cpu_count], rax
    inc rax                           ;rax stores our CPU #

    cmp rax, 1
    je .Ok
    hlt
.Ok
	; set cpu flags
	push 0
	lss eax, [rsp]
	popf

    mov [rel _gp_system_params], edx


    ; set the input/output permission level to 3
	; it will allow all access
	pushf
	pop rax
	or rax, 0x3000
	push rax
	popf

	; update the multiboot struct to point to a
	; virtual address
	;add rsi, (KERNEL_VIRTUAL_BASE & 0xFFFFFFFF)

	; push the parameters (just in case)
	;push rsi
	;push rdi

	; call kmain
	call _kmain
	jmp $

;-------------------------------------------------------------------------------
; stack space
;-------------------------------------------------------------------------------
[SECTION .bss]
ALIGN 0x8
stack: resb STACK_SIZE
