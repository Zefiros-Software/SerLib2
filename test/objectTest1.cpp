#include "testClasses.h"

#define SERIALISATION_NESTED_TEST( type )                                                                               \
SERIALISATION_TEST( Object, Nested_Zebra, NestedObject< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_NESTED_TEST );

#define SERIALISATION_TREE_TEST( type )                                                                                \
SERIALISATION_TEST( Object, Tree, TestClassTree<type>, type, MakeSeed<type>( 4242 ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_TEST );

#define SERIALISATION_TREE_REORDERED_TEST( type )                                                                                \
SERIALISATION_TEST2( Object, TreeReordered, SERIALISATION_PP_TEMPLATE2( TestClassTreeReordered, type, 5 ), SERIALISATION_PP_TEMPLATE2( TestClassTree, type, 5 ), type, 4242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_REORDERED_TEST );

#define SERIALISATION_OBJECT_VECTOR_TEST( type )                                                    \
SERIALISATION_TEST( Object, Vector, ObjectVector< type >, type, 424242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 424242 );

SERIALISATION_ALL_TYPES( SERIALISATION_OBJECT_VECTOR_TEST );