
#include "common/astar.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <iostream>

using namespace Math;

TEST( AStar, Basic )
{
    astar::Value vStart{ 0, 0 };
    astar::Value vGoal{ 10, 10 };

    astar::Traits::PredecessorMap result;

    bool bResult = astar::search(
        vStart, vGoal, []( const astar::Value& value ) { return true; }, result );
    ASSERT_TRUE( bResult );

    astar::Value v = vGoal;
    while( v != vStart )
    {
        auto iFind = result.find( v );
        ASSERT_TRUE( iFind != result.end() );
        astar::Value expected{ v.x - 1, v.y - 1 };
        ASSERT_EQ( iFind->second, expected );
        v = iFind->second;
    }
}


