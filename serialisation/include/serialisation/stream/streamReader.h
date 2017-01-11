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
#ifndef __SERIALISATION_STREAMREADER_H__
#define __SERIALISATION_STREAMREADER_H__

#include <fstream>
#include <limits>

class StreamReader
{
public:

    explicit StreamReader( const std::string &fileName );

    explicit StreamReader( std::ifstream &stream );

    explicit StreamReader( std::fstream &stream );

    explicit StreamReader( std::istream &stream );

    ~StreamReader();

    void ClearBuffer() const;

    void Close();

    void ReadBytes( char *const firstByte, size_t byteCount ) const;

    void ReadBlock( char *const firstByte, size_t byteCount ) const;

    size_t ReadSize();

    template< typename tPrimitive >
    void ReadPrimitive( tPrimitive &value )
    {
        ReadBytes( reinterpret_cast< char *const >( &value ), sizeof( tPrimitive ) );
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

    void Skip( size_t byteCount ) const
    {
        SeekG( byteCount );
    }
    void SeekG( std::streamoff count ) const;

    std::streamsize GCount() const;

    void ClearEOF() const;

private:

    std::ifstream mFileStream;
    std::istream *mStream;

    StreamReader &operator=( const StreamReader & ) = delete;
    StreamReader( const StreamReader & ) = delete;

};

#ifndef SERIALISATION_NO_HEADER_ONLY
#   include "../../src/streamReader.cpp"
#endif

#endif