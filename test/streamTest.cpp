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
        Serialisation::BinarySerialiser message1( ss );
        message1.Enter( t1 );
        message1.ClearBuffer();
        Serialisation::BinarySerialiser message2( ss );
        message2.Enter( t2 );
    }
    {
        Serialisation::BinaryDeserialiser message1( ss );
        message1.Enter( t3 );
        message1.ClearBuffer();
        Serialisation::BinaryDeserialiser message2( ss );
        message2.Enter( t4 );
    }

    t1.TestEqual( t3 );
    t2.TestEqual( t4 );
}