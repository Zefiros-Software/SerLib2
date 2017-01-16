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
#ifndef __SERIALISATION_EXCEPTIONS_H__
#define __SERIALISATION_EXCEPTIONS_H__

#include "serialisation/types.h"

#include <exception>
#include <sstream>

class InvalidTypeException
    : public std::exception
{
public:

    InvalidTypeException( Type::Type expected, Type::Type actual )
    {
        std::stringstream ss;
        ss << "Expected " << Type::ToString( expected ) << " but got " << Type::ToString( actual );
        mWhat = ss.str();
    }

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;
};

class EndOfStreamException
    : public std::exception
{
public:

    EndOfStreamException()
        : mWhat( "Unexpected end of stream" )
    {
    }

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;
};

class NoCleanExitException
    : public std::exception
{
public:

    NoCleanExitException()
        : mWhat( "Attempted to reuse message after unclean exit" )
    {
    }

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;
};

class FileOpenException
    : public std::exception
{
public:

    explicit FileOpenException( const std::string &file )
    {
        std::stringstream ss;
        ss << "Could not open file " << file;
        mWhat = ss.str();
    }

    const char *what() const noexcept override
    {
        return mWhat.c_str();
    }

private:

    std::string mWhat;
};

namespace ExceptionHelper
{
    namespace Strict
    {
        template< typename tException, typename tT >
        void AssertEqual( const tT &expected, const tT &actual, bool &cleanExit )
        {
#ifndef SERIALISATION_DISABLE_TYPE_CHECKS

            if ( expected != actual )
            {
                cleanExit = false;
                throw tException( expected, actual );
            }

#endif
        }
    }

    template< typename tT, bool tIsInteger = std::is_integral<tT>::value >
    class Compat
    {
    public:

        static  bool AssertCompatible( const Type::Type &actual, bool &cleanExit )
        {
            if ( actual != Type::UInt8 && actual != Type::UInt16 && actual != Type::UInt32 && actual != Type::UInt64 )
            {
                cleanExit = false;
                throw InvalidTypeException( Type::GetHeaderEnum<tT>(), actual );
            }

            // return whether conversion is needed
            return actual != Type::GetHeaderEnum<tT>();
        }
    };

    template <typename tT>
    class Compat<tT, false>
    {
    public:
        static bool AssertCompatible( const Type::Type &, bool & )
        {
            assert( false &&
                    "Whoops! Something went haywire. Please try to reproduce this exception in an example as small as possible and submit it as an issue. Thanks!" );
            return false;
        }
    };

    template< typename tException >
    void Assert( bool condition, bool &cleanExit )
    {
        if ( !condition )
        {
            cleanExit = false;
            throw tException();
        }
    }
}

#endif
