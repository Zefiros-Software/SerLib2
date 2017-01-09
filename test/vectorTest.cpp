#include "testClasses.h"

#define SERIALISATION_ARRAY_TEST( type )                                            \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, 42 * sizeof( type ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );