
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

struct BasicCostEstimate
{
    float cost;
    float estimate;

    inline bool operator<( const BasicCostEstimate& cmp ) const
    {
        return ( cost + estimate ) < ( cmp.cost + cmp.estimate );
    }
};

struct BasicPolicies
{
    static constexpr bool TerminateEarly = false;
    static constexpr int  MaxIterations  = 20000;
};

template < typename ValueType, typename Policies = BasicPolicies, typename CostEstimateType = BasicCostEstimate >
struct AStarTraits
{
    using Value              = ValueType;
    using CostEstimate       = CostEstimateType;
    using ValuePriorityQueue = std::multimap< CostEstimate, Value >;
    using ValueMap           = std::unordered_map< Value, typename ValuePriorityQueue::iterator, typename Value::Hash >;
    using CostMap            = std::unordered_map< Value, CostEstimate, typename Value::Hash >;
    using PredecessorMap     = std::unordered_map< Value, Value, typename Value::Hash >;

    static constexpr bool        TerminateEarly = Policies::TerminateEarly;
    static constexpr std::size_t MaxIterations  = Policies::MaxIterations;
};

enum ErrorCode
{
    eSuccess,
    eMaxIterations,
    eNoSolution
};

inline std::ostream& operator<<( std::ostream& os, astar::ErrorCode ec )
{
    switch( ec )
    {
        case eSuccess:
            return os << "AStar success";
        case eMaxIterations:
            return os << "AStar error: max iterations encountered";
        case eNoSolution:
            return os << "AStar error: could not find solution";
        default:
            return os << "AStar error: unknown error";
    }
}

namespace detail
{

template < typename Traits, typename GoalFunctor, typename AdjacencyFunctor, typename CostFunctor,
           typename EstimateFunctor >
inline ErrorCode astar_impl( typename Traits::Value start, typename Traits::PredecessorMap& result,
                             GoalFunctor&& goalFunctor, AdjacencyFunctor&& adjacencyFunctor, CostFunctor&& costFunctor,
                             EstimateFunctor&& estimateFunctor )
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

    int iterations = 0;
    while( !queue.empty() )
    {
        if( ++iterations == Traits::MaxIterations )
        {
            return eMaxIterations;
        }
        auto iFirst = queue.begin();

        const auto& bestValue = iFirst->second;

        if( goalFunctor( bestValue ) )
        {
            return eSuccess;
        }

        for( const auto& a : adjacencyFunctor( bestValue ) )
        {
            /*if constexpr( Traits::TerminateEarly )
            {
                if( a == goal )
                {
                    result[ a ] = bestValue;
                    return true;
                }
            }*/
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

    return eNoSolution;
}
} // namespace detail

struct BasicValue
{
    int x, y;

    inline bool operator<=( const BasicValue& value ) const
    {
        return ( x != value.x ) ? ( x < value.x ) : ( y < value.y );
    }
    inline bool operator==( const BasicValue& value ) const { return ( x == value.x ) && ( y == value.y ); }

    struct Hash
    {
        inline std::size_t operator()( const BasicValue& value ) const
        {
            return static_cast< std::size_t >( value.x + value.y );
        }
    };
};

inline std::ostream& operator<<( std::ostream& os, const BasicValue& value )
{
    return os << '(' << value.x << ',' << value.y << ')';
}

using BasicTraits = AStarTraits< BasicValue >;

template < typename Traits, typename ValuePredicate >
struct Adjacency
{
    using Angle = Math::Angle< 8 >;
    ValuePredicate m_predicate;

    Adjacency( ValuePredicate&& predicate )
        : m_predicate( predicate )
    {
    }

    inline const std::vector< typename Traits::Value >& operator()( const typename Traits::Value& value )
    {
        static std::vector< typename Traits::Value > values;
        values.clear();
        for( int i = 0; i != Angle::TOTAL_ANGLES; ++i )
        {
            int x = 0, y = 0;
            Math::toVectorDiscrete( static_cast< Angle::Value >( i ), x, y );
            typename Traits::Value adjacentValue{ value.x + x, value.y + y };
            if( m_predicate( adjacentValue ) )
            {
                values.emplace_back( adjacentValue );
            }
        }
        return values;
    }
};

// Simple astar search using basic traits and euclideon cost functions
template < typename ValuePredicate >
inline ErrorCode search( BasicTraits::Value start, BasicTraits::Value goal, ValuePredicate&& predicate,
                         BasicTraits::PredecessorMap& result )
{
    Adjacency< BasicTraits, ValuePredicate > adjacency( std::move( predicate ) );

    return detail::astar_impl< BasicTraits >(
        start, result, [ &goal ]( const BasicTraits::Value& best ) { return best == goal; }, adjacency,
        // Cost
        []( const BasicTraits::Value& value, const BasicTraits::Value& previous,
            const typename BasicTraits::CostEstimate& previousCostEstimate ) -> float
        {
            const int x = abs( value.x - previous.x );
            const int y = abs( value.y - previous.y );
            return previousCostEstimate.cost + std::sqrt( x * x + y * y );
        },
        // Estimate
        [ &goal ]( const BasicTraits::Value& value ) -> float
        {
            const int x = abs( goal.x - value.x );
            const int y = abs( goal.y - value.y );
            return std::sqrt( x * x + y * y );
        } );
}

// SImple astar search using basic traits with custom cost functions
template < typename ValuePredicate, typename CostFunctor, typename EstimateFunctor >
inline ErrorCode search( typename BasicTraits::Value start, typename BasicTraits::Value goal,
                         ValuePredicate&& predicate, CostFunctor&& costFunctor, EstimateFunctor&& estimateFunctor,
                         typename BasicTraits::PredecessorMap& result )
{
    Adjacency< BasicTraits, ValuePredicate > adjacency( std::move( predicate ) );
    return detail::astar_impl< BasicTraits >(
        start, result, [ &goal ]( const BasicTraits::Value& best ) { return best == goal; }, std::move( adjacency ),
        std::move( costFunctor ), std::move( estimateFunctor ) );
}

// Non-SImple astar search using custom traits with custom cost functions
template < typename Traits, typename GoalFunctor, typename AdjacencyFunctor, typename CostFunctor,
           typename EstimateFunctor >
inline ErrorCode search( typename Traits::Value start, GoalFunctor&& goalFunctor, AdjacencyFunctor&& adjacency,
                         CostFunctor&& costFunctor, EstimateFunctor&& estimateFunctor,
                         typename Traits::PredecessorMap& result )
{
    return detail::astar_impl< Traits >( start, result, std::move( goalFunctor ), std::move( adjacency ),
                                         std::move( costFunctor ), std::move( estimateFunctor ) );
}

} // namespace astar

#endif // GUARD_2023_June_24_astar
