#include "testClasses.h"

class GSeedSetter
{
public:

    GSeedSetter()
    {
        g_seed = 4242;
    }
};

#define SERIALISATION_ARRAY_TEST( type )                                            \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, 0, 0 );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );