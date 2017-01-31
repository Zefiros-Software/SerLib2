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
#ifndef __SERIALISATION_PROXY_H__
#define __SERIALISATION_PROXY_H__

#include "serialisation/types.h"

#define SERIALISATION_DESER_OBJECT_PROXY( objectProxy, addPending, read )                               \
    template< typename tParentMessage >                                                                 \
    struct objectProxy                                                                                  \
    {                                                                                                   \
        tParentMessage *parentMessage;                                                                  \
                                                                                                        \
        objectProxy( tParentMessage &m )                                                                \
            : parentMessage( &m )                                                                       \
        {                                                                                               \
        }                                                                                               \
                                                                                                        \
        template< typename tMessage, typename tT >                                                      \
         void AddPending( tMessage &message, tT &value, uint8_t index ) const  \
        {                                                                                               \
            message.addPending( value, index, *parentMessage );                                         \
        }                                                                                               \
                                                                                                        \
        template< typename tMessage, typename tT >                                                      \
         void Read( tMessage &message, tT &value, Type::Type type ) const      \
        {                                                                                               \
            message.read( value, type, *parentMessage );                                                \
        }                                                                                               \
    };


#define SERIALISATION_DESER_PROXY( primitiveProxy, addPending, read )                                       \
    struct primitiveProxy                                                                                   \
    {                                                                                                       \
        template< typename tMessage, typename tT >                                                          \
         void AddPending( tMessage &message, tT &value, uint8_t index ) const      \
        {                                                                                                   \
            message.addPending( value, index );                                                             \
        }                                                                                                   \
                                                                                                            \
        template< typename tMessage, typename tT >                                                          \
         void Read( tMessage &message, tT &value, Type::Type type ) const          \
        {                                                                                                   \
            message.read( value, type );                                                                    \
        }                                                                                                   \
    };

#endif