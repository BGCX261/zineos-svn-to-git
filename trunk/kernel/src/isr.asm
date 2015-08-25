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


EXTERN _isr_handler

; Service Routines (ISRs)

; Define an ISR Entry stub for interrupts
; that do not return an error and will push a 0
; to the stack
; PARAMS :
;       label
;       Int index
%macro DEFINE_ISR 1
    GLOBAL _isr%1       ; make the label global
    _isr%1:             ; define the label
    cli                 ; disable interrupts
    push qword 0        ; error code
    push qword %1       ; interrupt number
    jmp isr_common_stub ; jump to a common handler
%endmacro

; Define an ISR Entry stub for interrupts
; that return an error on the stack.
; PARAMS :
;       label
;       Int index
%macro DEFINE_ERR_ISR 1
    GLOBAL _isr%1       ; make the label global
    _isr%1:             ; define the label
    cli                 ; disable interrupts
    push qword %1       ; interrupt number
    jmp isr_common_stub ; jump to a common handler
%endmacro


; The ISR stubs
DEFINE_ISR      0
DEFINE_ISR      1
DEFINE_ISR      2
DEFINE_ISR      3
DEFINE_ISR      4
DEFINE_ISR      5
DEFINE_ISR      6
DEFINE_ISR      7
DEFINE_ERR_ISR  8
DEFINE_ISR      9
DEFINE_ERR_ISR  10
DEFINE_ERR_ISR  11
DEFINE_ERR_ISR  12
DEFINE_ERR_ISR  13
DEFINE_ERR_ISR  14
DEFINE_ISR      15
DEFINE_ISR      16
DEFINE_ISR      17
DEFINE_ISR      18
DEFINE_ISR      19
DEFINE_ISR      20
DEFINE_ISR      21
DEFINE_ISR      22
DEFINE_ISR      23
DEFINE_ISR      24
DEFINE_ISR      25
DEFINE_ISR      26
DEFINE_ISR      27
DEFINE_ISR      28
DEFINE_ISR      29
DEFINE_ISR      30
DEFINE_ISR      31

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
	PUSH_REGS
	mov RDI, RSP;
	call _isr_handler
	POP_REGS
	add RSP, 16
	iretq
