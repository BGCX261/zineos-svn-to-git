/**
 * Copyright (c) 2010, ZineOS development team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution. Neither the name of the ZineOS nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "kernel/system.h"
#include "kernel/idt.h"


// local data
namespace {
	Idt::Entry 	idt[256]	__attribute__((aligned(0x10)));
	Idt::Ptr 	idtp		__attribute__((aligned(0x10)));
}


/**
 * print out the registers
 */
void Idt::Regs::Dump()
{
	printf(
		"RAX = %p, RBX = %p, RCX = %p, RDX = %p\n"
		"RSI = %p, RDI = %p, RBP = %p\n"
		"R8  = %p, R9  = %p, R10 = %p, R11 = %p\n"
		"R12 = %p, R13 = %p, R14 = %p, R15 = %p\n"
		"RIP = %p, CS  = %p, RFLAGS = %p\n"
		"RSP = %p, SS  = %p\n"
		"interrupt=%p, error=%p\n",
		(void *)RAX, (void *)RBX, (void *)RCX, (void *)RDX,
		(void *)RSI, (void *)RDI, (void *)RBP,
		(void *)R8,  (void *)R9 , (void *)R10, (void *)R11,
		(void *)R12, (void *)R13, (void *)R14, (void *)R15,
		(void *)RIP, (void *)CS,  (void *)RFLAGS,
		(void *)RSP, (void *)SS,
		(void *)interrupt, (void *)error
	);
}


/**
 * Install the IDT
 */
void Idt::Install()
{
	idtp.limit = (sizeof (Idt::Entry) * 256) - 1;
	idtp.base = (uint64_t)&idt;
	Idt::Load(&idtp);
    // To Do:
    //  Set a default interrupts to catch problems
}


/**
 * Set gate
 */
void Idt::SetGate(uint8_t num,
				  HandlerFn handler,
				  uint16_t sel,
				  uint8_t dpl,
			      Gate type)
{
    idt[num].offset_low	= (uint64_t)handler & 0xFFFF;
    idt[num].offset_mid = ((uint64_t)handler >> 16) & 0xFFFF;
    idt[num].offset_high = (uint64_t)handler >> 32;

    idt[num].selector = sel;

    idt[num].attribs = 0; // null all bits
    idt[num].present = 1;
    idt[num].dpl = dpl;
    idt[num].type = (uint16_t)type;
}
