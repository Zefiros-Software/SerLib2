#include "testClasses.h"

template< typename tT >
class TestClassArray
{
public:

    TestClassArray()
    {
        mValue1.resize( ( rand() % 8192 ) + 8192 );

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
        return mValue1.size() * sizeof( tT );
    }

    void TestEqual( TestClassArray< tT > &t2 )
    {
        ExpectEqual( mValue1.size(), t2.mValue1.size() );

        for ( size_t i = 0, end = mValue1.size(); i < end; ++i )
        {
            ExpectEqual( mValue1[i], t2.mValue1[i] );
        }
    }

private:

    std::vector<tT> mValue1;
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

TEST( P( BasicTest ), FloatArray )
{
    for ( uint32_t i = 0; i < 1000; ++i )
    {
        TestFloatArray();
    }
}