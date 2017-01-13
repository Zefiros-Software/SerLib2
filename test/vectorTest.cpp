/**
 * Copyright (c) 2017 Mick van Duijn, Koen Visscher and Paul Visscher
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

#define SERIALISATION_ARRAY_TEST( type )                                                                               \
SERIALISATION_TEST( Container, Vector, TestClassArray< type >, type, MakeSeed<type>( 42 ) , GetRandom<uint32_t>() );

#define SERIALISATION_SMALL_ARRAY_TEST( type )                                                          \
SERIALISATION_TEST( Container, SmallVector, TestClassSmallArray< type >, type, MakeSeed<type>( 42 ) ,   \
                    GetRandom<uint32_t>() );

typedef TestClassArray< bool, 0x1 > PackedBoolVector;
SERIALISATION_TEST( Container, BoolPackedVector, PackedBoolVector, bool, MakeSeed<bool>( 424142 ), 424142 )

typedef TestClassArrayOfSize< bool, 0x1, 1024 > PackedBoolVectorOfSize;
SERIALISATION_TEST( Container, BoolPackedVectorOfSize, PackedBoolVectorOfSize, bool, MakeSeed<bool>( 4241 ), 4241 );

#define SERIALISATION_EDGE_CASE_ARRAY_TEST( type )                                                              \
SERIALISATION_TEST( Container, EdgeCaseVector, TestClassArrayEdgeCaseSize< type >, type, MakeSeed<type>( 42 ) , \
                    GetRandom<uint32_t>() );


#define SERIALISATION_ARRAY_REORDERED_TEST( type )                                                          \
SERIALISATION_TEST2( Container, VectorReordered, TestClassArrayWithMemberReordered< type >,                 \
                     TestClassArrayWithMember< type >, type, MakeSeed<type>( 42 ), GetRandom<uint32_t>() );


#define SERIALISATION_ARRAY_SKIPPED_TEST( type )                                                                    \
SERIALISATION_TEST2( Skipping, Vector, SkippedArray< type >, SinglePrimitive<uint8_t>, type, MakeSeed<type>( 42 ),  \
                     GetRandom<uint8_t>() );

typedef SkippedArray< bool, 0x1 > SkippedBoolVectorPacked;
SERIALISATION_TEST2( Skipping, VectorPacked, SkippedBoolVectorPacked, SinglePrimitive<uint8_t>, bool,
                     MakeSeed<bool>( 44 ), GetRandom<uint8_t>() );

#define SERIALISATION_ARRAY_NON_EXISTING_TEST( type )                                                       \
SERIALISATION_TEST2( NonExisting, Vector, SinglePrimitive<uint8_t>, SkippedArray< type >, type,             \
                     SERIALISATION_PP_TEMPLATE2( MakeSeed, type, uint8_t )( 42 ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_TEST );

SERIALISATION_SMALL_ARRAY_TEST( float );
SERIALISATION_SMALL_ARRAY_TEST( double );
SERIALISATION_EDGE_CASE_ARRAY_TEST( float );
SERIALISATION_EDGE_CASE_ARRAY_TEST( double );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_TEST );
SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_SKIPPED_TEST );
SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_NON_EXISTING_TEST );

#ifndef _DEBUG
#define SERIALISATION_ARRAY_REORDERED_REVERSE_TEST( type )                                                          \
SERIALISATION_TEST2( Container, VectorReorderedReverse, TestClassArrayWithMember< type >,                           \
                     TestClassArrayWithMemberReordered< type >, type, MakeSeed<type>( 42 ), GetRandom<uint32_t>() );

SERIALISATION_ALL_TYPES( SERIALISATION_ARRAY_REORDERED_REVERSE_TEST );
#endif
