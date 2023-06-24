
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2023_June_24_astar
#define GUARD_2023_June_24_astar

#include "common/angle.hpp"

#include <vector>
#include <tuple>
#include <map>
#include <array>
#include <unordered_set>
#include <unordered_map>

#include <ostream>

namespace astar
{

namespace detail
{

template < typename Traits, typename AdjacencyFunctor, typename CostFunctor, typename EstimateFunctor >
inline bool astar_impl( typename Traits::Value start, typename Traits::Value goal,
                        typename Traits::PredecessorMap& result, AdjacencyFunctor&& adjacencyFunctor,
                        CostFunctor&& costFunctor, EstimateFunctor&& estimateFunctor )
{
    using CostEstimate = typename Traits::CostEstimate;

    typename Traits::ValueMap           open;
    typename Traits::CostMap            closed;
    typename Traits::ValuePriorityQueue queue;

    {
        const CostEstimate estimate{ 0.0f, estimateFunctor( start ) };
        auto               iter = queue.insert( { estimate, start } );
        open.insert( { start, iter } );
    }

    while( !queue.empty() )
    {
        auto iFirst = queue.begin();

        const auto& bestValue = iFirst->second;

        if( bestValue == goal )
        {
            return true;
        }

        for( auto a : adjacencyFunctor( bestValue ) )
        {
            const CostEstimate estimate{ costFunctor( a, bestValue, iFirst->first ), estimateFunctor( a ) };

            auto iFindOpen = open.find( a );
            if( iFindOpen != open.end() )
            {
                // already open
                const CostEstimate& existingCost = iFindOpen->second->first;
                if( estimate < existingCost )
                {
                    queue.erase( iFindOpen->second );
                    auto iter         = queue.insert( { estimate, a } );
                    iFindOpen->second = iter;
                    result[ a ]       = bestValue;
                }
            }
            else
            {
                auto iFindClosed = closed.find( a );
                if( iFindClosed != closed.end() )
                {
                    // already open
                    const CostEstimate& existingCost = iFindClosed->second;
                    if( estimate < existingCost )
                    {
                        auto iter = queue.insert( { estimate, a } );
                        open[ a ] = iter;
                        closed.erase( iFindClosed );
                        result[ a ] = bestValue;
                    }
                }
                else
                {
                    auto iter = queue.insert( { estimate, a } );
                    open.insert( { a, iter } );
                    result[ a ] = bestValue;
                }
            }
        }

        closed[ bestValue ] = iFirst->first;
        open.erase( bestValue );
        queue.erase( iFirst );
    }

    return false;
}
} // namespace detail

template < typename ValueType >
struct AStarTraits
{
    using Value = ValueType;

    struct CostEstimate
    {
        float cost;
        float estimate;

        inline bool operator<( const CostEstimate& cmp ) const
        {
            return ( cost + estimate ) < ( cmp.cost + cmp.estimate );
        }
    };

    using ValueVector        = std::vector< Value >;
    using ValuePriorityQueue = std::multimap< CostEstimate, Value >;
    using ValueMap           = std::unordered_map< Value, typename ValuePriorityQueue::iterator, typename Value::Hash >;
    using CostMap            = std::unordered_map< Value, CostEstimate, typename Value::Hash >;
    using PredecessorMap     = std::unordered_map< Value, Value, typename Value::Hash >;
};

struct Value
{
    int x, y;

    inline bool operator<=( const Value& value ) const { return ( x != value.x ) ? ( x < value.x ) : ( y < value.y ); }
    inline bool operator==( const Value& value ) const { return ( x == value.x ) && ( y == value.y ); }

    struct Hash
    {
        inline std::size_t operator()( const Value& value ) const { return value.x + value.y; }
    };
};

inline std::ostream& operator<<( std::ostream& os, const Value& value )
{
    return os << '(' << value.x << ',' << value.y << ')';
}

using Traits = AStarTraits< Value >;

template < typename ValuePredicate >
struct Adjacency
{
    using Angle = Math::Angle< 8 >;
    ValuePredicate m_predicate;

    Adjacency( ValuePredicate&& predicate )
        : m_predicate( predicate )
    {
    }

    // Value
    inline const std::vector< Value >& operator()( const Value& value )
    {
        static std::vector< Value > values;
        values.clear();
        for( int i = 0; i != Angle::TOTAL_ANGLES; ++i )
        {
            int x = 0, y = 0;
            Math::toVectorDiscrete( static_cast< Angle::Value >( i ), x, y );
            Value adjacentValue{ value.x + x, value.y + y };
            if( m_predicate( adjacentValue ) )
            {
                values.emplace_back( adjacentValue );
            }
        }
        return values;
    }
};

template < typename ValuePredicate >
inline bool search( Value start, Value goal, ValuePredicate&& predicate, Traits::PredecessorMap& result )
{
    Adjacency< ValuePredicate > adjacency( std::move( predicate ) );

    return detail::astar_impl< Traits >(
        start, goal, result, adjacency,
        // Cost
        []( const Value& value, const Value& previous, const Traits::CostEstimate& previousCostEstimate ) -> float
        {
            const int x = abs( value.x - previous.x );
            const int y = abs( value.y - previous.y );
            return previousCostEstimate.cost + std::sqrt( x * x + y * y );
        },
        // Estimate
        [ &goal ]( const Value& value ) -> float
        {
            const int x = abs( goal.x - value.x );
            const int y = abs( goal.y - value.y );
            return std::sqrt( x * x + y * y );
        } );
}

} // namespace astar

#endif // GUARD_2023_June_24_astar
