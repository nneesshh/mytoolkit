/* /////////////////////////////////////////////////////////////////////////
 * File:        stlsoft/util/true_typedef.hpp
 *
 * Purpose:     Contains the true_typedef class template.
 *
 * Created:     16th January 2002
 * Updated:     19th February 2017
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


/** \file stlsoft/util/true_typedef.hpp
 *
 * \brief [C++ only] Definition of the stlsoft::true_typedef class template
 *   (\ref group__library__Utility "Utility" Library).
 */

#ifndef STLSOFT_INCL_STLSOFT_UTIL_HPP_TRUE_TYPEDEF
#define STLSOFT_INCL_STLSOFT_UTIL_HPP_TRUE_TYPEDEF

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION
# define STLSOFT_VER_STLSOFT_UTIL_HPP_TRUE_TYPEDEF_MAJOR     4
# define STLSOFT_VER_STLSOFT_UTIL_HPP_TRUE_TYPEDEF_MINOR     0
# define STLSOFT_VER_STLSOFT_UTIL_HPP_TRUE_TYPEDEF_REVISION  6
# define STLSOFT_VER_STLSOFT_UTIL_HPP_TRUE_TYPEDEF_EDIT      84
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

/* /////////////////////////////////////////////////////////////////////////
 * namespace
 */

#ifndef STLSOFT_NO_NAMESPACE
namespace stlsoft
{
#endif /* STLSOFT_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * classes
 */

/* U is the unique type, and is usually a type generated by the
 * STLSOFT_GEN_OPAQUE() macro, as in
 *
 *  STLSOFT_GEN_OPAQUE(h_uniquelong_tt)
 *  typedef true_typedef<long, h_uniquelong_tt>     uniquelong_tt;
 *
 * thus generating a unique type uniquelong over base-type long.
 */

/** This class is used to create strong types from base types, to facilitate
 * type disambiguation, overloading and mutual incompatibility.
 *
 * \ingroup group__library__Utility
 *
 * For example, the following code creates two types based on the \c int type:
 *
\code
  STLSOFT_GEN_OPAQUE(h_protocol_tt)
  STLSOFT_GEN_OPAQUE(h_port_tt)
  typedef true_typedef<long, h_protocol_tt>  protocol_tt;
  typedef true_typedef<long, h_port_tt>      port_tt;

  protocol_tt pro1(3);    // Ok
  port_tt     prt1(8088); // Ok
  protocol_tt pro2(pro1); // Ok, can copy from another protocol_tt
  port_tt     prt2(pro2); // Error, cannot copy from a protocol_tt
\endcode
 *
 *
 * \param T The base type, e.g. \c int, \c std::wstring
 * \param U The unique type, usually created by using STLSOFT_GEN_OPAQUE()
 */

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
class true_typedef
{
public:
    /// The value type
    typedef T                   value_type;
    /// The unique type
    typedef U                   unique_type;
    /// The type of the current parameterisation
    typedef true_typedef<T, U>  class_type;
    /// The reference type
    typedef T&                  reference;
    /// The non-mutating (const) reference type
    typedef T const&            const_reference;

// Construction
public:
    /// Default constructor
    true_typedef()
        : m_value(value_type())
    {}
    /// Construct from a value type instance
    ss_explicit_k true_typedef(value_type const& value)
        : m_value(value)
    {}
    /// Copy constructor
    true_typedef(class_type const& rhs)
        : m_value(rhs.m_value)
    {}

    /// Copy assignment operator
    class_type const& operator =(class_type const& rhs)
    {
        m_value = rhs.m_value;

        return *this;
    }

// Accessors
public:
    /// Provides non-mutating (const) access to the base type value
    const_reference base_type_value() const
    {
        return m_value;
    }
    /// Provides mutating access to the base type value
    reference base_type_value()
    {
        return m_value;
    }

// Members
private:
    value_type  m_value;

// Not to be implemented
private:
    // Not provided, as the syntax is less ambiguous when
    // assignment from an explicit temporary is made
    class_type const& operator =(value_type const& value);
};

/* /////////////////////////////////////////////////////////////////////////
 * implementation
 */

#ifndef STLSOFT_DOCUMENTATION_SKIP_SECTION

// Pre-increment

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>&
operator ++(true_typedef<T, U> &v)
{
    ++v.base_type_value();

    return v;
}

// Post-increment

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const
operator ++(true_typedef<T, U> &v, int)
{
    true_typedef<T, U>  r(v);

    v.base_type_value()++;

    return r;
}

// Pre-decrement

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>&
operator --(true_typedef<T, U> &v)
{
    --v.base_type_value();

    return v;
}

// Post-decrement

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const
operator --(true_typedef<T, U> &v, int)
{
    true_typedef<T, U>  r(v);

    v.base_type_value()--;

    return r;
}

// operator ==

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator ==(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return lhs.base_type_value() == rhs.base_type_value();
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator ==(
    true_typedef<T, U> const&                                   lhs
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return lhs.base_type_value() == rhs;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator ==(
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
    ss_typename_type_k true_typedef<T, U>::value_type const&    lhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
    T const&                                                    lhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   true_typedef<T, U> const&                                   rhs
)
{
    return lhs == rhs.base_type_value();
}


// operator !=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator !=(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return ! operator ==(lhs, rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator !=(
    true_typedef<T, U> const&                                   lhs
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return ! operator ==(lhs, rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator !=(
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
    ss_typename_type_k true_typedef<T, U>::value_type const&    lhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
    T const&                                                    lhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   true_typedef<T, U> const&                                   rhs
)
{
    return ! operator ==(lhs, rhs);
}

// operator <

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return lhs.base_type_value() < rhs.base_type_value();
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <(
    true_typedef<T, U> const&                                   lhs
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return lhs.base_type_value() < rhs;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <(
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
    ss_typename_type_k true_typedef<T, U>::value_type const&    lhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
    T const&                                                    lhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   true_typedef<T, U> const&                                   rhs
)
{
    return lhs < rhs.base_type_value();
}

// operator <=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <=(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return lhs.base_type_value() <= rhs.base_type_value();
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <=(
    true_typedef<T, U> const&                                   lhs
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return lhs.base_type_value() <= rhs;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator <=(
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
    ss_typename_type_k true_typedef<T, U>::value_type const&    lhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
    T const&                                                    lhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   true_typedef<T, U> const&                                   rhs
)
{
    return lhs <= rhs.base_type_value();
}

// operator >

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator >(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return lhs.base_type_value() > rhs.base_type_value();
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator >(
    true_typedef<T, U> const&                                   lhs 
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return lhs.base_type_value() > rhs;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator >(
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
    ss_typename_type_k true_typedef<T, U>::value_type const&    lhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
    T const&                                                    lhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   true_typedef<T, U> const&                                   rhs
)
{
    return lhs > rhs.base_type_value();
}

// operator >=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator >=(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return lhs.base_type_value() >= rhs.base_type_value();
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
operator >=(
    true_typedef<T, U> const&                                   lhs
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
,   ss_typename_type_k true_typedef<T, U>::value_type const&    rhs
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
,   T const&                                                    rhs
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
)
{
    return lhs.base_type_value() >= rhs;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
ss_bool_t
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator >=(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator >=(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return lhs >= rhs.base_type_value();
}

// operator +

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator +(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() + rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator +(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator +(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() + rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator +(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator +(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs + rhs.base_type_value());
}

// operator -

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator -(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() - rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator -(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator -(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() - rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator -(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator -(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs - rhs.base_type_value());
}

// operator *

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator *(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() * rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator *(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator *(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() * rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator *(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator *(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs * rhs.base_type_value());
}

// operator /

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator /(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() / rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator /(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator /(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() / rhs);
}


template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator /(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator /(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs / rhs.base_type_value());
}

// operator %

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator %(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() % rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator %(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator %(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() % rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator %(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator %(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs % rhs.base_type_value());
}

// operator ^

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator ^(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() ^ rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator ^(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator ^(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() ^ rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator ^(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator ^(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs ^ rhs.base_type_value());
}

// operator ~

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator ~(true_typedef<T, U> const& v)
{
    return true_typedef<T, U>(~v.base_type_value());
}

// operator <<

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator <<(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() << rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator <<(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator <<(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() << rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator <<(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator <<(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs << rhs.base_type_value());
}

// operator >>

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator >>(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() >> rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator >>(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator >>(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() >> rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator >>(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator >>(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs >> rhs.base_type_value());
}

// operator &

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator &(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() & rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator &(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator &(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() & rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator &(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator &(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs & rhs.base_type_value());
}

// operator |

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
operator |(
    true_typedef<T, U> const&   lhs
,   true_typedef<T, U> const&   rhs
)
{
    return true_typedef<T, U>(lhs.base_type_value() | rhs.base_type_value());
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator |(true_typedef<T, U> const& lhs, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator |(true_typedef<T, U> const& lhs, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs.base_type_value() | rhs);
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U>
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator |(ss_typename_type_k true_typedef<T, U>::value_type const& lhs, true_typedef<T, U> const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator |(T const& lhs, true_typedef<T, U> const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    return true_typedef<T, U>(lhs | rhs.base_type_value());
}

// operator +=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator +=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator +=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() += rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator +=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() += rhs.base_type_value();

    return v;
}

// operator -=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator -=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator -=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() -= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator -=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() -= rhs.base_type_value();

    return v;
}

// operator *=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator *=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator *=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() *= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator *=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() *= rhs.base_type_value();

    return v;
}

// operator /=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator /=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator /=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() /= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator /=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() /= rhs.base_type_value();

    return v;
}

// operator %=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator %=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator %=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() %= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator %=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() %= rhs.base_type_value();

    return v;
}

// operator ^=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator ^=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator ^=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() ^= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator ^=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() ^= rhs.base_type_value();

    return v;
}

// operator <<=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator <<=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator <<=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() <<= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator <<=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() <<= rhs.base_type_value();

    return v;
}

// operator >>=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator >>=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator >>=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() >>= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator >>=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() >>= rhs.base_type_value();

    return v;
}

// operator &=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator &=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator &=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() &= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator &=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() &= rhs.base_type_value();

    return v;
}

// operator |=

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
#ifdef STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT
operator |=(true_typedef<T, U> &v, ss_typename_type_k true_typedef<T, U>::value_type const& rhs)
#else /* ? STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
operator |=(true_typedef<T, U> &v, T const& rhs)
#endif /* STLSOFT_CF_TEMPLATE_OUTOFCLASSFN_QUALIFIED_TYPE_SUPPORT */
{
    v.base_type_value() |= rhs;

    return v;
}

template<
    ss_typename_param_k T
,   ss_typename_param_k U
>
inline
true_typedef<T, U> const&
operator |=(true_typedef<T, U> &v, true_typedef<T, U> const& rhs)
{
    v.base_type_value() |= rhs.base_type_value();

    return v;
}

#endif /* !STLSOFT_DOCUMENTATION_SKIP_SECTION */

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

#endif /* !STLSOFT_INCL_STLSOFT_UTIL_HPP_TRUE_TYPEDEF */

/* ///////////////////////////// end of file //////////////////////////// */
