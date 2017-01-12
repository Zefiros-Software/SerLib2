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
#include "serialisation/serialisation.h"

#include "gtest/gtest.h"

#include <stdlib.h>

class Testclass
{
public:

    Testclass()
    {
        std::stringstream ss;
        ss << rand();

        mValue1 = rand();

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() )
        {};

        mValue3 = ss.str();
    }

    template< typename tT >
    void SERIALISATION_CUSTOM_INTERFACE( Message< tT > &message )
    {
        message.Store( 0, mValue1 );
        message.Store( 1, mValue2 );
        message.Store( 2, mValue3 );
    }

private:

    uint32_t mValue1;
    float mValue2;
    std::string mValue3;
};

// class Testclass2
//     : public ISerialisable
// {
// public:

//     Testclass2()
//     {
//         std::stringstream ss;
//         ss << rand();

//         mValue1 = rand();

//         while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() );

//         mValue3 = ss.str();
//     }

//     void SERIALISATION_CUSTOM_INTERFACE( Message &message )
//     {
//         message.Store( 3, mValue4 );
//         message.Store( 2, mValue3 );
//         message.Store( 1, mValue2 );
//         message.Store( 0, mValue1 );
//     }

// private:

//     uint32_t mValue1;
//     float mValue2;
//     std::string mValue3;
//     Testclass mValue4;
// };

class Testclass3
{
public:

    Testclass3()
    {
        std::stringstream ss;
        ss << rand();

        mValue1 = rand();

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() )
        {};

        mValue3 = ss.str();
    }

    template<typename tMessage>
    void SERIALISATION_CUSTOM_INTERFACE( tMessage &message )
    {
        message.Store( 3, mValue4 );
        message.Store( 2, mValue3 );
        message.Store( 1, mValue2 );
        message.Store( 0, mValue1 );
    }

private:

    uint32_t mValue1;
    float mValue2;
    std::string mValue3;
    Testclass mValue4;
};


class Testclass3Reordered
{
public:

    Testclass3Reordered()
    {
        std::stringstream ss;
        ss << rand();

        mValue1 = rand();

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() )
        {};

        mValue3 = ss.str();
    }

    template<typename tMessage>
    void SERIALISATION_CUSTOM_INTERFACE( tMessage &message )
    {
        message.Store( 0, mValue1 );
        message.Store( 1, mValue2 );
        message.Store( 3, mValue4 );
        //message.Store( 2, mValue3 );
    }

private:

    uint32_t mValue1;
    float mValue2;
    std::string mValue3;
    Testclass mValue4;
};


int main( int argc, char **argv )
{

#ifdef _WIN32
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
    //_crtBreakAlloc = 742;
#endif

    testing::InitGoogleTest( &argc, argv );

    int32_t result = RUN_ALL_TESTS();

    ParallelFloatProcessor::TerminateWorkers();

    return result;

    //     {
    //         std::stringstream ss;
    //
    //         Testclass3 tc;
    //         Testclass3Reordered tc2;
    //
    //         {
    //             Message< BinarySerialisationMessage< BufferedStreamWriter<> > > message( ss );
    //             message.Enter( tc );
    //         }
    //
    //         {
    //             Message< BinaryDeserialisationMessage< BufferedStreamReader<> > > message( ss );
    //             message.Enter( tc2 );
    //         }
    //     }
    //
    //     for ( volatile uint32_t i = 1; i < 100000000; ++i )
    //     {
    //         volatile uint64_t x = VarIntSize<std::numeric_limits<uint64_t>::max()>();
    //         x += i;
    //         x += VarInt<std::numeric_limits<uint64_t>::max()>().first;
    //     }
    //
    //     std::cout << VarInt<std::numeric_limits<uint64_t>::max()>().first << std::endl
    //               << VarInt<std::numeric_limits<uint64_t>::max()>().second << std::endl
    //               << 0 + VarIntSize<std::numeric_limits<uint64_t>::max()>() << std::endl;
    //
    //     std::cout << VarInt<0x7F>().first << std::endl
    //               << VarInt<0x7F>().second << std::endl
    //               << 0 + VarIntSize<0x7F>() << std::endl;
    //
    //     std::cout << VarInt<0x80>().first << std::endl
    //               << VarInt<0x80>().second << std::endl
    //               << 0 + VarIntSize<0x80>() << std::endl;
    //
    //     constexpr auto varInt = VarInt<0x83>();
    //
    //     std::cout << FromVarInt< varInt.first, varInt.second >() << std::endl;
    //
    //     const uint64_t varIntFirst = varInt.first;
    //
    //     uint64_t varIntFirstVerify = 0;
    //
    //     std::stringstream ss;
    //
    //     ss.write( reinterpret_cast<const char *>( &varInt.first ), 2 );
    //     ss.flush();
    //
    //     std::cout.write( reinterpret_cast<const char *>( &varIntFirst ), 3 );
    //     std::cout << std::endl;
    //
    //     ss.read( reinterpret_cast<char *>( &varIntFirstVerify ), 2 );
    //
    //     std::cout << varIntFirstVerify << std::endl << varIntFirst << std::endl;
    //
    //     constexpr auto y = Type::GetHeaderEnum<bool>();
    //
    //     std::cout << y << std::endl;
    //
    // #ifdef _WIN32
    //     system( "pause" );
    // #endif

    //return result;
}

