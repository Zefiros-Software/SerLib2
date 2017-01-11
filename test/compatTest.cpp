#include "testClasses.h"

TEST( P( Compatibility ), uint8_t )
{
    SinglePrimitive<uint8_t> t1;
    SinglePrimitive<uint16_t> t2;

    t1.Init();
    t2.Init();

    SimpleSerialiseDeserialiseStream( t1, t2 );

    t1.TestEqual( t2 );
}