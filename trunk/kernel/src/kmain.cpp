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

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "kernel/system.h"
#include "kernel/video.h"
#include "kernel/multiboot.h"
#include "kernel/idt.h"
#include "kernel/isr.h"
#include "kernel/irq.h"
#include "kernel/initrd.h"
#include "kernel/virtual.h"
#include "icxxabi.h"

int show_boot_messages = 1;
void show_welcome ();
void show_boot_msg (const char * msg, const char * info, bool status);

System::Parameters *gp_system_params = 0;
uint64_t volatile g_cpu_count = 0;

extern "C" void kmain(uint64_t magic, multiboot_info_t * info)
{
    uint64_t volatile old_count=0;

	Video::Install();

	show_welcome();

	show_boot_msg("test", "info", true);
	show_boot_msg("test", "info", false);

	Idt::Install();
	Isr::install();
	Irq::Install();

	sti();

    VirtualMemory::Install();

    old_count = g_cpu_count;
	printf("In 64bit now - %d CPU's Active\n", old_count);
	for (;;)
	{
        if (old_count != g_cpu_count)
        {
            old_count = g_cpu_count;
            printf("In 64bit now - %d CPU's Active\n", old_count);
        }
	}
}


/*
 * Show Welcome message
 */
void show_welcome ()
{
	// current colors
	uint16_t colors = Video::GetColors();

	// ensure new line
	Video::StartLine();

	// background
	Video::SetBgColor(Video::Black);

    // show the message
    Video::SetFgColor(Video::LightGreen);
    Video::Write("Zine");
    Video::SetFgColor(Video::LightBlue);
    Video::Write("OS");
    Video::SetFgColor(Video::LightGray);
    Video::Write(" kernel 0.0.0.1 Alpha\n");

    // restore colors
    Video::SetColors(colors);
}

/**
 * show message
 */
void show_boot_msg (const char * msg, const char * info, bool status)
{
	// current colors
	uint16_t colors = Video::GetColors();

	// ensure new line
	Video::StartLine();

	// background
	Video::SetBgColor(Video::Black);

	// * name
    Video::SetFgColor(Video::LightGray);
    Video::Write(" * ");
    Video::SetFgColor(Video::White);
    Video::Write(msg);

	// ...
    Video::SetFgColor(Video::LightGray);
    int dots = 80 - 4 - 10 - strlen(msg);
    Video::Write(' ');
    for ( ; dots > 0; dots--)
        Video::Write('.');

	// [ status ]
    Video::SetFgColor(Video::White);
    Video::Write(" [");
    if (status == true)
    {
        Video::SetFgColor(Video::Green);
        Video::Write("  OK  ");
    }
    else
    {
        Video::SetFgColor(Video::Red);
        Video::Write(" FAIL ");
    }
    Video::SetFgColor(Video::White);
    Video::Write("]\n");

	// info
    if (info)
    {
        Video::SetFgColor(Video::LightGray);
        Video::Write("   ");
        Video::Write(info);
        Video::Write('\n');
    }

    // restore colors
    Video::SetColors(colors);
}
