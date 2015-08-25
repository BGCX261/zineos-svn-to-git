/*
 * This file is part of ZineOS Kernel
 *
 * ZineOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZineOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author : Albert Varaksin <albeva@hotmail.co.uk>
 * Copyright (C) The ZineOS development team
 * More info: http://nevasca.fbdevzone.com/
 */

#ifndef GDT_H_INCLUDED
#define GDT_H_INCLUDED

#include <kernel/common.h>

#ifdef __cplusplus
extern "C" {
#endif


void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();


/*
 * Defines a GDT table entry
 */
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));


/*
 * pointer to gdt table
 */
struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));


/*
 * Flush the GDT
 */
void gdt_load (struct gdt_ptr *);


#ifdef __cplusplus
};
#endif

#endif // GDT_H_INCLUDED
