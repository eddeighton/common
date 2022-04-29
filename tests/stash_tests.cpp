

#include "common/hash.hpp"

#include <gtest/gtest.h>

TEST( Stash, Basic )
{
    {
        common::Hash h1( 123u );
        common::Hash h2( 123u );
        ASSERT_EQ( h1, h2 );
    }

    {
        common::Hash h1( 123u );
        common::Hash h2( 124u );
        ASSERT_NE( h1, h2 );
    }
}

TEST( Stash, Basic2 )
{
    {
        common::Hash h1( std::string( "test" ) );
        common::Hash h2( std::string( "test" ) );
        ASSERT_EQ( h1, h2 );
    }
    {
        common::Hash h1( std::string( "test" ) );
        common::Hash h2( std::string( "tesx" ) );
        ASSERT_NE( h1, h2 );
    }
}

TEST( Stash, Basic3 )
{
    {
        common::Hash h1( "test" );
        common::Hash h2( "test" );
        ASSERT_EQ( h1, h2 );
    }
    {
        common::Hash h1( "test" );
        common::Hash h2( "tesx" );
        ASSERT_NE( h1, h2 );
    }
}


TEST( Stash, Basic4 )
{
    {
        common::Hash h1( 1u, 0.123f, true, std::string( "test" ), "test", std::vector< int >{ 1, 2, 3, 4, 5 } );
        common::Hash h2( 1u, 0.123f, true, std::string( "test" ), "test", std::vector< int >{ 1, 2, 3, 4, 5 } );
        ASSERT_EQ( h1, h2 );
    }
    {
        common::Hash h1( 1u, 0.123f, true, std::string( "test" ), "test", std::vector< int >{ 1, 2, 3, 4, 5 } );
        common::Hash h2( 1u, 0.123f, true, std::string( "test" ), "test", std::vector< int >{ 1, 1, 3, 4, 5 } );
        ASSERT_NE( h1, h2 );
    }
    {
        common::Hash h1( 1u, 0.123f, true,  std::string( "test" ), "test", std::vector< int >{ 1, 2, 3, 4, 5 } );
        common::Hash h2( 1u, 0.123f, false, std::string( "test" ), "test", std::vector< int >{ 1, 2, 3, 4, 5 } );
        ASSERT_NE( h1, h2 );
    }
}

TEST( Stash, Basic5 )
{
    {
        common::Hash h1( { 1, 2, 3 } );
        common::Hash h2( { 1, 2, 3 } );
        ASSERT_EQ( h1, h2 );
    }
    {
        common::Hash h1( { 1, 2, 3 } );
        common::Hash h2( { 1, 4, 3 } );
        ASSERT_NE( h1, h2 );
    }
}

TEST( Stash, Basic6 )
{
    common::Hash h1( { "this", "is", "a", "test" } );

    h1 ^= 123;
    h1 ^= "Testing";
    h1 ^= 123.123;
    h1 ^= 123.123f;
    h1 ^= std::vector{ 1,2,3,4,5 };
    h1 ^= std::vector{ 'a','n','r' };
    h1 ^= h1;
}