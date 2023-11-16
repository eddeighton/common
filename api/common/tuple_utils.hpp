

#ifndef TUPLE_UTILS_14_NOV_2015
#define TUPLE_UTILS_14_NOV_2015

#include <tuple>
#include <iostream>
#include <iomanip>

#include "assert_verify.hpp"

namespace common
{

template< unsigned int N >
struct SerializeOut
{
    template< typename... Args>
    static inline void serialize( std::ostream& os, const std::tuple<Args...> & t)
    {
        os << std::noskipws << std::get< N-1 >(t) << ',';
        SerializeOut< N-1 >::serialize( os, t );
    }
};

template<>
struct SerializeOut< 1 >
{
    template<typename... Args>
    static inline void serialize( std::ostream& os, const std::tuple<Args...> & t)
    {
        os << std::noskipws << std::get< 0 >( t );
    }
};

inline void parse_stack( std::istream& is, std::ostream& os, char charEnd )
{
    int iStack = ( charEnd == ')' ) ? 1 : 0;
    char c;
    //read up to the terminating character.  
    //Consume the terminating character and present the interior string
    //as a stream to deserialise the contained element
    while( is )
    {
        is >> std::noskipws >> c;
        if( '(' == c )
        {
            ++iStack;
        }
        else if( ')' == c )
        {
            --iStack;
        }
        if( ( 0 == iStack ) && ( charEnd == c ) )
        {
            break;
        }
        else
        {
            os << std::noskipws << c;
        }
    }
}

template< class T >
inline void read_element( std::istream& is, T& value, char charEnd )
{
    std::stringstream ss;
    parse_stack( is, ss, charEnd );
    ss >> std::noskipws >> value;
}

template<>
inline void read_element< std::string >( std::istream& is, std::string& value, char charEnd )
{
    std::stringstream ss;
    parse_stack( is, ss, charEnd );
    value = ss.str();
}


template< unsigned int N >
struct SerializeIn
{
    template< typename... Args>
    static inline void serialize( std::istream& is, std::tuple<Args...> & t)
    {
        read_element( is, std::get< N-1 >( t ), ',' );
        SerializeIn< N-1 >::serialize( is, t );
    }
};

template<>
struct SerializeIn< 1 >
{
    template<typename... Args>
    static inline void serialize( std::istream& is, std::tuple<Args...> & t)
    {
        read_element( is, std::get< 0 >( t ), ')' );
    }
};

template<typename... Args>
inline std::ostream& operator<<( std::ostream& os, const std::tuple<Args...>& t )
{
    os << '(';
    common::SerializeOut< sizeof...(Args) >::serialize( os, t );
    os << ')';
    return os;
}

template<typename... Args>
inline std::istream& operator>>( std::istream& is, std::tuple<Args...>& t )
{
    char c;
    is >> std::skipws >> c;
    VERIFY_RTE( c == '(' );
    common::SerializeIn< sizeof...(Args) >::serialize( is, t );
    return is;
}


}

#endif //TUPLE_UTILS_14_NOV_2015
