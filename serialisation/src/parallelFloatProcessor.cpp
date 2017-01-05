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

SERIALISATION_PARALLEL_WORKER_FUNCTION( WorkerSerialiseFloat, mFloatSourceCursor, mU32Buffer, FloatToUInt32 );
SERIALISATION_PARALLEL_WORKER_FUNCTION( WorkerDeserialiseFloat, mU32Buffer, mFloatSourceCursor, UInt32ToFloat );
SERIALISATION_PARALLEL_WORKER_FUNCTION( WorkerSerialiseDouble, mDoubleSourceCursor, mU64Buffer, DoubleToUInt64 );
SERIALISATION_PARALLEL_WORKER_FUNCTION( WorkerDeserialiseDouble, mU64Buffer, mDoubleSourceCursor, UInt64ToDouble );

ParallelFloatProcessor::ParallelFloatProcessor( size_t size ) : mFloatSourceCursor( nullptr ),
    mDoubleSourceCursor( nullptr ),
    mSourceSize( 0 ),
    mStop( false ),
    mCounter( 0 ),
    mGeneration( 0 ),
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
            ++mCounter;
            uint32_t myGeneration = 1;

            while ( !mStop )
            {
                for ( uint32_t waitIter = 0; mGeneration < myGeneration && waitIter < 4 * 40000; ++waitIter )
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
                ++myGeneration;
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

ParallelFloatProcessor *ParallelFloatProcessor::GetInstance( size_t size /*= 0*/, bool create /*= true */ )
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

    mSpinLock.lock();
    ++mGeneration;
    mTask = task;
    mSpinLock.unlock();

    mNotify.notify_all();
}

void ParallelFloatProcessor::WaitTaskComplete()
{
    while ( mCounter < mWorkerCount )
    {
    }
}
