#include "testClasses.h"

#define SERIALISATION_ARRAY_TEST( type )                                                                               \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, MakeSeed<type>( 42 ) , GetRandom<uint32_t>() );

#define SERIALISATION_SMALL_ARRAY_TEST( type )                                                          \
SERIALISATION_TEST( Container, SmallVector, TestClassSmallArray< type >, type, MakeSeed<type>( 42 ) ,   \
                    GetRandom<uint32_t>() );

#define SERIALISATION_EDGE_CASE_ARRAY_TEST( type )                                                              \
SERIALISATION_TEST( Container, EdgeCaseVector, TestClassArrayEdgeCaseSize< type >, type, MakeSeed<type>( 42 ) , \
                    GetRandom<uint32_t>() );


#define SERIALISATION_ARRAY_REORDERED_TEST( type )                                                          \
SERIALISATION_TEST2( Container, VectorReordered, TestClassArrayWithMemberReordered< type >,                 \
                     TestClassArrayWithMember< type >, type, MakeSeed<type>( 42 ), GetRandom<uint32_t>() );


#define SERIALISATION_ARRAY_SKIPPED_TEST( type )                                                                    \
SERIALISATION_TEST2( Skipping, Vector, SkippedArray< type >, SinglePrimitive<uint8_t>, type, MakeSeed<type>( 42 ),  \
                     GetRandom<uint8_t>() );


#define SERIALISATION_ARRAY_NON_EXISTING_TEST( type )                                                       \
SERIALISATION_TEST2( NonExisting, Vector, SinglePrimitive<uint8_t>, SkippedArray< type >, type,             \
                     SERIALISATION_PP_TEMPLATE2( MakeSeed, type, uint8_t )( 42 ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );

SERIALISATION_SMALL_ARRAY_TEST( float );
SERIALISATION_SMALL_ARRAY_TEST( double );
SERIALISATION_EDGE_CASE_ARRAY_TEST( float );
SERIALISATION_EDGE_CASE_ARRAY_TEST( double );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_TEST );
SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_SKIPPED_TEST );
SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_NON_EXISTING_TEST );

#ifndef _DEBUG
#define SERIALISATION_ARRAY_REORDERED_REVERSE_TEST( type )                                                          \
SERIALISATION_TEST2( Container, VectorReorderedReverse, TestClassArrayWithMember< type >,                           \
                     TestClassArrayWithMemberReordered< type >, type, MakeSeed<type>( 42 ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_REVERSE_TEST );
#endif
