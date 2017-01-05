#include "testClasses.h"

class GSeedSetter
{
public:

    GSeedSetter()
    {
        g_seed = 4242;
    }
};

void TestFloatArray()
{
    TestClassArray<float> tc;
    TestClassArray<float> tc2;

    uint32_t nObj = 1;

    std::stringstream ss;
    {
        Message< BinarySerialisationMessage<BufferedStreamWriter<> > > serMessage( ss );

        for ( uint32_t i = 0; i < nObj; ++i )
        {
            serMessage.Enter( tc );
        }
    }

    ss.seekg( 0 );
    {
        Message< BinaryDeserialisationMessage<BufferedStreamReader<> > > deserMessage( ss );

        for ( uint32_t i = 0; i < nObj; ++i )
        {
            deserMessage.Enter( tc2 );
        }
    }

    tc.TestEqual( tc2 );
}

// TEST( P( BasicTest ), FloatArray )
// {
//     for ( uint32_t i = 0; i < 1000; ++i )
//     {
//         TestFloatArray();
//     }
// }

#define SERIALISATION_ARRAY_TEST( type )                                            \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, 0, 0 );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );