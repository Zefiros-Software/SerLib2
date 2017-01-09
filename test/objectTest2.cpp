#include "testClasses.h"

#define SERIALISATION_TREE_REORDERED_TEST( type )                                                                                \
SERIALISATION_TEST2( Skipping, ObjectTree, SERIALISATION_PP_TEMPLATE2( TestClassTree, type, 5 ), SERIALISATION_PP_TEMPLATE2( TestClassTreeSkipping, type, 5 ), type, 4242 * ( std::is_same< type, std::string >::value ? 32 : sizeof( type ) ), 4241 );

SERIALISATION_ALL_TYPES( SERIALISATION_TREE_REORDERED_TEST );