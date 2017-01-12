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
#pragma once
#ifndef __SERIALISATION_READBUFFER_H__
#define __SERIALISATION_READBUFFER_H__

#include "serialisation/stream/streamReader.h"
#include "serialisation/defines.h"

template< size_t tBufferSize = SERIALISATION_SERIALISERS_BUFFERSIZE >
class BufferedStreamReader
{
public:

    template< typename tStream >

    explicit BufferedStreamReader( tStream &stream, bool &cleanExit )
        : mStreamReader( stream ),
          mReadIndex( 0 ),
          mReadSize( 0 ),
          mCleanExit( cleanExit )
    {
    }

    ~BufferedStreamReader()
    {
        Close();
    }

    void ClearBuffer()
    {
        mStreamReader.SeekG( static_cast< std::streamoff >( mReadIndex ) - mReadSize );
        mStreamReader.ClearEOF();

        mReadIndex = 0;
        mReadSize = 0;
    }

    void Close()
    {
        ClearBuffer();

        mStreamReader.Close();
    }

    void ReadBytes( char *const firstByte, size_t byteCount )
    {
        uint32_t diff = mReadSize - mReadIndex;

        char *c = firstByte;

        while ( diff < byteCount )
        {
            memcpy( c, mReadBuffer + mReadIndex, diff );

            mReadIndex += diff;
            c += diff;
            byteCount -= diff;

            FillReadBuffer();

            diff = mReadSize - mReadIndex;
        }

        memcpy( c, mReadBuffer + mReadIndex, byteCount );
        mReadIndex += static_cast<uint32_t>( byteCount );
    }

    void ReadBlock( char *const firstByte, size_t byteCount )
    {
        uint32_t diff = mReadSize - mReadIndex;

        if ( byteCount < diff )
        {
            memcpy( firstByte, mReadBuffer + mReadIndex, byteCount );
            mReadIndex += static_cast<uint32_t>( byteCount );
        }
        else
        {
            memcpy( firstByte, mReadBuffer + mReadIndex, diff );
            mReadIndex = mReadSize;

            mStreamReader.ReadBytes( firstByte + diff, byteCount - diff );
        }
    }

    size_t ReadSize()
    {
        size_t size = 0;
        uint8_t shift = 0;

        uint8_t byte;

        ReadPrimitive( byte );

        while ( byte & 0x80 )
        {
            size |= static_cast<size_t>( byte & 0x7F ) << shift;
            ReadPrimitive( byte );
            shift += 7;
        }

        size |= static_cast<size_t>( byte ) << shift;

        return size;
    }

    //  template< typename tPrimitive >
    //  void ReadPrimitive(tPrimitive &value)
    //  {
    //      uint32_t diff = mReadSize - mReadIndex;
    //      ReadBytes(reinterpret_cast<char *const>(&value), sizeof(tPrimitive));
    //  }

    template< typename tPrimitive >
    void ReadPrimitive( tPrimitive &value )
    {
        uint32_t diff = mReadSize - mReadIndex;

        if ( diff > sizeof( tPrimitive ) )
        {
            value = *reinterpret_cast<tPrimitive *>( mReadBuffer + mReadIndex );
            mReadIndex += sizeof( tPrimitive );
        }
        else
        {
            ReadBytes( reinterpret_cast<char *const>( &value ), sizeof( tPrimitive ) );
        }
    }

    template< typename tPrimitive >
    void ReadPrimitiveBlock( tPrimitive *first, size_t count )
    {
        const size_t maxBlockSize = std::numeric_limits< size_t >::max() / sizeof( tPrimitive );

        while ( count > 0 )
        {
            const size_t readBlockSize = count > maxBlockSize ? maxBlockSize : count;

            ReadBlock( reinterpret_cast< char *const >( first ), readBlockSize * sizeof( tPrimitive ) );
            count -= readBlockSize;
            first += readBlockSize;
        }
    }

    void Skip( size_t byteCount )
    {
        uint32_t diff = mReadSize - mReadIndex;

        if ( diff < byteCount )
        {
            mStreamReader.SeekG( byteCount - diff );
            mReadIndex = mReadSize;
        }
        else
        {
            mReadIndex += static_cast<int32_t>( byteCount );
        }
    }

private:

    char mReadBuffer[ tBufferSize ];

    StreamReader mStreamReader;

    uint32_t mReadIndex;
    uint32_t mReadSize;

    bool &mCleanExit;

    void FillReadBuffer()
    {
        mReadIndex = 0;
        mStreamReader.ReadBytes( mReadBuffer, SERIALISATION_SERIALISERS_BUFFERSIZE );
        mReadSize = static_cast<uint32_t>( mStreamReader.GCount() );

        mStreamReader.ClearEOF();

        ExceptionHelper::Assert<EndOfStreamException>( mReadSize > mReadIndex, mCleanExit );
    }

    BufferedStreamReader &operator=( const BufferedStreamReader & ) = delete;
    BufferedStreamReader( const BufferedStreamReader & ) = delete;
};

#ifndef SERIALISATION_NO_HEADER_ONLY
#   include "../../src/bufferedStreamReader.cpp"
#endif

#endif