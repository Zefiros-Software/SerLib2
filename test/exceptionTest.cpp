#include "testClasses.h"

#define SERIALISATION_INCOMPATIBLE_TYPE_TEST( type1, type2 )                                                           \
TEST( P( Exception ), Incompatible ## type1 ## type2 )                                                                 \
{                                                                                                                      \
    SinglePrimitive<type1> t1;                                                                                         \
    SinglePrimitive<type2> t2;                                                                                         \
                                                                                                                       \
    g_seed = MakeSeed<type1>( MakeSeed<type2>( 414241 ) );                                                             \
                                                                                                                       \
    t1.Init();                                                                                                         \
    t2.Init();                                                                                                         \
                                                                                                                       \
    auto test = [&]()                                                                                                  \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            SimpleSerialiseDeserialiseStream( t1, t2 );                                                                \
        }                                                                                                              \
        catch ( InvalidTypeException e )                                                                               \
        {                                                                                                              \
            ExpectEqual( e.what(),                                                                                     \
                         InvalidTypeException( Type::GetHeaderEnum<type2>(), Type::GetHeaderEnum<type1>() ).what() );  \
            throw e;                                                                                                   \
        }                                                                                                              \
    };                                                                                                                 \
                                                                                                                       \
    EXPECT_THROW( test(), InvalidTypeException );                                                                      \
}

#define SERIALISATION_INCOMPATIBLE_OBJECT_TYPE_TEST( type )                                                            \
TEST(P(Exception), Incompatible ## Object ## type)                                                                     \
{                                                                                                                      \
    NestedObject<type> t1;                                                                                             \
    SinglePrimitive<type, 1> t2;                                                                                       \
                                                                                                                       \
    g_seed = MakeSeed<NestedObject<type>>( MakeSeed<type>( 414241 ) );                                                 \
                                                                                                                       \
    t1.Init();                                                                                                         \
    t2.Init();                                                                                                         \
                                                                                                                       \
    auto test = [&]()                                                                                                  \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            SimpleSerialiseDeserialiseStream( t1, t2 );                                                                \
        }                                                                                                              \
        catch ( InvalidTypeException e )                                                                               \
        {                                                                                                              \
            ExpectEqual( e.what(),                                                                                     \
                         InvalidTypeException( Type::GetHeaderEnum<type>(), Type::Object ).what() );                   \
            throw e;                                                                                                   \
        }                                                                                                              \
    };                                                                                                                 \
                                                                                                                       \
    EXPECT_THROW( test(), InvalidTypeException );                                                                      \
}

#define SERIALISATION_INCOMPATIBLE_TYPE_OBJECT_TEST( type )                                                            \
TEST(P(Exception), Incompatible ## type ## Object )                                                                    \
{                                                                                                                      \
    NestedObject<type> t2;                                                                                             \
    SinglePrimitive<type, 1> t1;                                                                                       \
                                                                                                                       \
    g_seed = MakeSeed<NestedObject<type>>( MakeSeed<type>( 414241 ) );                                                 \
                                                                                                                       \
    t1.Init();                                                                                                         \
    t2.Init();                                                                                                         \
                                                                                                                       \
    auto test = [&]()                                                                                                  \
    {                                                                                                                  \
        try                                                                                                            \
        {                                                                                                              \
            SimpleSerialiseDeserialiseStream( t1, t2 );                                                                \
        }                                                                                                              \
        catch ( InvalidTypeException e )                                                                               \
        {                                                                                                              \
            ExpectEqual( e.what(),                                                                                     \
                         InvalidTypeException( Type::Object, Type::GetHeaderEnum<type>() ).what() );                   \
            throw e;                                                                                                   \
        }                                                                                                              \
    };                                                                                                                 \
                                                                                                                       \
    EXPECT_THROW( test(), InvalidTypeException );                                                                      \
}

SERIALISATION_INCOMPATIBLE_TYPE_TEST( uint8_t, float );
SERIALISATION_INCOMPATIBLE_TYPE_TEST( uint16_t, float );
SERIALISATION_INCOMPATIBLE_TYPE_TEST( String, float );
SERIALISATION_INCOMPATIBLE_TYPE_TEST( uint8_t, double );
SERIALISATION_INCOMPATIBLE_TYPE_TEST( uint16_t, double );
SERIALISATION_INCOMPATIBLE_TYPE_TEST( String, double );

SERIALISATION_ALL_TYPES( SERIALISATION_INCOMPATIBLE_OBJECT_TYPE_TEST );
SERIALISATION_ALL_TYPES( SERIALISATION_INCOMPATIBLE_TYPE_OBJECT_TEST );