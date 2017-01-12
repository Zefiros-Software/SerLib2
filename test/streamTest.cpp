/**
 * Copyright (c) 2017 Mick van Duijn, Koen Visscher and Paul Visscher
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

TEST( P( Stream ), CorruptStreamTest )
{
    g_seed = 4241;
    std::stringstream ss;
    SinglePrimitive<uint32_t> t1, t2;
    t1.Init();
    t2.Init();

    {
        Serialisation::BinarySerialiser message( ss );
        message.Enter( t1 );
    }

    {
        std::string temp = ss.str();
        temp.resize( temp.size() - 1 );
        ss.str( temp );
    }

    {
        Serialisation::BinaryDeserialiser message( ss );

        auto test1 = [&]()
        {
            try
            {
                message.Enter( t2 );
            }
            catch ( EndOfStreamException e )
            {
                ExpectEqual( EndOfStreamException().what(), e.what() );
                throw e;
            }
        };

        auto test2 = [&]()
        {
            try
            {
                message.Enter( t2 );
            }
            catch ( NoCleanExitException e )
            {
                ExpectEqual( NoCleanExitException().what(), e.what() );
                throw e;
            }
        };

        EXPECT_THROW( test1(), EndOfStreamException );
        EXPECT_THROW( test2(), NoCleanExitException );

        // despite unclean exit, this should still be equal in this case
        t1.TestEqual( t2 );
    }
}

TEST( P( Stream ), CorruptStreamTestTree )
{
    g_seed = 4241;
    std::stringstream ss;
    TestClassTree<uint32_t, 5> t1, t2;

    {
        Serialisation::BinarySerialiser message( ss );
        message.Enter( t1 );
    }

    {
        std::string temp = ss.str();
        temp.resize( temp.size() - 2 );
        ss.str( temp );
    }

    {
        Serialisation::BinaryDeserialiser message( ss );

        auto test1 = [&]()
        {
            try
            {
                message.Enter( t2 );
            }
            catch ( EndOfStreamException e )
            {
                ExpectEqual( EndOfStreamException().what(), e.what() );
                throw e;
            }
        };

        auto test2 = [&]()
        {
            try
            {
                message.Enter( t2 );
            }
            catch ( NoCleanExitException e )
            {
                ExpectEqual( NoCleanExitException().what(), e.what() );
                throw e;
            }
        };

        EXPECT_THROW( test1(), EndOfStreamException );
        EXPECT_THROW( test2(), NoCleanExitException );

        // despite unclean exit, this should still be equal in this case
        t1.TestEqual( t2 );
    }
}

#define SERIALISATION_TEST_IFSTREAM_OFSTREAM( type )                                        \
TEST( P( Stream ), IFStream_OFStream_ ## type )                                             \
{                                                                                           \
    SinglePrimitive< type > t1, t2;                                                         \
    g_seed = MakeSeed<type>( 414243 );                                                      \
    t1.Init();                                                                              \
    t2.Init();                                                                              \
                                                                                            \
    {                                                                                       \
        std::ofstream stream( "file.bin", std::ofstream::binary | std::ofstream::out );     \
        Serialisation::BinarySerialiser( stream ).Enter( t1 );                              \
    }                                                                                       \
    {                                                                                       \
        std::ifstream stream( "file.bin", std::ifstream::binary | std::ifstream::in );      \
        Serialisation::BinaryDeserialiser( stream ).Enter( t2 );                            \
    }                                                                                       \
                                                                                            \
    t1.TestEqual( t2 );                                                                     \
}

#define SERIALISATION_TEST_FSTREAM( type )                                                  \
TEST( P( Stream ), FStream_ ## type )                                                       \
{                                                                                           \
    SinglePrimitive< type > t1, t2;                                                         \
    g_seed = MakeSeed<type>( 414243 );                                                      \
    t1.Init();                                                                              \
    t2.Init();                                                                              \
                                                                                            \
    {                                                                                       \
        std::fstream stream( "file.bin", std::fstream::binary | std::fstream::out );        \
        Serialisation::BinarySerialiser( stream ).Enter( t1 );                              \
    }                                                                                       \
    {                                                                                       \
        std::fstream stream( "file.bin", std::fstream::binary | std::fstream::in );         \
        Serialisation::BinaryDeserialiser( stream ).Enter( t2 );                            \
    }                                                                                       \
                                                                                            \
    t1.TestEqual( t2 );                                                                     \
}

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_IFSTREAM_OFSTREAM );
SERIALISATION_ALL_TYPES( SERIALISATION_TEST_FSTREAM );