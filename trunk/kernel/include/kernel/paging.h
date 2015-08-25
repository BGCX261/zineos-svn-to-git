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

#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#include "kernel/common.h"

#ifdef __cplusplus
extern "C" {
#endif


enum {
    MEMORY_16MB = 0x01000000,
    MEMORY_4GB  = 0xFFFFFFFF,
    MEMORY_4KB  = 0x1000,
    MEMORY_MAX  = MEMORY_4GB
};


#define PAGING_GET_PAGE_INDEX(va) (((va) >> 12) & 0x3FF)
#define PAGING_GET_DIR_INDEX(va) ((va) >> 22)

/**
 * Flags for each Page entries
 */
enum PageFlags
{
    PAGE_PRESENT        = BIT(0),
    PAGE_READWRITE      = BIT(1),
    PAGE_USERMODE       = BIT(2),
    PAGE_DISABLE_PWT    = BIT(3),
    PAGE_DISABLE_PCD    = BIT(4),
    PAGE_ACCESSED       = BIT(5),
    PAGE_DIRTY          = BIT(6),
    PAGE_DIR_SIZE       = BIT(7),
    PAGE_ENTRY_PAT      = BIT(7),
    PAGE_GLOBAL         = BIT(8)
};

/**
 * Flags for directory entries
 */
enum DirFlags
{
	DIR_PRESENT			= BIT(0),
	DIR_READWRITE		= BIT(1),
	DIR_USERMODE		= BIT(2),
	DIR_WRITE_THROUGH	= BIT(3),
	DIR_CACHE_DISABLED	= BIT(4),
	DIR_ACCESSED		= BIT(5),
	DIR_DIRTY			= BIT(6),
	DIR_LARGE_PAGE		= BIT(7),
	DIR_GLOBAL			= BIT(8),
	DIR_ATTRIBUTE_INDEX	= BIT(12)
};

/*
 * Init paging
 */
void paging_init();


#ifdef __cplusplus
};
#endif


#endif // PAGING_H_INCLUDED
