
#include "common/math.hpp"

#include <gtest/gtest.h>

using namespace Math;

TEST( Math, POW10 )
{
    ASSERT_DOUBLE_EQ( 1.0, pow10( 0 ) );
    ASSERT_DOUBLE_EQ( 1000.0, pow10( 3 ) );
    ASSERT_DOUBLE_EQ( 1000000.0, pow10( 6 ) );
    ASSERT_DOUBLE_EQ( 0.001, pow10( -3 ) );
    ASSERT_DOUBLE_EQ( 0.000001, pow10( -6 ) );
}

TEST( BasicMath, IndexTest_X )
{
    ASSERT_EQ( 0, getXFrom2dIndex( 4, 0 ) );
    ASSERT_EQ( 1, getXFrom2dIndex( 4, 1 ) );
    ASSERT_EQ( 2, getXFrom2dIndex( 4, 2 ) );
    ASSERT_EQ( 3, getXFrom2dIndex( 4, 3 ) );
    ASSERT_EQ( 0, getXFrom2dIndex( 4, 4 ) );
    ASSERT_EQ( 1, getXFrom2dIndex( 4, 5 ) );
    ASSERT_EQ( 2, getXFrom2dIndex( 4, 6 ) );
    ASSERT_EQ( 3, getXFrom2dIndex( 4, 7 ) );
    ASSERT_EQ( 0, getXFrom2dIndex( 4, 8 ) );
}

TEST( BasicMath, IndexTest_Y )
{
    ASSERT_EQ( 0, getYFrom2dIndex( 4, 0 ) );
    ASSERT_EQ( 0, getYFrom2dIndex( 4, 1 ) );
    ASSERT_EQ( 0, getYFrom2dIndex( 4, 2 ) );
    ASSERT_EQ( 0, getYFrom2dIndex( 4, 3 ) );
    ASSERT_EQ( 1, getYFrom2dIndex( 4, 4 ) );
    ASSERT_EQ( 1, getYFrom2dIndex( 4, 5 ) );
    ASSERT_EQ( 1, getYFrom2dIndex( 4, 6 ) );
    ASSERT_EQ( 1, getYFrom2dIndex( 4, 7 ) );
    ASSERT_EQ( 2, getYFrom2dIndex( 4, 8 ) );

}

TEST( BasicMath, IndexTest3d )
{
    ASSERT_EQ( 0, getXFrom3dIndex( 2, 3, 0 ) );
    ASSERT_EQ( 0, getYFrom3dIndex( 2, 3, 0 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 0 ) );
    
    ASSERT_EQ( 1, getXFrom3dIndex( 2, 3, 1 ) );
    ASSERT_EQ( 0, getYFrom3dIndex( 2, 3, 1 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 1 ) );
    
    ASSERT_EQ( 0, getXFrom3dIndex( 2, 3, 2 ) );
    ASSERT_EQ( 1, getYFrom3dIndex( 2, 3, 2 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 2 ) );
    
    ASSERT_EQ( 1, getXFrom3dIndex( 2, 3, 3 ) );
    ASSERT_EQ( 1, getYFrom3dIndex( 2, 3, 3 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 3 ) );
    
    ASSERT_EQ( 0, getXFrom3dIndex( 2, 3, 4 ) );
    ASSERT_EQ( 2, getYFrom3dIndex( 2, 3, 4 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 4 ) );
    
    ASSERT_EQ( 1, getXFrom3dIndex( 2, 3, 5 ) );
    ASSERT_EQ( 2, getYFrom3dIndex( 2, 3, 5 ) );
    ASSERT_EQ( 0, getZFrom3dIndex( 2, 3, 5 ) );
    
    ASSERT_EQ( 0, getXFrom3dIndex( 2, 3, 6 ) );
    ASSERT_EQ( 0, getYFrom3dIndex( 2, 3, 6 ) );
    ASSERT_EQ( 1, getZFrom3dIndex( 2, 3, 6 ) );
    
    ASSERT_EQ( 1, getXFrom3dIndex( 2, 3, 7 ) );
    ASSERT_EQ( 0, getYFrom3dIndex( 2, 3, 7 ) );
    ASSERT_EQ( 1, getZFrom3dIndex( 2, 3, 7 ) );
    
    ASSERT_EQ( 0, getXFrom3dIndex( 2, 3, 8 ) );
    ASSERT_EQ( 1, getYFrom3dIndex( 2, 3, 8 ) );
    ASSERT_EQ( 1, getZFrom3dIndex( 2, 3, 8 ) );
    
    ASSERT_EQ( 1, getXFrom3dIndex( 2, 3, 9 ) );
    ASSERT_EQ( 1, getYFrom3dIndex( 2, 3, 9 ) );
    ASSERT_EQ( 1, getZFrom3dIndex( 2, 3, 9 ) );

}
