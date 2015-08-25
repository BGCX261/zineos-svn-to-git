;; Copyright (c) 2010, ZineOS development team
;; All rights reserved.
;;
;; Redistribution and use in source and binary forms, with or without
;; modification, are permitted provided that the following conditions are met:
;;
;; Redistributions of source code must retain the above copyright notice, this
;; list of conditions and the following disclaimer. Redistributions in binary
;; form must reproduce the above copyright notice, this list of conditions and
;; the following disclaimer in the documentation and/or other materials provided
;; with the distribution. Neither the name of the ZineOS nor the names of its
;; contributors may be used to endorse or promote products derived from this
;; software without specific prior written permission.
;;
;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
;; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;; POSSIBILITY OF SUCH DAMAGE.
;
;[BITS 32]
;%include "src/inc/defines.asm"
;
;
;;===============================================================================
;; External symbols
;;===============================================================================
;EXTERN data_end, end, bss, boot_start
;EXTERN start64
;EXTERN stack
;
;
;;-------------------------------------------------------------------------------
;; Multiboot header
;;-------------------------------------------------------------------------------
;[SECTION .text]
;ALIGN 4
;multiboot:
;	dd MULTIBOOT_HEADER_MAGIC
;	dd MULTIBOOT_HEADER_FLAGS
;	dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
;	dd multiboot
;	dd boot_start
;	dd (data_end - KERNEL_VIRTUAL_BASE)
;	dd (end - KERNEL_VIRTUAL_BASE)
;	dd start32
;
;
;;===============================================================================
;; loader code
;;===============================================================================
;; the kernel entry point
;GLOBAL start32
;start32:
;	; Stash values for multiboot we won't touch until 64 bit mode
;	;mov esi, ebx
;	;mov edi, eax
;
;	; clear screen
;	mov edi, 0xB8000
;	mov ecx, 80*25*2/4
;	xor eax, eax
;	rep stosd
;
;	; Useful macro for printing text
;	%macro print 1+
;		[SECTION .data]
;		%%string: db %1
;		%%length: equ $- %%string
;		[SECTION .text]
;		mov ESI, (%%string - KERNEL_VIRTUAL_BASE)
;		mov EDI, 0xB8000
;		mov ECX, %%length
;		call WriteLn
;	%endmacro
;
;	; disable interrupts
;	cli
;
;	; enable 64-bit page translation table entries
;	; by setting CR4.PAE = 1.
;	;
;	; Paging is not enabled until long mode.
;	mov eax, cr4
;	bts eax, 5
;	mov cr4, eax
;
;	; Build the page tables
;	call BuildPageTables
;
;	; Create long mode page table and init CR3 to
;	; point to the base of the PML4 page table
;	mov eax, PML4E_Table
;	mov cr3, eax
;
;	; Enable Long mode and SYSCALL / SYSRET instructions
;	mov ecx, 0xC0000080
;	rdmsr
;	bts eax, 8
;	bts eax, 0
;	wrmsr
;
;	; Load the 32 bit GDT
;	lgdt [GdtTablePtr]
;
;	; enable paging to activate long mode
;	mov eax, cr0
;	bts eax, 31
;	mov cr0, eax
;
;	; Jump into 64bit code
;	jmp CS_KERNEL:(start64 - KERNEL_VIRTUAL_BASE)
;
;	; end
;	jmp $
;
;
;;-------------------------------------------------------------------------------
;; Build page tables
;;-------------------------------------------------------------------------------
;BuildPageTables:
;	; clear the memory
;	cld
;	mov edi, (PAGE_TABLE_START)
;	mov ecx, ((PAGE_TABLE_END - PAGE_TABLE_START) / 4)
;	xor eax, eax
;	rep stosd
;	; PTE:
;	; contains 512 entries (4kb each)
;	mov ECX, 512
;	MOV EDI, (PTE_Table)
;	mov EAX, 0x87
;	.setPTE
;		mov [edi], eax
;		add eax, 0x1000
;		add edi, 8
;	loop .setPTE
;	; PDE
;	; contains 512 tables (2MB each)
;	; PDE_Table[0] = PTE_Table
;	MOV EDI, (PDE_Table)
;	MOV [EDI], dword (PTE_Table) + 0x7
;	; PDPE
;	; Contains 512 directories (1GB each)
;	; PDPE_Table[0] = PDE_Table
;	mov edi, (PDPE_Table)
;	mov [edi], dword (PDE_Table) + 0x7
;	; PML4E
;	; Contains 512 PDPE entries (512GB each)
;	; PML4E_Table[0] = PDPE_Table
;	mov edi, (PML4E_Table)
;	mov [edi], dword (PDPE_Table) + 0x7
;	; PML4E_Table[256] = PDPE_Table
;	add edi, 0x800
;	mov [edi], dword (PDPE_Table) + 0x7
;
;	ret
;
;
;;-------------------------------------------------------------------------------
;; Display text
;;-------------------------------------------------------------------------------
;WriteLn:
;	; ESI = string to display
;	; ECX = length of the string
;	; EDI = location to display the string
;	; Scrap(ESI,EAX)Mod(EDI,ECX=0)
;	mov ah, 0x07
;	.copying_string:
;	lodsb
;	stosw
;	loop .copying_string
;	retn
;
;
;;===============================================================================
;; Data Structures Follow
;;===============================================================================
;;-------------------------------------------------------------------------------
;; GDT tables
;;-------------------------------------------------------------------------------
;ALIGN 0x1000
;GdtTablePtr:
;	dw GdtTableEnd - GdtTable - 1
;	dq GdtTable
;GdtTable:
;	dq 0x0000000000000000		; Null Descriptor
;	dq 0x00cf9a000000ffff		; CS_KERNEL32
;	dq 0x00af9a000000ffff, 0	; CS_KERNEL
;	dq 0x00af93000000ffff, 0	; DS_KERNEL
;	dq 0, 0
;GdtTableEnd:
;
;;-------------------------------------------------------------------------------
;; Paging datastructures (PAE enabled, 4kb pages)
;;-------------------------------------------------------------------------------
;; [SECTION .bss]
;ALIGN 0x1000
;PAGE_TABLE_START:
;PML4E_Table: 	resb 0x1000
;PDPE_Table: 	resb 0x1000
;PDE_Table: 		resb 0x1000
;PTE_Table: 		resb 0x1000
;PAGE_TABLE_END:
