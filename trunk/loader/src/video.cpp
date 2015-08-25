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
#include "kernel/system.h"
#include "kernel/video.h"

#define VGA_MEMORY_ADDR 0xB8000

// hide local variables
static uint16_t * lineptr = (uint16_t *)VGA_MEMORY_ADDR;
static uint16_t attrib = 0x0F << 8;
static int csr_x = 0, csr_y = 0;
static void Scroll(void);
static void MoveCursor(void);
static void Out(char);


/**
 * Initalize
 */
void Video::Install (void)
{
	Video::Clear();
}


/**
 * Ensure that caret is at the start of the line.
 */
void Video::StartLine()
{
	if (csr_x == 0) return;
	Video::Write('\n');
}


/**
 * Write char to screen
 */
void Video::Write(char ch)
{
	Out(ch);
	MoveCursor();
}


/**
 * Write null terminated string to screen
 */
void Video::Write(const char *text)
{
	for (int i = 0; text[i] != '\0'; i++)
	{
		Out(text[i]);
	}
	MoveCursor();
}


/**
 * Clear the screen
 */
void Video::Clear()
{
	// space
	uint16_t blank = 0x20 | attrib;

	// Sets the entire screen to spaces in our current color
	memsetw ((void *)VGA_MEMORY_ADDR, blank, 80 * 25);

	// reset virtual cursor
	csr_x = 0;
	csr_y = 0;
	lineptr = (uint16_t *)VGA_MEMORY_ADDR;
	MoveCursor();
}


/**
 * Set foreground colour
 */
void Video::SetFgColor(Color fore)
{
	attrib = (attrib & 0xF000) | ((fore & 0xF) << 8);
}


/**
 * Set background colour
 */
void Video::SetBgColor(Color back)
{
	attrib = (attrib & 0x0F00) | ((back & 0xF) << 12);
}


/**
 * Set the colours
 */
void Video::SetColors(Color fore, Color back)
{
	attrib = ((back << 4) | (fore & 0x0F)) << 8;
}


/**
 * Get colours mask
 */
uint16_t Video::GetColors()
{
	return attrib >> 8;
}


/**
 * set colors
 */
void Video::SetColors(uint16_t colors)
{
	attrib = colors << 8;
}


/**
 * Scroll text on the screen
 */
void Scroll(void)
{
	// no need
	if (csr_y < 25) return;
	// Fill it with space. should it be 0?
	unsigned blank = 0x20 | attrib;
	unsigned temp = csr_y - 25 + 1;
	// move existing lines "up"
	memcpy((void *)VGA_MEMORY_ADDR,
			(void *)((uint16_t *)VGA_MEMORY_ADDR + temp * 80),
			(25 - temp) * 80 * 2);
	// clear new emptied lines
	memsetw((void *)((uint16_t *)VGA_MEMORY_ADDR + (25 - temp) * 80),
			blank,
			80);
	// last line
	csr_y = 24;
}


/**
 * Move cursor to the current location
 */
void MoveCursor(void)
{
	// linear cursor position
	unsigned temp = csr_y * 80 + csr_x;

	// This sends a command to indicies 14 and 15 in the
	// CRT Control Register of the VGA controller. These
	// are the high and low bytes of the index that show
	// where the hardware cursor is to be 'blinking'. To
	// learn more, you should look up some VGA specific
	// programming documents. A great start to graphics:
	// http://www.brackeen.com/home/vga */
	outportb(0x3D4, 14);
	outportb(0x3D5, temp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, temp);
}


/**
 * Write a single character to the screen
 */
void Out(char c)
{
	// backspace
	if(c == 0x08)
	{
		if(csr_x != 0) csr_x--;
	}
	// tab - increment by 8
	else if(c == 0x09)
	{
		csr_x = (csr_x + 8) & ~(8 - 1);
	}
	// carriage return
	else if(c == '\r')
	{
		csr_x = 0;
	}
	// new line
	else if(c == '\n')
	{
		csr_x = 0;
		csr_y++;
		Scroll();
		lineptr = (uint16_t *)VGA_MEMORY_ADDR + (csr_y * 80);
	}
	// visible character
	else if(c >= ' ')
	{
		lineptr[csr_x++] = c | attrib;
	}

	// new line ?
	if(csr_x >= 80)
	{
		csr_x = 0;
		csr_y++;
		Scroll();
		lineptr = (uint16_t *)VGA_MEMORY_ADDR + (csr_y * 80);
	}
}
