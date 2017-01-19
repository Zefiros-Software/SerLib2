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
#include "serialisation/internal/parallelFloatProcessor.h"



#define SERIALISATION_PARALLEL_WORKER_FUNCTION( workerFunction, source, dest, job )                 \
    SERIALISATION_NOINLINE void ParallelFloatProcessor::workerFunction( size_t start, size_t end )  \
    {                                                                                               \
        end = std::min( end, mSourceSize );                                                         \
                                                                                                    \
        if ( end <= start )                                                                         \
        {                                                                                           \
            return;                                                                                 \
        }                                                                                           \
                                                                                                    \
        size_t j = start;                                                                           \
                                                                                                    \
        for ( size_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )                 \
        {                                                                                           \
            dest[j] = Util::job( source[j] );                                                       \
            dest[j + 1] = Util::job( source[j + 1] );                                               \
            dest[j + 2] = Util::job( source[j + 2] );                                               \
            dest[j + 3] = Util::job( source[j + 3] );                                               \
            dest[j + 4] = Util::job( source[j + 4] );                                               \
            dest[j + 5] = Util::job( source[j + 5] );                                               \
            dest[j + 6] = Util::job( source[j + 6] );                                               \
            dest[j + 7] = Util::job( source[j + 7] );                                               \
        }                                                                                           \
                                                                                                    \
        for ( ; j < end; ++j )                                                                      \
        {                                                                                           \
            dest[j] = Util::job( source[j] );                                                       \
        }                                                                                           \
    }

SERIALISATION_NOINLINE void ParallelFloatProcessor::WorkerSerialiseFloat( size_t start, size_t end )
{
    end = std::min( end, mSourceSize );

    if ( end <= start )
    {
        return;
    }

    size_t j = start;

    for ( size_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )
    {
        mU32Buffer[j] = Util::FloatToUInt32( mFloatSourceCursor[j] );
        mU32Buffer[j + 1] = Util::FloatToUInt32( mFloatSourceCursor[j + 1] );
        mU32Buffer[j + 2] = Util::FloatToUInt32( mFloatSourceCursor[j + 2] );
        mU32Buffer[j + 3] = Util::FloatToUInt32( mFloatSourceCursor[j + 3] );
        mU32Buffer[j + 4] = Util::FloatToUInt32( mFloatSourceCursor[j + 4] );
        mU32Buffer[j + 5] = Util::FloatToUInt32( mFloatSourceCursor[j + 5] );
        mU32Buffer[j + 6] = Util::FloatToUInt32( mFloatSourceCursor[j + 6] );
        mU32Buffer[j + 7] = Util::FloatToUInt32( mFloatSourceCursor[j + 7] );
    }

    for ( ; j < end; ++j )
    {
        mU32Buffer[j] = Util::FloatToUInt32( mFloatSourceCursor[j] );
    }
};
SERIALISATION_NOINLINE void ParallelFloatProcessor::WorkerDeserialiseFloat( size_t start, size_t end )
{
    end = std::min( end, mSourceSize );

    if ( end <= start )
    {
        return;
    }

    size_t j = start;

    for ( size_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )
    {
        mFloatSourceCursor[j] = Util::UInt32ToFloat( mU32Buffer[j] );
        mFloatSourceCursor[j + 1] = Util::UInt32ToFloat( mU32Buffer[j + 1] );
        mFloatSourceCursor[j + 2] = Util::UInt32ToFloat( mU32Buffer[j + 2] );
        mFloatSourceCursor[j + 3] = Util::UInt32ToFloat( mU32Buffer[j + 3] );
        mFloatSourceCursor[j + 4] = Util::UInt32ToFloat( mU32Buffer[j + 4] );
        mFloatSourceCursor[j + 5] = Util::UInt32ToFloat( mU32Buffer[j + 5] );
        mFloatSourceCursor[j + 6] = Util::UInt32ToFloat( mU32Buffer[j + 6] );
        mFloatSourceCursor[j + 7] = Util::UInt32ToFloat( mU32Buffer[j + 7] );
    }

    for ( ; j < end; ++j )
    {
        mFloatSourceCursor[j] = Util::UInt32ToFloat( mU32Buffer[j] );
    }
};
SERIALISATION_NOINLINE void ParallelFloatProcessor::WorkerSerialiseDouble( size_t start, size_t end )
{
    end = std::min( end, mSourceSize );

    if ( end <= start )
    {
        return;
    }

    size_t j = start;

    for ( size_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )
    {
        mU64Buffer[j] = Util::DoubleToUInt64( mDoubleSourceCursor[j] );
        mU64Buffer[j + 1] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 1] );
        mU64Buffer[j + 2] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 2] );
        mU64Buffer[j + 3] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 3] );
        mU64Buffer[j + 4] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 4] );
        mU64Buffer[j + 5] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 5] );
        mU64Buffer[j + 6] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 6] );
        mU64Buffer[j + 7] = Util::DoubleToUInt64( mDoubleSourceCursor[j + 7] );
    }

    for ( ; j < end; ++j )
    {
        mU64Buffer[j] = Util::DoubleToUInt64( mDoubleSourceCursor[j] );
    }
};
SERIALISATION_NOINLINE void ParallelFloatProcessor::WorkerDeserialiseDouble( size_t start, size_t end )
{
    end = std::min( end, mSourceSize );

    if ( end <= start )
    {
        return;
    }

    size_t j = start;

    for ( size_t i = 0, blocks = ( end - start ) / 8; i < blocks; ++i, j += 8 )
    {
        mDoubleSourceCursor[j] = Util::UInt64ToDouble( mU64Buffer[j] );
        mDoubleSourceCursor[j + 1] = Util::UInt64ToDouble( mU64Buffer[j + 1] );
        mDoubleSourceCursor[j + 2] = Util::UInt64ToDouble( mU64Buffer[j + 2] );
        mDoubleSourceCursor[j + 3] = Util::UInt64ToDouble( mU64Buffer[j + 3] );
        mDoubleSourceCursor[j + 4] = Util::UInt64ToDouble( mU64Buffer[j + 4] );
        mDoubleSourceCursor[j + 5] = Util::UInt64ToDouble( mU64Buffer[j + 5] );
        mDoubleSourceCursor[j + 6] = Util::UInt64ToDouble( mU64Buffer[j + 6] );
        mDoubleSourceCursor[j + 7] = Util::UInt64ToDouble( mU64Buffer[j + 7] );
    }

    for ( ; j < end; ++j )
    {
        mDoubleSourceCursor[j] = Util::UInt64ToDouble( mU64Buffer[j] );
    }
};

ParallelFloatProcessor::ParallelFloatProcessor( uint32_t size )
    : mBarrier( size ),
      mStop( false ),
      mCounter( 0 ),
      mFloatSourceCursor( nullptr ),
      mDoubleSourceCursor( nullptr ),
      mSourceSize( 0 ),
      mWorkerCount( static_cast<uint32_t>( size - 1 ) )
{
    uint32_t nWorkers = static_cast<uint32_t>( size - 1 );
    mWorkers.reserve( nWorkers );
    mMainEnd = static_cast<uint32_t>( SERIALISATION_FLOAT_BUFFER_SIZE / size );

    for ( uint32_t pid = 0; pid < nWorkers; ++pid )
    {
        mWorkers.emplace_back( [pid, size, this]()
        {
            size_t myStart = ( pid + 1 ) * SERIALISATION_FLOAT_BUFFER_SIZE / size;
            size_t myEnd = myStart + SERIALISATION_FLOAT_BUFFER_SIZE / size;

            mBarrier.Wait( mStop );

            while ( !mStop )
            {
                switch ( mTask )
                {
                case Task::SerFloats:
                    WorkerSerialiseFloat( myStart, myEnd );
                    break;

                case Task::SerDoubles:
                    WorkerSerialiseDouble( myStart, myEnd );
                    break;

                case Task::DeserFloats:
                    WorkerDeserialiseFloat( myStart, myEnd );
                    break;

                case Task::DeserDoubles:
                    WorkerDeserialiseDouble( myStart, myEnd );
                    break;

                default:
                    break;
                }

                ++mCounter;

                mBarrier.Wait( mStop );
            }
        } );
    }
}

uint32_t *ParallelFloatProcessor::GetU32Buffer()
{
    return mU32Buffer;
}

uint64_t *ParallelFloatProcessor::GetU64Buffer()
{
    return mU64Buffer;
}

void ParallelFloatProcessor::SerialiseFloats( float *cursor, size_t size )
{
    mFloatSourceCursor = cursor;
    mSourceSize = size;

    if ( size < 1024 )
    {
        WorkerSerialiseFloat( 0, mSourceSize );
    }
    else
    {
        StartTask( Task::SerFloats );
        WorkerSerialiseFloat( 0, mMainEnd );
        WaitTaskComplete();
    }
}

void ParallelFloatProcessor::DeserialiseFloats( float *cursor, size_t size )
{
    mFloatSourceCursor = cursor;
    mSourceSize = size;

    if ( size < 1024 )
    {
        WorkerDeserialiseFloat( 0, mSourceSize );
    }
    else
    {
        StartTask( Task::DeserFloats );
        WorkerDeserialiseFloat( 0, mMainEnd );
        WaitTaskComplete();
    }
}

void ParallelFloatProcessor::SerialiseDoubles( double *cursor, size_t size )
{
    mDoubleSourceCursor = cursor;
    mSourceSize = size;

    if ( size < 1024 )
    {
        WorkerSerialiseDouble( 0, mSourceSize );
    }
    else
    {
        StartTask( Task::SerDoubles );
        WorkerSerialiseDouble( 0, mMainEnd );
        WaitTaskComplete();
    }
}

void ParallelFloatProcessor::DeserialiseDoubles( double *cursor, size_t size )
{
    mDoubleSourceCursor = cursor;
    mSourceSize = size;

    if ( size < 1024 )
    {
        WorkerDeserialiseDouble( 0, mSourceSize );
    }
    else
    {
        StartTask( Task::DeserDoubles );
        WorkerDeserialiseDouble( 0, mMainEnd );
        WaitTaskComplete();
    }
}

void ParallelFloatProcessor::TerminateWorkers()
{
    ParallelFloatProcessor *sProcessor = GetInstance();

    if ( sProcessor )
    {
        {
            sProcessor->mStop = true;
        }
        sProcessor->mBarrier.NotifyAbort();

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

ParallelFloatProcessor *ParallelFloatProcessor::GetInstance( uint32_t size /*= 0*/, bool create /*= true */ )
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

void ParallelFloatProcessor::StartTask( Task task )
{
    mCounter = 0;
    mTask = task;
    mBarrier.Wait( mStop );
}

void ParallelFloatProcessor::WaitTaskComplete() const
{
    while ( mCounter < mWorkerCount )
    {
    }
}
