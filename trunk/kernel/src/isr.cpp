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

#include <stdint.h>
#include <stdio.h>
#include "kernel/system.h"
#include "kernel/idt.h"
#include "kernel/isr.h"

/*
 * These are function prototypes for all of the exception
 * handlers: The first 32 entries in the IDT are reserved
 * by Intel, and are designed to service exceptions!
 */
extern "C" {
	void isr0();
	void isr1();
	void isr2();
	void isr3();
	void isr4();
	void isr5();
	void isr6();
	void isr7();
	void isr8();
	void isr9();
	void isr10();
	void isr11();
	void isr12();
	void isr13();
	void isr14();
	void isr15();
	void isr16();
	void isr17();
	void isr18();
	void isr19();
	void isr20();
	void isr21();
	void isr22();
	void isr23();
	void isr24();
	void isr25();
	void isr26();
	void isr27();
	void isr28();
	void isr29();
	void isr30();
	void isr31();
}

/*
 * This is a very repetitive function... it's not hard, it's
 * just annoying. As you can see, we set the first 32 entries
 * in the IDT to the first 32 ISRs. We can't use a for loop
 * for this, because there is no way to get the function names
 * that correspond to that given entry. We set the access
 * flags to 0x8E. This means that the entry is present, is
 * running in ring 0 (kernel level), and has the lower 5 bits
 * set to the required '14', which is represented by 'E' in
 * hex.
 */
void Isr::install()
{
    Idt::SetGate(0,  isr0,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(1,  isr1,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(2,  isr2,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(3,  isr3,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(4,  isr4,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(5,  isr5,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(6,  isr6,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(7,  isr7,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);

    Idt::SetGate(8,  isr8,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(9,  isr9,  KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(10, isr10, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(11, isr11, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(12, isr12, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(13, isr13, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(14, isr14, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(15, isr15, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);

    Idt::SetGate(16, isr16, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(17, isr17, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(18, isr18, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(19, isr19, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(20, isr20, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(21, isr21, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(22, isr22, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(23, isr23, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);

    Idt::SetGate(24, isr24, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(25, isr25, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(26, isr26, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(27, isr27, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(28, isr28, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(29, isr29, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(30, isr30, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
    Idt::SetGate(31, isr31, KERNEL_CODE_SEGMENT, 0, Idt::Gate::Trap);
}


/*
 * This is a simple string array. It contains the message that
 * corresponds to each and every exception. We get the correct
 * message by accessing like:
 * exception_message[interrupt_number]
 */
static const char * const exceptions[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",

    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};



/*
 * All of our Exception handling Interrupt Service Routines will
 * point to this function. This will tell us what exception has
 * happened! Right now, we simply halt the system by hitting an
 * endless loop. All ISRs disable interrupts while they are being
 * serviced as a 'locking' mechanism to prevent an IRQ from
 * happening and messing up kernel data structures
 */
extern "C" void isr_handler(Idt::Regs *regs)
{
    if (regs->interrupt < 32)
    {
        printf (
            "\nINT(%d) %s\n", (int)regs->interrupt, exceptions[regs->interrupt]
        );
        // print_regs (regs);
        regs->Dump();
        printf ("System halted\n");
        halt();
    }
}
