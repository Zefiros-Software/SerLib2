#include "testClasses.h"

#define SERIALISATION_PARENT_TEST( type )                                                                               \
SERIALISATION_TEST( Object, Parent_Zebra, ClassWithParent< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_TEST );

#define SERIALISATION_PARENT_REORDERED_TEST( type )                                                                               \
SERIALISATION_TEST2( Object, Parent_Zebra_Reordered, ClassWithParent< type >, ClassWithParentReordered< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_REORDERED_TEST );

#define SERIALISATION_PARENT_REORDERED_REVERSE_TEST( type )                                                                               \
SERIALISATION_TEST2( Object, Parent_Zebra_ReorderedReverse, ClassWithParentReordered< type >, ClassWithParent< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_REORDERED_REVERSE_TEST );

SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint32_t >, uint32_t, GenerateZebraValue<uint32_t>(),
                    GetRandom<uint32_t>() );
SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint64_t >, uint64_t, GenerateZebraValue<uint64_t>(),
                    GetRandom<uint64_t>() );

#define SERIALISATION_PARENT_SKIPPING_TEST( type )                                                                               \
SERIALISATION_TEST2( Skipping, Parent, SkippedParent< type >, SinglePrimitive< uint8_t >, type, 414141 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 414141 );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_SKIPPING_TEST );

#define SERIALISATION_PARENT_NON_EXISTING_TEST( type )                                                                               \
SERIALISATION_TEST2( NonExisting, Parent, SinglePrimitive< uint8_t >, SkippedParent< type >, type, 414141 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 414141 );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_NON_EXISTING_TEST );
