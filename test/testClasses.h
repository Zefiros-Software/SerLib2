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

template< typename tT, uint8_t tIndex >
class SinglePrimitiveIndexed
{
public:

    SinglePrimitiveIndexed()
        : mValue( GetRandom< tT >() ),
          mIndex( tIndex )
    {
    }

    SinglePrimitiveIndexed( const tT &val )
        : mValue( val ),
          mIndex( tIndex )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( mIndex, mValue );
    }

    void TestEqual( SinglePrimitiveIndexed< tT, tIndex > &other )
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

template< typename tT >
class TestClassArray
{
public:

    TestClassArray()
    {
        mValue1.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }
    }

    TestClassArray( uint32_t seed )
        : TestClassArray()
    {
        g_seed = seed;
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 0, mValue1 );
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof( tT );
    }

    void TestEqual( TestClassArray< tT > &t2 )
    {
        ExpectEqual( mValue1.size(), t2.mValue1.size() );

        for ( size_t i = 0, end = mValue1.size(); i < end; ++i )
        {
            ExpectEqual( mValue1[i], t2.mValue1[i] );
        }
    }

private:

    std::vector<tT> mValue1;
};

template< typename tT >
class NestedObject
    : public SinglePrimitive< tT >
{
public:

    NestedObject()
        : mObject()
    {}

    NestedObject( const tT &value )
        : mValue( value ),
          mObject( value )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 0, mValue );
        message.Store( 1, mObject );
    }

    void TestEqual( NestedObject< tT > &t2 )
    {
        ExpectEqual( mValue, t2.mValue );

        mObject.TestEqual( t2.mObject );
    }

private:

    tT mValue;

    SinglePrimitive< tT > mObject;
};

template< typename tT, uint32_t tLevel >
class TestClassTree
{
public:

    TestClassTree()
    {}

    TestClassTree( uint32_t seed )
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 0, mLeft );
        message.Store( 1, mRight );
    }

    void TestEqual( TestClassTree< tT, tLevel > &t2 )
    {
        mLeft.TestEqual( t2.mLeft );
        mRight.TestEqual( t2.mRight );
    }

private:

    TestClassTree < tT, tLevel - 1 > mLeft;
    TestClassTree < tT, tLevel - 1 > mRight;
};

template< typename tT >
class TestClassTree< tT, 0 >
{
public:

    TestClassTree()
        : mValue( GetRandom<tT>() )
    {}

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 0, mValue );
    }

    void TestEqual( TestClassTree< tT, 0 > &t2 )
    {
        ExpectEqual( mValue, t2.mValue );
    }

private:

    tT mValue;
};


#endif
