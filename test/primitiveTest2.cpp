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

#define  SERIALISATION_TEST_MULTI_PRIMITVE( type1, type2, type3, type4, type5 )                                         \
SERIALISATION_TEST(  MultiPrimitive, type1 ## type2 ## type3 ## type4 ## type5,                                         \
    MultiPrimitive< 0 PP_COMMA() type1 PP_COMMA() type2 PP_COMMA() type3 PP_COMMA() type4 PP_COMMA() type5 >, type5,    \
                    MakeSeed<type1>( 424142 ), 424142 );

#define SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS( type ) \
        SERIALISATION_TEST_MULTI_PRIMITVE( uint8_t, int32_t, double, String, type );

SERIALISATION_ALL_TYPES( SERIALISATION_TEST_MULTI_PRIMITVE_VARIATIONS );

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

#if defined( _DEBUG ) or defined( __clang__ )
#define  SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 0 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 7 );                     \
SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC( type, 27 );
#else
#define SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES( type )    \
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
#endif

SERIALISATION_ALL_NUMERIC_TYPES( SERIALISATION_TEST_SINGLE_PRIMITIVE_INDEXED_NUMERIC_ALL_INDICES );
