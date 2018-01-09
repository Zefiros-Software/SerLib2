/**
 * @cond ___LICENSE___
 *
 * Copyright (c) 2016-2018 Zefiros Software.
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
 *
 * @endcond
 */
#pragma once
#ifndef __SERIALISATION_BARRIER_H__
#define __SERIALISATION_BARRIER_H__

class MixedBarrier
{
public:

    /**
     * Constructor.
     *
     * @param   count Number of threads to wait for.
     */

    explicit MixedBarrier(uint32_t count)
        : mCurrentCon(&mConVar1),
          mPreviousCon(&mConVar2),
          mCount(count),
          mMax(count),
          mSpaces(count)
    {
    }

    /**
     * Sets the size of the barrier, thus the number of threads to wait for on a sync point.
     *
     * @param   count Number of threads to wait on.
     *
     * @post The amount of threads the barriers waits on equals count.
     */

    void SetSize(uint32_t count)
    {
        mCount = count;
        mMax = count;
        mSpaces = count;
    }

    /**
     * Waits for all the threads to reach the sync point, however the process can be aborted when `aborted` equals to
     * true.
     *
     * @param [in,out]  aborted Check whether the process should be aborted.
     *
     * @pre if aborted == true, all threads quit computations.
     *
     * @post all threads have waited for each other to reach the barrier.
     */

    void Wait(const std::atomic_bool &aborted)
    {
        const uint32_t myGeneration = mGeneration;

        if (!--mSpaces)
        {
            mSpaces = mMax;
            std::lock_guard< SpinLock > condVarLoc(mCondVarMutex);
            ++mGeneration;
            Reset();
        }
        else
        {
            size_t i = 0;

            while (mGeneration == myGeneration && ++i < 16000)
            {
                if ((i & 127) == 0 && aborted)
                {
                }
            }

            if (i >= 16000)
            {
                std::unique_lock< SpinLock > condVarLoc(mCondVarMutex);
                mCurrentCon->wait(condVarLoc, [&] {return mGeneration != myGeneration || aborted;});
            }
        }

        if (aborted)
        {
            mCurrentCon->notify_all();
        }
    }

    void NotifyAbort()
    {
        mCurrentCon->notify_all();
        ++mGeneration;
    }

private:

    SpinLock mCondVarMutex;
    std::condition_variable_any mConVar1;
    std::condition_variable_any mConVar2;

    std::condition_variable_any *mCurrentCon;
    std::condition_variable_any *mPreviousCon;

    uint32_t mCount;
    uint32_t mMax;

    std::atomic_uint_fast32_t mSpaces;
    std::atomic_uint_fast32_t mGeneration;

    void Reset()
    {
        mCount = mMax;
        std::condition_variable_any *tmpCon = mCurrentCon;
        mCurrentCon = mPreviousCon;
        mPreviousCon = tmpCon;

        tmpCon->notify_all();
    }
};

#endif