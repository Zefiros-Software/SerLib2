#include "testClasses.h"

#define SERIALISATION_ARRAY_TEST( type )                                                                               \
SERIALISATION_TEST( Object, Nested_Zebra, NestedObject< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );

#define SERIALISATION_TREE_TEST( type )                                                                                \
SERIALISATION_TEST( Object, Tree, SERIALISATION_PP_TEMPLATE2( TestClassTree, type, 5 ), type, 4242 * sizeof( type ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_TEST );