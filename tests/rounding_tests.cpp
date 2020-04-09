
#include "common/rounding.hpp"

#include <gtest/gtest.h>

using namespace Math;

TEST( Math, clamp_floats )
{
    ASSERT_FLOAT_EQ( 0.0f, clamp( 0.0f, 1.0f, -2.0f ) );
    ASSERT_FLOAT_EQ( 1.0f, clamp( 0.0f, 1.0f, 2.0f ) );
    ASSERT_FLOAT_EQ( 0.0f, clamp( 0.0f, 1.0f, -1.0f ) );
    ASSERT_FLOAT_EQ( 1.0f, clamp( 0.0f, 1.0f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.5f, clamp( 0.0f, 1.0f, 0.5f ) );
    ASSERT_FLOAT_EQ( -5.0f, clamp( -12.5f, -5.0f, 0.0f ) );
    ASSERT_FLOAT_EQ( -12.5f, clamp( -12.5f, -5.0f, -13.0f ) );
    ASSERT_FLOAT_EQ( -6.0f, clamp( -12.5f, -5.0f, -6.0f ) );
}
TEST( Math, clamp_ints )
{
    ASSERT_EQ(  0, clamp( 0, 1, -2 ) );
    ASSERT_EQ(  1, clamp( 0, 1,  2 ) );
    ASSERT_EQ(  0, clamp( 0, 1, -1 ) );
    ASSERT_EQ(  1, clamp( 0, 1,  1 ) );
    ASSERT_EQ(  2, clamp( 0, 3,  2 ) );
    ASSERT_EQ( -5, clamp( -12, -5, 0 ) );
    ASSERT_EQ(-12, clamp( -12, -5, -13 ) );
    ASSERT_EQ( -6, clamp( -12, -5, -6 ) );
}

TEST( Math, mapToRange_uint )
{
    ASSERT_EQ( 0u, mapToRange( 0u, 1u ) );
    ASSERT_EQ( 0u, mapToRange( 1u, 1u ) );
    ASSERT_EQ( 1u, mapToRange( 1u, 2u ) );
}

TEST( Math, mapToRange_int )
{
    ASSERT_EQ( 0, mapToRange( 0, 1 ) );
    ASSERT_EQ( 0, mapToRange( 1, 1 ) );
    ASSERT_EQ( 1, mapToRange( 1, 2 ) );
    ASSERT_EQ( 0, mapToRange( -1, 1 ) );
    ASSERT_EQ( 2, mapToRange( -2, 4 ) );
    ASSERT_EQ( 2, mapToRange( 2, 4 ) );
}

TEST( Math, mapToRange_floats )
{
    ASSERT_FLOAT_EQ( 0.0f, mapToRange( -2.0f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.0f, mapToRange( -1.0f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.0f, mapToRange( 0.0f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.0f, mapToRange( 1.0f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.0f, mapToRange( 2.0f, 1.0f ) );

    ASSERT_FLOAT_EQ( 0.5f, mapToRange( -2.5f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.5f, mapToRange( -1.5f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.5f, mapToRange( 0.5f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.5f, mapToRange( 1.5f, 1.0f ) );
    ASSERT_FLOAT_EQ( 0.5f, mapToRange( 2.5f, 1.0f ) );
}
TEST( Math, mapToRange_double )
{
    ASSERT_DOUBLE_EQ( 0.0, mapToRange( -2.0, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.0, mapToRange( -1.0, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.0, mapToRange( 0.0, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.0, mapToRange( 1.0, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.0, mapToRange( 2.0, 1.0 ) );

    ASSERT_DOUBLE_EQ( 0.5, mapToRange( -2.5, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.5, mapToRange( -1.5, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.5, mapToRange( 0.5, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.5, mapToRange( 1.5, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.5, mapToRange( 2.5, 1.0 ) );
}


TEST( Math, Quantize_float )
{
    ASSERT_FLOAT_EQ( 100.0f, quantize( 101.1111f, 10.0f ) );
    ASSERT_FLOAT_EQ( 101.0f, quantize( 101.1111f, 1.0f ) );
    ASSERT_FLOAT_EQ( 101.1f, quantize( 101.1111f, 0.1f ) );

    ASSERT_FLOAT_EQ( -110.0f, quantize( -101.1111f, 10.0f ) );
    ASSERT_FLOAT_EQ( -102.0f, quantize( -101.1111f, 1.0f ) );
    ASSERT_FLOAT_EQ( -101.2f, quantize( -101.1111f, 0.1f ) );
    ASSERT_FLOAT_EQ( -101.12f, quantize( -101.1111f, 0.01f ) );
}

TEST( Math, Quantize_uint )
{
    ASSERT_EQ( 3u, quantize( 5u, 3u ) );
    ASSERT_EQ( 100u, quantize( 123u, 100u ) );
}

TEST( Math, Quantize_int )
{
    ASSERT_EQ( 3, quantize( 5, 3 ) );
    ASSERT_EQ( 100, quantize( 123, 100 ) );
    ASSERT_EQ( -3, quantize( -3, 3 ) );
    ASSERT_EQ( -3, quantize( -2, 3 ) );
    ASSERT_EQ( -6, quantize( -5, 3 ) );
    ASSERT_EQ( -200, quantize( -123, 100 ) );
}

TEST( Math, roundRealOutToInt )
{
    ASSERT_EQ( 0, roundRealOutToInt( 0.0f ) );
    ASSERT_EQ( 1, roundRealOutToInt( 0.001f ) );
    ASSERT_EQ( -1, roundRealOutToInt( -0.001f ) );
    ASSERT_EQ( 124, roundRealOutToInt( 123.123f ) );
    ASSERT_EQ( -124, roundRealOutToInt( -123.123f ) );
}

TEST( Math, roundPositiveRealToUInt )
{
    ASSERT_EQ( 0u, roundPositiveRealToUInt( 0.0f ) );
    ASSERT_EQ( 0u, roundPositiveRealToUInt( 0.499f ) );
    ASSERT_EQ( 1u, roundPositiveRealToUInt( 0.51f ) );
    ASSERT_EQ( 1u, roundPositiveRealToUInt( 0.5f ) );
    ASSERT_EQ( 123u, roundPositiveRealToUInt( 123.123f ) );
    ASSERT_EQ( 124u, roundPositiveRealToUInt( 123.523f ) );
}

TEST( Math, mapToRange )
{
    ASSERT_THROW( mapToRange( 0.0, 0.0 ), std::runtime_error );
    ASSERT_THROW( mapToRange( 0, 0 ), std::runtime_error );
    ASSERT_THROW( mapToRange( 0u, 0u ), std::runtime_error );

    ASSERT_DOUBLE_EQ( 0.0, mapToRange( 0.0, 1.0 ) );
    ASSERT_DOUBLE_EQ( 0.0f, mapToRange( 0.0f, 1.0f ) );
    ASSERT_EQ( 0, mapToRange( 0, 1 ) );
    
    ASSERT_DOUBLE_EQ( 1, mapToRange( 1, 10 ) );
    ASSERT_DOUBLE_EQ( 1.0, mapToRange( 1.0, 10.0 ) );
    ASSERT_DOUBLE_EQ( 1u, mapToRange( 1u, 10u ) );
    
    ASSERT_DOUBLE_EQ( 1.9, mapToRange( 1.9, 10.0 ) );
    ASSERT_DOUBLE_EQ( 1.9, mapToRange( 11.9, 10.0 ) );
    ASSERT_DOUBLE_EQ( 1.9, mapToRange( -8.1, 10.0 ) );
    ASSERT_NEAR( 1.9, mapToRange( -18.1, 10.0 ), 0.0000001 );
    ASSERT_NEAR( 1.9, mapToRange( -108.1, 10.0 ), 0.0000001 );
}


TEST( Math, quantize_roundUpInt_PowerOfTwo )
{
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 1U, 2U ), 2U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 2U, 2U ), 2U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 11U, 2U ), 12U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 11U, 4U ), 12U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 11U, 8U ), 16U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 11U, 16U ), 16U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 1U, 16U ), 16U );
    ASSERT_EQ( quantize_roundUpInt_PowerOfTwo( 1U, 256U ), 256U );
}

TEST( Math, quantize_nearestGreaterPowerOfTwo )
{
    ASSERT_EQ( quantize_nearestPowerOfTwo( 1U ), 1 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 2U ), 2 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 3U ), 4 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 4U ), 4 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 5U ), 8 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 8U ), 8 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 9U ), 16 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 17U ), 32 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 123U ), 128 );
    ASSERT_EQ( quantize_nearestPowerOfTwo( 129U ), 256 );
}







