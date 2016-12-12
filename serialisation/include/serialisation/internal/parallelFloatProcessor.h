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

#include "serialisation/internal/spinLock.h"
#include "serialisation/defines.h"
#include "serialisation/util.h"

#include <condition_variable>
#include <vector>
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
          mCounter( 0 ),
          mGeneration( 0 ),
          mWorkerCount( size - 1 )
    {
        uint32_t nWorkers = size - 1;
        mWorkers.reserve( nWorkers );
        mMainEnd = SERIALISATION_FLOAT_BUFFER_SIZE / size;

        for ( uint32_t pid = 0; pid < nWorkers; ++pid )
        {
            mWorkers.emplace_back( [pid, size, this]()
            {
                size_t myStart = ( pid + 1 ) * SERIALISATION_FLOAT_BUFFER_SIZE / size;
                size_t myEnd = myStart + SERIALISATION_FLOAT_BUFFER_SIZE / size;
                ++mCounter;
                uint32_t myGeneration = 1;

                while ( !mStop )
                {
                    for ( uint32_t waitIter = 0; mGeneration < myGeneration && waitIter < 40000; ++waitIter )
                    {
                    }

                    if ( mGeneration < myGeneration )
                    {

                        mSpinLock.lock();
                        uint32_t generation = mGeneration;

                        uint32_t iter = 1;

                        while ( generation < myGeneration && !mStop )
                        {
                            uint32_t duration = std::max( 10u, iter );
                            mNotify.wait_for( mSpinLock, std::chrono::milliseconds( duration ), [&]()
                            {
                                return myGeneration <= mGeneration || mStop;
                            } );

                            ++iter;
                            generation = mGeneration;
                        }

                        mSpinLock.unlock();

                        if ( mStop )
                        {
                            break;
                        }
                    }

                    WorkerProcess( myStart, myEnd );
                    ++mCounter;
                    ++myGeneration;
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
        mCounter = 0;

        mSpinLock.lock();
        ++mGeneration;
        mSpinLock.unlock();

        mNotify.notify_all();

        WorkerProcess( 0, mMainEnd );

        while ( mCounter < mWorkerCount )
        {}
    }

    SERIALISATION_FORCEINLINE void ProcessSequential()
    {
        WorkerProcess( 0, mSourceSize );
    }

    static void TerminateWorkers()
    {
        ParallelFloatProcessor *sProcessor = GetInstance();

        if ( sProcessor )
        {
            {
                sProcessor->mStop = true;
            }
            sProcessor->mNotify.notify_all();

            for ( auto &t : sProcessor->mWorkers )
            {
                if ( t.joinable() )
                {
                    t.join();
                }
            }

            GetInstance( 0, false );
        }
    }

    static ParallelFloatProcessor *GetInstance( size_t size = 0, bool create = true )
    {
        static ParallelFloatProcessor *sProcessor = nullptr;

        if ( !sProcessor && size )
        {
            sProcessor = new ParallelFloatProcessor( size );
        }

        if ( !create )
        {
            delete sProcessor;
            sProcessor = nullptr;
        }

        return sProcessor;
    }

private:

    std::vector<std::thread> mWorkers;

    uint32_t mFloatBuffer[SERIALISATION_FLOAT_BUFFER_SIZE];

    std::mutex mLock;
    SpinLock mSpinLock;;
    std::condition_variable_any mNotify;
    std::atomic_bool mStop;
    std::atomic<uint32_t> mCounter;
    std::atomic<uint32_t> mGeneration;

    const float *mSourceCursor;
    size_t mSourceSize;
    uint32_t mMainEnd;
    uint32_t mWorkerCount;

    SERIALISATION_NOINLINE void WorkerProcess( uint32_t start, size_t end )
    {
        end = std::min( end, mSourceSize );

        if ( end <= start )
        {
            return;
        }

        uint32_t j = start;

        for ( uint32_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )
        {
            mFloatBuffer[j] = Util::FloatToUInt32( mSourceCursor[j] );
            mFloatBuffer[j + 1] = Util::FloatToUInt32( mSourceCursor[j + 1] );
            mFloatBuffer[j + 2] = Util::FloatToUInt32( mSourceCursor[j + 2] );
            mFloatBuffer[j + 3] = Util::FloatToUInt32( mSourceCursor[j + 3] );
            mFloatBuffer[j + 4] = Util::FloatToUInt32( mSourceCursor[j + 4] );
            mFloatBuffer[j + 5] = Util::FloatToUInt32( mSourceCursor[j + 5] );
            mFloatBuffer[j + 6] = Util::FloatToUInt32( mSourceCursor[j + 6] );
            mFloatBuffer[j + 7] = Util::FloatToUInt32( mSourceCursor[j + 7] );
        }

        for ( ; j < end; ++j )
        {
            mFloatBuffer[j] = Util::FloatToUInt32( mSourceCursor[j] );
        }
    }
};

#endif
