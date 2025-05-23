/**************************************************************************
 *	HyCompiler.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyCompiler_h__
#define HyCompiler_h__

 // SPINE RUNTIME ////////////////////////////////////////////////////////////
#ifdef HY_USE_SPINE
	#define SPINE_USE_STD_FUNCTION
	#include "spine/spine.h"
#endif
//////////////////////////////////////////////////////////////////////////////

 // Data types
typedef signed char			int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed int			int32;		// TODO: Make this 'signed long' - Box2d already declares a typedef int32 (incorrectly?) as signed int
typedef unsigned int		uint32;		// TODO: Make this 'unsigned long' - Box2d already declares a typedef int32 (incorrectly?) as unsigned int
typedef long long			int64;
typedef unsigned long long	uint64;

// These macros use the (compiler-specific) project settings
// to create independent versions for the game.
#if !defined(HARMONY_COMPILER_DEFINED)

	//#if defined(UNICODE)
	//	#define HY_UNICODE
	//	typedef std::wstring HyString;
	//	typedef std::wostringstream HyStringStream;
	//	typedef wchar_t HyChar;
	//	#define HyT(x) L ## x
	//#else
	//	#define HY_ASCII
	//	typedef std::string HyString;
	//	typedef std::ostringstream HyStringStream;
	//	typedef char HyChar;
	//	#define HyT(x) x
	//#endif

	#if defined(_MSC_VER)
		#define HY_COMPILER_MSVC
		#include "HyCompiler_MSVC.h"
	#elif defined(__MWERKS__)
		#define HY_COMPILER_MWERKS
		#include "HyCompiler_MWerks.h"
	#elif defined(__GNUC__) || defined(__clang__)
		#define HY_COMPILER_GNU
		#include "HyCompiler_GNU.h"
	#else
		#define HY_COMPILER_UNKNOWN
	#endif

	#define HARMONY_COMPILER_DEFINED
#endif

//
// Add any generic macros the compiler doesn't support
//

// For manually aligning data
#ifndef HY_PACKDATA_BEGIN
  #define HY_PACKDATA_BEGIN(align)
#endif

#ifndef HY_PACKDATA_END
  #define HY_PACKDATA_END(align)
#endif

// Compiler support
#ifndef HY_INLINE
  #define HY_INLINE        inline
#endif

#ifndef HY_FORCE_INLINE
  #define HY_FORCE_INLINE  inline
#endif

//-----------------------------------------------------------------------------------------
// Safety checks
//-----------------------------------------------------------------------------------------
#if (defined(HY_DEBUG) && defined(HY_RELEASE)) || (!defined(HY_DEBUG) && !defined(HY_RELEASE))
	#error HyCompiler.h: Invalid Build Configuration.
#endif

#if (defined(HY_ENV_32) && defined(HY_ENV_64)) || (!defined(HY_ENV_32) && !defined(HY_ENV_64))
	#error HyCompiler.h: Invalid Platform Environment.
#endif

#if defined(HY_COMPILER_UNKNOWN) || (!defined(HARMONY_COMPILER_DEFINED))
	#error HyCompiler.h: Could not determine the compiler.
#endif

#ifndef HY_NOEXCEPT
	#error HyCompiler.h: HY_NOEXCEPT was not defined
#endif

#endif /* HyCompiler_h__ */
