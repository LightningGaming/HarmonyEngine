/**************************************************************************
 *	HyPlatform.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyPlatform_h__
#define __HyPlatform_h__

#include "Afx/Platforms/HyPlatAfx.h"

#if defined(HY_PLATFORM_32BIT)	// 32 bit environment
	typedef int32 intx;
	typedef uint32 uintx;
#elif defined(HY_PLATFORM_64BIT) // 64 bit environment
	typedef int64 intx;
	typedef uint64 uintx;
#endif

//#ifndef HARMONY_STATICASSERT_DEFINED
//	/* Force a compilation error if condition is true */
//	#define HyAssertCompilation(condition) ((void)HY_BUILD_BUG_ON_ZERO(condition))
//	/*	Force a compilation error if condition is true, but also produce a
//		138    result (of value 0 and type size_t), so the expression can be used
//		139    e.g. in a structure initializer (or where-ever else comma expressions
//		140    aren't permitted). */
//	#define HY_BUILD_BUG_ON_ZERO(e) (sizeof(char[1 - 2*!!(e)]))
//	//#define HY_BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))
//
//	#define HARMONY_STATICASSERT_DEFINED
//#endif

//-----------------------------------------------------------------------------------------
// Generic cross-platform typedefs
//-----------------------------------------------------------------------------------------

// Other basic types
typedef unsigned char	tbyte;
typedef uint32			tbool;

typedef size_t			tMEMSIZE;
typedef size_t			tDISC_SIZE;

typedef void *			PVOID;

//-----------------------------------------------------------------------------------------
// Endian - Don't use these functions directly, use the #define's below 'em
//-----------------------------------------------------------------------------------------
HY_INLINE uint16 EndianSwap16(uint16 var)
{
	return (uint16)((((var)&0xff)<<8) | ( ((var)>>8)&0xff));
}

HY_INLINE uint32 EndianSwap32(uint32 var)
{
	return ((((var)&0xff)<<24) | ((((var)>>8)&0xff)<<16) |
		((((var)>>16)&0xff)<<8) | ((((var)>>24)&0xff)) );
}

#if HY_ENDIAN_LITTLE == 1
	#define EndianToBig16(_v)          EndianSwap16(_v)
	#define EndianToLittle16(_v)
	#define EndianToBig32(_v)          EndianSwap32(_v)
	#define EndianToLittle32(_v)
#elif HY_ENDIAN_BIG == 1
	#define EndianToBig16(_v)
	#define EndianToLittle16(_v)       EndianSwap16(_v)
	#define EndianToBig32(_v)
	#define EndianToLittle32(_v)       EndianSwap32(_v)
#else
	#error No Endian is set.
#endif

//-----------------------------------------------------------------------------------------
// Alignment
//-----------------------------------------------------------------------------------------
#define sgxAlign(_v, _align)   (((tMEMSIZE)(_v)+(_align)-1 ) & \
	(~((_align)-1)))
#define sgxAlignUp(_v, _align) (sgxAlign((_v), (_align)) + (_align))

#define sgxAlign4(_v)      sgxAlign(_v,4)
#define sgxAlign16(_v)     sgxAlign(_v,16)
#define sgxAlign32(_v)     sgxAlign(_v,32)
#define sgxAlign128(_v)    sgxAlign(_v,128)

#define sgxIsAligned(_v, _align) (((tMEMSIZE)(_v)&((_align)-1)) == 0)

//-----------------------------------------------------------------------------------------
// Misc
//-----------------------------------------------------------------------------------------

#define HY_TCP_PORT 1313

// Some simple constants to replace the C++ types of 
// true and false.
// Most platforms define these too, so use ifndef 
// around each of them,
#ifdef TRUE
	#undef TRUE
#endif
#define TRUE	1

#ifdef FALSE
	#undef FALSE
#endif
#define FALSE	0

#ifdef NULL
	#undef NULL
#endif
#define NULL	0

// The size of an enum can vary, so add this element to the end
// of any enumeration we use that's intended for saving to disc
#define HY_ENUM_PADDING	0x7fffffff

//-----------------------------------------------------------------------------------------
// Safety checks
//-----------------------------------------------------------------------------------------
#if (defined(HY_PLATFORM_32BIT) && defined(HY_PLATFORM_64BIT)) || (!defined(HY_PLATFORM_32BIT) && !defined(HY_PLATFORM_64BIT))
#error HyPlatform.h: Invalid Architecture definition.
#endif

#if defined(HY_PLATFORM_UNKNOWN)
#error HyPlatform.h: Could not determine build platform.
#endif

#ifndef HyAssert
#error HyPlatform.h: HyAssert is not defined!
#endif

#ifndef HyError
#error HyPlatform.h: HyError is not defined!
#endif

#endif // __HyPlatform_h__
