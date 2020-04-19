/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef ROUNDING_05_2015
#define ROUNDING_05_2015

#include <cmath>
#include <cfenv>
#include <limits>
#include <algorithm>

#include "assert_verify.hpp"

namespace Math
{

template< class T >
inline T clamp( T min, T max, T value )
{
    VERIFY_RTE_MSG( min < max, "clamp: min must be less than max" );
    return std::max( std::min( max, value ), min );
}

template<class T>   inline T        mod             (const T& num,      const T& base       )   {   return num % base;}
template<>          inline float    mod<float>      (const float& num,  const float& base   )   {   return std::fmod( num, base ); }
template<>          inline double   mod<double>     (const double& num, const double& base  )   {   return std::fmod( num, base ); }

template< class T >
inline T quantize( T f, T fStep )
{
    if( f < T() )
    {
        const T fMod = mod( -f, fStep );
        if( fMod == T() )
            return f;
        else
            return f - ( fStep - fMod );
    }
    else
    {
        return f - mod( f, fStep );
    }
}
template<>
inline unsigned int quantize< unsigned int >( unsigned int f, unsigned int fStep )
{
    return f - mod( f, fStep );
}

template< class T >
inline T quantize_roundUp( T f, T fStep )
{
    return quantize( f + fStep / T(2), fStep );
}

constexpr size_t quantize_roundUpInt_PowerOfTwo( size_t numToRound, size_t multiple ) 
{
    //ASSERT( multiple && ((multiple & (multiple -1)) == 0) );
    return (numToRound + multiple - 1) & ~(multiple - 1);
}

inline size_t quantize_nearestPowerOfTwo( size_t v )
{
    size_t nearest = 1U;
    while( v > nearest )
        nearest <<= 1U;
    return nearest;
}
/*
inline size_t quantize_nearestLowerPowerOfTwo( size_t v ) 
{
    size_t power = 1U;
    while ( v >>= 1U ) power <<= 1U;
    return power;
}

inline size_t quantize_nearestGreaterPowerOfTwo( size_t v ) 
{
    size_t power = 2U;
    while ( v >>= 1U ) power <<= 1U;
    return power;
}
*/

template< class T >
inline int roundRealOutToInt( T real )
{
    if( ( real > T() && std::numeric_limits< T >::epsilon() > real ) ||
        ( real < T() && std::numeric_limits< T >::epsilon() > -real ) )
        return int();
    else if( real < T() )
        return static_cast< int >( std::floor( real ) );
    else
        return static_cast< int >( std::ceil( real ) );
    //std::fesetround(FE_TONEAREST);
    //return static_cast< int >( std::rint( real ) );
}

template< class T >
inline T mapToRange( T value, T range )
{
    VERIFY_RTE_MSG( range > T(), "mapToRange: range must be greater than zero" );
    
    if( value < T() )
    {
        while( value < T() )
            value += range;
        
        
    }
    else if( value >= range )
    {
        value = mod( value, range );
    }
    
    return value;
}

//This is a comment made using VI within git bash
template<>
inline unsigned int mapToRange( unsigned int value, unsigned int range )
{
    VERIFY_RTE_MSG( range > 0u, "mapToRange: range must be greater than zero" );
    return value % range;
}

template< class T >
inline unsigned int roundPositiveRealToUInt( T real )
{
    VERIFY_RTE_MSG( real >= T(), "argument less than zero" );
    //std::fesetround(FE_TONEAREST);
    //return static_cast< unsigned int >( std::rint( real ) );
    return static_cast< unsigned int >( std::floor( T( 0.5 ) + real ) );
}

}

#endif //EDS_MATH_14_09_2013
