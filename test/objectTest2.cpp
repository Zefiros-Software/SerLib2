#include "testClasses.h"

#define SERIALISATION_TREE_SKIPPING_TEST( type )                                                                                \
SERIALISATION_TEST2( Skipping, ObjectTree, SERIALISATION_PP_TEMPLATE2( TestClassTree, type, 5 ), SERIALISATION_PP_TEMPLATE2( TestClassTreeSkipping, type, 5 ), type, 4242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_SKIPPING_TEST );

#define SERIALISATION_OBJECT_VECTOR_SKIPPING_TEST( type )                                                                                \
SERIALISATION_TEST2( Skipping, ObjectVector, SkippedObjectVector< type >, SinglePrimitive< uint8_t >, type, 414242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 414242 );

SERIALISATION_ALL_TYPES( SERIALISATION_OBJECT_VECTOR_SKIPPING_TEST );