#include "testClasses.h"

TEST( P( Exception ), boolFloat )
{
    SinglePrimitive<bool> b;
    SinglePrimitive<float> f;

    g_seed = MakeSeed<bool>( MakeSeed<float>( 414241 ) );

    b.Init();
    f.Init();

    auto test = [&]()
    {
        try
        {
            SimpleSerialiseDeserialiseStream( b, f );
        }
        catch ( InvalidTypeException e )
        {
            ExpectEqual( e.what(), InvalidTypeException( Type::Float, Type::UInt8 ).what() );
            throw e;
        }
    };

    EXPECT_THROW( test(), InvalidTypeException );
}