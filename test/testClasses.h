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
#ifndef __SERIALISATION_TESTCLASSES_H__
#define __SERIALISATION_TESTCLASSES_H__

#include "serialisation/serialisation.h"

#include "helper.h"

template< typename tT, uint8_t tIndex = 0 >
class SinglePrimitive
{
public:

    static constexpr uint8_t index = tIndex;

    SinglePrimitive()
        : mValue( GetRandom< tT >() )
    {
    }

    SinglePrimitive( const tT &val )
        : mValue( val )
    {
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( tIndex, mValue );
    }

    void TestEqual( SinglePrimitive< tT, tIndex > &other )
    {
        ExpectEqual( mValue, other.mValue );
    }

private:

    tT mValue;
};

template< typename tT >
class SinglePrimitiveIndexed
{
public:

    SinglePrimitiveIndexed( uint8_t index )
        : mValue( GetRandom< tT >() ),
          mIndex( index )
    {
    }

    SinglePrimitiveIndexed( uint8_t index, const tT &val )
        : mValue( val ),
          mIndex( index )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( mIndex, mValue );
    }

    void TestEqual( SinglePrimitiveIndexed< tT > &other )
    {
        ExpectEqual( mValue, other.mValue );
    }

private:

    tT mValue;
    uint8_t mIndex;
};

template< typename tT, typename tLevel >
class MultiPrimitiveLeveled
{
public:

    static constexpr uint8_t index = tLevel::index + 1;

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( index, mValue );
        mLevel.OnStore( message );
    }

private:

    tT mValue;
    tLevel mLevel;
};

template< uint8_t tIndex, typename tT, typename... tRest >
class MultiPrimitive
    : public MultiPrimitive < tIndex + 1, tRest... >
{
public:

    MultiPrimitive()
        : mValue( GetRandom< tT >() ),
          MultiPrimitive < tIndex + 1, tRest... > ()
    {
    }

    MultiPrimitive( const tT &val )
        : mValue( val ),
          MultiPrimitive < tIndex + 1, tRest... > ()
    {
    }

    template< typename tT, typename... tRest >
    MultiPrimitive( const tT &val, tRest &&... rest )
        : mValue( val ),
          MultiPrimitive < tIndex + 1, tRest... > ( std::forward<tRest>( rest )... )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( tIndex, mValue );
        MultiPrimitive < tIndex + 1, tRest... >::OnStore( message );
    }

    void TestEqual( MultiPrimitive< tIndex, tT, tRest... > &other )
    {
        ExpectEqual( mValue, other.mValue );
        MultiPrimitive < tIndex + 1, tRest... >::TestEqual( other );
    }

private:

    tT mValue;
};

template< uint8_t tIndex, typename tT >
class MultiPrimitive< tIndex, tT >
    : public SinglePrimitive< tT, tIndex >
{
};

#define SERIALISATION_TEST_SINGLE_PRIMITIVE( type )     \
TEST( P( SinglePrimitive ), type )                      \
{                                                       \
    for ( uint32_t i = 0; i < 1000; ++i )               \
    {                                                   \
        SinglePrimitive< type > tc1, tc2;               \
                                                        \
        SimpleSerialiseDeserialiseStream( tc1, tc2 );   \
                                                        \
        tc1.TestEqual( tc2 );                           \
    }                                                   \
}

#define SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED( type )                 \
TEST( P( SinglePrimitiveIndexed ), type )                                   \
{                                                                           \
    for ( uint32_t i = 0; i < 1000 / 28; ++i )                              \
    {                                                                       \
        for ( uint8_t index = 0; index < 28; ++index )                      \
        {                                                                   \
            SinglePrimitiveIndexed< type > tc1( index ), tc2( index );      \
                                                                            \
            SimpleSerialiseDeserialiseStream( tc1, tc2 );                   \
                                                                            \
            tc1.TestEqual( tc2 );                                           \
        }                                                                   \
    }                                                                       \
}

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_SINGLE_PRIMITIVE );

#define  SERIALISATION_TEST_MULTI_PRIMITVE( type1, type2, type3, type4, type5 )                         \
TEST( P( MultiPrimitive ), CONCAT( CONCAT( CONCAT( CONCAT( type1, type2 ), type3 ), type4 ), type5 ) )  \
{                                                                                                       \
    for ( uint32_t i = 0; i < 1000; ++i )                                                               \
    {                                                                                                   \
        MultiPrimitive< 0, type1, type2, type3, type4, type5 > tc1, tc2;                                \
                                                                                                        \
        SimpleSerialiseDeserialiseStream( tc1, tc2 );                                                   \
                                                                                                        \
        tc1.TestEqual( tc2 );                                                                           \
    }                                                                                                   \
}

#define SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS( type ) \
        SERIALISATION_TEST_MULTI_PRIMITVE( uint8_t, int32_t, double, String, type )

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS );

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED );

#endif
