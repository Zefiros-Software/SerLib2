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
#ifndef __SERIALISATION_PARALLELFLOATPROCESSOR_H__
#define __SERIALISATION_PARALLELFLOATPROCESSOR_H__

#include "serialisation/defines.h"
#include "serialisation/util.h"

#include <atomic>
#include <thread>
#include <mutex>

class ParallelFloatProcessor
{
public:

    ParallelFloatProcessor( size_t size )
        : mSourceCursor( nullptr ),
          mSourceSize( 0 ),
          mStop( false ),
          mCounter( 0 )
    {
        uint32_t nWorkers = size - 1;
        mWorkers.reserve( nWorkers );
        mMainStart = nWorkers * 2048 / size;

        for ( uint32_t pid = 0; pid < nWorkers; ++pid )
        {
            mWorkers.emplace_back( [pid, size, this]()
            {
                size_t myStart = pid * 2048 / size;
                size_t myEnd = myStart + 2048 / size;
                ++mCounter;

                while ( !mStop )
                {
                    std::unique_lock<std::mutex> lock( mLock );

                    mNotify.wait( lock, [this]()
                    {
                        return mCounter < mWorkers.size();
                    } );

                    Process( myStart, myEnd );
                    ++mCounter;

                    while ( mCounter < mWorkers.size() )
                    {
                    }
                }
            } );
        }
    }

    SERIALISATION_FORCEINLINE void SetSource( const float *cursor, size_t size )
    {
        mSourceCursor = cursor;
        mSourceSize = size;
    }

    SERIALISATION_FORCEINLINE uint32_t *GetBuffer()
    {
        return mFloatBuffer;
    }

    SERIALISATION_FORCEINLINE void Process()
    {
        while ( mCounter < mWorkers.size() )
        {}

        {
            std::unique_lock<std::mutex> lock( mLock );

            mCounter = 0;
        }

        mNotify.notify_all();

        Process( mMainStart, mSourceSize );
    }

    static ParallelFloatProcessor *GetInstance( size_t size = 0 )
    {
        static ParallelFloatProcessor *sProcessor = nullptr;

        if ( !sProcessor && size )
        {
            sProcessor = new ParallelFloatProcessor( size );
        }

        return sProcessor;
    }


private:

    std::vector<std::thread> mWorkers;

    uint32_t mFloatBuffer[2048];

    std::mutex mLock;
    std::condition_variable mNotify;
    std::atomic_bool mStop;
    std::atomic_uint32_t mCounter;

    const float *mSourceCursor;
    size_t mSourceSize;
    uint32_t mMainStart;

    SERIALISATION_FORCEINLINE void Process( uint32_t start, size_t end )
    {
        end = std::min( end, mSourceSize );

        for ( uint32_t j = start; j < end; ++j )
        {
            mFloatBuffer[j] = Util::FloatToUInt32( mSourceCursor[j] );
        }
    }
};

#endif
