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

#define SERIALISATION_PARENT_TEST( type )                                                                               \
SERIALISATION_TEST( Object, Parent_Zebra, ClassWithParent< type >, type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_TEST );

#define SERIALISATION_PARENT_REORDERED_TEST( type )                                                                    \
SERIALISATION_TEST2( Object, Parent_Zebra_Reordered, ClassWithParent< type >, ClassWithParentReordered< type >, type,  \
                     GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_REORDERED_TEST );

#define SERIALISATION_PARENT_REORDERED_REVERSE_TEST( type )                                                            \
SERIALISATION_TEST2( Object, Parent_Zebra_ReorderedReverse, ClassWithParentReordered< type >, ClassWithParent< type >, \
                     type, GenerateZebraValue<type>(), GetRandom<type>() );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_REORDERED_REVERSE_TEST );

SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint32_t >, uint32_t, GenerateZebraValue<uint32_t>(),
                    GetRandom<uint32_t>() );
SERIALISATION_TEST( Object, MultiParent, ClassWithMultipleParents< uint64_t >, uint64_t, GenerateZebraValue<uint64_t>(),
                    GetRandom<uint64_t>() );

#define SERIALISATION_PARENT_SKIPPING_TEST( type )                                              \
SERIALISATION_TEST2( Skipping, Parent, SkippedParent< type >, SinglePrimitive< uint8_t >, type, \
                     MakeSeed<type>( 414141 ), 189 );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_SKIPPING_TEST );

#define SERIALISATION_PARENT_NON_EXISTING_TEST( type )                                              \
SERIALISATION_TEST2( NonExisting, Parent, SinglePrimitive< uint8_t >, SkippedParent< type >, type,  \
                     SERIALISATION_PP_TEMPLATE2( MakeSeed, type, uint8_t )( 414141 ), 414141 );

SERIALISATION_ALL_TYPES( SERIALISATION_PARENT_NON_EXISTING_TEST );
