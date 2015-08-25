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

EXTERN _irq_dispatcher

; IRQ Stuff

; Define an IRQ Entry stub for interrupts
; that do not return an error and will push a 0
; to the stack
; PARAMS :
;       label
;       Int index
%macro DEFINE_IRQ 2
    GLOBAL _irq%1       ; make the label global
    _irq%1:             ; define the label
    cli                 ; disable interrupts
    push qword 0        ; error code
    push qword %2       ; interrupt number
    jmp irq_common_stub ; jump to a common handler
%endmacro


; Define IRQ Stubs
DEFINE_IRQ 0,   32
DEFINE_IRQ 1,   33
DEFINE_IRQ 2,   34
DEFINE_IRQ 3,   35
DEFINE_IRQ 4,   36
DEFINE_IRQ 5,   37
DEFINE_IRQ 6,   38
DEFINE_IRQ 7,   39
DEFINE_IRQ 8,   40
DEFINE_IRQ 9,   41
DEFINE_IRQ 10,  42
DEFINE_IRQ 11,  43
DEFINE_IRQ 12,  44
DEFINE_IRQ 13,  45
DEFINE_IRQ 14,  46
DEFINE_IRQ 15,  47

;
; The irq common stuff.
; push state to the stack
; load kernel mode.
; call irq handler
irq_common_stub:
	PUSH_REGS
	mov RDI, RSP;
	call _irq_dispatcher
	POP_REGS
	add RSP, 16
	iretq
