/********************************************************************************
*                                                                               *
*                        Assembler optimized operations                         *
*                                                                               *
*********************************************************************************/

#ifndef ASSEMBLER_OPTIMIZED_OPERATIONS_H
#define ASSEMBLER_OPTIMIZED_OPERATIONS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


#if defined(_MSC_VER) && ((defined(_M_IX86) && _M_IX86>=400) || (defined(_M_AMD64) || defined(_M_X64)))
// Get the intrinsic definitions
#if defined(_M_X64) || _M_IX86_FP>=1
#include "xmmintrin.h"	// For mm_prefetch
#endif
#if defined(_M_X64) || _M_IX86_FP>=2
#include "emmintrin.h"
#endif
#ifndef BitScanForward	// Try to avoid pulling in WinNT.h

#ifdef __cplusplus 
extern "C" unsigned char _BitScanForward(unsigned long *index, unsigned long mask);
extern "C" unsigned char _BitScanReverse(unsigned long *index, unsigned long mask);
#else
unsigned char _BitScanForward(unsigned long *index, unsigned long mask);
unsigned char _BitScanReverse(unsigned long *index, unsigned long mask);
#endif

#define BitScanForward _BitScanForward
#define BitScanReverse _BitScanReverse
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)

#if defined(_M_AMD64) || defined(_M_X64)

#ifdef __cplusplus 
extern "C" unsigned char _BitScanForward64(unsigned long *index, unsigned __int64 mask);
extern "C" unsigned char _BitScanReverse64(unsigned long *index, unsigned __int64 mask);
#else
unsigned char _BitScanForward64(unsigned long *index, unsigned long mask);
unsigned char _BitScanReverse64(unsigned long *index, unsigned long mask);
#endif

#define BitScanForward64 _BitScanForward64
#define BitScanReverse64 _BitScanReverse64
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#endif

#endif
#include <stdlib.h>		// For byteswap
//        unsigned short __cdecl _byteswap_ushort(unsigned short);
//        unsigned long  __cdecl _byteswap_ulong (unsigned long);
//        unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);
#pragma intrinsic(_byteswap_ushort)
#pragma intrinsic(_byteswap_ulong)
#pragma intrinsic(_byteswap_uint64)

/* One has a choice of increments: 32 for P6, 64 for Athlon and 128 for P4, so we choose 64 */
INLINE void myprefetchmemT(const void *ptr)
{
#if defined(_M_X64) || _M_IX86_FP>=1
	_mm_prefetch((const char *) ptr, _MM_HINT_T2);
#endif
}
INLINE void myprefetchmemNT(const void *ptr)
{
#if defined(_M_X64) || _M_IX86_FP>=1
	_mm_prefetch((const char *) ptr, _MM_HINT_NTA);
#endif
}

INLINE u32 _mybitscan(u32 x)
{
	u32 m;
#if defined(BitScanForward)
	unsigned long _m;
	BitScanForward(&_m, x);
	m=(unsigned int) _m;
#else
#error Unknown implementation
#endif
	return m;
}

INLINE u32 mybitscan(u32 x)
{
	return (0==x)?32:_mybitscan(x);
}

INLINE u32 _mybitscan64(u64 x)
{
	u32 m;
#if defined(BitScanForward64)
	unsigned long _m;
	BitScanForward64(&_m, x);
	m=(unsigned int) _m;
#else
	u32 *_x=(u32 *) &x;
	m=mybitscan(_x[0]);
	if(32==m) m=32+mybitscan(_x[1]);
#endif
	return m;
}

INLINE u32 mybitscan64(u64 x)
{
	return (0==x)?64:_mybitscan64(x);
}

INLINE u32 _mybitscanrev(u32 x)
{
	u32 m;
#if defined(BitScanReverse)
	unsigned long _m;
	BitScanReverse(&_m, x);
	m=(unsigned int) _m;
#else
#error Unknown implementation
#endif
	return m;
}

INLINE u32 mybitscanrev(u32 x)
{
	return (0==x)?32:_mybitscanrev(x);
}

INLINE u32 mybitscanrev_add1(u32 x)
{
	return (0==x)?0:(_mybitscanrev(x)+1);
}

INLINE u32 _mybitscanrev64(u64 x)
{
	u32 m;
#if defined(BitScanReverse64)
	unsigned long _m;
	BitScanReverse64(&_m, x);
	m=(unsigned int) _m;
#else
	u32 *_x=(u32 *) &x;
	m=32+mybitscanrev(_x[1]);
	if(64==m) { m=mybitscanrev(_x[0]); if(32==m) m=64; }
#endif
	return m;
}

INLINE u32 mybitscanrev64(u64 x)
{
	return (0==x)?64:_mybitscanrev64(x);
}

INLINE u32 mybitscanrev64_add1(u64 x)
{
	return (0==x)?0:(_mybitscanrev64(x)+1);
}

// INLINE void myendianswap16(u16 &v)
// {	// Can't improve on this
// 	v=((v & 0xff)<<8)|(v>>8);
// }
// INLINE void myendianswap(u32 &v)
// {
// 	v=_byteswap_ulong(v);			// Invokes bswap x86 instruction
// }
// INLINE void myendianswap64(u64 &v)
// {
// 	v=_byteswap_uint64(v);			// Invokes bswap x86 instruction
// }
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#if defined(__x86_64__) || defined(__SSE__)
#include "xmmintrin.h"
#endif
#if defined(__x86_64__) || defined(__SSE2__)
#include "emmintrin.h"
#endif

INLINE void myprefetchmemT(const void *ptr)
{
	__builtin_prefetch(ptr, 0, 3);
}
INLINE void myprefetchmemNT(const void *ptr)
{
	__builtin_prefetch(ptr, 0, 0);
}
INLINE u32 mybitscan(u32 x)
{
	return (u32) __builtin_ctz(x);
}
INLINE u32 mybitscan64(u64 x)
{
	return (u32) __builtin_ctzl(x);
}
INLINE u32 mybitscanrev(u32 x)
{
	return (u32) sizeof(x)*__CHAR_BIT__ - 1 - (unsigned) __builtin_clz(x);
}
INLINE u32 mybitscanrev64(u64 x)
{
	return (u32) sizeof(x)*__CHAR_BIT__ - 1 - (unsigned) __builtin_clzl(x);
}
INLINE void myendianswap(u16 &v)
{	// Can't improve on this
	v=((v & 0xff)<<8)|(v>>8);
}
INLINE void myendianswap(u32 &v)
{
	v=__builtin_bswap32(v);
}
INLINE void myendianswap64(u64 &v)
{
	v=__builtin_bswap64(v);
}
#else

/*! \ingroup myassemblerops
Pretches a cache line into the processor cache temporally (ie; it will
be used multiple times) */
INLINE void myprefetchmemT(const void *ptr)
{
}
/*! \ingroup myassemblerops
Pretches a cache line into the processor cache non-temporally (ie; it
will be used only once) */
INLINE void myprefetchmemNT(const void *ptr)
{
}
/*! \ingroup myassemblerops
Forward scans an unsigned integer, returning the index of the first
set bit. Compiles into 21 x86 cycles with no branching, though on
x86 and x64 it directly uses the bsl instruction */
INLINE u32 mybitscan(u32 x)
{
   x = ~x & (x - 1);
   x = x - ((x >> 1) & 0x55555555);
   x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
   x = (x + (x >> 4)) & 0x0F0F0F0F;
   x = x + (x << 8);
   x = x + (x << 16);
   return x >> 24;
}
INLINE u32 mybitscan64(u64 x)
{
	u32 m;
	union
	{
		u64 l;
		u32 i[2];
	} _x;
	_x.l=x;
	m=mybitscan(_x.i[!FOX_BIGENDIAN]);
	if(32==m) m=32+mybitscan(_x.i[!!FOX_BIGENDIAN]);
	return m;
}
/*! \ingroup myassemblerops
Backward scans an unsigned integer, returning the index of the
first set bit. Compiles into roughly 24 x86 cycles with no
branching, though on x86 and x64 it directly uses the bsr instruction.
You should note that this implementation uses illegal C++ which may
fail with aggressive enough optimisation - in this situation, enable
the alternative 36 x86 cycle implementation in the source code.
*/
INLINE u32 mybitscanrev(u32 x)
{
#if 1
	union {
		unsigned asInt[2];
		double asDouble;
	};
	int n;

	asDouble = (double)x + 0.5;
	n = (asInt[!FOX_BIGENDIAN] >> 20) - 1023;
	return n;
#else
	const u32 allbits1=~(u32)0;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
	x = ~x;
	x = x - ((x >> 1) & (allbits1/3));
	x = (x & (allbits1/15*3)) + ((x >> 2) & (allbits1/15*3));
	x = ((x + (x >> 4)) & (allbits1/255*15)) * (allbits1/255);
	x = (8*sizeof(x)-1) - (x >> (8*(sizeof(x)-1)));
	return (unsigned) x;
#endif
}
INLINE u32 mybitscanrev64(u64 x)
{
  const u64 allbits1=~(u64)0;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >>16);
  x = x | (x >>32);
	x = ~x;
  x = x - ((x >> 1) & (allbits1/3));
  x = (x & (allbits1/15*3)) + ((x >> 2) & (allbits1/15*3));
  x = ((x + (x >> 4)) & (allbits1/255*15)) * (allbits1/255);
  x = (8*sizeof(x)-1) - (x >> (8*(sizeof(x)-1)));
  return (unsigned) x;
}

/*! \ingroup myassemblerops
Endian swaps the two bytes at \em v
*/
INLINE void myendianswap16(u16 &v)
{	// Can't improve on this
	v=((v & 0xff)<<8)|(v>>8);
}

/*! \ingroup myassemblerops
Endian swaps the four bytes at \em v
*/
INLINE void myendianswap(u32 &v)
{
	u8 *p=(u8 *) &v, t;
	t=p[0]; p[0]=p[3]; p[3]=t;
	t=p[1]; p[1]=p[2]; p[2]=t;
}

/*! \ingroup myassemblerops
Endian swaps the eight bytes at \em v
*/
INLINE void myendianswap64(u64 &v)
{
	u8 *p=(u8 *) &v, t;
	t=p[0]; p[0]=p[7]; p[7]=t;
	t=p[1]; p[1]=p[6]; p[6]=t;
	t=p[2]; p[2]=p[5]; p[5]=t;
	t=p[3]; p[3]=p[4]; p[4]=t;
}
#endif

#ifdef __cplusplus
}
#endif

#endif // ASSEMBLER_OPTIMIZED_OPERATIONS_H
