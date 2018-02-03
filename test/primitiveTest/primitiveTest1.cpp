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
#include "testClasses.h"

#define SERIALISATION_TEST_SINGLE_PRIMITIVE_NUMERIC( type )                                                                   \
    SERIALISATION_TEST( SinglePrimitive, Max, SinglePrimitive< type >, type, std::numeric_limits< type >::max(), ( type )( 1 ) ); \
    SERIALISATION_TEST( SinglePrimitive, Min, SinglePrimitive< type >, type, std::numeric_limits< type >::min(), ( type )( 1 ) ); \
    SERIALISATION_TEST( SinglePrimitive, Zebra, SinglePrimitive< type >, type, GenerateZebraValue< type >(), ( type )( 1 ) );     \
    SERIALISATION_TEST( SinglePrimitive, InvZebra, SinglePrimitive< type >, type, GenerateInvZebraValue< type >(), ( type )( 1 ) );

SERIALISATION_ALL_NUMERIC_TYPES(SERIALISATION_TEST_SINGLE_PRIMITIVE_NUMERIC);
SERIALISATION_TEST(SinglePrimitive, 0_1, SinglePrimitive< String >, String, GenerateRandomString(false, 0),
                   GenerateRandomString(false, 1));
SERIALISATION_TEST(SinglePrimitive, FF, SinglePrimitive< bool >, bool, false, false);
SERIALISATION_TEST(SinglePrimitive, FT, SinglePrimitive< bool >, bool, false, true);
SERIALISATION_TEST(SinglePrimitive, TF, SinglePrimitive< bool >, bool, true, false);
SERIALISATION_TEST(SinglePrimitive, TT, SinglePrimitive< bool >, bool, true, true);
