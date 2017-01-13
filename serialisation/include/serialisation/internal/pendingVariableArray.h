/**
 * Copyright (c) 2017 Mick van Duijn, Koen Visscher and Paul Visscher
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
#ifndef __SERIALISATION_PENDINGVARIABLEARRAY_H__
#define __SERIALISATION_PENDINGVARIABLEARRAY_H__

#include "serialisation/types.h"

#include <functional>
#include <stdint.h>
#include <assert.h>
#include <array>

class PendingVariableArray
{
public:

    PendingVariableArray();

    uint32_t IsPending( uint8_t index ) const;

    void ReadNow( uint8_t index, Type::Type type );

    uint32_t AnyPending() const;

    template< typename tFunc >
    void SetPending( uint8_t index, const tFunc &lambda )
    {
        assert( !( ( mIsPendingMask >> index ) & 0x1 ) );

        mIsPendingMask |= ( 1u << index );
        mPendingVariables[index] = lambda;
    }

private:

    std::array<std::function<void( Type::Type )>, 28> mPendingVariables;
    uint32_t mIsPendingMask;
};

#endif