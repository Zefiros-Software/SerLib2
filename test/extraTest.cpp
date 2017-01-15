/**
 * Copyright (c) 2017 Zefiros Software.
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
#include "testClasses.h"

TEST( P( Extra ), CompleteReorder )
{
    g_seed = 434241;

    CompleteNormalOrder t1;
    CompleteReordered t2;

    std::stringstream ss;

    Serialisation::BinarySerialiser( ss ).Enter( t1 );
    Serialisation::BinaryDeserialiser( ss ).Enter( t2 );

    t1.TestEqual( t2 );
}

TEST( P( Extra ), LDExp )
{
    g_seed = 454142;

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        float f = GetRandom<float>();
        int32_t exp = GetRandom<uint32_t>() % ( 4 * 127 );

        ExpectEqual( Util::LDExp( f, exp ), ldexp( f, exp ) );
    }

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        double d = GetRandom<double>();
        int32_t exp = GetRandom<uint32_t>() % ( 4 * 1023 );

        ExpectEqual( Util::LDExp( d, exp ), ldexp( d, exp ) );
    }

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        float f = GetRandom<float>();
        int32_t exp = GetRandom<uint32_t>() % ( 4 * 126 );
        exp = -exp;

        ExpectEqual( Util::LDExp( f, exp ), ldexp( f, exp ) );
    }

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        double f = GetRandom<double>();
        int32_t exp = GetRandom<uint32_t>() % ( 4 * 1022 );
        exp = -exp;

        ExpectEqual( Util::LDExp( f, exp ), ldexp( f, exp ) );
    }

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        union
        {
            float f;
            uint32_t i;
        } y;
        y.i = GetFastRand() & ( ( 1 << 23 ) - 1 );

        int32_t e;
        int32_t e2;

        ExpectEqual( Util::FRExp( y.f, &e ), frexp( y.f, &e2 ) );
        ExpectEqual( e, e2 );

        y.i |= 0xff << 23;

        ExpectEqual( isnan( Util::FRExp( y.f, &e ) ), isnan( frexp( y.f, &e2 ) ) );
    }

    for ( uint32_t i = 0; i < 10000; ++i )
    {
        union
        {
            double d;
            uint64_t i;
        } y;

        y.i = GetFastRand() & ( ( 1ull << 52 ) - 1 );

        int32_t e;
        int32_t e2;

        ExpectEqual( Util::FRExp( y.d, &e ), frexp( y.d, &e2 ) );
        ExpectEqual( e, e2 );

        y.i |= 0x7ffull << 52;

        ExpectEqual( isnan( Util::FRExp( y.d, &e ) ), isnan( frexp( y.d, &e2 ) ) );
    }

    {
        int32_t e;
        int32_t e2;

        ExpectEqual( Util::FRExp( 0.0f, &e ), frexp( 0.0f, &e2 ) );
        ExpectEqual( e, e2 );

        ExpectEqual( Util::FRExp( 0.0, &e ), frexp( 0.0, &e2 ) );
        ExpectEqual( e, e2 );
    }
}