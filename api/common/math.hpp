/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef MATHGENERICS_03_09_2013
#define MATHGENERICS_03_09_2013

#include <cmath>

namespace Math
{

inline double pow10( int iPower )
{
    switch( iPower )
    {
        case 20:
            return 100000000000000000000.0;
        case 19:
            return 10000000000000000000.0;
        case 18:
            return 1000000000000000000.0;
        case 17:
            return 100000000000000000.0;
        case 16:
            return 10000000000000000.0;
        case 15:
            return 1000000000000000.0;
        case 14:
            return 100000000000000.0;
        case 13:
            return 10000000000000.0;
        case 12:
            return 1000000000000.0;
        case 11:
            return 100000000000.0;
        case 10:
            return 10000000000.0;
        case 9:
            return 1000000000.0;
        case 8:
            return 100000000.0;
        case 7:
            return 10000000.0;
        case 6:
            return 1000000.0;
        case 5:
            return 100000.0;
        case 4:
            return 10000.0;
        case 3:
            return 1000.0;
        case 2:
            return 100.0;
        case 1:
            return 10.0;
        case 0:
            return 1.0;
        case -1:
            return 0.1;
        case -2:
            return 0.01;
        case -3:
            return 0.001;
        case -4:
            return 0.0001;
        case -5:
            return 0.00001;
        case -6:
            return 0.000001;
        case -7:
            return 0.0000001;
        case -8:
            return 0.00000001;
        case -9:
            return 0.000000001;
        default:
            return std::pow( 10.0, iPower );
    }
}

template < class T1, class T2 >
unsigned int getXFrom2dIndex( T1 sizeX, T2 index )
{
    return static_cast< unsigned int >( index ) % static_cast< unsigned int >( sizeX );
}

template < class T1, class T2 >
unsigned int getYFrom2dIndex( T1 sizeX, T2 index )
{
    return static_cast< unsigned int >( index ) / static_cast< unsigned int >( sizeX );
}

template < class T1, class T2, class T3 >
unsigned int getXFrom3dIndex( T1 sizeX, T2, T3 index )
{
    return static_cast< unsigned int >( index ) % static_cast< unsigned int >( sizeX );
}

template < class T1, class T2, class T3 >
unsigned int getYFrom3dIndex( T1 sizeX, T2 sizeY, T3 index )
{
    return ( static_cast< unsigned int >( index ) / static_cast< unsigned int >( sizeX ) )
           % static_cast< unsigned int >( sizeY );
}

template < class T1, class T2, class T3 >
unsigned int getZFrom3dIndex( T1 sizeX, T2 sizeY, T3 index )
{
    return static_cast< unsigned int >( index ) / static_cast< unsigned int >( sizeY * sizeX );
}

} // namespace Math

#endif // MATHGENERICS_03_09_2013