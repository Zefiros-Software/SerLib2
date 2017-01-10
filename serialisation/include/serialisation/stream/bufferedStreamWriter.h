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
#ifndef __SERIALISATION_WRITEBUFFER_H__
#define __SERIALISATION_WRITEBUFFER_H__

#include "serialisation/stream/streamWriter.h"
#include "serialisation/defines.h"

#include <cstring>

template< uint32_t tBufferSize = SERIALISATION_SERIALISERS_BUFFERSIZE, typename tStreamWriter = StreamWriter>
class BufferedStreamWriter
{
public:

    template< typename tStream >
    BufferedStreamWriter( tStream &streamInitializer )
        : mStreamWriter( streamInitializer ),
          mWriteIndex( 0 )
    {
    }

    ~BufferedStreamWriter()
    {
        Close();
    }

    void ClearBuffer()
    {
        mStreamWriter.WriteBytes( mWriteBuffer, mWriteIndex );
        mWriteIndex = 0;

        mStreamWriter.ClearBuffer();
    }

    void Close()
    {
        ClearBuffer();

        mStreamWriter.Close();
    }

    struct Buffer
    {
        const char *const firstByte;
        size_t byteCount;
    };

    void WriteBytes( const Buffer &value )
    {
        uint32_t diff = tBufferSize - mWriteIndex;

        if ( value.byteCount <= diff )
        {
            memcpy( mWriteBuffer + mWriteIndex, value.firstByte, value.byteCount );
            mWriteIndex += static_cast<uint32_t>( value.byteCount );
        }
        else
        {
            ClearBuffer();

            mStreamWriter.WriteBytes( value.firstByte, value.byteCount );
        }
    }

    void WriteSize( size_t size )
    {
        uint8_t bufferIndex;

        for ( bufferIndex = 0; size >= 0x80; size >>= 7, ++bufferIndex )
        {
            mVarIntBuffer[bufferIndex] = static_cast<uint8_t>( ( size & 0x7F ) | 0x80 );
        }

        mVarIntBuffer[bufferIndex] = static_cast<uint8_t>( size );

        WriteBytes( {reinterpret_cast<char *>( mVarIntBuffer ), ++bufferIndex} );
    }

    template< typename tPrimitive >
    void WritePrimitive( const tPrimitive &value )
    {
        uint32_t diff = tBufferSize - mWriteIndex;

        if ( sizeof( tPrimitive ) <= diff )
        {
            *reinterpret_cast< tPrimitive * >( mWriteBuffer + mWriteIndex ) = value;
            mWriteIndex += sizeof( tPrimitive );
        }
        else
        {
            WriteBytes( {reinterpret_cast< const char *const >( &value ), sizeof( tPrimitive )} );
        }
    }

    template< typename tPrimitive >
    SERIALISATION_NOINLINE void WritePrimitiveBlock( const tPrimitive *first, size_t count )
    {
        const size_t maxBlockSize = std::numeric_limits< size_t >::max() / sizeof( tPrimitive );

        while ( count > 0 )
        {
            const size_t writeBlockSize = count > maxBlockSize ? maxBlockSize : count;

            WriteBytes( {reinterpret_cast< const char *const >( first ), writeBlockSize * sizeof( tPrimitive )} );
            count -= writeBlockSize;
            first += writeBlockSize;
        }
    }

private:

    char mWriteBuffer[ tBufferSize ];

    StreamWriter mStreamWriter;

    uint8_t mVarIntBuffer[ 10 ];

    uint32_t mWriteIndex;

    BufferedStreamWriter &operator=( const BufferedStreamWriter & );
    BufferedStreamWriter( const BufferedStreamWriter & );
};

#ifndef SERIALISATION_NO_HEADER_ONLY
#   include "../../src/bufferedStreamWriter.cpp"
#endif

#endif