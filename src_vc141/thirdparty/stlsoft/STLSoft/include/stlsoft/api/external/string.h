/* /////////////////////////////////////////////////////////////////////////
 * File:        stlsoft/api/external/string.h
 *
 * Purpose:     External preprocessor aliases for string functions.
 *
 * Created:     15th November 2002
 * Updated:     23rd March 2017
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 2002-2017, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/* WARNING: this file contains undocumented internal features that are
 * subject to change at any time, so if you use them it is at your own risk.
 */

#ifndef STLSOFT_INCL_STLSOFT_API_external_h_string
#define STLSOFT_INCL_STLSOFT_API_external_h_string

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */
#ifdef STLSOFT_TRACE_INCLUDE
# pragma message(__FILE__)
#endif /* STLSOFT_TRACE_INCLUDE */

#ifndef STLSOFT_INCL_H_STRING
# define STLSOFT_INCL_H_STRING
# include <string.h>
#endif /* !STLSOFT_INCL_H_STRING */

/* /////////////////////////////////////////////////////////////////////////
 * String Functions
 */

/*

 * from winstl/system/system_traits.hpp

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# if !defined(__STDC__)
#  define WINSTL_SYSTEM_TRAITS_HAS_strnicmp_
# endif
# if !defined(__MFC_COMPAT__)
#  define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
# endif
#elif defined(STLSOFT_COMPILER_IS_DMC)
# define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
#elif defined(STLSOFT_COMPILER_IS_GCC)
# if !defined(__STRICT_ANSI__)
#  define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
# endif
#elif defined(STLSOFT_COMPILER_IS_INTEL) || \
      defined(STLSOFT_COMPILER_IS_MSVC)
# if !defined(__STDC__) && \
     !defined(STLSOFT_USING_SAFE_STR_FUNCTIONS)
#  define WINSTL_SYSTEM_TRAITS_HAS_strnicmp_
# endif
# define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
#elif defined(STLSOFT_COMPILER_IS_MWERKS)
# define WINSTL_SYSTEM_TRAITS_HAS_strnicmp_
# define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
#elif defined(STLSOFT_COMPILER_IS_WATCOM)
# define WINSTL_SYSTEM_TRAITS_HAS__strnicmp_
#endif



#if defined(STLSOFT_COMPILER_IS_BORLAND)
# if !defined(__STDC__)
#  define WINSTL_SYSTEM_TRAITS_HAS_wcsnicmp_
# endif
# if !defined(__MFC_COMPAT__)
#  define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
# endif
#elif defined(STLSOFT_COMPILER_IS_DMC)
# define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
#elif defined(STLSOFT_COMPILER_IS_GCC)
# if !defined(__STRICT_ANSI__)
#  define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
# endif
#elif defined(STLSOFT_COMPILER_IS_INTEL) || \
      defined(STLSOFT_COMPILER_IS_MSVC)
# if !defined(__STDC__)
#  define WINSTL_SYSTEM_TRAITS_HAS_wcsnicmp_
# endif
# define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
#elif defined(STLSOFT_COMPILER_IS_MWERKS)
# define WINSTL_SYSTEM_TRAITS_HAS_wcsnicmp_
# define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
#elif defined(STLSOFT_COMPILER_IS_WATCOM)
# define WINSTL_SYSTEM_TRAITS_HAS__wcsnicmp_
#endif

 */


/* *************************************
 * stricmp
 */

#ifndef STLSOFT_API_EXTERNAL_string_stricmp
# if 0

  /* Borland */
# elif defined(STLSOFT_COMPILER_IS_BORLAND)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(stricmp)
#  elif !defined(__MFC_COMPAT__)
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(_stricmp)
#  endif

  /* Digital Mars */
# elif defined(STLSOFT_COMPILER_IS_DMC)

#  define STLSOFT_API_EXTERNAL_string_stricmp                               STLSOFT_NS_GLOBAL(stricmp)

  /* GCC */
# elif defined(STLSOFT_COMPILER_IS_GCC)

#  if 0
#  elif !defined(__STRICT_ANSI__)
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(_stricmp)
#  endif

  /* Intel */
# elif defined(STLSOFT_COMPILER_IS_INTEL) && \
       defined(_WIN32)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(stricmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(_stricmp)
#  endif

  /* Microsoft Visual C++ */
# elif defined(STLSOFT_COMPILER_IS_MSVC)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(stricmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_stricmp                              STLSOFT_NS_GLOBAL(_stricmp)
#  endif

  /* Metrowerks CodeWarrior */
# elif defined(STLSOFT_COMPILER_IS_MWERKS)

#  define STLSOFT_API_EXTERNAL_string_stricmp                               STLSOFT_NS_GLOBAL(stricmp)

  /* Watcom */
# elif defined(STLSOFT_COMPILER_IS_WATCOM)

#  define STLSOFT_API_EXTERNAL_string_stricmp                               STLSOFT_NS_GLOBAL(_stricmp)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_stricmp */

#ifndef STLSOFT_API_EXTERNAL_string_stricmp
# ifdef _WIN32
#  define STLSOFT_API_EXTERNAL_string_stricmp                               STLSOFT_NS_GLOBAL(lstrcmpiA)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_stricmp */


/* *************************************
 * wcsicmp
 */

#ifndef STLSOFT_API_EXTERNAL_string_wcsicmp
# if 0

  /* Borland */
# elif defined(STLSOFT_COMPILER_IS_BORLAND)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(wcsicmp)
#  elif !defined(__MFC_COMPAT__)
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(_wcsicmp)
#  endif

  /* Digital Mars */
# elif defined(STLSOFT_COMPILER_IS_DMC)

#  define STLSOFT_API_EXTERNAL_string_wcsicmp                               STLSOFT_NS_GLOBAL(wcsicmp)

  /* GCC */
# elif defined(STLSOFT_COMPILER_IS_GCC)

#  if 0
#  elif !defined(__STRICT_ANSI__)
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(_wcsicmp)
#  endif

  /* Intel */
# elif defined(STLSOFT_COMPILER_IS_INTEL) && \
       defined(_WIN32)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(wcsicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(_wcsicmp)
#  endif

  /* Microsoft Visual C++ */
# elif defined(STLSOFT_COMPILER_IS_MSVC)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(wcsicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_wcsicmp                              STLSOFT_NS_GLOBAL(_wcsicmp)
#  endif

  /* Metrowerks CodeWarrior */
# elif defined(STLSOFT_COMPILER_IS_MWERKS)

#  define STLSOFT_API_EXTERNAL_string_wcsicmp                               STLSOFT_NS_GLOBAL(wcsicmp)

  /* Watcom */
# elif defined(STLSOFT_COMPILER_IS_WATCOM)

#  define STLSOFT_API_EXTERNAL_string_wcsicmp                               STLSOFT_NS_GLOBAL(_wcsicmp)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_wcsicmp */

#ifndef STLSOFT_API_EXTERNAL_string_wcsicmp
# ifdef _WIN32
#  define STLSOFT_API_EXTERNAL_string_wcsicmp                               STLSOFT_NS_GLOBAL(lstrcmpiW)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_wcsicmp */


/* *************************************
 * strnicmp
 */

#ifndef STLSOFT_API_EXTERNAL_string_strnicmp
# if 0

  /* Borland */
# elif defined(STLSOFT_COMPILER_IS_BORLAND)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(strnicmp)
#  elif !defined(__MFC_COMPAT__)
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(_strnicmp)
#  endif

  /* Digital Mars */
# elif defined(STLSOFT_COMPILER_IS_DMC)

#  define STLSOFT_API_EXTERNAL_string_strnicmp                              STLSOFT_NS_GLOBAL(strnicmp)

  /* GCC */
# elif defined(STLSOFT_COMPILER_IS_GCC)

#  if 0
#  elif !defined(__STRICT_ANSI__)
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(_strnicmp)
#  endif

  /* Intel */
# elif defined(STLSOFT_COMPILER_IS_INTEL) && \
       defined(_WIN32)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(strnicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(_strnicmp)
#  endif

  /* Microsoft Visual C++ */
# elif defined(STLSOFT_COMPILER_IS_MSVC)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(strnicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_strnicmp                             STLSOFT_NS_GLOBAL(_strnicmp)
#  endif

  /* Metrowerks CodeWarrior */
# elif defined(STLSOFT_COMPILER_IS_MWERKS)

#  define STLSOFT_API_EXTERNAL_string_strnicmp                              STLSOFT_NS_GLOBAL(strnicmp)

  /* Watcom */
# elif defined(STLSOFT_COMPILER_IS_WATCOM)

#  define STLSOFT_API_EXTERNAL_string_strnicmp                              STLSOFT_NS_GLOBAL(_strnicmp)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_strnicmp */


/* *************************************
 * wcsnicmp
 */

#ifndef STLSOFT_API_EXTERNAL_string_wcsnicmp
# if 0

  /* Borland */
# elif defined(STLSOFT_COMPILER_IS_BORLAND)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(wcsnicmp)
#  elif !defined(__MFC_COMPAT__)
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(_wcsnicmp)
#  endif

  /* Digital Mars */
# elif defined(STLSOFT_COMPILER_IS_DMC)

#  define STLSOFT_API_EXTERNAL_string_wcsnicmp                              STLSOFT_NS_GLOBAL(wcsnicmp)

  /* GCC */
# elif defined(STLSOFT_COMPILER_IS_GCC)

#  if 0
#  elif !defined(__STRICT_ANSI__)
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(_wcsnicmp)
#  endif

  /* Intel */
# elif defined(STLSOFT_COMPILER_IS_INTEL) && \
       defined(_WIN32)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(wcsnicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(_wcsnicmp)
#  endif

  /* Microsoft Visual C++ */
# elif defined(STLSOFT_COMPILER_IS_MSVC)

#  if 0
#  elif !defined(__STDC__)
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(wcsnicmp)
#  else
#   define STLSOFT_API_EXTERNAL_string_wcsnicmp                             STLSOFT_NS_GLOBAL(_wcsnicmp)
#  endif

  /* Metrowerks CodeWarrior */
# elif defined(STLSOFT_COMPILER_IS_MWERKS)

#  define STLSOFT_API_EXTERNAL_string_wcsnicmp                              STLSOFT_NS_GLOBAL(wcsnicmp)

  /* Watcom */
# elif defined(STLSOFT_COMPILER_IS_WATCOM)

#  define STLSOFT_API_EXTERNAL_string_wcsnicmp                              STLSOFT_NS_GLOBAL(_wcsnicmp)
# endif
#endif /* !STLSOFT_API_EXTERNAL_string_wcsnicmp */


/* /////////////////////////////////////////////////////////////////////////
 * inclusion control
 */

#ifdef STLSOFT_CF_PRAGMA_ONCE_SUPPORT
# pragma once
#endif /* STLSOFT_CF_PRAGMA_ONCE_SUPPORT */

#endif /* !STLSOFT_INCL_STLSOFT_API_external_h_string */

/* ///////////////////////////// end of file //////////////////////////// */
