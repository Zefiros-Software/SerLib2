/**
 * Copyright (c) 2017 Mick van Duijn, Koen Visscher and Paul Visscher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <assert.h>

#ifndef __SERIALISATION_DEFINES_H__
#define __SERIALISATION_DEFINES_H__

#ifdef SERIALISATION_NO_HEADER_ONLY
#   define SERIALISATION_INLINE
#else
#   define SERIALISATION_INLINE inline
#endif

#ifndef SERIALISATION_CUSTOM_INTERFACE
#   define SERIALISATION_CUSTOM_INTERFACE OnStore
#endif

#ifndef SERIALISATION_SERIALISERS_BUFFERSIZE
#   define SERIALISATION_SERIALISERS_BUFFERSIZE 8192
#endif

#ifndef SERIALISATION_FLOAT_BUFFER_SIZE
#   define SERIALISATION_FLOAT_BUFFER_SIZE 2048u
#endif

#if __cplusplus > 199711L || ( defined _MSC_VER && _MSC_VER >= 1700 )
#define SERIALISATION_SUPPORT_STDARRAY
#include <array>
#endif

#define CASE_DEFAULT_ASSERT_HAYWIRE \
    default:                        \
        assert( false && "Whoops! Something went haywire. Please try to reproduce this exception in an example as small as possible and submit it as an issue. Thanks!"); \
        break;

#ifdef DEBUG
#define SERIALISATION_ASSERT_INDEX_IN_RANGE( index ) assert( index < 28 && "Index should be less than 28 for members" )
#define SERIALISATION_ASSERT_PARENT_INDEX_IN_RANGE( index ) assert( index < 4 && "Index should be less than 4 for StoreParent" );
#else
#define  SERIALISATION_ASSERT_INDEX_IN_RANGE( index )
#define  SERIALISATION_ASSERT_PARENT_INDEX_IN_RANGE( index )
#endif // DEBUG

// From boost
#if !defined(SERIALISATION_FORCEINLINE)
#  if defined(_MSC_VER)
#    define SERIALISATION_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
// Clang also defines __GNUC__ (as 4)
#    define SERIALISATION_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define SERIALISATION_FORCEINLINE inline
#  endif
#endif

// From boost
#if !defined(SERIALISATION_NOINLINE)
#  if defined(_MSC_VER)
#    define SERIALISATION_NOINLINE __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
// Clang also defines __GNUC__ (as 4)
#    if defined(__CUDACC__)
#      define SERIALISATION_NOINLINE __attribute__ ((noinline))
#    else
#      define SERIALISATION_NOINLINE __attribute__ ((__noinline__))
#    endif
#  else
#    define SERIALISATION_NOINLINE
#  endif
#endif

#endif