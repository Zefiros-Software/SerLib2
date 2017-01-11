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
#include "serialisation/stream/streamReader.h"
#include "serialisation/defines.h"

StreamReader::StreamReader( const std::string &fileName )
    : mFileStream( fileName.c_str(), std::ifstream::binary | std::ifstream::in ),
      mStream( &mFileStream )
{
    if ( !mFileStream.is_open() )
    {
        throw std::string( "File does not exist" );
    }
}

StreamReader::StreamReader( std::ifstream &stream )
    : mStream( &stream )
{
    assert( mStream->flags() & std::ios::binary );
}

StreamReader::StreamReader( std::fstream &stream )
    : mStream( &stream )
{
    assert( ( stream.flags() & std::ios::in ) && "Not an input stream" );
    assert( ( stream.flags() & std::ios::binary ) && "File stream is not in binary mode" );
}

StreamReader::StreamReader( std::istream &stream )
    : mStream( &stream )
{

}

StreamReader::~StreamReader()
{
    Close();
}

SERIALISATION_INLINE void StreamReader::ClearBuffer() const
{

}

SERIALISATION_INLINE void StreamReader::Close()
{
    ClearBuffer();

    if ( mFileStream.is_open() )
    {
        mFileStream.close();
    }
}

SERIALISATION_INLINE void StreamReader::ReadBytes( char *const firstByte, size_t byteCount ) const
{
    mStream->read( firstByte, byteCount );
}

SERIALISATION_INLINE void StreamReader::SeekG( std::streamoff count ) const
{
    mStream->seekg( count, std::ios_base::cur );
}

SERIALISATION_INLINE std::streamsize StreamReader::GCount() const
{
    return mStream->gcount();
}

SERIALISATION_INLINE void StreamReader::ClearEOF() const
{
    if ( mStream->eof() )
    {
        mStream->clear();
    }
}
