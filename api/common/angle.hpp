/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef DISCRETE_ANGLE_15_09_2013
#define DISCRETE_ANGLE_15_09_2013

#define _USE_MATH_DEFINES
#include <cmath>

#include "rounding.hpp"
#include "assert_verify.hpp"

namespace Math
{
   
template< char TOTAL_ANGLES >
struct Angle
{
};

template<>
struct Angle< 4 >
{
    static const char TOTAL_ANGLES = 4;
    enum Value : char
    {
        eEast,
        eNorth,
        eWest,
        eSouth
    };
};

template<>
struct Angle< 8 >
{
    static const char TOTAL_ANGLES = 8;
    enum Value : char
    {
        eEast,
        eNorthEast,
        eNorth,
        eNorthWest,
        eWest,
        eSouthWest,
        eSouth,
        eSouthEast
    };
};

template<>
struct Angle< 16 >
{
    static const char TOTAL_ANGLES = 16;
    enum Value : char
    {
        eEast,
        eEastNorthEast,
        eNorthEast,
        eNorthNorthEast,
        eNorth,
        eNorthNorthWest,
        eNorthWest,
        eWestNorthWest,
        eWest,
        eWestSouthWest,
        eSouthWest,
        eSouthSouthWest,
        eSouth,
        eSouthSouthEast,
        eSouthEast,
        eEastSouthEast
    };
};

template< class TAngleTraits >
inline typename TAngleTraits::Value rotate( typename TAngleTraits::Value v, char amt )
{
    return static_cast< typename TAngleTraits::Value >( 
        mapToRange< char >( v - amt, TAngleTraits::TOTAL_ANGLES ) % TAngleTraits::TOTAL_ANGLES );
}

template< class TAngleTraits >
inline typename TAngleTraits::Value opposite( typename TAngleTraits::Value v )
{
    return rotate< TAngleTraits >( v, TAngleTraits::TOTAL_ANGLES / 2 );
}

template< class TAngleTraits, class TValueType >
inline void toVector( typename TAngleTraits::Value v, TValueType& x, TValueType& y )
{
    const double angle = TValueType( v ) * TValueType( M_PI ) * TValueType( 2 ) / 
        static_cast< TValueType >( TAngleTraits::TOTAL_ANGLES );
    x = cos( angle );
    y = sin( angle );
}

template< class TAngleTraits, class TValueType >
inline void toVectorDiscrete( typename TAngleTraits::Value v, TValueType& x, TValueType& y )
{
    const double angle = v * M_PI * 2.0 / TAngleTraits::TOTAL_ANGLES;
    x = TValueType( roundRealOutToInt( cos( angle ) ) );
    y = TValueType( roundRealOutToInt( sin( angle ) ) );
}

template< class TValueType >
inline void toVectorDiscrete( typename Angle< 8 >::Value v, TValueType& x, TValueType& y )
{
    switch( v )
    {
        default:
            ASSERT( false );
        case Angle< 8 >::eEast         :    x = TValueType( 1 ) ;    y = TValueType( 0 ) ; break;
        case Angle< 8 >::eNorthEast    :    x = TValueType( 1 ) ;    y = TValueType( -1 ); break;
        case Angle< 8 >::eNorth        :    x = TValueType( 0 ) ;    y = TValueType( -1 ); break;
        case Angle< 8 >::eNorthWest    :    x = TValueType( -1 );    y = TValueType( -1 ); break;
        case Angle< 8 >::eWest         :    x = TValueType( -1 );    y = TValueType( 0 ) ; break;
        case Angle< 8 >::eSouthWest    :    x = TValueType( -1 );    y = TValueType( 1 ) ; break;
        case Angle< 8 >::eSouth        :    x = TValueType( 0 ) ;    y = TValueType( 1 ) ; break;
        case Angle< 8 >::eSouthEast    :    x = TValueType( 1 ) ;    y = TValueType( 1 ) ; break;
    }
}
template< class TValueType >
inline void toVectorDiscrete( typename Angle< 4 >::Value v, TValueType& x, TValueType& y )
{
    switch( v )
    {
        default:
            ASSERT( false );
        case Angle< 4 >::eEast         :    x = TValueType( 1 ) ;  y =  TValueType( 0 ) ;  break;
        case Angle< 4 >::eNorth        :    x = TValueType( 0 ) ;  y =  TValueType( -1 );  break;
        case Angle< 4 >::eWest         :    x = TValueType( -1 );  y =  TValueType( 0 ) ;  break;
        case Angle< 4 >::eSouth        :    x = TValueType( 0 ) ;  y =  TValueType( 1 ) ;  break;
    }
}

template< class TAngleTraits, class TValueType >
inline typename TAngleTraits::Value fromVector( TValueType x, TValueType y )
{
    const double angle = atan2( y, x );///Y FIRST!!!! THEN X WTF!!!
    const double d = ( TAngleTraits::TOTAL_ANGLES * angle ) / ( TValueType( M_PI ) * TValueType( 2 ) );
    const unsigned int ui = roundPositiveRealToUInt( mapToRange< double >( d, TAngleTraits::TOTAL_ANGLES ) );
    return static_cast< typename TAngleTraits::Value >( ui % TAngleTraits::TOTAL_ANGLES );
}

}

#endif //DISCRETE_ANGLE_15_09_2013
