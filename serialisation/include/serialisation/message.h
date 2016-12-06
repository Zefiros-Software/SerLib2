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
#ifndef __SERIALISATION_MESSAGE_H__
#define __SERIALISATION_MESSAGE_H__

#include "serialisation/defines.h"
#include "serialisation/types.h"

template< typename tInternalMessage >
class Message
{
public:

    template< typename tStream >
    Message( tStream &streamInitializer )
        : mInternalMessage( streamInitializer )
    {
    }

    inline void Store( uint8_t index, bool &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, uint8_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, uint16_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, uint32_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, uint64_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, int8_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, int16_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, int32_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, int64_t &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, float &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, double &value )
    {
        StorePrimitive( index, value );
    }

    inline void Store( uint8_t index, std::string &value )
    {
        StorePrimitive( index, value );
    }

    template< typename tT >
    inline void Store( uint8_t index, std::vector< tT > &value )
    {
        mInternalMessage.StoreVector( value, index );
    }

    template< typename tSerialisable >
    void Store( uint8_t index , tSerialisable &serialisable )
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE( index );

        mInternalMessage.StoreObject( serialisable, index, *this );
    }

    template< typename tSerialisable >
    void Enter( tSerialisable &serialisable )
    {
        mInternalMessage.StoreEntryPoint( serialisable, *this );
    }

    void ClearBuffer()
    {
        mInternalMessage.ClearBuffer();
    }

private:

    tInternalMessage mInternalMessage;

    template< typename tT >
    void StorePrimitive( uint8_t index, tT &value )
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE( index );

        mInternalMessage.Store( value, index );
    }
};

#endif
