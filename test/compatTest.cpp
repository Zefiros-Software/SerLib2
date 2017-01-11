#include "testClasses.h"

#define SERIALISATION_COMPATIBILITY_TEST( type1, type2 )    \
TEST( P( Compatibility ), type1 ## type2 )                  \
{                                                           \
    g_seed = 41424344;                                      \
    SinglePrimitive< type1 > t1;                            \
    SinglePrimitive< type2 > t2;                            \
                                                            \
    t1.Init();                                              \
    t2.Init();                                              \
                                                            \
    SimpleSerialiseDeserialiseStream( t1, t2 );             \
                                                            \
    t2.TestEqual( t1 );                                     \
}

#define SERIALISATION_FLOAT_COMPATIBILITY_TEST( type1, type2 )                                      \
TEST( P( Compatibility ), type1 ## type2 )                                                          \
{                                                                                                   \
    g_seed = 41424344;                                                                              \
    SinglePrimitive< type1 > t1;                                                                    \
    SinglePrimitive< type2 > t2;                                                                    \
                                                                                                    \
    t1.Init();                                                                                      \
    t2.Init();                                                                                      \
                                                                                                    \
    SimpleSerialiseDeserialiseStream( t1, t2 );                                                     \
                                                                                                    \
    ExpectEqual<float>( static_cast<float>( t1.GetValue() ), static_cast<float>( t2.GetValue() ) ); \
}

SERIALISATION_COMPATIBILITY_TEST( uint8_t, uint16_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, uint32_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, uint64_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, int8_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, int16_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, int32_t );
SERIALISATION_COMPATIBILITY_TEST( uint8_t, int64_t );

SERIALISATION_COMPATIBILITY_TEST( uint16_t, uint8_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, uint32_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, uint64_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, int8_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, int16_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, int32_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, int64_t );

SERIALISATION_COMPATIBILITY_TEST( uint32_t, uint8_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, uint16_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, uint64_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, int8_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, int16_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, int32_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, int64_t );

SERIALISATION_COMPATIBILITY_TEST( uint64_t, uint8_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, uint16_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, uint32_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, int8_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, int16_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, int32_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, int64_t );

SERIALISATION_FLOAT_COMPATIBILITY_TEST( float, double );
SERIALISATION_FLOAT_COMPATIBILITY_TEST( double, float );

SERIALISATION_COMPATIBILITY_TEST( size_t, uint8_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, uint16_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, uint32_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, uint64_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, int8_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, int16_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, int32_t );
SERIALISATION_COMPATIBILITY_TEST( size_t, int64_t );

SERIALISATION_COMPATIBILITY_TEST( uint8_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( uint16_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( uint32_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( uint64_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( int8_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( int16_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( int32_t, size_t );
SERIALISATION_COMPATIBILITY_TEST( int64_t, size_t );
