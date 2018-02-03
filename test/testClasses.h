/**
 * @cond ___LICENSE___
 *
 * Copyright (c) 2016-2018 Zefiros Software.
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
 *
 * @endcond
 */
#pragma once
#ifndef __SERIALISATION_TESTCLASSES_H__
#define __SERIALISATION_TESTCLASSES_H__

#include "serialisation/serialisation.h"

#include "helper.h"

template< typename tC >
class SkippedPrimitive;
template< typename tC, uint8_t tFlags >
class SkippedArray;
template< typename tC >
class SkippedObjectVector;
template< typename tC >
class SkippedParent;
template< typename tC, uint8_t tI >
class SinglePrimitive;

template< typename tT, uint8_t tIndex = 0 >
class SinglePrimitive
{
public:

    template< typename tC >
    friend class SkippedPrimitive;
    template< typename tC, uint8_t tFlags >
    friend class SkippedArray;
    template< typename tC >
    friend class SkippedObjectVector;
    template< typename tC >
    friend class SkippedParent;
    template< typename tC, uint8_t tI >
    friend class SinglePrimitive;

    void Init()
    {
        mValue = GetRandom<tT>();
    }

    explicit SinglePrimitive(const tT &val)
        : mValue(val)
    {
    }

    SinglePrimitive()
    {
    }

    const tT &GetValue() const
    {
        return mValue;
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(tIndex, mValue);
    }

    void TestEqual(SinglePrimitive< tT, tIndex > &other)
    {
        ExpectEqual(mValue, other.mValue);
    }

    template< typename tC >
    void TestEqual(SkippedPrimitive< tC > &other)
    {
        other.TestEqual(*this);
    }

    template< typename tC, uint8_t tFlags >
    void TestEqual(SkippedArray< tC, tFlags > &other)
    {
        other.TestEqual(*this);
    }

    template< typename tC >
    void TestEqual(SkippedObjectVector< tC > &other)
    {
        other.TestEqual(*this);
    }

    template< typename tC >
    void TestEqual(SkippedParent< tC > &other)
    {
        other.TestEqual(*this);
    }

    template< typename tC >
    void TestEqual(SinglePrimitive< tC > &other)
    {
        ExpectEqual(mValue, static_cast<tT>(other.mValue));
    }

    size_t GetMemberSize() const
    {
        return SizeHelper<tT>::GetSize(mValue);
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

    explicit SkippedPrimitive(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    SkippedPrimitive(): mExtra(0)
    {
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mValue);
        message.Store(0, mExtra);
    }

    void TestEqual(SkippedPrimitive< tT > &other)
    {
        ExpectEqual(mValue, other.mValue);
        ExpectEqual(mExtra, other.mExtra);
    }

    void TestEqual(SinglePrimitive< uint8_t, 0 > &other) const
    {
        ExpectEqual(mExtra, other.mValue);
    }

private:

    tT mValue;
    uint8_t mExtra;
};

template< typename tT, uint8_t tFlags = 0 >
class SkippedArray
{
public:

    void Init()
    {
        mValue1.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }

        mValue2 = GetRandom<uint8_t>();
    }

    explicit SkippedArray(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mValue1, tFlags);
        message.Store(0, mValue2);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    void TestEqual(SinglePrimitive< uint8_t > &t2) const
    {
        ExpectEqual(mValue2, t2.mValue);
    }

private:

    std::vector<tT> mValue1;
    uint8_t mValue2;
};

template< typename tT, uint8_t tIndex >
class SinglePrimitiveIndexed
{
public:

    explicit SinglePrimitiveIndexed(const tT &val)
        : mValue(val),
          mIndex(tIndex)
    {
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(mIndex, mValue);
    }

    void TestEqual(SinglePrimitiveIndexed< tT, tIndex > &other)
    {
        ExpectEqual(mValue, other.mValue);
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

    explicit MultiPrimitive(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(tIndex, mValue);
        MultiPrimitive < tIndex + 1, tRest... >::OnStore(message);
    }

    void TestEqual(MultiPrimitive< tIndex, tT, tRest... > &other)
    {
        ExpectEqual(mValue, other.mValue);
        MultiPrimitive < tIndex + 1, tRest... >::TestEqual(static_cast <  MultiPrimitive < tIndex + 1,
                                                           tRest... > & >(other));
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

    explicit MultiPrimitiveReordered(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        MultiPrimitiveReordered < tIndex + 1, tRest... >::OnStore(message);
        message.Store(tIndex, mValue);
    }

    void TestEqual(MultiPrimitiveReordered< tIndex, tT, tRest... > &other)
    {
        ExpectEqual(mValue, other.mValue);
        MultiPrimitiveReordered < tIndex + 1, tRest... >::TestEqual(static_cast < MultiPrimitiveReordered < tIndex + 1,
                                                                    tRest... > & >(other));
    }

    void TestEqual(MultiPrimitive< tIndex, tT, tRest... > &other)
    {
        ExpectEqual(mValue, other.mValue);
        MultiPrimitiveReordered < tIndex + 1, tRest... >::TestEqual(static_cast < MultiPrimitive < tIndex + 1,
                                                                    tRest... > & >(other));
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

template< typename tT, uint8_t tFlags = 0 >
class TestClassArray
{
public:

    void Init()
    {
        mValue1.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }
    }

    explicit TestClassArray(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue1, tFlags);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    void TestEqual(TestClassArray< tT, tFlags > &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);
    }

private:

    std::vector<tT> mValue1;
};

template< typename tT, uint8_t tFlags, size_t tSize >
class TestClassArrayOfSize
{
public:

    void Init()
    {
        mValue1.resize(tSize);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }
    }

    explicit TestClassArrayOfSize(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue1, tFlags);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    void TestEqual(TestClassArrayOfSize< tT, tFlags, tSize > &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);
    }

private:

    std::vector<tT> mValue1;
};

template< typename tT >
class TestClassSmallArray
{
public:

    void Init()
    {
        mValue1.resize((GetRandom< uint32_t >() % 1024));

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }
    }

    explicit TestClassSmallArray(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue1);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    void TestEqual(TestClassSmallArray< tT > &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);
    }

private:

    std::vector<tT> mValue1;
};

template< typename tT >
class TestClassArrayEdgeCaseSize
{
public:

    void Init()
    {
        mValue1.resize(1025);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }
    }

    explicit TestClassArrayEdgeCaseSize(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue1);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    void TestEqual(TestClassArrayEdgeCaseSize< tT > &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);
    }

private:

    std::vector<tT> mValue1;
};

template< typename tT, uint8_t tFlags = 0 >
class TestClassArrayWithMember
{
public:

    void Init()
    {
        mValue1.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }

        mValue2 = GetRandom< tT >();
    }

    explicit TestClassArrayWithMember(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue1, tFlags);
        message.Store(1, mValue2);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);

        ExpectEqual(mValue2, t2.mValue2);
    }

    std::vector<tT> mValue1;
    tT mValue2;
};

template< typename tT, uint8_t tFlags = 0 >
class TestClassArrayWithMemberReordered
{
public:

    void Init()
    {
        mValue1.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mValue1.begin(); it != mValue1.end(); ++it)
        {
            *it = static_cast<tT>(GetRandom< tT >());
        }

        mValue2 = GetRandom< tT >();
    }

    explicit TestClassArrayWithMemberReordered(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mValue2);
        message.Store(0, mValue1, tFlags);
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof(tT);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);

        ExpectEqual(mValue2, t2.mValue2);
    }

    std::vector<tT> mValue1;
    tT mValue2;
};

template< typename tT >
class NestedObject
{
public:

    NestedObject()
    {}

    explicit NestedObject(const tT &value)
        : mValue(value),
          mObject(value)
    {
    }

    void Init()
    {
        mValue = GetRandom<tT>();
        mObject.Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue);
        message.Store(1, mObject);
    }

    void TestEqual(NestedObject< tT > &t2)
    {
        ExpectEqual(mValue, t2.mValue);

        mObject.TestEqual(t2.mObject);
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

    explicit ClassWithParent(const tT &value)
        : tParent(value),
          mValue(value),
          mObject(value)
    {
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.template StoreParent< tParent >(0, this);
        message.Store(1, mObject);
        message.Store(2, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        tParent::TestEqual(t2);

        mObject.TestEqual(t2.mObject);

        ExpectEqual(mValue, t2.mValue);
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

    explicit SkippedParent(uint32_t seed)
    {
        g_seed = seed;
        SinglePrimitive<tT>::Init();
        mValue = GetRandom<uint8_t>();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.template StoreParent< tParent >(1, this);
        message.Store(0, mValue);
    }

    void TestEqual(SinglePrimitive<uint8_t> &t2) const
    {
        ExpectEqual(mValue, t2.mValue);
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

    explicit ClassWithParentReordered(const tT &value)
        : tParent(value),
          mValue(value),
          mObject(value)
    {
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(2, mValue);
        message.Store(1, mObject);
        message.template StoreParent< tParent >(0, this);
    }


    template< typename tC >
    void TestEqual(tC &t2)
    {
        tParent::TestEqual(t2);

        mObject.TestEqual(t2.mObject);

        ExpectEqual(mValue, t2.mValue);
    }

    tT mValue;

    SinglePrimitive< tT > mObject;
};

typedef SinglePrimitive<uint8_t> SingleU8;
typedef SinglePrimitive<uint16_t> SingleU16;
typedef SinglePrimitive<float> SingleFloat;

template< typename tT, typename tMember = SinglePrimitive< tT > >
class ClassWithMultipleParents
    : public ClassWithParent< tT >,
      public SingleFloat,
      public SingleU8,
      public SingleU16
{

    typedef ClassWithParent< tT > tParent1;
public:

    explicit ClassWithMultipleParents(const tT &value)
        : tParent1(value),
          SingleFloat(1.0f * value),
          SingleU8(static_cast<uint8_t>(value)),
          SingleU16(static_cast<uint16_t>(value)),
          mValue(value),
          mObject(value)
    {
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.template StoreParent< tParent1 >(0, this);
        message.template StoreParent< SingleFloat >(1, this);
        message.template StoreParent< SingleU8 >(2, this);
        message.template StoreParent< SingleU16 >(3, this);
        message.Store(1, mObject);
        message.Store(2, mValue);
    }

    void TestEqual(ClassWithMultipleParents< tT > &t2)
    {
        tParent1::TestEqual(static_cast<tParent1 &>(t2));
        SingleFloat::TestEqual(static_cast<SingleFloat &>(t2));
        SingleU8::TestEqual(static_cast<SingleU8 &>(t2));
        SingleU16::TestEqual(static_cast<SingleU16 &>(t2));

        mObject.TestEqual(t2.mObject);

        ExpectEqual(mValue, t2.mValue);
    }

private:

    tT mValue;

    tMember mObject;
};

template< typename tT >
class ObjectVector
{
public:

    void Init()
    {
        mObjects.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
        {
            *it = SinglePrimitive<tT>(GetRandom< tT >());
        }

        mValue = GetRandom<uint8_t>();
    }

    explicit ObjectVector(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mObjects);
        message.Store(1, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mObjects.size(), t2.mObjects.size());

        for (size_t i = 0, end = mObjects.size(); i < end; ++i)
        {
            mObjects[i].TestEqual(t2.mObjects[i]);
        }
    }

    std::vector< SinglePrimitive< tT > > mObjects;
    uint8_t mValue;

};

template< typename tT >
class ObjectVectorReordered
{
public:

    void Init()
    {
        mObjects.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
        {
            *it = SinglePrimitive<tT>(GetRandom< tT >());
        }

        mValue = GetRandom<uint8_t>();
    }

    explicit ObjectVectorReordered(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mValue);
        message.Store(0, mObjects);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mObjects.size(), t2.mObjects.size());

        for (size_t i = 0, end = mObjects.size(); i < end; ++i)
        {
            mObjects[i].TestEqual(t2.mObjects[i]);
        }
    }

    std::vector< SinglePrimitive< tT > > mObjects;
    uint8_t mValue;

};

template< typename tT >
class SkippedObjectVector
{
public:

    void Init()
    {
        mObjects.resize((GetRandom< uint32_t >() % 8192) + 8192);

        for (auto it = mObjects.begin(); it != mObjects.end(); ++it)
        {
            *it = SinglePrimitive<tT>(GetRandom< tT >());
        }

        mValue = GetRandom<uint8_t>();
    }

    explicit SkippedObjectVector(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mObjects);
        message.Store(0, mValue);
    }

    void TestEqual(SinglePrimitive< uint8_t > &other) const
    {
        ExpectEqual(mValue, other.mValue);
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

    explicit TestClassTreeSkipping(uint32_t seed)
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mRight);
    }


    template< typename tC >
    void TestEqual(tC &t2)
    {
        mRight.TestEqual(t2.mRight);
    }

    TestClassTreeSkipping < tT, tLevel - 1 > mRight;
};

template< typename tT, uint32_t tLevel = 5 >
class TestClassTree
{
public:

    TestClassTree()
    {}

    explicit TestClassTree(uint32_t seed)
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mLeft);
        message.Store(1, mRight);
    }


    template< typename tC >
    void TestEqual(tC &t2)
    {
        mLeft.TestEqual(t2.mLeft);
        mRight.TestEqual(t2.mRight);
    }

    void TestEqual(TestClassTreeSkipping< tT, tLevel > &other)
    {
        mRight.TestEqual(other.mRight);
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

    explicit TestClassTreeReordered(uint32_t seed)
    {
        g_seed = seed;
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(1, mRight);
        message.Store(0, mLeft);
    }

    void TestEqual(TestClassTreeReordered< tT, tLevel > &t2)
    {
        mLeft.TestEqual(t2.mLeft);
        mRight.TestEqual(t2.mRight);
    }


    template< typename tC >
    void TestEqual(tC &t2)
    {
        mLeft.TestEqual(t2.mLeft);
        mRight.TestEqual(t2.mRight);
    }

    TestClassTreeReordered < tT, tLevel - 1 > mLeft;
    TestClassTreeReordered < tT, tLevel - 1 > mRight;
};

template< typename tT >
class TestClassTree< tT, 0 >
{
public:

    TestClassTree()
        : mValue(GetRandom<tT>())
    {}

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mValue, t2.mValue);
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
        : mValue(GetRandom<tT>())
    {}

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mValue, t2.mValue);
    }

    tT mValue;
};

class CompleteNormalOrder
{
public:

    CompleteNormalOrder()
        : mValue1(GetRandom<uint8_t>()),
          mValue2(GetRandom<uint16_t>()),
          mValue3(GetRandom<uint32_t>()),
          mValue4(GetRandom<uint64_t>()),
          mValue5(GetRandom<float>()),
          mValue6(GetRandom<double>())
    {
        mObject.Init();
    }

    virtual ~CompleteNormalOrder()
    {}

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(2, mValue1);
        message.Store(3, mValue2);
        message.Store(4, mValue3);
        message.Store(5, mValue4);
        message.Store(6, mValue5);
        message.Store(7, mValue6);
        message.Store(0, mObject);
        message.Store(1, mObject2);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mValue1, t2.mValue1);
        ExpectEqual(mValue2, t2.mValue2);
        ExpectEqual(mValue3, t2.mValue3);
        ExpectEqual(mValue4, t2.mValue4);
        ExpectEqual(mValue5, t2.mValue5);
        ExpectEqual(mValue6, t2.mValue6);
    }

    virtual void TestEqual(CompleteNormalOrder &t2)
    {
        TestEqual< CompleteNormalOrder >(t2);

        mObject.TestEqual(t2.mObject);
        mObject2.mObjectVector.TestEqual(t2.mObject2.mObjectVector);
    }

    NestedObject< std::string > mObject;

    struct Private
    {
        ObjectVector<bool> mObjectVector;

        Private()
            : mObjectVector(GetRandom<uint32_t>())
        {}

        template< typename tM >
        void OnStore(Message<tM> &message)
        {
            message.Store(0, mObjectVector);
        }
    } mObject2;

    uint8_t mValue1;
    uint16_t mValue2;
    uint32_t mValue3;
    uint64_t mValue4;
    float mValue5;
    double mValue6;

};

class CompleteReordered
    : public CompleteNormalOrder
{
public:

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        //message.Store(0, mObject);
        //message.Store(2, mObjectVector);
        message.Store(3, mValue2);
        message.Store(6, mValue5);
        message.Store(4, mValue3);
        message.Store(5, mValue4);
        message.Store(2, mValue1);
        message.Store(7, mValue6);
    }

    void TestEqual(CompleteNormalOrder &t2) override
    {
        CompleteNormalOrder::TestEqual< CompleteNormalOrder >(t2);
    }
};


#endif
