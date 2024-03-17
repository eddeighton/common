
#include "common/angle.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <iostream>

using namespace Math;

TEST( DiscreteAngle, Rotate_16 )
{
    typedef Angle< 16 > A;
    ASSERT_EQ( A::eNorthNorthEast, rotate< A >( A::eNorth, 1 ) );
    ASSERT_EQ( A::eNorthNorthWest, rotate< A >( A::eNorth, -1 ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, -A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorthNorthEast, rotate< A >( A::eNorth, 1 + A::TOTAL_ANGLES * 3 ) );
    ASSERT_EQ( A::eNorthNorthWest, rotate< A >( A::eNorth, -( 1 + A::TOTAL_ANGLES * 3 ) ) );
    ASSERT_EQ( A::eSouthSouthEast, rotate< A >( A::eEast, 3 ) );
    ASSERT_EQ( A::eSouthSouthEast, rotate< A >( A::eEast, 3 + A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eSouthSouthEast, rotate< A >( A::eEast, 3 - A::TOTAL_ANGLES ) );
}
TEST( DiscreteAngle, Rotate_8 )
{
    typedef Angle< 8 > A;
    ASSERT_EQ( A::eNorthEast, rotate< A >( A::eNorth, 1 ) );
    ASSERT_EQ( A::eNorthWest, rotate< A >( A::eNorth, -1 ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, -A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorthEast, rotate< A >( A::eNorth, 1 + A::TOTAL_ANGLES * 3 ) );
    ASSERT_EQ( A::eNorthWest, rotate< A >( A::eNorth, -( 1 + A::TOTAL_ANGLES * 3 ) ) );
    ASSERT_EQ( A::eSouthWest, rotate< A >( A::eEast, 3 ) );
    ASSERT_EQ( A::eSouthWest, rotate< A >( A::eEast, 3 + A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eSouthWest, rotate< A >( A::eEast, 3 - A::TOTAL_ANGLES ) );
}

TEST( DiscreteAngle, Rotate_4 )
{
    typedef Angle< 4 > A;
    ASSERT_EQ( A::eEast, rotate< A >( A::eNorth, 1 ) );
    ASSERT_EQ( A::eWest, rotate< A >( A::eNorth, -1 ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eNorth, -A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eEast, rotate< A >( A::eNorth, 1 + A::TOTAL_ANGLES * 3 ) );
    ASSERT_EQ( A::eWest, rotate< A >( A::eNorth, -( 1 + A::TOTAL_ANGLES * 3 ) ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eEast, 3 ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eEast, 3 + A::TOTAL_ANGLES ) );
    ASSERT_EQ( A::eNorth, rotate< A >( A::eEast, 3 - A::TOTAL_ANGLES ) );
}

TEST( DiscreteAngle, opposite_16 )
{
    typedef Angle< 16 > A;
    ASSERT_EQ( A::eSouth, opposite< A >( A::eNorth ) );
    ASSERT_EQ( A::eSouthWest, opposite< A >( A::eNorthEast ) );
    ASSERT_EQ( A::eNorthEast, opposite< A >( A::eSouthWest ) );
    ASSERT_EQ( A::eNorth, opposite< A >( A::eSouth ) );
    ASSERT_EQ( A::eEastNorthEast, opposite< A >( A::eWestSouthWest ) );
}
TEST( DiscreteAngle, opposite_8 )
{
    typedef Angle< 8 > A;
    ASSERT_EQ( A::eSouth, opposite< A >( A::eNorth ) );
    ASSERT_EQ( A::eSouthWest, opposite< A >( A::eNorthEast ) );
    ASSERT_EQ( A::eNorthEast, opposite< A >( A::eSouthWest ) );
    ASSERT_EQ( A::eNorth, opposite< A >( A::eSouth ) );
}
TEST( DiscreteAngle, opposite_4 )
{
    typedef Angle< 4 > A;
    ASSERT_EQ( A::eSouth, opposite< A >( A::eNorth ) );
    ASSERT_EQ( A::eEast, opposite< A >( A::eWest ) );
    ASSERT_EQ( A::eWest, opposite< A >( A::eEast ) );
    ASSERT_EQ( A::eNorth, opposite< A >( A::eSouth ) );
}

TEST( DiscreteAngle, toVectorDiscrete_16_float )
{
    typedef std::pair< float, float > VectorType;
    typedef Angle< 16 >               A;
    VectorType                        r;
    toVectorDiscrete< A, float >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 1.0f ), r );
    toVectorDiscrete< A, float >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, 1.0f ), r );
    toVectorDiscrete< A, float >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 1.0f ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_16_double )
{
    typedef std::pair< double, double > VectorType;
    typedef Angle< 16 >                 A;
    VectorType                          r;
    toVectorDiscrete< A, double >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, 0.0 ), r );
    toVectorDiscrete< A, double >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, 0.0 ), r );
    toVectorDiscrete< A, double >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, 1.0 ), r );
    toVectorDiscrete< A, double >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0, 1.0 ), r );
    toVectorDiscrete< A, double >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, 1.0 ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_16_int )
{
    typedef std::pair< int, int > VectorType;
    typedef Angle< 16 >           A;
    VectorType                    r;
    toVectorDiscrete< A, int >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, 0 ), r );
    toVectorDiscrete< A, int >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, -1 ), r );
    toVectorDiscrete< A, int >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, -1 ), r );
    toVectorDiscrete< A, int >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, -1 ), r );
    toVectorDiscrete< A, int >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, 0 ), r );
    toVectorDiscrete< A, int >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, 1 ), r );
    toVectorDiscrete< A, int >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, 1 ), r );
    toVectorDiscrete< A, int >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, 1 ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_8_float )
{
    typedef std::pair< float, float > VectorType;
    typedef Angle< 8 >                A;
    VectorType                        r;
    toVectorDiscrete< A, float >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 1.0f ), r );
    toVectorDiscrete< A, float >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, 1.0f ), r );
    toVectorDiscrete< A, float >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 1.0f ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_8_double )
{
    typedef std::pair< double, double > VectorType;
    typedef Angle< 8 >                  A;
    VectorType                          r;
    toVectorDiscrete< A, double >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, 0.0 ), r );
    toVectorDiscrete< A, double >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, -1.0 ), r );
    toVectorDiscrete< A, double >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, 0.0 ), r );
    toVectorDiscrete< A, double >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0, 1.0 ), r );
    toVectorDiscrete< A, double >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0, 1.0 ), r );
    toVectorDiscrete< A, double >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0, 1.0 ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_8_int )
{
    typedef std::pair< int, int > VectorType;
    typedef Angle< 8 >            A;
    VectorType                    r;
    toVectorDiscrete< A, int >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, 0 ), r );
    toVectorDiscrete< A, int >( A::eSouthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, -1 ), r );
    toVectorDiscrete< A, int >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, -1 ), r );
    toVectorDiscrete< A, int >( A::eSouthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, -1 ), r );
    toVectorDiscrete< A, int >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, 0 ), r );
    toVectorDiscrete< A, int >( A::eNorthWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, 1 ), r );
    toVectorDiscrete< A, int >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, 1 ), r );
    toVectorDiscrete< A, int >( A::eNorthEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, 1 ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_4_float )
{
    typedef std::pair< float, float > VectorType;
    typedef Angle< 4 >                A;
    VectorType                        r;
    toVectorDiscrete< A, float >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, -1.0f ), r );
    toVectorDiscrete< A, float >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 0.0f ), r );
    toVectorDiscrete< A, float >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, 1.0f ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_4_double )
{
    typedef std::pair< double, double > VectorType;
    typedef Angle< 4 >                  A;
    VectorType                          r;
    toVectorDiscrete< A, double >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1.0f, 0.0f ), r );
    toVectorDiscrete< A, double >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, -1.0f ), r );
    toVectorDiscrete< A, double >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1.0f, 0.0f ), r );
    toVectorDiscrete< A, double >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0.0f, 1.0f ), r );
}
TEST( DiscreteAngle, toVectorDiscrete_4_int )
{
    typedef std::pair< int, int > VectorType;
    typedef Angle< 4 >            A;
    VectorType                    r;
    toVectorDiscrete< A, int >( A::eEast, r.first, r.second );
    ASSERT_EQ( VectorType( 1, 0 ), r );
    toVectorDiscrete< A, int >( A::eSouth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, -1 ), r );
    toVectorDiscrete< A, int >( A::eWest, r.first, r.second );
    ASSERT_EQ( VectorType( -1, 0 ), r );
    toVectorDiscrete< A, int >( A::eNorth, r.first, r.second );
    ASSERT_EQ( VectorType( 0, 1 ), r );
}

TEST( DiscreteAngle, fromVector_4_float )
{
    typedef Angle< 4 >                A;

    ASSERT_EQ( ( A::eEast ), ( fromVector< A >( 1.0f, 0.0f ) ) );
    ASSERT_EQ( ( A::eSouth ), ( fromVector< A >( 0.0f, -1.0f ) ) );
    ASSERT_EQ( ( A::eWest ), ( fromVector< A >( -1.0f, 0.0f ) ) );
    ASSERT_EQ( ( A::eNorth ), ( fromVector< A >( 0.0f, 1.0f ) ) );
}
TEST( DiscreteAngle, fromVector_4_int )
{
    typedef Angle< 4 >            A;

    ASSERT_EQ( ( A::eEast ), ( fromVector< A >( 1, 0 ) ) );
    ASSERT_EQ( ( A::eSouth ), ( fromVector< A >( 0, -1 ) ) );
    ASSERT_EQ( ( A::eWest ), ( fromVector< A >( -1, 0 ) ) );
    ASSERT_EQ( ( A::eNorth ), ( fromVector< A >( 0, 1 ) ) );
}
TEST( DiscreteAngle, fromVector_8_float )
{
    typedef Angle< 8 >                A;

    ASSERT_EQ( ( A::eEast ), ( fromVector< A >( 1.0f, 0.0f ) ) );
    ASSERT_EQ( ( A::eSouth ), ( fromVector< A >( 0.0f, -1.0f ) ) );
    ASSERT_EQ( ( A::eWest ), ( fromVector< A >( -1.0f, 0.0f ) ) );
    ASSERT_EQ( ( A::eNorth ), ( fromVector< A >( 0.0f, 1.0f ) ) );
}
TEST( DiscreteAngle, fromVector_8_int )
{
    typedef Angle< 8 >            A;

    ASSERT_EQ( ( A::eEast ), ( fromVector< A >( 1, 0 ) ) );
    ASSERT_EQ( ( A::eSouth ), ( fromVector< A >( 0, -1 ) ) );
    ASSERT_EQ( ( A::eWest ), ( fromVector< A >( -1, 0 ) ) );
    ASSERT_EQ( ( A::eNorth ), ( fromVector< A >( 0, 1 ) ) );
}

TEST( DiscreteAngle, Difference )
{
    typedef Angle< 8 > A;

    ASSERT_EQ( difference< A >( A::eEast, A::eEast ), 0 );
    ASSERT_EQ( difference< A >( A::eEast, A::eNorthEast ), 1 );
    ASSERT_EQ( difference< A >( A::eNorthEast, A::eEast ), 1 );
    ASSERT_EQ( difference< A >( A::eNorthEast, A::eNorth ), 1 );
    ASSERT_EQ( difference< A >( A::eNorth, A::eNorthEast ), 1 );
    ASSERT_EQ( difference< A >( A::eEast, A::eSouthEast ), 1 );
    ASSERT_EQ( difference< A >( A::eSouthEast, A::eEast ), 1 );
}