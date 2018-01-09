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
#include "testClasses.h"

#define SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, index )                      \
    SERIALISATION_TEST( SinglePrimitiveIndexed, Max ## _ ## index,                                  \
                        SinglePrimitiveIndexed< type PP_COMMA() index >, type,    \
                        std::numeric_limits< type >::max(), ( type )( 1 ) );                        \
    SERIALISATION_TEST( SinglePrimitiveIndexed, Min ## _ ## index,                                  \
                        SinglePrimitiveIndexed< type PP_COMMA() index >, type,    \
                        std::numeric_limits< type >::min(), ( type )( 1 ) );                        \
    SERIALISATION_TEST( SinglePrimitiveIndexed, Zebra ## _ ## index,                                \
                        SinglePrimitiveIndexed< type PP_COMMA() index >, type,    \
                        GenerateZebraValue< type >(), ( type )( 1 ) );                              \
    SERIALISATION_TEST( SinglePrimitiveIndexed, InvZebra ## _ ## index,                             \
                        SinglePrimitiveIndexed< type PP_COMMA() index >, type,    \
                        GenerateInvZebraValue< type >(), ( type )( 1 ) );

#ifdef _DEBUG
#   define SERIALISATION_LIMIT_INDICES_TEST
#endif
#ifdef __clang__
#   define SERIALISATION_LIMIT_INDICES_TEST
#endif

#ifndef SERIALISATION_LIMIT_INDICES_TEST
#define  SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 0 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 1 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 2 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 3 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 5 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 7 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 11 );                    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 13 );                    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 17 );                    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 19 );                    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 23 );                    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 27 );
#else
#define  SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 0 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 7 );                     \
    SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 27 );
#endif // !SERIALISATION_LIMIT_INDICES_TEST

SERIALISATION_ALL_NUMERIC_TYPES(SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES);
