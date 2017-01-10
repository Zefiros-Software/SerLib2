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
#ifndef __SERIALISATION_EXCEPTIONS_H__
#define __SERIALISATION_EXCEPTIONS_H__

#include "serialisation/types.h"

#include <exception>
#include <sstream>

namespace ExceptionHelper
{
    namespace Strict
    {
        template< typename tException, typename tT >
        inline void AssertEqual( const tT &expected, const tT &actual )
        {
#ifndef SERIALISATION_DISABLE_TYPE_CHECKS

            if ( expected != actual )
            {
                throw tException( expected, actual );
            }

#endif
        }
    }

    namespace Compat
    {
        template< typename tT >
        inline void AssertCompatible( const Type::Type &actual )
        {
#ifndef SERIALISATION_DISABLE_TYPE_CHECKS
#ifndef SERIALISATION_ENABLE_STRICT_TYPES

            switch ( Type::GetHeaderEnum< tT >() )
            {
            case Type::Array:
                Strict::AssertEqual< InvalidTypeException >( Type::Array, actual );
                break;

            case Type::Object:
                Strict::AssertEqual< InvalidTypeException >( Type::Object, actual );
                break;

            case Type::String:
                Strict::AssertEqual< InvalidTypeException >( Type::String, actual );
                break;

            case Type::UInt8:
            case Type::UInt16:
            case Type::UInt32:
            case Type::UInt64:
                if ( actual != Type::UInt8 && actual != Type::UInt16 && actual != Type::UInt32 && actual != Type::UInt64 )
                {
                    throw InvalidTypeException( Type::GetHeaderEnum<tT>(), actual );
                }

                break;

            default:
                assert( false &&
                        "Whoops! Something went haywire. Please try to reproduce this exception in an example as small as possible and submit it as an issue. Thanks!" );
                break;
            }

#else
            Strict::AssertEqual< InvalidTypeException >( Type::GetHeaderEnum< tT >(), actual );
#endif
#endif
        }
    }

    template< typename tException >
    inline void Assert( bool condition )
    {
        if ( !condition )
        {
            throw tException();
        }
    }
}

class InvalidTypeException
    : public std::exception
{
public:

    InvalidTypeException( Type::Type expected, Type::Type actual )
        : mExpected( expected ),
          mActual( actual )
    {
        std::stringstream ss;
        ss << "Expected " << expected << " but got " << actual;
        mWhat = ss.str();
    }

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;

    Type::Type mExpected;
    Type::Type mActual;
};

class EndOfStreamException
    : public std::exception
{
public:

    EndOfStreamException()
        : mWhat( "Unexpected end of stream" )
    {
    }

    virtual const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;
};

#endif
