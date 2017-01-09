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
inline void ExpectEqual( const tT &t1, const tT &t2 )
{
    EXPECT_EQ( t1, t2 );
}

template<>
inline void ExpectEqual< float >( const float &f1, const float &f2 )
{
    EXPECT_FLOAT_EQ( f1, f2 );
}

template<>
inline void ExpectEqual< double >( const double &d1, const double &d2 )
{
    EXPECT_DOUBLE_EQ( d1, d2 );
}

template< typename T >
inline T GenerateZebraValue()
{
    const uint16_t bits = sizeof( T ) << 3;
    T result = 0;

    for ( uint16_t i = 0; i < bits; ++++i )
    {
        result |= ( T )( 1ull << i );
    }

    return result;
}

template<>
inline std::string GenerateZebraValue()
{
    std::stringstream ss;
    ss << GenerateZebraValue< uint64_t >();
    return ss.str();
}

template<>
inline float GenerateZebraValue()
{
    return ( float )GenerateZebraValue< int32_t >();
}

template<>
inline double GenerateZebraValue()
{
    return ( double )GenerateZebraValue< int64_t >();
}

template< typename T >
T GenerateInvZebraValue()
{
    return GenerateZebraValue< T >() ^ std::numeric_limits<T>::max();
}

template<>
inline std::string GenerateInvZebraValue()
{
    std::stringstream ss;
    ss << GenerateInvZebraValue< uint64_t >();
    return ss.str();
}

template<>
inline float GenerateInvZebraValue()
{
    return ( float )GenerateInvZebraValue< int32_t >();
}

template<>
inline double GenerateInvZebraValue()
{
    return ( double )GenerateInvZebraValue< int64_t >();
}

extern uint32_t g_seed;

inline int GetFastRand()
{
    g_seed = ( 214013 * g_seed + 2531011 );
    return ( g_seed >> 16 ) & 0x7FFF;
}

template< typename T >
inline T GetRandom()
{
    return static_cast<T>( GetFastRand() );
}

template<>
inline float GetRandom< float >()
{
    // return with max an arbitrary number
    return ( float )GetFastRand() / ( float )( std::numeric_limits< uint32_t >::max() / ( 1e-8 / 3.0f ) );
}

template<>
inline bool GetRandom< bool >()
{
    // return with max an arbitrary number
    return ( GetRandom<uint32_t>() % 2 ) == 1;
}


template<>
inline double GetRandom< double >()
{
    // return with max an arbitrary number
    return ( double )GetFastRand() / ( float )( std::numeric_limits< uint32_t >::max() / ( 1e-16 / 3.0f ) );
}

std::string GenerateRandomString( bool random = true, uint32_t index = 0 );

template<>
inline std::string GetRandom<std::string>()
{
    return GenerateRandomString();
}



template< typename tT1, typename tT2 >
void SimpleSerialiseDeserialiseStream( tT1 &t1, tT2 &t2 )
{
    std::stringstream ss;

    {
        Serialisation::BinarySerialiser( ss ).Enter( t1 );
    }
    {
        Serialisation::BinaryDeserialiser( ss ).Enter( t2 );
    }
}

template< typename tT1, typename tT2 >
void SimpleSerialiseDeserialiseBackwards( const std::string &file, tT1 &c1, tT2 &c2 )
{
    /* Enable this when you need to regenerate the backwards compatibility files
    {
        // clear the file if needed
        std::ofstream ofs;
        ofs.open( file.c_str(), std::ofstream::out | std::ofstream::trunc );
        ofs.close();
        Serialisation::BinarySerialiser( file ).Enter( c1 );
    }
    /// */

    {
        Serialisation::BinaryDeserialiser( file ).Enter( c2 );
    }
}


template< typename tT1, typename tT2 >
void SimpleSerialiseDeserialiseFile( tT1 &c1, tT2 &c2 )
{
    // clear file
    {
        std::ofstream ofs;
        ofs.open( "file.bin", std::ofstream::out | std::ofstream::trunc );
        ofs.close();
    }
    {
        Serialisation::BinarySerialiser( "file.bin" ).Enter( c1 );
    }
    {
        Serialisation::BinaryDeserialiser( "file.bin" ).Enter( c2 );
    }
}

#define SERIALISATION_TEST2( test, name, testClass, testClass2, type, init1, init2 )    \
TEST( P( test ), type ## name ## _stream )                                              \
{                                                                                       \
    testClass tc1( init1 );                                                             \
    testClass2 tc2( init2 );                                                            \
    SimpleSerialiseDeserialiseStream( tc1, tc2 );                                       \
    tc1.TestEqual( tc2 );                                                               \
}                                                                                       \
TEST( P( test ), type ## name ## _file )                                                \
{                                                                                       \
    testClass tc1( init1 );                                                             \
    testClass2 tc2( init2 );                                                            \
    SimpleSerialiseDeserialiseFile( tc1, tc2 );                                         \
    tc1.TestEqual( tc2 );                                                               \
}                                                                                       \
TEST( P( test ), type ## name ## _backwards )                                           \
{                                                                                       \
    testClass tc1( init1 );                                                             \
    testClass2 tc2( init2 );                                                            \
    std::string file = TEST_FILE( test, type ## name );                                 \
    SimpleSerialiseDeserialiseBackwards( file, tc1, tc2 );                              \
    tc1.TestEqual( tc2 );                                                               \
}

#define SERIALISATION_TEST( test, name, testClass, type, init1, init2 )                 \
        SERIALISATION_TEST2( test, name, testClass, testClass, type, init1, init2 );

#define SERIALISATION_PP_TEMPLATE2( tClass, t1, t2 ) tClass< t1, t2 >
#define SERIALISATION_PP_TEMPLATE6( tClass, t1, t2, t3, t4, t5, t6 ) tClass< t1, t2, t3, t4, t5, t6 >

#endif
