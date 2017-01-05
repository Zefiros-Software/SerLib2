/**
 * Copyright (c) 2016 Mick van Duijn, Koen Visscher and Paul Visscher
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
#ifndef __SERIALISATION_HELPERS_H__
#define __SERIALISATION_HELPERS_H__

#include "gtest/gtest.h"

#include <string>

#define CONCATEXT( a, b ) a##b
#define CONCAT( a, b ) CONCATEXT( a, b )
#define P( prefix ) CONCAT( PREFIX, prefix )

#ifndef TEST_FILES_DIR
#   define TEST_FILES_DIR "../../test/test-files/"
#endif

#define TEST_FILE_DETAIL( file, suite, seperator, test, extension ) file #suite #seperator #test #extension
#define TEST_FILE( suite, test ) TEST_FILE_DETAIL(TEST_FILES_DIR, suite, _, test, .bin )

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

template< typename tT >
void ExpectEqual( const tT &t1, const tT &t2 )
{
    EXPECT_EQ( t1, t2 );
}

template<>
void ExpectEqual< float >( const float &f1, const float &f2 )
{
    EXPECT_FLOAT_EQ( f1, f2 );
}

template<>
void ExpectEqual< double >( const double &d1, const double &d2 )
{
    EXPECT_DOUBLE_EQ( d1, d2 );
}

template< typename T >
T GenerateZebraValue()
{
    const uint16_t bits = sizeof( T ) << 3;
    T result = 0;

    for ( uint16_t i = 0; i < bits; ++++i )
    {
        result |= ( T )( 1ull << i );
    }

    return result;
}

template< typename T >
T GenerateInvZebraValue()
{
    return GenerateZebraValue< T >() ^ std::numeric_limits<T>::max();
}

extern uint32_t g_seed;

inline int GetFastRand()
{
    g_seed = ( 214013 * g_seed + 2531011 );
    return ( g_seed >> 16 ) & 0x7FFF;
}

template< typename T >
T GetRandom()
{
    return static_cast<T>( GetFastRand() );
}

template<>
float GetRandom< float >();


template<>
double GetRandom< double >();

std::string GenerateRandomString();

template<>
std::string GetRandom<std::string>()
{
    return GenerateRandomString();
}



template< typename tT1, typename tT2 >
void SimpleSerialiseDeserialiseStream( tT1 &t1, tT2 &t2 )
{
    std::stringstream ss;

    Serialisation::BinarySerialiser( ss ).Enter( t1 );
    Serialisation::BinaryDeserialiser( ss ).Enter( t2 );
}

#endif
