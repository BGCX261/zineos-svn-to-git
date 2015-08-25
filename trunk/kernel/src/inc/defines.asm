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

; multiboot definitions
%define MULTIBOOT_HEADER_MAGIC 0x1BADB002
%define MULTIBOOT_HEADER_FLAGS 0x00010003

; where is the kernel?
; %define KERNEL_VIRTUAL_BASE  0xffffffff80000000
%define KERNEL_VIRTUAL_BASE  0xFFFF800000000000
%define KERNEL_PHYSICAL_BASE 0x100000
; %define KERNEL_VMA_BASE      0xffffffff80000000
%define KERNEL_VMA_BASE      0xFFFF800000000000
%define KERNEL_LMA_BASE      0x100000

; the gdt entry to use for the kernel
%define CS_KERNEL   0x08
%define DS_KERNEL   0x10


; other definitions
%define STACK_SIZE 0x4000

; push all registers
%macro PUSH_REGS 0
	push RAX
	push RBX
	push RCX
	push RDX
	push RSI
	push RDI
	push RBP
	push R8
	push R9
	push R10
	push R11
	push R12
	push R13
	push R14
	push R15
%endmacro

; pop registers
%macro POP_REGS 0
	pop R15
	pop R14
	pop R13
	pop R12
	pop R11
	pop R10
	pop R9
	pop R8
	pop RBP
	pop RDI
	pop RSI
	pop RDX
	pop RCX
	pop RBX
	pop RAX
%endmacro
