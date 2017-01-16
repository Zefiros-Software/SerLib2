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
#pragma once
#ifndef __SERIALISATION_BINARYSERMESSAGE_H__
#define __SERIALISATION_BINARYSERMESSAGE_H__

#include "serialisation/internal/parallelFloatProcessor.h"
#include "serialisation/message.h"
#include "serialisation/util.h"

template< typename tStreamWriter, size_t tBufferSize >
class BinarySerialisationHeaderMessage;

template< typename tStreamWriter, size_t tBufferSize >
class BinarySerialisationValueMessage;

template< typename tStreamWriter, size_t tBufferSize = SERIALISATION_FLOAT_BUFFER_SIZE >
class BinarySerialisationMessage
{
public:

    template< typename tS, size_t tB >
    friend class BinarySerialisationHeaderMessage;

    template< typename tS, size_t tB >
    friend class BinarySerialisationValueMessage;

    typedef Message< BinarySerialisationHeaderMessage< tStreamWriter, tBufferSize > > tHeaderMessage;
    typedef Message< BinarySerialisationHeaderMessage< tStreamWriter, tBufferSize > > tValueMessage;

    template< typename tStream >
    explicit BinarySerialisationMessage( tStream &streamInitializer )
        : mStreamWriter( streamInitializer )
    {
    }

    template< typename tT >
    void Store( const tT &value, uint8_t index, uint8_t /*flags*/ )
    {
        WriteHeader< tT >( index );
        WritePrimitive( value );
    }

    template< typename tT >
    void StoreVector( std::vector< tT > &value, uint8_t index, uint8_t flags )
    {
        WriteArrayHeader< tT >( index, value.size() );
        WriteVectorBody( value, flags );
    }

    void StoreVector( std::vector< bool > &value, uint8_t index, uint8_t flags )
    {
        flags = flags & 0x1;
        WriteArrayHeader< bool >( index, value.size(), flags );
        WriteVectorBody( value, flags );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObjectVector( std::vector< tSerialisable > &value, uint8_t index, uint8_t flags, tMessage &message )
    {
        flags = flags & 0x1;
        WriteArrayHeader<tSerialisable>( index, value.size(), flags );

        if ( flags )
        {
            {
                tHeaderMessage( *this ).Enter( value[0] );
            }

            tValueMessage tempMessage( *this );

            for ( auto &t : value )
            {
                tempMessage.Enter( t );
            }
        }
        else
        {
            WriteObjectVectorBody( value, flags, message );
        }
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject( tSerialisable &serialisable, uint8_t index, uint8_t /*flags*/, tMessage &message )
    {
        WriteHeader< tSerialisable >( index );
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
        WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint( tSerialisable &serialisable, tMessage &message )
    {
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
        WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
    }

    void ClearBuffer()
    {
        mStreamWriter.ClearBuffer();
    }

private:

    tStreamWriter mStreamWriter;

    uint8_t mBoolPackBuffer[1024];

    template< typename tT >
    void WriteHeader( uint8_t index )
    {
        WritePrimitive( Util::CreateHeader< tT >( index ) );
    }

    template< typename tT >
    void WriteArrayHeader( uint8_t index, size_t size, uint8_t flags = 0 )
    {
        WritePrimitive( Util::CreateHeader( index, Type::Array ) );
        WriteHeader< tT >( flags );
        mStreamWriter.WriteSize( size );
    }

    template< typename tT >
    void WritePrimitive( const  tT &value )
    {
        mStreamWriter.WritePrimitive( value );
    }

    void WritePrimitive( const bool &value )
    {
        uint8_t val = value ? 1 : 0;
        WritePrimitive( val );
    }

    void WritePrimitive( const float &value )
    {
        uint32_t flexman = Util::FloatToUInt32( value );
        WritePrimitive( flexman );
    }

    void WritePrimitive( const double &value )
    {
        uint64_t flexman = Util::DoubleToUInt64( value );
        WritePrimitive( flexman );
    }

    void WritePrimitive( const std::string &value )
    {
        mStreamWriter.WriteSize( value.size() );
        mStreamWriter.WriteBytes( {value.c_str(), value.size()} );
    }

    template< typename tT >
    void WriteVectorBody( std::vector< tT > &value, uint8_t /*flags*/ )
    {
        mStreamWriter.WritePrimitiveBlock( value.data(), value.size() );
    }

    void WriteVectorBody( std::vector<bool> &value, uint8_t flags )
    {
        if ( flags )
        {
            size_t j = 0;

            constexpr uint8_t tBit = 0x1;
            constexpr uint8_t fBit = 0x0;

            const size_t size = value.size();

            for ( size_t k = 0, kEnd = ( size + 1023 ) / 1024; k < kEnd; ++k )
            {
                size_t i = 0;

                for ( size_t end = std::min( ( size - j ) / 8, size_t( 1024 ) ); i < end; ++i, j += 8 )
                {
                    mBoolPackBuffer[i] = value[j] ? tBit : fBit;
                    mBoolPackBuffer[i] |= value[j + 1] ? ( tBit << 1 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 2] ? ( tBit << 2 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 3] ? ( tBit << 3 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 4] ? ( tBit << 4 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 5] ? ( tBit << 5 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 6] ? ( tBit << 6 ) : fBit;
                    mBoolPackBuffer[i] |= value[j + 7] ? ( tBit << 7 ) : fBit;
                }

                mStreamWriter.WritePrimitiveBlock( mBoolPackBuffer, i );
            }

            if ( j < size )
            {
                uint8_t temp = 0;
                temp = value[j] ? tBit : fBit;
                ++j;

                for ( size_t k = 1; j < size; ++j, ++k )
                {
                    temp |= value[j] ? ( tBit << k ) : fBit;
                }

                mStreamWriter.WritePrimitive( temp );
            }
        }
        else
        {
            for ( const bool &b : value )
            {
                WritePrimitive( b );
            }
        }
    }

    void WriteVectorBody( std::vector<std::string> &value, uint8_t /*flags*/ )
    {
        for ( const std::string &s : value )
        {
            WritePrimitive( s );
        }
    }
    void WriteVectorBody( std::vector< float > &value, uint8_t /*flags*/ )
    {
        float *fCursor = &value[0];
        ParallelFloatProcessor &floatProcessor = *ParallelFloatProcessor::GetInstance( 4 );

        for ( size_t i = 0, end = value.size(); i < end; i += tBufferSize, fCursor += tBufferSize )
        {
            const size_t blockSize = std::min( tBufferSize, end - i );
            floatProcessor.SerialiseFloats( fCursor, blockSize );
            mStreamWriter.WritePrimitiveBlock( floatProcessor.GetU32Buffer(), blockSize );
        }
    }

    void WriteVectorBody( std::vector< double > &value, uint8_t /*flags*/ )
    {
        double *fCursor = &value[0];
        ParallelFloatProcessor &floatProcessor = *ParallelFloatProcessor::GetInstance( 4 );

        for ( size_t i = 0, end = value.size(); i < end;  i += tBufferSize, fCursor += tBufferSize )
        {
            const size_t blockSize = std::min( tBufferSize, end - i );
            floatProcessor.SerialiseDoubles( fCursor, blockSize );
            mStreamWriter.WritePrimitiveBlock( floatProcessor.GetU64Buffer(), blockSize );
        }
    }

    template< typename tSerialisable, typename tMessage >
    void WriteObjectVectorBody( std::vector<tSerialisable> &value, uint8_t /*flags*/, tMessage &message )
    {
        for ( auto &t : value )
        {
            SerialisationHelper< tSerialisable >::OnStore( message, t );
            WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
        }
    }
};

template< typename tStreamWriter, size_t tBufferSize = SERIALISATION_FLOAT_BUFFER_SIZE >
class BinarySerialisationHeaderMessage
{
public:

    explicit BinarySerialisationHeaderMessage( BinarySerialisationMessage< tStreamWriter, tBufferSize > &message )
        : mMessage( message )
    {
    }

    template< typename tT >
    void Store( const tT &/*value*/, uint8_t index, uint8_t /*flags*/ )
    {
        mMessage.template WriteHeader< tT >( index );
    }

    template< typename tT >
    void StoreVector( std::vector< tT > &value, uint8_t index, uint8_t flags )
    {
        mMessage.template WriteArrayHeader< tT >( index, value.size(), flags );
    }

    void StoreVector( std::vector< bool > &value, uint8_t index, uint8_t flags )
    {
        flags = flags & 0x1;
        mMessage.template WriteArrayHeader( index, value.size(), flags );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject( tSerialisable &/*serialisable*/, uint8_t index, uint8_t /*flags*/, tMessage &/*message*/ )
    {
        mMessage.template WriteHeader< tSerialisable >( index );
        mMessage.WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObjectVector( std::vector< tSerialisable > &value, uint8_t index, uint8_t flags,
                            tMessage &/*message*/ )
    {
        flags = flags & 0x0;
        mMessage.template WriteArrayHeader<tSerialisable>( index, value.size(), flags );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint( tSerialisable &serialisable, tMessage &message )
    {
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
        mMessage.WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
    }

    void ClearBuffer()
    {
        mMessage.ClearBuffer();
    }

private:

    BinarySerialisationMessage< tStreamWriter, tBufferSize > &mMessage;
};

template< typename tStreamWriter, size_t tBufferSize = SERIALISATION_FLOAT_BUFFER_SIZE >
class BinarySerialisationValueMessage
{
public:

    explicit BinarySerialisationValueMessage( BinarySerialisationMessage< tStreamWriter, tBufferSize > &message )
        : mMessage( message )
    {
    }

    template< typename tT >
    void Store( const tT &value, uint8_t /*index*/, uint8_t /*flags*/ )
    {
        mMessage.WritePrimitive( value );
    }

    template< typename tT >
    void StoreVector( std::vector< tT > &value, uint8_t /*index*/, uint8_t flags )
    {
        mMessage.WriteVectorBody( value, flags );
    }

    void StoreVector( std::vector< bool > &value, uint8_t /*index*/, uint8_t flags )
    {
        flags = flags & 0x1;
        mMessage.WriteVectorBody( value, flags );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObjectVector( std::vector< tSerialisable > &value, uint8_t /*index*/, uint8_t flags, tMessage &message )
    {
        mMessage.WriteObjectVectorBody( value, flags, message );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject( tSerialisable &serialisable, uint8_t /*index*/, uint8_t /*flags*/, tMessage &message )
    {
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint( tSerialisable &serialisable, tMessage &message )
    {
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
    }

    void ClearBuffer()
    {
    }

private:
    BinarySerialisationMessage< tStreamWriter, tBufferSize > &mMessage;
};

#endif