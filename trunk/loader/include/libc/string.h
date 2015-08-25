/*
 * string.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Some functions within this file are from Linux Kernel source code
 * include/asm-x86/string_32.h
 * memset and related macros and optimized versions
 * memcpy and related macros and optimized versions
 */

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include "kernel/common.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Copy memory block from one address to another.
 *
 * Use for non constant size
 *
 * This function is taken from Linux kernel source
 * include/asm-x86/string_32.h
 */
INLINE_FUNCTION(void * __memcpy(void * to, const void * from, size_t n))
{
    int d0, d1, d2;
    __asm__ __volatile__(
        "rep ; movsl\n\t"
        "movl %4,%%ecx\n\t"
        "andl $3,%%ecx\n\t"
        "jz 1f\n\t"
        "rep ; movsb\n\t"
        "1:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        : "0" (n/4), "g" (n), "1" ((long) to), "2" ((long) from)
        : "memory");
    return (to);
}


/**
 * Copy memory block from one address to another.
 *
 * Use for constant size
 */
INLINE_FUNCTION(void * __constant_memcpy (void * to, const void * from, size_t n))
{
	long esi, edi;
	if (!n) return to;
	switch (n) {
		case 1: *(char*)to = *(char*)from; return to;
		case 2: *(short*)to = *(short*)from; return to;
		case 4: *(int*)to = *(int*)from; return to;
		case 3: *(short*)to = *(short*)from;
			*((char*)to+2) = *((char*)from+2); return to;
		case 5: *(int*)to = *(int*)from;
			*((char*)to+4) = *((char*)from+4); return to;
		case 6: *(int*)to = *(int*)from;
			*((short*)to+2) = *((short*)from+2); return to;
		case 8: *(int*)to = *(int*)from;
			*((int*)to+1) = *((int*)from+1); return to;
	}
	esi = (long) from;
	edi = (long) to;
	if (n >= 5*4) {
		/* large block: use rep prefix */
		int ecx;
		__asm__ __volatile__(
			"rep ; movsl"
			: "=&c" (ecx), "=&D" (edi), "=&S" (esi)
			: "0" (n/4), "1" (edi),"2" (esi)
			: "memory"
		);
	} else {
		/* small block: don't clobber ecx + smaller code */
		if (n >= 4*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 3*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 2*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
		if (n >= 1*4) __asm__ __volatile__("movsl"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
	}
	switch (n % 4) {
		/* tail */
		case 0: return to;
		case 1: __asm__ __volatile__("movsb"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
		case 2: __asm__ __volatile__("movsw"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
		default: __asm__ __volatile__("movsw\n\tmovsb"
			:"=&D"(edi),"=&S"(esi):"0"(edi),"1"(esi):"memory");
			return to;
	}
}
#define memcpy(t, f, n) \
    (__builtin_constant_p(n) ? \
        __constant_memcpy((t),(f),(n)) : \
        __memcpy((t),(f),(n)) \
    )



/**
 * Generic memset
 */
INLINE_FUNCTION(void * __memset_generic(void * s, char c,size_t count))
{
    int d0, d1;
    __asm__ __volatile__(
        "rep\n\t"
        "stosb"
        : "=&c" (d0), "=&D" (d1)
        :"a" (c),"1" (s),"0" (count)
        :"memory");
    return s;
}

#define __constant_count_memset(s,c,count) __memset_generic((s),(c),(count))

/*
 * memset(x,0,y) is a reasonably common thing to do, so we want to fill
 * things 32 bits at a time even when we don't know the size of the
 * area at compile-time..
 */
INLINE_FUNCTION(void * __constant_c_memset(void * s, unsigned long c, size_t count))
{
    int d0, d1;
    __asm__ __volatile__(
        "rep ; stosl\n\t"
        "testb $2,%b3\n\t"
        "je 1f\n\t"
        "stosw\n"
        "1:\ttestb $1,%b3\n\t"
        "je 2f\n\t"
        "stosb\n"
        "2:"
        :"=&c" (d0), "=&D" (d1)
        :"a" (c), "q" (count), "0" (count/4), "1" ((long) s)
        :"memory");
    return (s);
}

/*
 * This looks horribly ugly, but the compiler can optimize it totally,
 * as we by now know that both pattern and count is constant..
 */
INLINE_FUNCTION(void * __constant_c_and_count_memset(void * s, unsigned long pattern, size_t count))
{
	switch (count) {
		case 0:
			return s;
		case 1:
			*(unsigned char *)s = pattern & 0xff;
			return s;
		case 2:
			*(unsigned short *)s = pattern & 0xffff;
			return s;
		case 3:
			*(unsigned short *)s = pattern & 0xffff;
			*(2+(unsigned char *)s) = pattern & 0xff;
			return s;
		case 4:
			*(unsigned long *)s = pattern;
			return s;
	}
    #define COMMON(x) \
    __asm__  __volatile__( \
        "rep ; stosl" \
        x \
        : "=&c" (d0), "=&D" (d1) \
        : "a" (pattern),"0" (count/4),"1" ((long) s) \
        : "memory")
    {
        int d0, d1;
        switch (count % 4) {
            case 0: COMMON(""); return s;
            case 1: COMMON("\n\tstosb"); return s;
            case 2: COMMON("\n\tstosw"); return s;
            default: COMMON("\n\tstosw\n\tstosb"); return s;
        }
    }
    #undef COMMON
}

#define __constant_c_x_memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_c_and_count_memset((s),(c),(count)) : \
 __constant_c_memset((s),(c),(count)))

#define __memset(s, c, count) \
(__builtin_constant_p(count) ? \
 __constant_count_memset((s),(c),(count)) : \
 __memset_generic((s),(c),(count)))

#define __HAVE_ARCH_MEMSET
#define memset(s, c, count) \
(__builtin_constant_p(c) ? \
 __constant_c_x_memset((s),(0x01010101UL*(unsigned char)(c)),(count)) : \
 __memset((s),(c),(count)))


/**
 * Fille region with 16 bit
 */
INLINE_FUNCTION(void * memsetw(void * dest, short val, size_t count))
{
    short * temp = (short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}


/**
 * Fille region with 32 bit
 */
INLINE_FUNCTION(void * memsetdw(void * dest, int val, size_t count))
{
    int * temp = (int *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}



/**
 * Get string size
 */
INLINE_FUNCTION(size_t strlen(const char *str))
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}


/**
 * Compare two spans of memory to each other
 */
INLINE_FUNCTION(size_t memcmp( const void *pMem1, const void *pMem2, size_t len))
{
    uint8_t *p1 = (uint8_t*)pMem1;
    uint8_t *p2 = (uint8_t*)pMem2;
    size_t  val;
    while (len--)
    {
        val = *p1-*p2;
        //We  want val to be zero, if not we return
        if (val)
            return val; //if p1 > p2, positive, otherwise negative!
        ++p1;
        ++p2;
    }
    return 0;
}


/**
 * Compare two strings together
*/
INLINE_FUNCTION(size_t strcmp( const char *str1, const char *str2))
{
    while (*str1 && *str2)
    {
        if (*str1 != *str2)
            return 1;
        ++str1;
        ++str2;
    }

    if (*str1 == *str2) //Both hit NULL at the same time?!
        return  0;      //Cool, they match
    return 1;
}


/**
 * Move memory
 */
INLINE_FUNCTION(void * memmove (void * dest, const void * source, size_t length))
{
    uint8_t * d = (uint8_t *)dest, * s = (uint8_t *)source;

    // Moving from low mem to hi mem; start at end.
    if (s < d)
    {
        for (s += length, d += length; length; --length)
        {
            *--d = *--s;
        }
    }
    // Moving from hi mem to low mem; start at beginning.
    else if (s != d)
    {
        for (; length; --length)
        {
            *d++ = *s++;
        }
    }

    return dest;
}


#ifdef __cplusplus
};
#endif

#endif // STRING_H_INCLUDED
