/* /////////////////////////////////////////////////////////////////////////
 * File:        stlsoft/shims/attribute/get_ptr/std/auto_ptr.hpp
 *
 * Purpose:     Contains the get_ptr access shim.
 *
 * Created:     10th January 2002
 * Updated:     19th February 2017
 *
 * Thanks to:   Nevin Liber for spotting a mistake in the get_ptr
 *              definition.
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


/** \file stlsoft/shims/attribute/get_ptr/std/auto_ptr.hpp
 *
 * \brief [C++] Pointer attribute shims for std::auto_ptr
 *   (\ref group__concept__Shim__Attribute__get_ptr "get_ptr Pointer Attribute Shim").
 */

#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR
#define STLSOFT_INCL_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define STLSOFT_VER_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR_MAJOR     4
# define STLSOFT_VER_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR_MINOR     1
# define STLSOFT_VER_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR_REVISION  6
# define STLSOFT_VER_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR_EDIT      56
#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */
#ifdef STLSOFT_TRACE_INCLUDE
# pragma message(__FILE__)
#endif /* STLSOFT_TRACE_INCLUDE */

#if defined(STLSOFT_COMPILER_IS_WATCOM)
# define _STLSOFT_PTR_ACCESS_NO_AUTO_PTR
#endif /* compiler */

#ifndef _STLSOFT_PTR_ACCESS_NO_AUTO_PTR
# include <memory>                  // for std::auto_ptr
#endif /* _STLSOFT_PTR_ACCESS_NO_AUTO_PTR */

/* /////////////////////////////////////////////////////////////////////////
 * namespace
 */

#ifndef STLSOFT_NO_NAMESPACE
namespace stlsoft
{
#endif /* STLSOFT_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * get_ptr functions
 */

#ifndef _STLSOFT_PTR_ACCESS_NO_AUTO_PTR

/* std::auto_ptr */
/** 
 * \ingroup group__concept__Shim__Attribute__get_ptr
 */
template <ss_typename_param_k T>
inline T *get_ptr(STLSOFT_NS_QUAL_STD(auto_ptr)<T> const& p)
{
    return p.get();
}

#endif /* _STLSOFT_PTR_ACCESS_NO_AUTO_PTR */

/* ////////////////////////////////////////////////////////////////////// */

#ifndef STLSOFT_NO_NAMESPACE
} /* namespace stlsoft */
#endif /* STLSOFT_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * inclusion control
 */

#ifdef STLSOFT_CF_PRAGMA_ONCE_SUPPORT
# pragma once
#endif /* STLSOFT_CF_PRAGMA_ONCE_SUPPORT */

#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ATTRIBUTE_GET_PTR_STD_HPP_AUTO_PTR */

/* ///////////////////////////// end of file //////////////////////////// */
