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
#include "testClasses.h"

template< typename tObject, typename tT, size_t tBaseSize = 512 >
class SinglePrimitiveVectorStatic
{
public:

    SinglePrimitiveVectorStatic()
    {}

    SinglePrimitiveVectorStatic(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    void Init()
    {
        mVec.resize(tBaseSize + (GetRandom<uint32_t>() % tBaseSize));

        for (auto &s : mVec)
        {
            s.Init();
        }

        mValue = GetRandom< tT >();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mVec);
        message.Store(1, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        ExpectEqual(mVec.size(), t2.mVec.size());

        for (auto it = mVec.begin(), end = mVec.end(), it2 = t2.mVec.begin(); it != end; ++it, ++it2)
        {
            it->TestEqual(*it2);
        }

        ExpectEqual(mValue, t2.mValue);
    }

    size_t GetMemberSize() const
    {
        size_t size = SizeHelper<tT>::GetSize(mValue);

        for (auto &t : mVec)
        {
            size += t.GetMemberSize();
        }

        return size;
    }

    std::vector< tObject > mVec;
    tT mValue;
};

template< typename tObject, typename tT >
class DynamicWithStaticMember
{
public:

    DynamicWithStaticMember()
    {}

    DynamicWithStaticMember(uint32_t seed)
    {
        g_seed = seed;
        Init();
    }

    void Init()
    {
        mDynamic.Init();

        mValue = GetRandom< tT >();
    }

    template< typename tM >
    void OnStore(Message< tM > &message)
    {
        message.Store(0, mDynamic);
        message.Store(1, mValue);
    }

    template< typename tC >
    void TestEqual(tC &t2)
    {
        mDynamic.TestEqual(t2.mDynamic);

        ExpectEqual(mValue, t2.mValue);
    }

    size_t GetMemberSize() const
    {
        return SizeHelper<tT>::GetSize(mValue) + mDynamic.GetMemberSize();
    }

    SinglePrimitiveVectorStatic< tObject, tT > mDynamic;
    tT mValue;
};

// SERIALISATION_TEST( Static, SinglePrimitive, SinglePrimitiveVectorStatic< SinglePrimitive< uint32_t > PP_COMMA()  uint32_t >,
//                     uint32_t, 41, 42 );
//
// #define SERIALISATION_STATIC_MULTI_PRIMITIVE_TEST( type )                                                               \
// SERIALISATION_TEST( Static, MultiPrimitive, SinglePrimitiveVectorStatic <                                               \
//                         MultiPrimitive< 0 PP_COMMA() float PP_COMMA() double PP_COMMA() std::string PP_COMMA() type >   \
//                     PP_COMMA() type >, type, 41, 42 );
//
// SERIALISATION_ALL_TYPES( SERIALISATION_STATIC_MULTI_PRIMITIVE_TEST );

SERIALISATION_TEST(Static, NestedDynamic,
                   SinglePrimitiveVectorStatic< DynamicWithStaticMember< SinglePrimitive< uint32_t > PP_COMMA()  uint32_t > PP_COMMA() uint32_t PP_COMMA() 64 >,
                   uint32_t, 41, 42);
