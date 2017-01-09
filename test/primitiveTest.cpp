#include "testClasses.h"

#define SERIALISATION_TEST_SINGLE_PRIMITIVE_NUMERIC( type )                                                                   \
SERIALISATION_TEST( SinglePrimitive, Max, SinglePrimitive< type >, type, std::numeric_limits< type >::max(), ( type )( 1 ) ); \
SERIALISATION_TEST( SinglePrimitive, Min, SinglePrimitive< type >, type, std::numeric_limits< type >::min(), ( type )( 1 ) ); \
SERIALISATION_TEST( SinglePrimitive, Zebra, SinglePrimitive< type >, type, GenerateZebraValue< type >(), ( type )( 1 ) );     \
SERIALISATION_TEST( SinglePrimitive, InvZebra, SinglePrimitive< type >, type, GenerateInvZebraValue< type >(), ( type )( 1 ) );

SERIALISATION_ALL_NUMERIC_TYPES( SERIALISATION_TEST_SINGLE_PRIMITIVE_NUMERIC );
SERIALISATION_TEST( SinglePrimitive, 0_1, SinglePrimitive< String >, String, GenerateRandomString( false, 0 ),
                    GenerateRandomString( false, 1 ) );
SERIALISATION_TEST( SinglePrimitive, FF, SinglePrimitive< bool >, bool, false, false );
SERIALISATION_TEST( SinglePrimitive, FT, SinglePrimitive< bool >, bool, false, true );
SERIALISATION_TEST( SinglePrimitive, TF, SinglePrimitive< bool >, bool, true, false );
SERIALISATION_TEST( SinglePrimitive, TT, SinglePrimitive< bool >, bool, true, true );

#define SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, index )                                                                                                                   \
SERIALISATION_TEST( SinglePrimitiveIndexed, Max ## _ ## index, SERIALISATION_PP_TEMPLATE2( SinglePrimitiveIndexed, type, index ), type, std::numeric_limits< type >::max(), ( type )( 1 ) ); \
SERIALISATION_TEST( SinglePrimitiveIndexed, Min ## _ ## index, SERIALISATION_PP_TEMPLATE2( SinglePrimitiveIndexed, type, index ), type, std::numeric_limits< type >::min(), ( type )( 1 ) ); \
SERIALISATION_TEST( SinglePrimitiveIndexed, Zebra ## _ ## index, SERIALISATION_PP_TEMPLATE2( SinglePrimitiveIndexed, type, index ), type, GenerateZebraValue< type >(), ( type )( 1 ) );     \
SERIALISATION_TEST( SinglePrimitiveIndexed, InvZebra ## _ ## index, SERIALISATION_PP_TEMPLATE2( SinglePrimitiveIndexed, type, index ), type, GenerateInvZebraValue< type >(), ( type )( 1 ) );

#ifndef _DEBUG
#define  SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 0 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 1 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 2 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 3 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 5 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 7 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 11 );                    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 13 );                    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 17 );                    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 19 );                    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 23 );                    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 27 );
#else
#define  SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 0 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 7 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 27 );
#endif // !_DEBUG

SERIALISATION_ALL_NUMERIC_TYPES( SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES );

#define  SERIALISATION_TEST_MULTI_PRIMITVE( type1, type2, type3, type4, type5 )                         \
SERIALISATION_TEST(  MultiPrimitive, type1 ## type2 ## type3 ## type4 ## type5, SERIALISATION_PP_TEMPLATE6( MultiPrimitive, 0, type1, type2, type3, type4, type5 ), type5, 424142 * ( std::is_same< type1, std::string >::value ? 32 : sizeof( type1 ) ), 424142 );

#define SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS( type ) \
        SERIALISATION_TEST_MULTI_PRIMITVE( uint8_t, int32_t, double, String, type );

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS );

#define SERIALISATION_TEST_MULTI_PRIMITVE_REORDERED( type1, type2, type3, type4, type5 )                         \
SERIALISATION_TEST2(  MultiPrimitiveReordered, type1 ## type2 ## type3 ## type4 ## type5, SERIALISATION_PP_TEMPLATE6( MultiPrimitiveReordered, 0, type1, type2, type3, type4, type5 ), SERIALISATION_PP_TEMPLATE6( MultiPrimitive, 0, type1, type2, type3, type4, type5 ), type5, 424142 * ( std::is_same< type1, std::string >::value ? 32 : sizeof( type1 ) ), 424142 );

#define SERIALISATION_TEST_MULTI_PRIMITVE_REORDERED_VARIATIONS( type ) \
        SERIALISATION_TEST_MULTI_PRIMITVE_REORDERED( uint8_t, int32_t, double, String, type );

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_MULTI_PRIMITVE_REORDERED_VARIATIONS );
