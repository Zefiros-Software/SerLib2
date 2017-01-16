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
#ifndef __SERIALISATION_STACK_H__
#define __SERIALISATION_STACK_H__

#include "serialisation/defines.h"

#include <vector>

template< typename tT >
class Stack
{
public:

    explicit Stack( size_t size )
        : mStack( size ),
          mSize( size ),
          mCursor( 0 )
    {
    }

    void Push( tT &t )
    {
        if ( mCursor == mSize )
        {
            mSize = static_cast<size_t>( mSize * 1.6 ) + 1;
            mStack.resize( mSize );
        }

        mStack[mCursor++] = t;
    }

    tT Pop()
    {
        return mStack[--mCursor];
    }


private:

    std::vector< tT > mStack;

    size_t mSize;
    size_t mCursor;
};

#endif