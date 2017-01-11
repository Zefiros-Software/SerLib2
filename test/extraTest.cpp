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
}