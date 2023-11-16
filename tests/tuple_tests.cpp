
#include "common/tuple_utils.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <limits>
#include <cstdint>

using namespace common;

#define TupleComparison( TYPE, ...)                \
do                                                 \
{                                                  \
    TYPE value( __VA_ARGS__ );                     \
    std::stringstream ss;                          \
    ss << value;                                   \
    TYPE result;                                   \
    ss >> result;                                  \
    ASSERT_EQ( value, result );                    \
}while(false);

TEST( Tuple, IO_1 )
{
    typedef std::tuple< std::int32_t, std::uint32_t, std::int8_t, std::uint8_t > T1;

    TupleComparison( T1, 0,0,0,0 );

    TupleComparison
    (
        T1,
        std::numeric_limits< std::int32_t >::min(),
        std::numeric_limits< std::uint32_t >::min(),
        std::numeric_limits< std::int8_t >::min(),
        std::numeric_limits< std::uint8_t >::min()
    );

    TupleComparison
    (
        T1,
        std::numeric_limits< std::int32_t >::max() ,
        std::numeric_limits< std::uint32_t >::max(),
        std::numeric_limits< std::int8_t >::max()  ,
        std::numeric_limits< std::uint8_t >::max()
    );
}

TEST( Tuple, IO_2 )
{
    typedef std::tuple< int, bool, std::string, char > T1;

    TupleComparison( T1, 0, false, "", 'a' );
    TupleComparison( T1, std::numeric_limits<int>::max(), true, "test test", 'a' );
    TupleComparison( T1, 1234123, false, "", 'a' );
    TupleComparison( T1, -123123, true, "", 'a' );
    TupleComparison( T1, std::numeric_limits<int>::min(), false, "test", 'a' );
}

TEST( Tuple, IO_3 )
{
    typedef std::tuple< std::string > T1;
    TupleComparison( T1, "A B C D " );
    TupleComparison( T1, "THIS IS A STRING" )
    TupleComparison( T1, "1234567890 !\"£$%^&*_+-=" );
    TupleComparison( T1, "<>.:@;'#~[]{} The quick brown fox jumped over the lazy dog" );
}
/*
TEST( Tuple, IO_4 )
{
    typedef std::tuple< bool, int, char > T2;
    typedef std::tuple< double, unsigned int, char > T3;
    typedef std::tuple< bool, int, float > T4;
    typedef std::tuple< T2, T3, std::string, T4 > T1;
    TupleComparison( T1, T2( false, 1, 'x' ), T3( 123.123, 123U, 'R' ), "test test", T4( true, -123, 0.123123f ) );

}*/
/*
TEST( Tuple, IO_5 )
{
    typedef std::tuple< int, std::string, float > T1;
    typedef std::tuple< T1, T1, double > T2;
    typedef std::tuple< T1, T2, std::string > T3;

    TupleComparison( T3, T1( 0, "a b c", 0.123f ), T2( T1( 1, "d e f", 123.123f ), T1( 321, "", 0.0f ), 0.0 ), "nice test" );
}
*/
