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

    template< typename tC >
    friend class SkippedPrimitive;
    template< typename tC >
    friend class SkippedArray;
    template< typename tC >
    friend class SkippedObjectVector;
    template< typename tC >
    friend class SkippedParent;

    void Init()
    {
        mValue = GetRandom<tT>();
    }

    SinglePrimitive( const tT &val )
        : mValue( val )
    {
    }

    SinglePrimitive()
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
class SkippedPrimitive
{
public:

    void Init()
    {
        mValue = GetRandom<tT>();
        mExtra = GetRandom<uint8_t>();
    }

    SkippedPrimitive( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    SkippedPrimitive()
    {
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 1, mValue );
        message.Store( 0, mExtra );
    }

    void TestEqual( SkippedPrimitive< tT > &other )
    {
        ExpectEqual( mValue, other.mValue );
        ExpectEqual( mExtra, other.mExtra );
    }

    void TestEqual( SinglePrimitive< uint8_t, 0 > &other )
    {
        ExpectEqual( mExtra, other.mValue );
    }

private:

    tT mValue;
    uint8_t mExtra;
};

template< typename tT >
class SkippedArray
{
public:

    void Init()
    {
        mValue1.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }

        mValue2 = GetRandom<uint8_t>();
    }

    SkippedArray( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 1, mValue1 );
        message.Store( 0, mValue2 );
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof( tT );
    }

    void TestEqual( SinglePrimitive< uint8_t > &t2 )
    {
        ExpectEqual( mValue2, t2.mValue );
    }

private:

    std::vector<tT> mValue1;
    uint8_t mValue2;
};

template< typename tT, uint8_t tIndex >
class SinglePrimitiveIndexed
{
public:

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

template< uint8_t tIndex, typename tT, typename... tRest >
class MultiPrimitive
    : public MultiPrimitive < tIndex + 1, tRest... >
{
public:

    void Init()
    {
        mValue = GetRandom< tT >();
        MultiPrimitive < tIndex + 1, tRest... >::Init();
    }

    MultiPrimitive()
    {

    }

    MultiPrimitive( uint32_t seed )
    {
        g_seed = seed;
        Init();
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

    tT mValue;
};


template< uint8_t tIndex, typename tT, typename... tRest >
class MultiPrimitiveReordered
    : public MultiPrimitiveReordered < tIndex + 1, tRest... >
{
public:

    void Init()
    {
        mValue = GetRandom< tT >();
        MultiPrimitiveReordered < tIndex + 1, tRest... >::Init();
    }

    MultiPrimitiveReordered()
    {

    }

    MultiPrimitiveReordered( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        MultiPrimitiveReordered < tIndex + 1, tRest... >::OnStore( message );
        message.Store( tIndex, mValue );
    }

    void TestEqual( MultiPrimitiveReordered< tIndex, tT, tRest... > &other )
    {
        ExpectEqual( mValue, other.mValue );
        MultiPrimitiveReordered < tIndex + 1, tRest... >::TestEqual( other );
    }

    void TestEqual( MultiPrimitive< tIndex, tT, tRest... > &other )
    {
        ExpectEqual( mValue, other.mValue );
        MultiPrimitiveReordered < tIndex + 1, tRest... >::TestEqual( other );
    }

    tT mValue;
};

template< uint8_t tIndex, typename tT >
class MultiPrimitive< tIndex, tT >
    : public SinglePrimitive< tT, tIndex >
{
};

template< uint8_t tIndex, typename tT >
class MultiPrimitiveReordered< tIndex, tT >
    : public SinglePrimitive< tT, tIndex >
{
};

template< typename tT >
class TestClassArray
{
public:

    void Init()
    {
        mValue1.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }
    }

    TestClassArray( uint32_t seed )
    {
        g_seed = seed;
        Init();
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
class TestClassArrayWithMember
{
public:

    void Init()
    {
        mValue1.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }

        mValue2 = GetRandom< tT >();
    }

    TestClassArrayWithMember( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 0, mValue1 );
        message.Store( 1, mValue2 );
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof( tT );
    }

    template< typename tC >
    void TestEqual( tC &t2 )
    {
        ExpectEqual( mValue1.size(), t2.mValue1.size() );

        for ( size_t i = 0, end = mValue1.size(); i < end; ++i )
        {
            ExpectEqual( mValue1[i], t2.mValue1[i] );
        }

        ExpectEqual( mValue2, t2.mValue2 );
    }

    std::vector<tT> mValue1;
    tT mValue2;
};

template< typename tT >
class TestClassArrayWithMemberReordered
{
public:

    void Init()
    {
        mValue1.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }

        mValue2 = GetRandom< tT >();
    }

    TestClassArrayWithMemberReordered( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 1, mValue2 );
        message.Store( 0, mValue1 );
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof( tT );
    }

    template< typename tC >
    void TestEqual( tC &t2 )
    {
        ExpectEqual( mValue1.size(), t2.mValue1.size() );

        for ( size_t i = 0, end = mValue1.size(); i < end; ++i )
        {
            ExpectEqual( mValue1[i], t2.mValue1[i] );
        }

        ExpectEqual( mValue2, t2.mValue2 );
    }

    std::vector<tT> mValue1;
    tT mValue2;
};

template< typename tT >
class NestedObject
{
public:

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

    tT mValue;

    SinglePrimitive< tT > mObject;
};

template< typename tT >
class ClassWithParent
    : public SinglePrimitive< tT >
{
public:

    typedef SinglePrimitive< tT > tParent;

    ClassWithParent()
        : tParent(),
          mObject()
    {}

    ClassWithParent( const tT &value )
        : tParent( value ),
          mValue( value ),
          mObject( value )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.StoreParent< tParent >( 0, this );
        message.Store( 1, mObject );
        message.Store( 2, mValue );
    }

    template< typename tC >
    void TestEqual( tC &t2 )
    {
        tParent::TestEqual( t2 );

        mObject.TestEqual( t2.mObject );

        ExpectEqual( mValue, t2.mValue );
    }

    tT mValue;

    SinglePrimitive< tT > mObject;
};

template< typename tT >
class SkippedParent
    : public SinglePrimitive< tT >
{
public:

    typedef SinglePrimitive< tT > tParent;

    SkippedParent( uint32_t seed )
    {
        g_seed = seed;
        Init();
        mValue = GetRandom<uint8_t>();
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.StoreParent< tParent >( 1, this );
        message.Store( 0, mValue );
    }

    void TestEqual( SinglePrimitive<uint8_t> &t2 )
    {
        ExpectEqual( mValue, t2.mValue );
    }

    uint8_t mValue;
};

template< typename tT >
class ClassWithParentReordered
    : public SinglePrimitive< tT >
{
public:

    typedef SinglePrimitive< tT > tParent;

    ClassWithParentReordered()
        : tParent(),
          mObject()
    {}

    ClassWithParentReordered( const tT &value )
        : tParent( value ),
          mValue( value ),
          mObject( value )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 2, mValue );
        message.Store( 1, mObject );
        message.StoreParent< tParent >( 0, this );
    }


    template< typename tC >
    void TestEqual( tC &t2 )
    {
        tParent::TestEqual( t2 );

        mObject.TestEqual( t2.mObject );

        ExpectEqual( mValue, t2.mValue );
    }

    tT mValue;

    SinglePrimitive< tT > mObject;
};

template< typename tT >
class ClassWithMultipleParents
    : public ClassWithParent< tT >, public SinglePrimitive< float >, public SinglePrimitive< uint8_t >,
      public SinglePrimitive< uint16_t >
{
public:

    typedef ClassWithParent< tT > tParent1;
    typedef SinglePrimitive< float > tParent2;
    typedef SinglePrimitive< uint8_t > tParent3;
    typedef SinglePrimitive< uint16_t > tParent4;

    ClassWithMultipleParents( const tT &value )
        : mValue( value ),
          mObject( value ),
          tParent1( value ),
          tParent2( value ),
          tParent3( value ),
          tParent4( value )
    {
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.StoreParent< tParent1 >( 0, this );
        message.StoreParent< tParent2 >( 1, this );
        message.StoreParent< tParent3 >( 2, this );
        message.StoreParent< tParent4 >( 3, this );
        message.Store( 1, mObject );
        message.Store( 2, mValue );
    }

    void TestEqual( ClassWithMultipleParents< tT > &t2 )
    {
        tParent1::TestEqual( static_cast< ClassWithParent & >( t2 ) );
        tParent2::TestEqual( t2 );
        tParent3::TestEqual( t2 );
        tParent4::TestEqual( t2 );

        mObject.TestEqual( t2.mObject );

        ExpectEqual( mValue, t2.mValue );
    }

private:

    tT mValue;

    SinglePrimitive< tT > mObject;
};

template< typename tT >
class ObjectVector
{
public:

    void Init()
    {
        mObjects.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mObjects.begin(); it != mObjects.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }
    }

    ObjectVector( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 0, mObjects );
    }

    void TestEqual( ObjectVector< tT > &t2 )
    {
        ExpectEqual( mObjects.size(), t2.mObjects.size() );

        for ( size_t i = 0, end = mObjects.size(); i < end; ++i )
        {
            mObjects[i].TestEqual( t2.mObjects[i] );
        }
    }

private:

    std::vector< SinglePrimitive< tT > > mObjects;

};

template< typename tT >
class SkippedObjectVector
{
public:

    void Init()
    {
        mObjects.resize( ( GetRandom< uint32_t >() % 8192 ) + 8192 );

        for ( auto it = mObjects.begin(); it != mObjects.end(); ++it )
        {
            *it = static_cast<tT>( GetRandom< tT >() );
        }

        mValue = GetRandom<uint8_t>();
    }

    SkippedObjectVector( uint32_t seed )
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 1, mObjects );
        message.Store( 0, mValue );
    }

    void TestEqual( SinglePrimitive< uint8_t > &other )
    {
        ExpectEqual( mValue, other.mValue );
    }

    std::vector< SinglePrimitive< tT > > mObjects;
    uint8_t mValue;

};

template< typename tT, uint32_t tLevel >
class TestClassTreeSkipping
{
public:

    TestClassTreeSkipping()
    {}

    TestClassTreeSkipping( uint32_t seed )
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 1, mRight );
    }


    template< typename tC >
    void TestEqual( tC &t2 )
    {
        mRight.TestEqual( t2.mRight );
    }

    TestClassTreeSkipping < tT, tLevel - 1 > mRight;
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


    template< typename tC >
    void TestEqual( tC &t2 )
    {
        mLeft.TestEqual( t2.mLeft );
        mRight.TestEqual( t2.mRight );
    }

    void TestEqual( TestClassTreeSkipping< tT, tLevel > &other )
    {
        mRight.TestEqual( other.mRight );
    }

    TestClassTree < tT, tLevel - 1 > mLeft;
    TestClassTree < tT, tLevel - 1 > mRight;
};

template< typename tT, uint32_t tLevel >
class TestClassTreeReordered
{
public:

    TestClassTreeReordered()
    {}

    TestClassTreeReordered( uint32_t seed )
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 1, mRight );
        message.Store( 0, mLeft );
    }

    void TestEqual( TestClassTreeReordered< tT, tLevel > &t2 )
    {
        mLeft.TestEqual( t2.mLeft );
        mRight.TestEqual( t2.mRight );
    }


    template< typename tC >
    void TestEqual( tC &t2 )
    {
        mLeft.TestEqual( t2.mLeft );
        mRight.TestEqual( t2.mRight );
    }

    TestClassTreeReordered < tT, tLevel - 1 > mLeft;
    TestClassTreeReordered < tT, tLevel - 1 > mRight;
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

    template< typename tC >
    void TestEqual( tC &t2 )
    {
        ExpectEqual( mValue, t2.mValue );
    }

    tT mValue;
};

template< typename tT >
class TestClassTreeSkipping< tT, 0 >
    : public TestClassTree< tT, 0 >
{
};

template< typename tT >
class TestClassTreeReordered< tT, 0 >
{
public:

    TestClassTreeReordered()
        : mValue( GetRandom<tT>() )
    {}

    template< typename tM >
    void OnStore( Message< tM > &message )
    {
        message.Store( 0, mValue );
    }

    template< typename tC >
    void TestEqual( tC &t2 )
    {
        ExpectEqual( mValue, t2.mValue );
    }

    tT mValue;
};


#endif
