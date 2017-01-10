#include "testClasses.h"

#define SERIALISATION_ARRAY_TEST( type )                                            \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, 42 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );

#define SERIALISATION_ARRAY_REORDERED_TEST( type )                                                  \
SERIALISATION_TEST2(    Container,                                                                  \
                        VectorReordered,                                                            \
                        TestClassArrayWithMemberReordered< type >,                                  \
                        TestClassArrayWithMember< type >,                                           \
                        type,                                                                       \
                        42 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ),    \
                        GetRandom<uint32_t>()                                                       \
                   );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_TEST );

#ifndef _DEBUG
#define SERIALISATION_ARRAY_REORDERED_REVERSE_TEST( type )                                          \
SERIALISATION_TEST2(    Container,                                                                  \
                        VectorReorderedReverse,                                                     \
                        TestClassArrayWithMember< type >,                                           \
                        TestClassArrayWithMemberReordered< type >,                                  \
                        type,                                                                       \
                        42 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ),    \
                        GetRandom<uint32_t>()                                                       \
                   );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_REVERSE_TEST );
#endif

#define SERIALISATION_ARRAY_SKIPPED_TEST( type )                                                                    \
SERIALISATION_TEST2( Skipping, Vector, SkippedArray< type >, SinglePrimitive<uint8_t>, type, MakeSeed<type>( 42 ),  \
                     GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_SKIPPED_TEST );

#define SERIALISATION_ARRAY_NON_EXISTING_TEST( type )                                                                  \
SERIALISATION_TEST2( NonExisting, Vector, SinglePrimitive<uint8_t>, SkippedArray< type >, type, MakeSeed<type>( 42 ),  \
                     GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_NON_EXISTING_TEST );
