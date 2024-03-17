
#include "common/astar.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <iostream>

using namespace Math;

TEST( AStar, Basic )
{
    astar::BasicValue vStart{ 0, 0 };
    astar::BasicValue vGoal{ 10, 10 };

    astar::BasicTraits::PredecessorMap result;

    bool bResult = astar::search(
        vStart, vGoal, []( const astar::BasicValue& ) { return true; }, result );
    ASSERT_EQ( bResult, astar::eSuccess );

    astar::BasicValue v = vGoal;
    while( v != vStart )
    {
        auto iFind = result.find( v );
        ASSERT_TRUE( iFind != result.end() );
        astar::BasicValue expected{ v.x - 1, v.y - 1 };
        ASSERT_EQ( iFind->second, expected );
        v = iFind->second;
    }
}
