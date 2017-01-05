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

#include <string.h>

typedef std::string String;

#define SERIALISATION_ALL_NUMERIC_TYPES( interfaceMacro )   \
interfaceMacro( uint8_t )                                   \
interfaceMacro( uint16_t )                                  \
interfaceMacro( uint32_t )                                  \
interfaceMacro( uint64_t )                                  \
interfaceMacro( int8_t )                                    \
interfaceMacro( int16_t )                                   \
interfaceMacro( int32_t )                                   \
interfaceMacro( int64_t )                                   \
interfaceMacro( float )                                     \
interfaceMacro( double )

#define SERIALISATION_ALL_TYPES( interfaceMacro )           \
interfaceMacro( bool )                                      \
SERIALISATION_ALL_NUMERIC_TYPES( interfaceMacro )           \
interfaceMacro( String )

#define SERIALISATION_INTERFACE_PRIMITIVE( type )       \
inline void Store( uint8_t index, type &value )         \
{                                                       \
    StorePrimitive( index ,value );                     \
}

#define SERIALISATION_INTERFACE_PRIMITIVE_CONTAINER( storeContainer, container )  \
inline void Store( uint8_t index, container &value )                            \
{                                                                                       \
    storeContainer( index, value );                                                     \
}

#define SERIALISATION_INTERFACE_PRIMITIVE_VECTOR( type ) \
        SERIALISATION_INTERFACE_PRIMITIVE_CONTAINER( StoreVector, std::vector< type > )

template< typename tInternalMessage >
class Message
{
public:

    template< typename tStream >
    Message( tStream &streamInitializer )
        : mInternalMessage( streamInitializer )
    {
    }

    template< typename tSerialisable >
    void Store( uint8_t index, tSerialisable &serialisable )
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE( index );

        mInternalMessage.StoreObject( serialisable, index, *this );
    }

    SERIALISATION_ALL_TYPES( SERIALISATION_INTERFACE_PRIMITIVE );

    template< typename tT >
    inline void Store( uint8_t index, std::vector< tT > &value )
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE( index );

        mInternalMessage.StoreObjectVector( value, index, *this );
    }

    SERIALISATION_ALL_TYPES( SERIALISATION_INTERFACE_PRIMITIVE_VECTOR );

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

    template< typename tT >
    void StoreVector( uint8_t index, std::vector< tT > &value )
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE( index );

        mInternalMessage.StoreVector( value, index );
    }
};

#endif
