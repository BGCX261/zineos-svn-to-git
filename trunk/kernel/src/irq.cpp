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

#include <stdio.h>
#include <stdint.h>
#include "kernel/system.h"
#include "kernel/idt.h"
#include "kernel/irq.h"


// These are own ISRs that point to our special IRQ handler
// instead of the regular 'fault_handler' function
extern "C" {
	void irq0();
	void irq1();
	void irq2();
	void irq3();
	void irq4();
	void irq5();
	void irq6();
	void irq7();
	void irq8();
	void irq9();
	void irq10();
	void irq11();
	void irq12();
	void irq13();
	void irq14();
	void irq15();
}


/*
 * This array is actually an array of function pointers. We use
 * this to handle custom IRQ handlers for a given IRQ
 */
static Irq::HandlerFn routines[16];


/*
 * This installs a custom IRQ handler for the given IRQ
 */
void Irq::SetHandler(int irq, Irq::HandlerFn fn)
{
    routines[irq] = fn;
}


/*
 * Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 * is a problem in protected mode, because IDT entry 8 is a
 * Double Fault! Without remapping, every time IRQ0 fires,
 * you get a Double Fault Exception, which is NOT actually
 * what's happening. We send commands to the Programmable
 * Interrupt Controller (PICs - also called the 8259's) in
 * order to make IRQ0 to 15 be remapped to IDT entries 32 to
 * 47
 */
static inline void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

/*
 * We first remap the interrupt controllers, and then we install
 * the appropriate ISRs to the correct entries in the IDT. This
 * is just like installing the exception handlers
 */
void Irq::Install()
{
    irq_remap();

    Idt::SetGate(32, irq0,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(33, irq1,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(34, irq2,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(35, irq3,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(36, irq4,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(37, irq5,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(38, irq6,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(39, irq7,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(40, irq8,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(41, irq9,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(42, irq10, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(43, irq11, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(44, irq12, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(45, irq13, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(46, irq14, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
    Idt::SetGate(47, irq15, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Interrupt);
}


/*
 * Each of the IRQ ISRs point to this function, rather than
 * the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 * to be told when you are done servicing them, so you need
 * to send them an "End of Interrupt" command (0x20). There
 * are two 8259 chips: The first exists at 0x20, the second
 * exists at 0xA0. If the second controller (an IRQ from 8 to
 * 15) gets an interrupt, you need to acknowledge the
 * interrupt at BOTH controllers, otherwise, you only send
 * an EOI command to the first controller. If you don't send
 * an EOI, you won't raise any more IRQs
 */
extern "C" void irq_dispatcher(Idt::Regs *r)
{
	//printf("IRQ %d:\n", r->interrupt - 32);
	//r->Dump();
	//printf("-------------------------------------------------------------------------------\n");

    // Find out if we have a custom handler to run for this
    // IRQ, and then finally, run it
    Irq::HandlerFn fn = routines[r->interrupt - 32];
    if (fn) fn(r);

    // If the IDT entry that was invoked was greater than 40
    // (meaning IRQ8 - 15), then we need to send an EOI to
    // the slave controller
    if (r->interrupt >= 40)
    {
        outportb(0xA0, 0x20);
    }

    // In either case, we need to send an EOI to the master
    // interrupt controller too
    outportb(0x20, 0x20);
}
