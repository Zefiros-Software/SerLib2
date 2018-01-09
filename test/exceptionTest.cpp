/**
 * Copyright (c) 2017 Zefiros Software.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
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

TEST(P(Exception), FileOpenException)
{
    SinglePrimitive<bool> t1;

    g_seed = 21;
    t1.Init();

    auto test = [&](std::string file)
    {
        try
        {
            Serialisation::BinaryDeserialiser(file).Enter(t1);
        }
        catch (FileOpenException e)
        {
            ExpectEqual(FileOpenException(file).what(), e.what());
            throw e;
        }
    };

    std::string file = GetRandom<std::string>();

    EXPECT_THROW(test(file), FileOpenException);
}

SERIALISATION_INCOMPATIBLE_TYPE_TEST(uint8_t, float);
SERIALISATION_INCOMPATIBLE_TYPE_TEST(uint16_t, float);
SERIALISATION_INCOMPATIBLE_TYPE_TEST(String, float);
SERIALISATION_INCOMPATIBLE_TYPE_TEST(uint8_t, double);
SERIALISATION_INCOMPATIBLE_TYPE_TEST(uint16_t, double);
SERIALISATION_INCOMPATIBLE_TYPE_TEST(String, double);

SERIALISATION_ALL_TYPES(SERIALISATION_INCOMPATIBLE_OBJECT_TYPE_TEST);
SERIALISATION_ALL_TYPES(SERIALISATION_INCOMPATIBLE_TYPE_OBJECT_TEST);