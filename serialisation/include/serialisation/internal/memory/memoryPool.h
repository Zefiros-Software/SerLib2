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
#ifndef __SERIALISATION_MEMORYPOOL_H__
#define __SERIALISATION_MEMORYPOOL_H__

#include <vector>

template< typename tT >
class MemoryPool
{
public:

    MemoryPool( size_t size = 10 )
        : mRemaining( size ),
          mAvailable( size ),
          mSize( size )
    {
        std::vector< tT > *init = new std::vector< tT >( size );

        size_t i = 0;

        for ( tT &t : *init )
        {
            mAvailable[i++] = &t;
        }

        mPool.push_back( init );
    }

    ~MemoryPool()
    {
        assert( mRemaining == mSize );
        mAvailable.clear();

        for ( size_t i = 0, iEnd = mPool.size(); i < iEnd; ++i )
        {
            delete mPool[i];
        }

        mPool.clear();
    }

    tT *GetInstance()
    {
        if ( !mRemaining )
        {
            Grow();
        }

        tT *t = mAvailable.back();
        mAvailable.pop_back();
        --mRemaining;

        return t;
    }

    void ReturnInstance( tT *t )
    {
        mAvailable.push_back( t );
        ++mRemaining;
    }

private:

    std::vector< std::vector< tT > * > mPool;
    std::vector< tT * > mAvailable;

    size_t mRemaining;
    size_t mSize;

    void Grow()
    {
        size_t addedSize = static_cast<size_t>( mSize * 0.6 ) + 1;

        std::vector< tT > *vec = new std::vector<tT>( addedSize );
        mAvailable.resize( mRemaining + addedSize );

        size_t i = mRemaining;

        for ( tT &t : *vec )
        {
            mAvailable[i++] = &t;
        }

        mRemaining += addedSize;
        mSize += addedSize;
    }
};

#endif
