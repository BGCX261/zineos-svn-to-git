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


[BITS 16]
global start
start:
jmp .skip
;Used to track how many CPU's have run this code
.CPU_Count dd 0xdeadbeef        ;Set externally
.location dd 0xdeadbeef         ;Set externally
.codeseg dd 0x08
ALIGN 8
.gdt_start:
.gdt_size:      dw	.gdt_end - .gdt_start
.gdt_location:	dd  .gdt_start
                db	0
                db	0

.gdt_code:      ; Code segment, read/execute, nonconforming
                dw 0xFFFF
                dw 0
                db 0
                db 10011010b
                db 11001111b
                db 0

.gdt_data:      ; Data segment, read/write, expand down
                dw 0xFFFF
                dw 0
                db 0
                db 10010010b
                db 11001111b
                db 0
.gdt_end:       ; Used to calculate the size of the GDT

.skip:
    ;First clear interrupts just incase
    cli
    cld

    mov eax, 1
    lock xadd [cs:.CPU_Count], eax ;Update CPU counter
    inc eax

    lgdt [cs:.gdt_start]

    ;Enable 32-bit
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp far dword [cs:.location] ;This jumps into our kernel
