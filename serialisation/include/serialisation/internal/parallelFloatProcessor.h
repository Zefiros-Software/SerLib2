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

    ParallelFloatProcessor( size_t size );

    uint32_t *GetU32Buffer();
    uint64_t *GetU64Buffer();

    void SerialiseFloats( float *cursor, size_t size );
    void DeserialiseFloats( float *cursor, size_t size );

    void SerialiseDoubles( double *cursor, size_t size );
    void DeserialiseDoubles( double *cursor, size_t size );

    static void TerminateWorkers();

    static ParallelFloatProcessor *GetInstance( size_t size = 0, bool create = true );

private:

    enum Task
    {
        SerFloats,
        SerDoubles,
        DeserFloats,
        DeserDoubles
    };

    std::vector<std::thread> mWorkers;

    uint32_t mU32Buffer[SERIALISATION_FLOAT_BUFFER_SIZE];
    uint64_t mU64Buffer[SERIALISATION_FLOAT_BUFFER_SIZE];

    std::mutex mLock;
    SpinLock mSpinLock;;
    std::condition_variable_any mNotify;
    std::atomic_bool mStop;
    std::atomic<uint32_t> mCounter;
    std::atomic<uint32_t> mGeneration;

    float *mFloatSourceCursor;
    double *mDoubleSourceCursor;

    size_t mSourceSize;

    uint32_t mMainEnd;
    uint32_t mWorkerCount;

    Task mTask;

    void StartTask( Task task );
    void WaitTaskComplete() const;

    void WorkerSerialiseFloat( size_t start, size_t end );
    void WorkerDeserialiseFloat( size_t start, size_t end );
    void WorkerSerialiseDouble( size_t start, size_t end );
    void WorkerDeserialiseDouble( size_t start, size_t end );
};

#endif
