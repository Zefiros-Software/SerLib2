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
#include "serialisation/util.h"

#include <math.h>

double Util::LDExp( double x, int32_t n )
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

    u.i = static_cast<uint64_t>( 0x3ff + n ) << 52;
    x = y * u.f;
    return x;
}

float Util::LDExp( float x, int32_t n )
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

    u.i = static_cast<uint32_t>( 0x7f + n ) << 23;
    x = y * u.f;
    return x;
}

double Util::FRExp( double x, int32_t *e )
{
    union
    {
        double d;
        uint64_t i;
    } y = {x};
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

float Util::FRExp( float x, int32_t *e )
{
    union
    {
        float f;
        uint32_t i;
    } y = {x};
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

uint32_t Util::FloatToUInt32( const float f )
{
    int32_t exp = 0;
    float fi = FRExp( f, &exp );
    --exp;

    uint32_t result = ZigZag<int32_t, uint32_t>( exp );
    result |= ZigZag<int32_t, uint32_t>( static_cast<int32_t>( LDExp( fi, 23 ) ) ) << 8;

    return result;
}

float Util::UInt32ToFloat( const uint32_t i )
{
    int32_t exp = ZagZig<uint32_t, int32_t>( i & 0xff );
    ++exp;
    return LDExp( LDExp( static_cast<float>( ZagZig<uint32_t, int32_t>( i >> 8 ) ), -23 ), exp );
}

uint64_t Util::DoubleToUInt64( const double f )
{
    int32_t exp = 0;
    double fi = FRExp( f, &exp );
    --exp;

    uint64_t result = ZigZag<int64_t, uint64_t>( exp );
    result |= ZigZag<int64_t, uint64_t>( static_cast<int64_t>( LDExp( fi, 52 ) ) ) << 11;

    return result;
}

double Util::UInt64ToDouble( const uint64_t i )
{
    int32_t exp = ZagZig<uint32_t, int32_t>( i & 0x7ff );
    ++exp;
    return LDExp( LDExp( static_cast<double>( ZagZig<uint64_t, int64_t>( i >> 11 ) ), -52 ), exp );
}
