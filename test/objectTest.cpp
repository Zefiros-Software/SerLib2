#include "testClasses.h"

#define SERIALISATION_ARRAY_TEST( type )                                                                               \
SERIALISATION_TEST( Object_Zebra, Nested, NestedObject< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );