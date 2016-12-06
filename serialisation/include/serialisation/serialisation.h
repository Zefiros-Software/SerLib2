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
#ifndef __SERIALISATION_SERIALISATION_H__
#define __SERIALISATION_SERIALISATION_H__

#include "serialisation/internal/message/binary/binaryDeserMessage.h"
#include "serialisation/internal/message/binary/binarySerMessage.h"
#include "serialisation/stream/bufferedStreamReader.h"
#include "serialisation/stream/bufferedStreamWriter.h"
#include "serialisation/stream/streamReader.h"
#include "serialisation/stream/streamWriter.h"
#include "serialisation/serialisationHelper.h"
#include "serialisation/message.h"

template< uint64_t tRemainingSize, bool tHasTwoBytes >
struct VarIntSecond
{
    static constexpr uint16_t GetValue()
    {
        return ( ( ( tRemainingSize & 0x7F ) | 0x80 ) << 8 ) | ( ( tRemainingSize >> 7 ) & 0x7F );
    }
};

template< uint64_t tRemainingSize >
struct VarIntSecond< tRemainingSize, false >
{
    static constexpr uint16_t GetValue()
    {
        return ( tRemainingSize & 0x7F ) << 8;
    }
};

template< uint64_t tRemainingSize, bool tBytesLeft, uint8_t tByteIndex = 0, uint64_t tFirstEightBytes = 0 >
struct VarIntFirst
{
    static constexpr std::pair< uint64_t, uint16_t > GetValue()
    {
        return VarIntFirst < ( tRemainingSize >> 7 ),
               ( ( tRemainingSize >> 7 ) >= 0x80 ),
               tByteIndex + 1,
               ( tFirstEightBytes << 8 ) | ( tRemainingSize & 0x7F ) | 0x80 >::GetValue();
    }
};

template< uint64_t tRemainingSize, uint8_t tByteIndex, uint64_t tFirstEightBytes >
struct VarIntFirst< tRemainingSize, false, tByteIndex, tFirstEightBytes >
{
    static constexpr std::pair< uint64_t, uint16_t > GetValue()
    {
        return std::pair< uint64_t, uint16_t >( ( tFirstEightBytes << 8 ) | ( tRemainingSize & 0x7F ), 0 );
    }
};

template< uint64_t tRemainingSize, uint64_t tFirstEightBytes >
struct VarIntFirst< tRemainingSize, true, 8, tFirstEightBytes >
{
    static constexpr std::pair< uint64_t, uint16_t > GetValue()
    {
        return std::pair< uint64_t, uint16_t >( tFirstEightBytes, VarIntSecond < tRemainingSize,
                                                ( tRemainingSize >= 0x80 ) >::GetValue() );
    }
};

template< uint64_t tSize >
constexpr std::pair< uint64_t, uint16_t > VarInt()
{
    return VarIntFirst < tSize, ( tSize >= 0x80 ) >::GetValue();
}

template< uint64_t tVarIntValue, uint64_t tVarIntFirst, uint16_t tVarIntSecond >
struct FromVarIntHelper
{
    static constexpr uint64_t GetValue()
    {
        return FromVarIntHelper < ( tVarIntValue << 7 ) | ( tVarIntSecond & 0x7F ), tVarIntFirst,
               ( tVarIntSecond >> 8 ) >::GetValue();
    }
};

template< uint64_t tVarIntValue, uint64_t tVarIntFirst >
struct FromVarIntHelper< tVarIntValue, tVarIntFirst, 0 >
{
    static constexpr uint64_t GetValue()
    {
        return FromVarIntHelper < ( tVarIntValue << 7 ) | ( tVarIntFirst & 0x7F ), ( tVarIntFirst >> 8 ), 0 >::GetValue();
    }
};

template< uint64_t tVarIntValue >
struct FromVarIntHelper< tVarIntValue, 0, 0 >
{
    static constexpr uint64_t GetValue()
    {
        return tVarIntValue;
    }
};

template< uint64_t tVarIntFirst, uint16_t tVarIntSecond, typename tT = uint64_t >
constexpr tT FromVarInt()
{
    return static_cast<tT>( FromVarIntHelper< 0, tVarIntFirst, tVarIntSecond >::GetValue() );
}

template< uint64_t tSize, bool tBytesLeft >
struct VarIntSizeHelper
{
    static constexpr uint8_t value = VarIntSizeHelper < ( tSize >> 7 ), ( ( tSize >> 7 ) >= 0x80 ) >::value + 1;
};

template< uint64_t tSize >
struct VarIntSizeHelper< tSize, false >
{
    static constexpr uint8_t value = 1;
};

template< uint64_t tSize >
constexpr uint8_t VarIntSize()
{
    return VarIntSizeHelper < tSize, ( tSize >= 0x80 ) >::value;
}

namespace Serialisation
{
    typedef Message< BinarySerialisationMessage< BufferedStreamWriter<> > > BinarySerialiser;
    typedef Message< BinaryDeserialisationMessage< BufferedStreamReader<> > > BinaryDeserialiser;
}

#endif
