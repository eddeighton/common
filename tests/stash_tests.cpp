

#include "common/hash.hpp"
#include "common/stash.hpp"

#include <gtest/gtest.h>

static const std::size_t szTestValue1 = 123;
static const std::size_t szTestValue2 = 124;

TEST( Stash, Basic )
{
    {
        const std::size_t szHash = szTestValue1;
        common::Hash h1( szHash );
        ASSERT_EQ( h1.get(), szHash );
    }
    {
        const std::size_t szHash = szTestValue1;
        common::Hash h1{ szTestValue1 };
        ASSERT_EQ( h1.get(), szHash );
    }
    {
        const std::size_t szHash = szTestValue1;
        common::Hash h1{ szTestValue1, szTestValue1 };
        ASSERT_NE( h1.get(), szHash );
    }
    {
        common::Hash h1( szTestValue1 );
        common::Hash h2( szTestValue1 );
        ASSERT_EQ( h1, h2 );
    }

    {
        common::Hash h1( szTestValue1 );
        common::Hash h2( szTestValue2 );
        ASSERT_NE( h1, h2 );
    }
}

TEST( Stash, Determinants )
{
    {
        const std::size_t szHash = szTestValue1;
        task::DeterminantHash h1( szTestValue1 );
        ASSERT_EQ( h1.get(), szHash );
    }
    {
        const std::size_t szHash = szTestValue1;
        task::DeterminantHash h1{ szTestValue1 };
        ASSERT_EQ( h1.get(), szHash );
    }
    {
        const std::size_t szHash = szTestValue1;
        task::DeterminantHash h1{ szTestValue1, szTestValue1 };
        ASSERT_NE( h1.get(), szHash );
    }
    {
        task::DeterminantHash h1( szTestValue1 );
        task::DeterminantHash h2( szTestValue1 );
        ASSERT_EQ( h1, h2 );
    }

    {
        task::DeterminantHash h1( szTestValue1 );
        task::DeterminantHash h2( szTestValue2 );
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

    h1 ^= szTestValue1;
    h1 ^= "Testing";
    h1 ^= szTestValue2;
    h1 ^= 0.123f;
    h1 ^= std::vector{ 1,2,3,4,5 };
    h1 ^= std::vector{ 'a','n','r' };
}