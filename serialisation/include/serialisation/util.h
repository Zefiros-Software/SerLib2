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
#ifndef __SERIALISATION_UTIL_H__
#define __SERIALISATION_UTIL_H__

#include "serialisation/types.h"

#include <stdlib.h>
#include <math.h>

namespace Util
{
    inline double LDExp( double x, int32_t n )
    {
        union
        {
            double f;
            uint64_t i;
        } u;
        double y = x;

        if ( n > 1023 )
        {
            y *= 8.9884656743115795e+307;
            n -= 1023;

            if ( n > 1023 )
            {
                y *= 8.9884656743115795e+307;
                n -= 1023;

                if ( n > 1023 )
                {
                    n = 1023;
                }
            }
        }
        else if ( n < -1022 )
        {
            y *= 2.2250738585072014e-308;
            n += 1022;

            if ( n < -1022 )
            {
                y *= 2.2250738585072014e-308;
                n += 1022;

                if ( n < -1022 )
                {
                    n = -1022;
                }
            }
        }

        u.i = ( uint64_t )( 0x3ff + n ) << 52;
        x = y * u.f;
        return x;
    }

    inline float LDExp( float x, int32_t n )
    {
        union
        {
            float f;
            uint32_t i;
        } u;
        float_t y = x;

        if ( n > 127 )
        {
            y *= 1.7014118346046923e+38f;
            n -= 127;

            if ( n > 127 )
            {
                y *= 1.7014118346046923e+38f;
                n -= 127;

                if ( n > 127 )
                {
                    n = 127;
                }
            }
        }
        else if ( n < -126 )
        {
            y *= 1.1754943508222875e-38f;
            n += 126;

            if ( n < -126 )
            {
                y *= 1.1754943508222875e-38f;
                n += 126;

                if ( n < -126 )
                {
                    n = -126;
                }
            }
        }

        u.i = ( uint32_t )( 0x7f + n ) << 23;
        x = y * u.f;
        return x;
    }

    inline double FRExp( double x, int32_t *e )
    {
        union
        {
            double d;
            uint64_t i;
        } y = { x };
        int ee = y.i >> 52 & 0x7ff;

        if ( !ee )
        {
            if ( x )
            {
                x = FRExp( x * 1.8446744073709552e+19, e );
                *e -= 64;
            }
            else
            {
                *e = 0;
            }

            return x;
        }
        else if ( ee == 0x7ff )
        {
            return x;
        }

        *e = ee - 0x3fe;
        y.i &= 0x800fffffffffffffull;
        y.i |= 0x3fe0000000000000ull;
        return y.d;
    }

    inline float FRExp( float x, int32_t *e )
    {
        union
        {
            float f;
            uint32_t i;
        } y = { x };
        int ee = y.i >> 23 & 0xff;

        if ( !ee )
        {
            if ( x )
            {
                x = FRExp( x * 1.8446744073709552e+19f, e );
                *e -= 64;
            }
            else
            {
                *e = 0;
            }

            return x;
        }
        else if ( ee == 0xff )
        {
            return x;
        }

        *e = ee - 0x7e;
        y.i &= 0x807ffffful;
        y.i |= 0x3f000000ul;
        return y.f;
    }

    template< typename S, typename U >
    U ZigZag( const S s )
    {
        return ( s >> ( ( sizeof( S ) << 3 ) - 1 ) ) ^ ( s << 1 );
    }

    template< typename U, typename S >
    S ZagZig( const U u )
    {
        return ( u >> 1 ) ^ ( -( static_cast< S >( u ) & 1 ) );
    }

    inline uint32_t FloatToUInt32( const float f )
    {
        int32_t exp;
        float fi = FRExp( f, &exp );
        --exp;

        uint32_t result = ZigZag< int32_t, uint32_t >( exp );
        result |= ZigZag< int32_t, uint32_t >( static_cast<int32_t>( LDExp( fi, 23 ) ) ) << 8;

        return result;
    }

    inline float UInt32ToFloat( const uint32_t i )
    {
        int32_t exp = ZagZig< uint32_t, int32_t >( i & 0xff );
        ++exp;
        return LDExp( LDExp( static_cast<float>( ZagZig< uint32_t, int32_t >( i >> 8 ) ), -23 ), exp );
    }

    inline uint64_t DoubleToUInt64( const double f )
    {
        int32_t exp;
        double fi = frexp( f, &exp );
        --exp;

        uint64_t result = ZigZag< int64_t, uint64_t >( exp );
        result |= ZigZag< int64_t, uint64_t >( static_cast<int64_t>( ldexp( fi, 52 ) ) ) << 11;

        return result;
    }

    inline double UInt64ToDouble( const uint64_t i )
    {
        int32_t exp = ZagZig< uint32_t, int32_t >( i & 0x7ff );
        ++exp;
        return ldexp( ldexp( static_cast<double>( ZagZig< uint64_t, int64_t >( i >> 11 ) ), -52 ), exp );
    }

    //     template< typename T >
    //     uint8_t CalculateVarIntSize( T val )
    //     {
    //         uint8_t size = 1;

    //         for ( val >>= 7; val > 0; val >>= 7 )
    //         {
    //             ++size;
    //         }

    //         return size;
    //     }

    //     template< typename T >
    //     T CreateHeader( const T index, const Internal::Type::Type t )
    //     {
    //         return ( index << 3 ) | ( static_cast< T >( t ) & 0x07 );
    //     }

    template< typename tT >
    Type::Type GetHeaderType( const tT header )
    {
        return static_cast< Type::Type >( header & 0x07 );
    }

    template< typename tT >
    tT GetHeaderIndex( const tT header )
    {
        return static_cast< tT >( header >> 3 );
    }
    //}

    // #ifndef SERIALISATION_NO_HEADER_ONLY
    // #   include "../../src/util.cpp"
    // #endif


    constexpr uint8_t CreateHeader( uint8_t index, Type::Type type )
    {
        return ( index << 3 ) | ( type & 0x07 );
    }

    // Index: highest 5 bits
    // Enum:  lowest  3 bits
    template< typename tT >
    constexpr uint8_t CreateHeader( uint8_t index )
    {
        return CreateHeader( index, Type::GetHeaderEnum< tT >() );
    }

    //     template< uint8_t tIndex, typename tT >
    //     constexpr uint8_t CreateHeader( uint8_t index)
    //     {
    //         return static_cast<uint8_t>( ( tIndex << 3 ) | Internal::Type::GetEnum< tT >() );
    //     }
}

#endif