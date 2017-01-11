/**
 * Copyright (c) 2016 Mick van Duijn, Koen Visscher and Paul Visscher
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
#include "serialisation/util.h"

template< typename tStreamWriter, size_t tBufferSize = SERIALISATION_FLOAT_BUFFER_SIZE >
class BinarySerialisationMessage
{
public:

    template< typename tStream >
    BinarySerialisationMessage( tStream &streamInitializer )
        : mStreamWriter( streamInitializer )
    {
    }

    template< typename tT >
    void Store( const tT &value, uint8_t index )
    {
        WriteHeader< tT >( index );
        WritePrimitive( value );
    }

    template< typename tT >
    void StoreVector( const std::vector< tT > &value, uint8_t index )
    {
        WriteArrayHeader< tT >( index, value.size() );
        mStreamWriter.WritePrimitiveBlock( value.data(), value.size() );
    }

    void StoreVector( const std::vector< bool > &value, uint8_t index )
    {
        WriteArrayHeader< bool >( index, value.size() );

        for ( const bool &b : value )
        {
            WritePrimitive( b );
        }
    }

    void StoreVector( const std::vector< std::string > &value, uint8_t index )
    {
        WriteArrayHeader< std::string >( index, value.size() );

        for ( const std::string &s : value )
        {
            WritePrimitive( s );
        }
    }

    SERIALISATION_NOINLINE void StoreVector( std::vector< float > &value, uint8_t index )
    {
        WriteArrayHeader< float >( index, value.size() );

        float *fCursor = &value[0];
        ParallelFloatProcessor &floatProcessor = *ParallelFloatProcessor::GetInstance( 4 );

        for ( size_t i = 0, end = value.size(); i < end; i += tBufferSize, fCursor += tBufferSize )
        {
            const size_t blockSize = std::min( tBufferSize, end - i );
            floatProcessor.SerialiseFloats( fCursor, blockSize );
            mStreamWriter.WritePrimitiveBlock( floatProcessor.GetU32Buffer(), blockSize );
        }
    }

    SERIALISATION_NOINLINE void StoreVector( std::vector< double > &value, uint8_t index )
    {
        WriteArrayHeader< double >( index, value.size() );

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
    inline void StoreObjectVector( std::vector< tSerialisable > &value, uint8_t index, tMessage &message )
    {
        WriteArrayHeader<tSerialisable>( index, value.size() );

        for ( auto &t : value )
        {
            SerialisationHelper< tSerialisable >::OnStore( message, t );
            WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
        }
    }

    template< typename tSerialisable, typename tMessage >
    inline void StoreObject( tSerialisable &serialisable, uint8_t index, tMessage &message )
    {
        WriteHeader< tSerialisable >( index );
        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );
        WritePrimitive( Util::CreateHeader( 0, Type::Terminator ) );
    }

    template< typename tSerialisable, typename tMessage >
    inline void StoreEntryPoint( tSerialisable &serialisable, tMessage &message )
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

    template< typename tT >
    void WriteHeader( uint8_t index )
    {
        WritePrimitive( Util::CreateHeader< tT >( index ) );
    }

    template< typename tT >
    void WriteArrayHeader( uint8_t index, size_t size )
    {
        WritePrimitive( Util::CreateHeader( index, Type::Array ) );
        WriteHeader< tT >( 0 );
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
};

#endif