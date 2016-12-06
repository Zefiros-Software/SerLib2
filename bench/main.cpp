#define BENCHMARK_HAS_CXX11
#include "benchmark/benchmark.h"

#include "serialisation/serialisation.h"
#include <sstream>
#include <chrono>

class Testclass
{
public:

    Testclass()
    {
        std::stringstream ss;
        ss << rand();

        mValue1 = rand();

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() );

        mValue3 = ss.str();
    }

    template< typename tT >
    void SERIALISATION_CUSTOM_INTERFACE( Message< tT > &message )
    {
        message.Store( 0, mValue1 );
        message.Store( 1, mValue2 );
        message.Store( 2, mValue3 );
    }

    size_t GetMemberSize()
    {
        return sizeof( uint32_t ) + sizeof( float ) + mValue3.length();
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

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() );

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



    size_t GetMemberSize()
    {
        return sizeof( uint32_t ) + sizeof( float ) + mValue3.length() + mValue4.GetMemberSize();
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

        while ( ( mValue2 = Util::UInt32ToFloat( rand() ) ) == std::numeric_limits< float >::infinity() );

        mValue3 = ss.str();
    }

    template<typename tMessage>
    void SERIALISATION_CUSTOM_INTERFACE( tMessage &message )
    {
        message.Store( 0, mValue1 );
        message.Store( 1, mValue2 );
        message.Store( 2, mValue3 );
        message.Store( 3, mValue4 );
    }

    size_t GetMemberSize()
    {
        return sizeof( uint32_t ) + sizeof( float ) + mValue3.length() + mValue4.GetMemberSize();
    }

private:

    uint32_t mValue1;
    float mValue2;
    std::string mValue3;
    Testclass mValue4;
};

// template< typename tImpl >
// static void BM_OLS( benchmark::State &state )
// {
//     double a = Random::GenerateRandomF64( -NUMERICAL_OLS_MAX_SAMPLES, NUMERICAL_OLS_MAX_SAMPLES );
//     double b = a + Random::GenerateRandomF64( -NUMERICAL_OLS_MAX_SAMPLES, NUMERICAL_OLS_MAX_SAMPLES );

//     double alpha = Random::GenerateRandomF64( -NUMERICAL_OLS_MAX_SAMPLES, NUMERICAL_OLS_MAX_SAMPLES );
//     double beta = Random::GenerateRandomF64( -10000, 10000 );

//     vec ex = linspace( a, b, state.range_x() );
//     vec end = ex * beta + alpha;

//     while ( state.KeepRunning() )
//     {
//         LinReg<tImpl> ols( end, ex );
//         ols.Fit();
//     }

//     state.SetItemsProcessed( state.iterations() );
//     state.SetBytesProcessed( state.iterations() * state.range_x() * sizeof( double ) );
//     state.SetComplexityN( state.range_x() );
// }

// BENCHMARK_TEMPLATE( BM_OLS, LinealOLSImpl )->Range( 10, 20000 )->Complexity();
// BENCHMARK_TEMPLATE( BM_OLS, QROLSImpl )->Range( 10, 20000 )->Complexity();
// BENCHMARK_TEMPLATE( BM_OLS, SVDOLSImpl )->Range( 10, 20000 )->Complexity();
// BENCHMARK_TEMPLATE( BM_OLS, FGLSImpl )->Range( 10, 20000 )->Complexity();

template< typename tFunc >
static inline void TimeManual( benchmark::State &state, const tFunc &func )
{
    auto start = std::chrono::high_resolution_clock::now();

    func();

    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(
            end - start );

    state.SetIterationTime( elapsed_seconds.count() );
}

static void BM_VarInt( benchmark::State &state )
{
    while ( state.KeepRunning() )
    {
        benchmark::DoNotOptimize( VarInt<std::numeric_limits<uint64_t>::max()>() );
    }

    state.SetItemsProcessed( state.iterations() );
    state.SetBytesProcessed( state.iterations() * sizeof( uint64_t ) );
}

static void BM_VarIntSize( benchmark::State &state )
{
    while ( state.KeepRunning() )
    {
        benchmark::DoNotOptimize( VarIntSize<std::numeric_limits<uint64_t>::max()>() );
    }

    state.SetItemsProcessed( state.iterations() );
    state.SetBytesProcessed( state.iterations() * sizeof( uint64_t ) );
}

static void BM_StoreU32( benchmark::State &state )
{
    uint32_t x = static_cast<uint32_t>( state.range_x() );
    {
        std::stringstream ss;
        Message< BinarySerialisationMessage< BufferedStreamWriter<> > > message( ss );

        while ( state.KeepRunning() )
        {
            message.Store( 0, x );
        }
    }

    state.SetItemsProcessed( state.iterations() );
    state.SetBytesProcessed( state.iterations() * sizeof( uint64_t ) );
    state.SetComplexityN( static_cast<int>( x ) );
}

static void BM_CreateHeaderCT( benchmark::State &state )
{
    while ( state.KeepRunning() )
    {
        benchmark::DoNotOptimize( Util::CreateHeader<uint32_t>( 0 ) );
    }

    state.SetItemsProcessed( state.iterations() );
    state.SetBytesProcessed( state.iterations() * sizeof( uint64_t ) );
}

static void BM_CreateHeader( benchmark::State &state )
{
    volatile uint8_t index = 0;

    while ( state.KeepRunning() )
    {
        benchmark::DoNotOptimize( Util::CreateHeader<uint32_t>( index ) );
    }

    state.SetItemsProcessed( state.iterations() );
    state.SetBytesProcessed( state.iterations() * sizeof( uint64_t ) );
}

static void BM_NormalNestedDeser( benchmark::State &state )
{
    Testclass3 tc, tc2;

    uint32_t nObj = 200;

    std::stringstream ssSource;
    {
        Serialisation::BinarySerialiser serMessage( ssSource );

        for ( uint32_t i = 0; i < nObj; ++i )
        {
            serMessage.Enter( tc );
        }
    }

    while ( state.KeepRunning() )
    {
        {
            std::stringstream ss( ssSource.str() );
            Message< BinaryDeserialisationMessage< BufferedStreamReader<> > > deserMessage( ss );

            TimeManual( state, [&]()
            {
                for ( uint32_t i = 0; i < nObj; ++i )
                {
                    deserMessage.Enter( tc2 );
                }
            } );
        }
    }

    state.SetItemsProcessed( state.iterations() * nObj );
    state.SetBytesProcessed( state.iterations() * nObj * tc.GetMemberSize() );
}

static void BM_ReorderedNestedDeser( benchmark::State &state )
{
    Testclass3 tc;
    Testclass3Reordered tc2;

    uint32_t nObj = 200;

    std::stringstream ssSource;
    {
        Serialisation::BinarySerialiser serMessage( ssSource );

        for ( uint32_t i = 0; i < nObj; ++i )
        {
            serMessage.Enter( tc );
        }
    }

    while ( state.KeepRunning() )
    {
        {
            std::stringstream ss( ssSource.str() );
            Message< BinaryDeserialisationMessage< BufferedStreamReader<> > > deserMessage( ss );

            TimeManual( state, [&]()
            {
                for ( uint32_t i = 0; i < nObj; ++i )
                {
                    deserMessage.Enter( tc2 );
                }
            } );
        }
    }

    state.SetItemsProcessed( state.iterations() * nObj );
    state.SetBytesProcessed( state.iterations() * nObj * tc.GetMemberSize() );
}

static void BM_NormalNestedSer( benchmark::State &state )
{
    Testclass3 tc;

    uint32_t nObj = 200;

    while ( state.KeepRunning() )
    {
        {
            std::stringstream ss;
            Message< BinarySerialisationMessage<BufferedStreamWriter<> > > serMessage( ss );

            TimeManual( state, [&]()
            {
                for ( uint32_t i = 0; i < nObj; ++i )
                {
                    serMessage.Enter( tc );
                }
            } );
        }
    }

    state.SetItemsProcessed( state.iterations() * nObj );
    state.SetBytesProcessed( state.iterations() * nObj * tc.GetMemberSize() );
}

template< typename tT >
class TestClassArray
{
public:

    TestClassArray()
    {
        mValue1.resize( ( rand() % 8192 ) + 4096 );

        for ( auto it = mValue1.begin(); it != mValue1.end(); ++it )
        {
            *it = static_cast<tT>( rand() );
        }
    }

    template< typename tT >
    void OnStore( Message< tT > &message )
    {
        message.Store( 0, mValue1 );
    }

    size_t GetMemberSize()
    {
        return mValue1.size() * sizeof( float );
    }



private:

    std::vector<tT> mValue1;
};

static void BM_FloatArraySer( benchmark::State &state )
{
    TestClassArray<float> tc;

    uint32_t nObj = 1;

    while ( state.KeepRunning() )
    {
        {
            std::stringstream ss;
            Message< BinarySerialisationMessage<BufferedStreamWriter<> > > serMessage( ss );

            TimeManual( state, [&]()
            {
                for ( uint32_t i = 0; i < nObj; ++i )
                {
                    serMessage.Enter( tc );
                }
            } );
        }
    }

    state.SetItemsProcessed( state.iterations() * nObj );
    state.SetBytesProcessed( state.iterations() * nObj * tc.GetMemberSize() );
}

//BENCHMARK( BM_NormalNestedDeser )->MinTime( 5.0 );
//BENCHMARK( BM_NormalNestedSer )->MinTime( 5.0 );
// BENCHMARK( BM_NormalNestedSer )->MinTime( 2.0 );
// BENCHMARK( BM_NormalNestedDeser )->MinTime( 2.0 );
// BENCHMARK( BM_NormalNestedSer )->MinTime( 2.0 );
// BENCHMARK( BM_NormalNestedDeser )->MinTime( 2.0 );
// BENCHMARK( BM_NormalNestedSer )->MinTime( 2.0 );
BENCHMARK( BM_FloatArraySer )->MinTime( 4.0 );

BENCHMARK_MAIN();