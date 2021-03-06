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
#ifndef __SERIALISATION_BINARYDESERMESSAGE_H__
#define __SERIALISATION_BINARYDESERMESSAGE_H__

#include "serialisation/internal/message/binary/proxy.h"

#include "serialisation/internal/memory/memoryPool.h"
#include "serialisation/internal/memory/stack.h"

#include "serialisation/internal/parallelFloatProcessor.h"
#include "serialisation/internal/pendingVariableArray.h"

#include "serialisation/serialisationHelper.h"
#include "serialisation/exceptions.h"
#include "serialisation/defines.h"
#include "serialisation/message.h"
#include "serialisation/types.h"
#include "serialisation/util.h"

#include <functional>
#include <assert.h>
#include <vector>
#include <array>

template< typename tStreamReader, size_t tBufferSize, typename tMainMessage >
class BinaryDeserialisationValueMessage;

template< typename tStreamReader, size_t tBufferSize = SERIALISATION_FLOAT_BUFFER_SIZE >
class BinaryDeserialisationMessage
{
public:

    template< typename tS, size_t tB, typename tM >
    friend class BinaryDeserialisationValueMessage;

    typedef BinaryDeserialisationMessage< tStreamReader, tBufferSize > tMain;
    typedef BinaryDeserialisationValueMessage< tStreamReader, tBufferSize, Message< tMain >> tValueMessage;

    template< typename tStream >
    explicit BinaryDeserialisationMessage(tStream &streamInitializer)
        : mStreamReader(streamInitializer, mCleanExit),
          mCurrentType(Type::Terminator),
          mCurrentIndex(0),
          mCurrentIndexPending(false),
          mObjectSkipping(false),
          mCleanExit(true),
          mCurrentPendingVariables(nullptr),
          mVariableHistory(1)
    {
    }

    ~BinaryDeserialisationMessage()
    {
        if (!mCleanExit)
        {
            ReleasePendingVariableArray(mCurrentPendingVariables);

            for (auto it = mVariableHistory.Pop(); it != nullptr; it = mVariableHistory.Pop())
            {
                ReleasePendingVariableArray(it);
            }
        }
    }

    template< typename tT >
    void Store(tT &value, uint8_t index, uint8_t /*flags*/)
    {
        StoreTemplate(value, index, StorePrimitiveProxy());
    }

    template< typename tT >
    void StoreVector(std::vector< tT > &value, uint8_t index, uint8_t /*flags*/)
    {
        StoreTemplate(value, index, StorePrimitiveVectorProxy());
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject(tSerialisable &serialisable, uint8_t index, uint8_t /*flags*/, tMessage &message)
    {
        StoreTemplate(serialisable, index, StoreObjectProxy< tMessage >(message));
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObjectVector(std::vector< tSerialisable > &value, uint8_t index, uint8_t /*flags*/, tMessage &message)
    {
        StoreTemplate(value, index, StoreObjectVectorProxy< tMessage >(message));
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint(tSerialisable &serialisable, tMessage &message)
    {
        ExceptionHelper::Assert<NoCleanExitException>(mCleanExit, mCleanExit);

        mVariableHistory.Push(mCurrentPendingVariables);
        mCurrentPendingVariables = InitPendingVariableArray();

        SerialisationHelper< tSerialisable >::OnStore(message, serialisable);

        ReadRemaining();

        ReleasePendingVariableArray(mCurrentPendingVariables);
        mCurrentPendingVariables = mVariableHistory.Pop();
    }

    void ClearBuffer()
    {
        mStreamReader.ClearBuffer();
    }

private:

    uint8_t mBoolPackBuffer[1024];

    tStreamReader mStreamReader;

    Type::Type mCurrentType;
    uint8_t mCurrentIndex;
    bool mCurrentIndexPending;

    bool mObjectSkipping;

    bool mCleanExit;

    PendingVariableArray *mCurrentPendingVariables;

    Stack< PendingVariableArray * > mVariableHistory;
    UnsychronisedMemoryPoolInstantiator< PendingVariableArray > mPendingPool;

    PendingVariableArray *InitPendingVariableArray()
    {
        PendingVariableArray *pva = mPendingPool.Create();
        pva->Clear();

        return pva;
    }

    void ReleasePendingVariableArray(PendingVariableArray *pva)
    {
        mPendingPool.Destroy(pva);
    }


    template< typename tT >
    void AddPendingPrimitive(tT &value, uint8_t index)
    {
        mCurrentPendingVariables->SetPending(index, [this, &value](Type::Type type)
        {
            ReadPrimitive(value, type);
        });
    }


    template< typename tT >
    void AddPendingPrimitiveVector(tT &value, uint8_t index)
    {
        mCurrentPendingVariables->SetPending(index, [this, &value](Type::Type type)
        {
            ReadPrimitiveVector(value, type);
        });
    }

    template< typename tSerialisable, typename tMessage >
    void AddPendingObject(tSerialisable &serialisable, uint8_t index, tMessage &message)
    {
        mCurrentPendingVariables->SetPending(index, [this, &serialisable, &message](Type::Type type)
        {
            ReadObject(serialisable, type, message);
        });
    }

    template< typename tSerialisable, typename tMessage >
    void AddPendingObjectVector(std::vector<tSerialisable> &value, uint8_t index, tMessage &message)
    {
        mCurrentPendingVariables->SetPending(index, [this, &value, &message](Type::Type type)
        {
            ReadObjectVector(value, type, message);
        });
    }

    template< typename tProxy, typename tT >
    void StoreTemplate(tT &value, uint8_t index, const tProxy &proxy)
    {
        if (mCurrentIndexPending)
        {
            if (index == mCurrentIndex)
            {
                mCurrentIndexPending = false;
                proxy.Read(*this, value, mCurrentType);
            }
            else
            {
                proxy.AddPending(*this, value, index);
            }
        }
        else
        {
            uint8_t rIndex;
            Type::Type type;
            ReadHeader(rIndex, type);

            if (!mCurrentPendingVariables->AnyPending() && index == rIndex)
            {
                proxy.Read(*this, value, type);
            }
            else
            {
                while (mCurrentPendingVariables->IsPending(rIndex))
                {
                    mCurrentPendingVariables->ReadNow(rIndex, type);
                    ReadHeader(rIndex, type);
                }

                if (index == rIndex)
                {
                    proxy.Read(*this, value, type);
                }
                else
                {
                    mCurrentIndex = rIndex;
                    mCurrentType = type;
                    mCurrentIndexPending = true;

                    proxy.AddPending(*this, value, index);
                }
            }
        }
    }

    void ReadHeader(uint8_t &index, Type::Type &type)
    {
        mStreamReader.ReadPrimitive(index);

        type = Util::GetHeaderType(index);
        index = Util::GetHeaderIndex(index);
    }

    void ReadArrayHeader(uint8_t &flags, Type::Type &subType, size_t &size)
    {
        ReadHeader(flags, subType);
        size = mStreamReader.ReadSize();
    }

    template< typename tSerialisable, typename tMessage  >
    void ReadObject(tSerialisable &serialisable, Type::Type type, tMessage &message)
    {
        ExceptionHelper::Strict::AssertEqual< InvalidTypeException >(Type::Object, type, mCleanExit);

        StoreEntryPoint(serialisable, message);
    }

    template< typename tSerialisable, typename tMessage  >
    void ReadObjectVector(std::vector<tSerialisable> &value, Type::Type type, tMessage &message)
    {
        ExceptionHelper::Strict::AssertEqual< InvalidTypeException >(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);

        ExceptionHelper::Strict::AssertEqual< InvalidTypeException >(Type::Object, subType, mCleanExit);

        value.resize(size);

        flags = flags & 0x1;

        if (flags)
        {
            PendingVariableArray *tempPendingVariables = InitPendingVariableArray();

            tValueMessage internalMessage(*this, message, *tempPendingVariables);

            internalMessage.CacheHeaders();
            {
                Message< tValueMessage > tempMessage(internalMessage);

                for (auto &t : value)
                {
                    tempMessage.Enter(t);
                }
            }
            ReleasePendingVariableArray(tempPendingVariables);
        }
        else
        {
            for (auto &t : value)
            {
                StoreEntryPoint(t, message);
            }
        }
    }

    template< typename tT >
    void ReadPrimitiveNoAssert(tT &value)
    {
        mStreamReader.ReadPrimitive(value);
    }

    void ReadPrimitiveNoAssert(std::string &value)
    {
        size_t size = mStreamReader.ReadSize();
        value.resize(size);
        mStreamReader.ReadBytes(&value[0], size);
    }

    template< typename tT >
    void ReadPrimitiveType(tT &value, Type::Type type)
    {
        switch (type)
        {
        case Type::UInt8:
            {
                uint8_t val;
                ReadPrimitiveNoAssert(val);
                value = static_cast<tT>(val);
            }
            break;

        case Type::UInt16:

            {
                uint16_t val;
                ReadPrimitiveNoAssert(val);
                value = static_cast<tT>(val);
            }
            break;

        case Type::UInt32:

            {
                uint32_t val;
                ReadPrimitiveNoAssert(val);
                value = static_cast<tT>(val);
            }
            break;

        case Type::UInt64:

            {
                uint64_t val;
                ReadPrimitiveNoAssert(val);
                value = static_cast<tT>(val);
            }
            break;

            CASE_DEFAULT_ASSERT_HAYWIRE;
        }
    }

    template< typename tT >
    void ReadPrimitive(tT &value, Type::Type type)
    {
#ifndef SERIALISATION_DISABLE_TYPE_CHECKS
#ifndef SERIALISATION_ENABLE_STRICT_TYPES

        if (ExceptionHelper::Compat<tT>::AssertCompatible(type, mCleanExit))
        {
            ReadPrimitiveType(value, type);
        }
        else
        {
            ReadPrimitiveNoAssert(value);
        }

#else
        ExceptionHelper::Strict::AssertEqual(Type::GetHeaderEnum<tT>(), type);
        ReadPrimitiveNoAssert(value);
#endif
#else
        ReadPrimitiveNoAssert(value);
#endif
    }

    void ReadPrimitive(bool &value, Type::Type type)
    {
        uint8_t val;
        ReadPrimitive(val, type);
        value = val != 0;
    }

    void ReadPrimitive(float &value, Type::Type type)
    {
        if (type == Type::UInt64)
        {
            double temp;
            ReadPrimitive(temp, type);
            value = static_cast<float>(temp);
        }
        else
        {
            ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::UInt32, type, mCleanExit);

            uint32_t flexman;
            ReadPrimitiveNoAssert(flexman);
            value = Util::UInt32ToFloat(flexman);
        }
    }

    void ReadPrimitive(double &value, Type::Type type)
    {
        if (type == Type::UInt32)
        {
            float temp;
            ReadPrimitive(temp, type);
            value = temp;
        }
        else
        {
            ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::UInt64, type, mCleanExit);

            uint64_t flexman;
            ReadPrimitiveNoAssert(flexman);
            value = Util::UInt64ToDouble(flexman);
        }
    }

    void ReadPrimitive(std::string &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::String, type, mCleanExit);

        ReadPrimitiveNoAssert(value);
    }

    template< typename tT >
    void ReadPrimitiveVector(std::vector< tT > &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);

        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::GetHeaderEnum<tT>(), subType, mCleanExit);

        value.resize(size);
        mStreamReader.ReadPrimitiveBlock(&value[0], size);
    }

    void ReadPrimitiveVector(std::vector< bool > &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);
        flags = flags & 0x1;


        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::UInt8, subType, mCleanExit);

        value.resize(size);

        if (flags)
        {
            size_t j = 0;

            constexpr uint8_t tBit = 0x1;

            for (size_t k = 0, kEnd = (size + 1023) / 1024; k < kEnd; ++k)
            {
                size_t end = std::min((size - j) / 8, size_t(1024));

                mStreamReader.ReadPrimitiveBlock(mBoolPackBuffer, end);

                for (size_t i = 0 ; i < end; ++i, j += 8)
                {
                    value[j] = (mBoolPackBuffer[i] & tBit) > 0;
                    value[j + 1] = (mBoolPackBuffer[i] & (tBit << 1)) > 0;
                    value[j + 2] = (mBoolPackBuffer[i] & (tBit << 2)) > 0;
                    value[j + 3] = (mBoolPackBuffer[i] & (tBit << 3)) > 0;
                    value[j + 4] = (mBoolPackBuffer[i] & (tBit << 4)) > 0;
                    value[j + 5] = (mBoolPackBuffer[i] & (tBit << 5)) > 0;
                    value[j + 6] = (mBoolPackBuffer[i] & (tBit << 6)) > 0;
                    value[j + 7] = (mBoolPackBuffer[i] & (tBit << 7)) > 0;
                }

            }

            if (j < size)
            {
                uint8_t temp;
                mStreamReader.ReadPrimitive(temp);;

                value[j] = (temp & tBit) > 0;
                ++j;

                for (size_t k = 1; j < size; ++j, ++k)
                {
                    value[j] = (temp & (tBit << k)) > 0;
                }
            }
        }
        else
        {
            uint8_t temp;

            for (size_t i = 0; i < size; ++i)
            {
                ReadPrimitiveNoAssert(temp);
                value[i] = temp > 0;
            }
        }
    }

    void ReadPrimitiveVector(std::vector< std::string > &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);

        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::String, subType, mCleanExit);

        value.resize(size);

        for (size_t i = 0; i < size; ++i)
        {
            ReadPrimitiveNoAssert(value[i]);
        }
    }


    void ReadPrimitiveVector(std::vector< float > &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);

        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::UInt32, subType, mCleanExit);

        value.resize(size);

        float *fCursor = &value[0];
        ParallelFloatProcessor &floatProcessor = *ParallelFloatProcessor::GetInstance(4);

        for (size_t i = 0, end = value.size(); i < end; i += tBufferSize, fCursor += tBufferSize)
        {
            const size_t blockSize = std::min(tBufferSize, end - i);
            mStreamReader.ReadPrimitiveBlock(floatProcessor.GetU32Buffer(), blockSize);
            floatProcessor.DeserialiseFloats(fCursor, blockSize);

        }
    }

    void ReadPrimitiveVector(std::vector< double > &value, Type::Type type)
    {
        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::Array, type, mCleanExit);

        uint8_t flags;
        Type::Type subType;
        size_t size;
        ReadArrayHeader(flags, subType, size);

        ExceptionHelper::Strict::AssertEqual<InvalidTypeException>(Type::UInt64, subType, mCleanExit);

        value.resize(size);

        double *fCursor = &value[0];
        ParallelFloatProcessor &floatProcessor = *ParallelFloatProcessor::GetInstance(4);

        for (size_t i = 0, end = value.size(); i < end; i += tBufferSize, fCursor += tBufferSize)
        {
            const size_t blockSize = std::min(tBufferSize, end - i);
            mStreamReader.ReadPrimitiveBlock(floatProcessor.GetU64Buffer(), blockSize);
            floatProcessor.DeserialiseDoubles(fCursor, blockSize);

        }
    }

    void ReadRemaining()
    {
        uint8_t index;
        Type::Type type;

        if (mCurrentIndexPending)
        {
            index = mCurrentIndex;
            type = mCurrentType;
            mCurrentIndexPending = false;
        }
        else
        {
            ReadHeader(index, type);
        }

        while (type != Type::Terminator)
        {

            if (!mObjectSkipping && mCurrentPendingVariables->IsPending(index))
            {
                mCurrentPendingVariables->ReadNow(index, type);
            }
            else
            {
                SkipVariable(type);
            }

            ReadHeader(index, type);
        }
    }

    void SkipArray(uint8_t flags, Type::Type subType, size_t size)
    {
        switch (subType)
        {
        case Type::Object:
            {
                bool objectSkipping = mObjectSkipping;

                if (!objectSkipping)
                {
                    mObjectSkipping = true;
                }

                for (size_t i = 0; i < size; ++i)
                {
                    ReadRemaining();
                }

                if (!objectSkipping)
                {
                    mObjectSkipping = false;
                }
            }
            break;

        case Type::String:
            {
                size_t strSize;

                for (size_t i = 0; i < size; ++i)
                {
                    strSize = mStreamReader.ReadSize();
                    mStreamReader.Skip(strSize);
                }
            }
            break;

        case Type::UInt8:
            {
                if (flags & 0x1)
                {
                    mStreamReader.Skip((size + 7) / 8);
                }
                else
                {
                    mStreamReader.Skip(size);
                }
            }
            break;

        case Type::UInt16:
        case Type::UInt32:
        case Type::UInt64:
            {
                mStreamReader.Skip(size * GetSize(subType));
            }
            break;

            CASE_DEFAULT_ASSERT_HAYWIRE;
        }
    }

    void SkipVariable(Type::Type type)
    {
        switch (type)
        {
        case Type::Array:
            {
                uint8_t flags;
                Type::Type subType;
                size_t size;
                ReadArrayHeader(flags, subType, size);

                SkipArray(flags, subType, size);
            }
            break;

        case Type::Object:
            {
                if (mObjectSkipping)
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
                mStreamReader.Skip(size);
            }
            break;

        case Type::UInt8:
        case Type::UInt16:
        case Type::UInt32:
        case Type::UInt64:
            mStreamReader.Skip(GetSize(type));
            break;

            CASE_DEFAULT_ASSERT_HAYWIRE;
        }
    }

    SERIALISATION_DESER_PROXY(StorePrimitiveProxy, AddPendingPrimitive, ReadPrimitive);
    SERIALISATION_DESER_PROXY(StorePrimitiveVectorProxy, AddPendingPrimitiveVector, ReadPrimitiveVector);

    SERIALISATION_DESER_OBJECT_PROXY(StoreObjectProxy, AddPendingObject, ReadObject);
    SERIALISATION_DESER_OBJECT_PROXY(StoreObjectVectorProxy, AddPendingObjectVector, ReadObjectVector);
};

template< typename tStreamReader, size_t tBufferSize, typename tMainMessage >
class BinaryDeserialisationValueMessage
{
public:

    explicit BinaryDeserialisationValueMessage(BinaryDeserialisationMessage< tStreamReader, tBufferSize > &message,
                                               tMainMessage &parentMessage, PendingVariableArray &pendingVariables)
        : mMessage(message),
          mParentMessage(parentMessage),
          mPendingVariables(pendingVariables),
          mHeaderIndex(0),
          mHeaderCount(0)
    {
        mIndices.reserve(10);
        mTypes.reserve(10);
    }

    void CacheHeaders()
    {
        uint8_t index;
        Type::Type type;

        mMessage.ReadHeader(index, type);

        mHeaderCount = 0;

        mIndices.clear();
        mTypes.clear();

        while (type != Type::Terminator)
        {
            mIndices.push_back(index);
            mTypes.push_back(type);
            mMessage.ReadHeader(index, type);
            ++mHeaderCount;
        }
    }

    template< typename tT >
    void Store(tT &value, uint8_t index, uint8_t /*flags*/)
    {
        StoreTemplate(value, index, StorePrimitiveProxy());
    }

    template< typename tT >
    void StoreVector(std::vector< tT > &value, uint8_t index, uint8_t /*flags*/)
    {
        StoreTemplate(value, index, StorePrimitiveVectorProxy());
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObject(tSerialisable &serialisable, uint8_t index, uint8_t /*flags*/, tMessage &/*message*/)
    {
        StoreTemplate(serialisable, index, StoreObjectProxy< tMainMessage >(mParentMessage));
    }

    template< typename tSerialisable, typename tMessage >
    void StoreObjectVector(std::vector< tSerialisable > &value, uint8_t index, uint8_t /*flags*/, tMessage &/*message*/)
    {
        StoreTemplate(value, index, StoreObjectVectorProxy< tMainMessage >(mParentMessage));
    }

    template< typename tSerialisable, typename tMessage >
    void StoreEntryPoint(tSerialisable &serialisable, tMessage &message)
    {
        mPendingVariables.Clear();
        mHeaderIndex = 0;

        SerialisationHelper< tSerialisable >::OnStore(message, serialisable);

        for (; mHeaderIndex < mHeaderCount; ++mHeaderIndex)
        {
            const uint8_t index = mIndices[mHeaderIndex];

            if (mPendingVariables.IsPending(index))
            {
                mPendingVariables.ReadNow(index, mTypes[mHeaderIndex]);
            }
        }
    }

    void ClearBuffer()
    {
    }

private:

    BinaryDeserialisationMessage< tStreamReader, tBufferSize > &mMessage;
    tMainMessage &mParentMessage;
    PendingVariableArray &mPendingVariables;

    std::vector<uint8_t> mIndices;
    std::vector<Type::Type> mTypes;

    size_t mHeaderIndex;
    size_t mHeaderCount;

    template< typename tProxy, typename tT >
    void StoreTemplate(tT &value, uint8_t index, const tProxy &proxy)
    {
        if (mHeaderIndex >= mHeaderCount)
        {
            // the last member of this object has been read
            return;
        }

        uint8_t rIndex = mIndices[mHeaderIndex];

        if (index == rIndex)
        {
            proxy.Read(*this, value, mTypes[mHeaderIndex]);
            ++mHeaderIndex;
        }
        else
        {
            while (mPendingVariables.IsPending(rIndex))
            {
                const Type::Type type = mTypes[mHeaderIndex];
                mPendingVariables.ReadNow(rIndex, type);
                rIndex = mIndices[++mHeaderIndex];
            }

            if (index == rIndex)
            {
                proxy.Read(*this, value, mTypes[mHeaderIndex]);
            }
            else
            {
                proxy.AddPending(*this, value, index);
            }
        }
    }

    template< typename tT >
    void ReadPrimitive(tT &value, Type::Type type)
    {
        mMessage.ReadPrimitive(value, type);
    }

    template< typename tT >
    void ReadPrimitiveVector(std::vector< tT > &value, Type::Type type)
    {
        mMessage.ReadPrimitiveVector(value, type);
    }

    template< typename tSerialisable, typename tMessage  >
    void ReadObject(tSerialisable &serialisable, Type::Type type, tMessage &message)
    {
        ExceptionHelper::Strict::AssertEqual< InvalidTypeException >(Type::Object, type, mMessage.mCleanExit);

        mMessage.StoreEntryPoint(serialisable, message);
    }

    template< typename tSerialisable, typename tMessage  >
    void ReadObjectVector(std::vector<tSerialisable> &value, Type::Type type, tMessage &message)
    {
        mMessage.ReadObjectVector(value, type, message);
    }

    template< typename tT >
    void AddPendingPrimitive(tT &value, uint8_t index)
    {
        mPendingVariables.SetPending(index, [this, &value](Type::Type type)
        {
            mMessage.ReadPrimitive(value, type);
        });
    }


    template< typename tT >
    void AddPendingPrimitiveVector(tT &value, uint8_t index)
    {
        mPendingVariables.SetPending(index, [this, &value](Type::Type type)
        {
            mMessage.ReadPrimitiveVector(value, type);
        });
    }

    template< typename tSerialisable, typename tMessage >
    void AddPendingObject(tSerialisable &serialisable, uint8_t index, tMessage &/*message*/)
    {
        mPendingVariables.SetPending(index, [this, &serialisable](Type::Type type)
        {
            ExceptionHelper::Strict::AssertEqual< InvalidTypeException >(Type::Object, type, mMessage.mCleanExit);

            mMessage.StoreEntryPoint(serialisable, mParentMessage);
        });
    }

    template< typename tSerialisable, typename tMessage >
    void AddPendingObjectVector(std::vector<tSerialisable> &value, uint8_t index, tMessage &/*message*/)
    {
        mPendingVariables.SetPending(index, [this, &value](Type::Type type)
        {
            mMessage.ReadObjectVector(value, type, mParentMessage);
        });
    }

    SERIALISATION_DESER_PROXY(StorePrimitiveProxy, AddPendingPrimitive, ReadPrimitive);
    SERIALISATION_DESER_PROXY(StorePrimitiveVectorProxy, AddPendingPrimitive, ReadPrimitiveVector);

    SERIALISATION_DESER_OBJECT_PROXY(StoreObjectProxy, AddPendingObject, ReadObject);
    SERIALISATION_DESER_OBJECT_PROXY(StoreObjectVectorProxy, AddPendingObjectVector, ReadObjectVector);
};

#endif