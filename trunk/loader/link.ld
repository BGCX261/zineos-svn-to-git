/*
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

PHYSICAL_BASE = 0x100000;

/* start from the entry point */
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start32)

SECTIONS
{
	. = PHYSICAL_BASE;
	/* code section */
	.text ALIGN(4096) :
	{
		code = .;
		*(.text*)
		*(.gnu.linkonce.t*)
		*(.rodata*)
		*(.gnu.linkonce.r*)
		code_end = .;
	}

	/* data section */
	.data ALIGN(4096) :
	{
		data = .;

		_start_ctors = .;
		*(.ctor*)
		_end_ctors = .;

		_start_dtors = .;
		*(.dtor*)
		_end_dtors = .;

		*(.data*)
		*(.gnu.linkonce.d*)

		data_end = .;
	}

	/* static code */
	.bss ALIGN(4096) :
	{
		bss = .;
		*(.COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
		. = ALIGN(4096);
		bss_end = .;
	}

	end = .;
	/DISCARD/ :
	{
		*(.comment*)
		*(.eh_frame*)
	}
}
