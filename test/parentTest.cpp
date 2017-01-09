#include "testClasses.h"

#define SERIALISATION_PARENT_TEST( type )                                                                               \
SERIALISATION_TEST( Object, Parent_Zebra, ClassWithParent< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_TEST );

SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint32_t >, uint32_t, GenerateZebraValue<uint32_t>(),
                    GetRandom<uint32_t>() );
SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint64_t >, uint64_t, GenerateZebraValue<uint64_t>(),
                    GetRandom<uint64_t>() );