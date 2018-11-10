/* /////////////////////////////////////////////////////////////////////////
 * File:    pantheios/implicit_link/bel.syslog.h
 *
 * Purpose: Implicitly links in the Pantheios syslog Local Back-End Library
 *
 * Created: 30th May 2006
 * Updated: 29th June 2016
 *
 * Home:    http://pantheios.org/
 *
 * Copyright (c) 2006-2016, Matthew Wilson and Synesis Software
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


/** \file pantheios/implicit_link/bel.syslog.h
 *
 * [C, C++] Implicitly links in the
 *   \ref group__backend__stock_backends__syslog "Pantheios syslog Local Back-End Library"
 *   as the local back-end for the given link-unit.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG
#define PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG

/* /////////////////////////////////////////////////////////////////////////
 * version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG_MAJOR    1
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG_MINOR    0
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG_REVISION 2
# define PANTHEIOS_VER_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG_EDIT     6
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_
# include <pantheios/implicit_link/implicit_link_base_.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_IMPLICIT_LINK_BASE_ */
#ifndef PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEC_SYSLOG
# include <pantheios/implicit_link/bec.syslog.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEC_SYSLOG */

/* /////////////////////////////////////////////////////////////////////////
 * implicit-linking directives
 */

#ifdef PANTHEIOS_IMPLICIT_LINK_SUPPORT

# if defined(__BORLANDC__)
# elif defined(__COMO__)
# elif defined(__DMC__)
# elif defined(__GNUC__)
# elif defined(__INTEL_COMPILER)

#  pragma comment(lib, PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("bel.syslog"))
#  pragma message("     " PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("bel.syslog"))

# elif defined(__MWERKS__)
# elif defined(__WATCOMC__)
# elif defined(_MSC_VER)

#  pragma comment(lib, PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("bel.syslog"))
#  pragma message("     " PANTHEIOS_IMPL_LINK_LIBRARY_NAME_("bel.syslog"))

# else /* ? compiler */
#  error Compiler not recognised
# endif /* compiler */

#endif /* PANTHEIOS_IMPLICIT_LINK_SUPPORT */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_IMPLICIT_LINK_H_BEL_SYSLOG */

/* ///////////////////////////// end of file //////////////////////////// */
