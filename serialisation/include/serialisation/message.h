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

#define SERIALISATION_INTERFACE_PRIMITIVE( type )                                               \
    void Store( uint8_t index, type &value, uint8_t flags = Type::GetDefaultFlags<type>() )  \
    {                                                                                               \
        StorePrimitive( index ,value, flags );                                                      \
    }

#define SERIALISATION_INTERFACE_PRIMITIVE_CONTAINER( storeContainer, container )                            \
    void Store( uint8_t index, container &value, uint8_t flags = Type::GetDefaultFlags<container>() )    \
    {                                                                                                           \
        storeContainer( index, value, flags );                                                                  \
    }

#define SERIALISATION_INTERFACE_PRIMITIVE_VECTOR( type ) \
    SERIALISATION_INTERFACE_PRIMITIVE_CONTAINER( StoreVector, std::vector< type > )

template< typename tInternalMessage >
class Message
{
public:

    template< typename... tArgs >
    explicit Message(tArgs &&... args)
        : mInternalMessage(std::forward<tArgs>(args)...)
    {
    }

    SERIALISATION_ALL_TYPES(SERIALISATION_INTERFACE_PRIMITIVE);

    template< typename tSerialisable >
    void Store(uint8_t index, tSerialisable &serialisable, uint8_t flags = Type::GetDefaultFlags<tSerialisable>())
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE(index);

        StoreHelper< tSerialisable >::Store(index, serialisable, flags, this);
    }

    SERIALISATION_ALL_TYPES(SERIALISATION_INTERFACE_PRIMITIVE_VECTOR);

    template< typename tT >
    void Store(uint8_t index, std::vector< tT > &value, uint8_t flags = Type::GetDefaultFlags<tT>())
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE(index);

        mInternalMessage.StoreObjectVector(value, index, flags, *this);
    }

    template< typename tParent, typename tSerialisable >
    void StoreParent(uint8_t index, tSerialisable &serialisable, uint8_t flags = Type::GetDefaultFlags<tParent>())
    {
        SERIALISATION_ASSERT_PARENT_INDEX_IN_RANGE(index);

        mInternalMessage.template StoreObject< tParent >(serialisable, index, flags, *this);
    }

    template< typename tParent, typename tSerialisable >
    void StoreParent(uint8_t index, tSerialisable *serialisable, uint8_t flags = Type::GetDefaultFlags<tParent>())
    {
        StoreParent< tParent, tSerialisable >(index, *serialisable, flags);
    }

    template< typename tSerialisable >
    void Enter(tSerialisable &serialisable)
    {
        mInternalMessage.StoreEntryPoint(serialisable, *this);
    }

    void ClearBuffer()
    {
        mInternalMessage.ClearBuffer();
    }

private:

    tInternalMessage mInternalMessage;

    template< typename tT >
    void StorePrimitive(uint8_t index, tT &value, uint8_t flags)
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE(index);

        mInternalMessage.Store(value, index, flags);
    }

    template< typename tT >
    void StoreVector(uint8_t index, std::vector< tT > &value, uint8_t flags)
    {
        SERIALISATION_ASSERT_INDEX_IN_RANGE(index);

        mInternalMessage.StoreVector(value, index, flags);
    }

    template< typename tSerialisable >
    void StoreObject(uint8_t index, tSerialisable &serialisable, uint8_t flags)
    {
        mInternalMessage.StoreObject(serialisable, index, flags, *this);
    }

    template< typename tSerialisable, bool tIsIntegral = std::is_integral< tSerialisable >::value >
    struct StoreHelper
    {
        static void Store(uint8_t index, tSerialisable &serialisable, uint8_t flags, Message< tInternalMessage > *message)
        {
            message->StoreObject(index, serialisable, flags);
        }
    };

    template< typename tSerialisable >
    struct StoreHelper< tSerialisable, true >
    {
        static void Store(uint8_t index, tSerialisable &serialisable, uint8_t flags, Message< tInternalMessage > *message)
        {
            message->StorePrimitive(index, serialisable, flags);
        }
    };
};

#endif
