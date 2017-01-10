#include "testClasses.h"

TEST( P( Stream ), SuccessiveMessagesInterrupted )
{
    g_seed = 4241;
    std::stringstream ss;
    SinglePrimitive<uint32_t> t1, t2, t3, t4;
    t1.Init();
    t2.Init();
    t3.Init();
    t4.Init();

    {
        Serialisation::BinarySerialiser message( ss );
        message.Enter( t1 );
    }
    {
        Serialisation::BinarySerialiser message( ss );
        message.Enter( t2 );
    }
    {
        Serialisation::BinaryDeserialiser message( ss );
        message.Enter( t3 );
    }
    {
        Serialisation::BinaryDeserialiser message( ss );
        message.Enter( t4 );
    }

    t1.TestEqual( t3 );
    t2.TestEqual( t4 );
}