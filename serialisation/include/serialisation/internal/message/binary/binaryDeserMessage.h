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
#ifndef __SERIALISATION_BINARYDESERMESSAGE_H__
#define __SERIALISATION_BINARYDESERMESSAGE_H__

#include "serialisation/defines.h"
#include "serialisation/util.h"
#include "serialisation/internal/memory/memoryPool.h"
#include "serialisation/internal/memory/stack.h"

#include <array>
#include <assert.h>
#include <functional>
#include <vector>

class PendingVariableArray
{
public:

    uint32_t IsPending( uint8_t index ) const
    {
        //return mIsPending[index];
        return ( mIsPendingMask >> index ) & 0x1;
    }

    void ReadNow( uint8_t index, Type::Type type )
    {
        //assert( mIsPending[index] );
        assert( ( mIsPendingMask >> index ) & 0x1 );

        mPendingVariables[index]( type );

        //mIsPending[index] = false;
        mIsPendingMask ^= 1u << index;
    }

    uint32_t AnyPending() const
    {
        //         for ( const bool &isPending : mIsPending )
        //         {
        //             if ( isPending )
        //             {
        //                 return true;
        //             }
        //         }
        //
        //         return false;
        return mIsPendingMask;
    }

    template< typename tFunc >
    void SetPending( uint8_t index, const tFunc &lambda )
    {
        //assert( !mIsPending[index] );
        assert( !( ( mIsPendingMask >> index ) & 0x1 ) );

        //mIsPending[index] = true;
        mIsPendingMask |= ( 1u << index );
        mPendingVariables[index] = lambda;
    }

private:

    std::array<std::function<void( Type::Type )>, 28> mPendingVariables;
    //std::array<bool, 28> mIsPending;
    uint32_t mIsPendingMask;
};

template< typename tStreamReader >
class BinaryDeserialisationMessage
{
public:

    template< typename tStream >
    explicit BinaryDeserialisationMessage( tStream &streamInitializer )
        : mStreamReader( streamInitializer ),
          mVariableHistory( 1 ),
          mCurrentPendingVariables( nullptr ),
          mCurrentIndexPending( false ),
          mObjectSkipping( false )
    {
    }

    template< typename tT >
    SERIALISATION_FORCEINLINE void Store( tT &value, uint8_t index )
    {
        StoreTemplate( value, index, StorePrimitiveProxy() );
    }

    template< typename tT >
    void StoreVector( std::vector< tT > &value, uint8_t index )
    {
        StoreTemplate( value, index, StorePrimitiveProxy() );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject( tSerialisable &serialisable, uint8_t index, tMessage &message )
    {
        StoreTemplate( serialisable, index, StoreObjectProxy< tMessage >( message ) );
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint( tSerialisable &serialisable, tMessage &message )
    {
        mVariableHistory.Push( mCurrentPendingVariables );
        mCurrentPendingVariables = InitPendingVariableArray();

        SerialisationHelper< tSerialisable >::OnStore( message, serialisable );

        ReadRemaining();

        ReleasePendingVariableArray( mCurrentPendingVariables );
        mCurrentPendingVariables = mVariableHistory.Pop();
    }

    void ClearBuffer()
    {
        mStreamReader.ClearBuffer();
    }

private:

    tStreamReader mStreamReader;

    Type::Type mCurrentType;
    uint8_t mCurrentIndex;
    bool mCurrentIndexPending;

    bool mObjectSkipping;

    PendingVariableArray *mCurrentPendingVariables;

    Stack< PendingVariableArray * > mVariableHistory;
    MemoryPool< PendingVariableArray > mPendingPool;

    PendingVariableArray *InitPendingVariableArray()
    {
        return mPendingPool.GetInstance();
    }

    void ReleasePendingVariableArray( PendingVariableArray *pva )
    {
        mPendingPool.ReturnInstance( pva );
    }


    template< typename tT >
    void AddPendingPrimitive( tT &value, uint8_t index )
    {
        mCurrentPendingVariables->SetPending( index, [this, &value]( Type::Type type )
        {
            ReadPrimitive( value, type );
        } );
    }

    template< typename tSerialisable, typename tMessage >
    void AddPendingObject( tSerialisable &serialisable, uint8_t index, tMessage &message )
    {
        mCurrentPendingVariables->SetPending( index, [this, &serialisable, &message]( Type::Type type )
        {
            ReadObject( serialisable, type, message );
        } );
    }

    template< typename tProxy, typename tT >
    SERIALISATION_FORCEINLINE void StoreTemplate( tT &value, uint8_t index, const tProxy &proxy )
    {
        if ( mCurrentIndexPending )
        {
            if ( index == mCurrentIndex )
            {
                mCurrentIndexPending = false;
                proxy.Read( *this, value, mCurrentType );
            }
            else
            {
                proxy.AddPending( *this, value, index );
            }
        }
        else
        {
            uint8_t rIndex;
            Type::Type type;
            ReadHeader( rIndex, type );

            if ( !mCurrentPendingVariables->AnyPending() && index == rIndex )
            {
                proxy.Read( *this, value, type );
            }
            else
            {
                while ( mCurrentPendingVariables->IsPending( rIndex ) )
                {
                    mCurrentPendingVariables->ReadNow( rIndex, type );
                    ReadHeader( rIndex, type );
                }

                if ( index == rIndex )
                {
                    proxy.Read( *this, value, type );
                }
                else
                {
                    mCurrentIndex = rIndex;
                    mCurrentType = type;
                    mCurrentIndexPending = true;

                    proxy.AddPending( *this, value, index );
                }
            }
        }
    }

    SERIALISATION_FORCEINLINE void ReadHeader( uint8_t &index, Type::Type &type )
    {
        mStreamReader.ReadPrimitive( index );

        type = Util::GetHeaderType( index );
        index = Util::GetHeaderIndex( index );
    }

    template< typename tSerialisable, typename tMessage  >
    void ReadObject( tSerialisable &serialisable, Type::Type type, tMessage &message )
    {
        StoreEntryPoint( serialisable, message );
    }

    template< typename tT >
    void ReadPrimitive( tT &value, Type::Type type )
    {
        mStreamReader.ReadPrimitive( value );
    }

    void ReadPrimitive( bool &value, Type::Type type )
    {
        uint8_t val;
        ReadPrimitive( val, type );
        value = val != 0;
    }

    void ReadPrimitive( float &value, Type::Type type )
    {
        uint32_t flexman;
        ReadPrimitive( flexman, type );
        value = Util::UInt32ToFloat( flexman );
    }

    void ReadPrimitive( double &value, Type::Type type )
    {
        uint64_t flexman;
        ReadPrimitive( flexman, type );
        value = Util::UInt64ToDouble( flexman );
    }

    template< typename tT >
    void ReadPrimitive( std::vector< tT > &value, Type::Type type )
    {
        uint8_t flags;
        Type::Type subType;
        ReadHeader( flags, subType );
        size_t size = mStreamReader.ReadSize();

        value.resize( size );
        mStreamReader.ReadPrimitiveBlock( &value[0], size );
    }

    SERIALISATION_FORCEINLINE void ReadPrimitive( std::string &value, Type::Type type )
    {
        size_t size = mStreamReader.ReadSize();
        value.resize( size );
        mStreamReader.ReadBytes( &value[0], size );
    }

    void ReadRemaining()
    {
        uint8_t header;
        uint8_t index;
        Type::Type type;

        if ( mCurrentIndexPending )
        {
            index = mCurrentIndex;
            type = mCurrentType;
            mCurrentIndexPending = false;
        }
        else
        {
            mStreamReader.ReadPrimitive( header );
            type = Util::GetHeaderType( header );
            index = Util::GetHeaderIndex( header );
        }

        while ( type != Type::Terminator )
        {

            if ( !mObjectSkipping && mCurrentPendingVariables->IsPending( index ) )
            {
                mCurrentPendingVariables->ReadNow( index, type );
            }
            else
            {
                SkipVariable( type );
            }

            mStreamReader.ReadPrimitive( header );
            type = Util::GetHeaderType( header );
            index = Util::GetHeaderIndex( header );
        }
    }

    void SkipVariable( Type::Type type )
    {
        switch ( type )
        {
        //         case Type::Array:
        //             break;

        case Type::Object:
            {
                if ( mObjectSkipping )
                {
                    ReadRemaining();
                }
                else
                {
                    mObjectSkipping = true;
                    ReadRemaining();
                    mObjectSkipping = false;
                }
            }
            break;

        case Type::String:
            {
                size_t size = mStreamReader.ReadSize();
                mStreamReader.Skip( size );
            }
            break;

        case Type::UInt8:
            mStreamReader.Skip( sizeof( uint8_t ) );
            break;

        case Type::UInt16:
            mStreamReader.Skip( sizeof( uint16_t ) );
            break;

        case Type::UInt32:
            mStreamReader.Skip( sizeof( uint32_t ) );
            break;

        case Type::UInt64:
            mStreamReader.Skip( sizeof( uint64_t ) );
            break;

        default:
            assert( false &&
                    "Whoops! Something went haywire. Please try to reproduce this exception in an example as small as possible and submit it as an issue. Thanks!" );
            break;
        }
    }

    template< typename tMessage >
    struct StoreObjectProxy
    {
        tMessage *message;

        StoreObjectProxy( tMessage &m )
            : message( &m )
        {
        }

        template< typename tMessage, typename tT >
        SERIALISATION_FORCEINLINE void AddPending( tMessage &iMessage, tT &value, uint8_t index ) const
        {
            iMessage.AddPendingObject( value, index, *message );
        }

        template< typename tMessage, typename tT >
        SERIALISATION_FORCEINLINE void Read( tMessage &iMessage, tT &value, Type::Type type ) const
        {
            iMessage.ReadObject( value, type, *message );
        }
    };

    struct StorePrimitiveProxy
    {
        template< typename tMessage, typename tT >
        SERIALISATION_FORCEINLINE void AddPending( tMessage &message, tT &value, uint8_t index ) const
        {
            message.AddPendingPrimitive( value, index );
        }

        template< typename tMessage, typename tT >
        SERIALISATION_FORCEINLINE void Read( tMessage &message, tT &value, Type::Type type ) const
        {
            message.ReadPrimitive( value, type );
        }
    };
};

#endif