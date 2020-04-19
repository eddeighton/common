
#include "common/inplace_allocator.h"

#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <algorithm>
#include <random>

TEST( InplaceAllocator, InplaceAllocator_1 )
{
    ASSERT_TRUE( true );
    std::vector< size_t > buffer( 1024 );

    InplaceAllocator ia = inplace_allocator_initialise( buffer.data(), buffer.size() );

    std::set< size_t* > pointers;
    for( size_t i = 0U; i < buffer.size(); ++i )
    {
        size_t* p = inplace_allocator_new( &ia );
        ASSERT_EQ( true, inplace_allocator_test( &ia, p ) );
        pointers.insert( p );
    }
    ASSERT_EQ( buffer.size(), pointers.size() );

}

TEST( InplaceAllocator, InplaceAllocator_2 )
{
    ASSERT_TRUE( true );
    std::vector< size_t > buffer( 1024 );

    InplaceAllocator ia = inplace_allocator_initialise( buffer.data(), buffer.size() );

    std::set< size_t* > pointers;
    for( size_t i = 0U; i < buffer.size(); ++i )
        pointers.insert( inplace_allocator_new( &ia ) );

    size_t* pLimit = inplace_allocator_new( &ia );
    ASSERT_EQ( false, inplace_allocator_test( &ia, pLimit ) );

}


TEST( InplaceAllocator, InplaceAllocator_3 )
{
	std::random_device rd;
    std::mt19937 g(rd());
	
    ASSERT_TRUE( true );
    std::vector< size_t > buffer( 1024 );

    InplaceAllocator ia = inplace_allocator_initialise( buffer.data(), buffer.size() );

    std::set< size_t* > pointers;
    for( size_t i = 0; i < buffer.size(); ++i )
    {
        size_t* p = inplace_allocator_new( &ia );
        *p = 123456789U;
        pointers.insert( p );
    }

    std::vector< size_t* > ptrs_randomized( pointers.begin(), pointers.end() );
    std::shuffle( ptrs_randomized.begin(), ptrs_randomized.end(), g );

    size_t uiRemaining = 0;
    for( std::vector< size_t* >::iterator 
         i = ptrs_randomized.begin(),
         iEnd = ptrs_randomized.end(); i!=iEnd; ++i )
    {
        ASSERT_EQ( true, inplace_allocator_test( &ia, *i ) );
        inplace_allocator_free( &ia, *i );
        ASSERT_EQ( ++uiRemaining, inplace_allocator_remaining( &ia ) );
    }
    ASSERT_EQ( buffer.size(), inplace_allocator_remaining( &ia ) );
}

TEST( InplaceAllocator, InplaceAllocator_4 )
{
	std::random_device rd;
    std::mt19937 g(rd());
	
    ASSERT_TRUE( true );
    std::vector< size_t > buffer( 1024 );

    InplaceAllocator ia = inplace_allocator_initialise( buffer.data(), buffer.size() );

    std::set< size_t* > pointers;
    for( size_t i = 0; i < buffer.size(); ++i )
    {
        size_t* p = inplace_allocator_new( &ia );
        *p = 123456789U;
        pointers.insert( p );
    }

    std::vector< size_t* > ptrs_randomized( pointers.begin(), pointers.end() );
    std::shuffle( ptrs_randomized.begin(), ptrs_randomized.end(), g );
    std::list< size_t* > ptrs_list( ptrs_randomized.begin(), ptrs_randomized.end() );

    size_t uiRemaining = 0;
    ASSERT_EQ( uiRemaining, inplace_allocator_remaining( &ia ) );
    while( !ptrs_list.empty() )
    {
        //grab off the front
        inplace_allocator_free( &ia, ptrs_list.front() );
        ptrs_list.pop_front();
        ++uiRemaining;
        ASSERT_EQ( uiRemaining, inplace_allocator_remaining( &ia ) );

        if( rand() % 2 == 0 )
        {
            //add one back!
            size_t* p = inplace_allocator_new( &ia );
            ASSERT_EQ( true, inplace_allocator_test( &ia, p ) );
            ptrs_list.push_back( p );
            --uiRemaining;
            ASSERT_EQ( uiRemaining, inplace_allocator_remaining( &ia ) );
        }
    }
    ASSERT_EQ( buffer.size(), inplace_allocator_remaining( &ia ) );
}
