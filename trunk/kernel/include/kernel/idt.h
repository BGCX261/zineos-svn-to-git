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

#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include <kernel/common.h>
#include <stdint.h>

namespace Idt
{

	/**
	 * Registers
	 */
	struct Regs
	{
		uint64_t R15, R14, R13, R12, R11, R10, R9, R8;
		uint64_t RBP, RDI, RSI, RDX, RCX, RBX, RAX;
		uint64_t interrupt, error;
		uint64_t RIP, CS, RFLAGS, RSP, SS;

		/**
		 * Dump the registers to screen
		 */
		void Dump();
	} __attribute__((packed));


	/**
	 * Gate types
	 */
	enum class Gate : uint8_t
	{
		LDT			= 0b0010,
		FreeTSS		= 0b1001,
		BusyTSS		= 0b1011,
		Call		= 0b1100,
		Interrupt	= 0b1110,
		Trap		= 0b1111
	};


	/**
	 * IDT callback function. This method is called
	 * by the CPU
	 */
	typedef void(*HandlerFn)();


	/**
	 * Defines an IDT entry
	 */
	struct Entry
	{
		uint16_t offset_low; // 0  ... 15
		uint16_t selector;   // 16 ... 31 segment selector
		union {
			uint16_t attribs;
			struct {
				uint16_t ist:3;			// Interrupt stack table
				uint16_t zero0:5;		// reserved
				uint16_t type:4;		// entry type
				uint16_t zero1:1;		// reserved
				uint16_t dpl:2;			// description privilege level
				uint16_t present:1;		// segment present flag
			};
		};
		uint16_t offset_mid;
		uint32_t offset_high;
		uint32_t preserved;
	} __attribute__((packed));


	/**
	 * IDT pointer
	 */
	struct Ptr
	{
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed));


	/**
	 * setup the IDT
	 */
	void Install();

	/**
	 * set idt gate
	 */
	void SetGate(uint8_t num,
				 HandlerFn handler,
				 uint16_t sel,
				 uint8_t dpl,
				 Gate type);

	/**
	* Load IDT table
	*/
	extern void Load(Ptr *) asm ("_idt_load");
};

#endif // IDT_H_INCLUDED
