#include "testClasses.h"

#define SERIALISATION_TREE_SKIPPING_TEST( type )                                                                                \
SERIALISATION_TEST2( Skipping, ObjectTree, SERIALISATION_PP_TEMPLATE2( TestClassTree, type, 5 ), SERIALISATION_PP_TEMPLATE2( TestClassTreeSkipping, type, 5 ), type, 4242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_SKIPPING_TEST );

#define SERIALISATION_OBJECT_VECTOR_SKIPPING_TEST( type )                                                       \
SERIALISATION_TEST2( Skipping, ObjectVector, SkippedObjectVector< type >, SinglePrimitive< uint8_t >, type,     \
                     MakeSeed<type>( 414242 ), static_cast<uint8_t>( 414242 ) );

SERIALISATION_ALL_TYPES( SERIALISATION_OBJECT_VECTOR_SKIPPING_TEST );

#define SERIALISATION_OBJECT_VECTORNON_EXISTINGG_TEST( type )                                                   \
SERIALISATION_TEST2( NonExisting, ObjectVector, SinglePrimitive< uint8_t >, SkippedObjectVector< type >, type,  \
                     SERIALISATION_PP_TEMPLATE2(MakeSeed, type, uint8_t)( 414242 ), 414242 );

SERIALISATION_ALL_TYPES( SERIALISATION_OBJECT_VECTORNON_EXISTINGG_TEST );
