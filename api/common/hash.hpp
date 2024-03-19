
#ifndef COMMON_HASH_UTILS_28_OCT_2020
#define COMMON_HASH_UTILS_28_OCT_2020

#include "assert_verify.hpp"

#include "boost/filesystem/path.hpp"

#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional>
#include <type_traits>
#include <iomanip>

namespace common
{
using HashCodeType = std::size_t;

namespace internal
{
struct HashCode
{
    HashCodeType m_data = HashCodeType();
};
struct HashCombiner
{
    inline HashCodeType operator()( HashCodeType left, HashCodeType right ) const
    {
        return left ^ ( right + 0x9e3779b9 + ( left << 6 ) + ( left >> 2 ) );
    }
};

template < typename T, class Enable = void >
struct HashFunctor
{
    inline HashCodeType operator()( const T& value ) const { return std::hash< T >{}( value ); }
};

template < typename T >
struct HashFunctor< T, typename std::enable_if< std::is_base_of< HashCode, T >::value >::type >
{
    inline HashCodeType operator()( const T& value ) const { return value.m_data; }
};

template < typename T >
struct HashFunctor< T, typename std::enable_if< !std::is_base_of< HashCode, T >::value
                                                && std::is_default_constructible< typename T::Hash >::value >::type >
{
    inline HashCodeType operator()( const T& value ) const { return typename T::Hash{}( value ); }
};

HashCodeType hash_file( const boost::filesystem::path& file );

// NOTE: this specialisation was ONLY introduced due to vc++ issue - further investigation justified to
// understand why requires this on windows.
template <>
struct HashFunctor< HashCodeType >
{
    inline HashCodeType operator()( HashCodeType value ) const { return value; }
};

template <>
struct HashFunctor< boost::filesystem::path >
{
    inline HashCodeType operator()( const boost::filesystem::path& value ) const { return hash_file( value ); }
};
template <>
struct HashFunctor< boost::filesystem::path& >
{
    inline HashCodeType operator()( const boost::filesystem::path& value ) const { return hash_file( value ); }
};
template <>
struct HashFunctor< const boost::filesystem::path >
{
    inline HashCodeType operator()( const boost::filesystem::path& value ) const { return hash_file( value ); }
};
template <>
struct HashFunctor< const boost::filesystem::path& >
{
    inline HashCodeType operator()( const boost::filesystem::path& value ) const { return hash_file( value ); }
};

template < std::size_t Length >
struct HashFunctor< char[ Length ] >
{
    inline HashCodeType operator()( const char str[ Length ] )
    {
        HashCodeType hashCode = 0x79e37b99;
        for( std::size_t sz = 0U; sz != Length; ++sz )
        {
            hashCode = HashCombiner()( hashCode, std::hash< char >{}( str[ sz ] ) );
        }
        return hashCode;
    }
};

template < typename T, std::size_t Length >
struct HashFunctor< std::array< T, Length > >
{
    inline HashCodeType operator()( const std::array< T, Length >& list ) const
    {
        HashCodeType hashCode = 0x79e47b99;
        for( const auto& value : list )
        {
            hashCode = HashCombiner()( hashCode, HashFunctor< T >()( value ) );
        }
        return hashCode;
    }
};

template < typename T >
struct HashFunctor< std::vector< T, std::allocator< T > > >
{
    inline HashCodeType operator()( const std::vector< T, std::allocator< T > >& list ) const
    {
        HashCodeType hashCode = 0x79e57b99;
        for( const auto& value : list )
        {
            hashCode = HashCombiner()( hashCode, HashFunctor< T >()( value ) );
        }
        return hashCode;
    }
};

template < typename T >
struct HashFunctor< std::initializer_list< T > >
{
    inline HashCodeType operator()( const std::initializer_list< T > list ) const
    {
        HashCodeType hashCode = 0x79e37b99;
        for( const auto& value : list )
        {
            hashCode = HashCombiner()( hashCode, HashFunctor< T >()( value ) );
        }
        return hashCode;
    }
};

struct HashFunctorVariadic
{
    template < typename Head >
    inline HashCodeType operator()( const Head& head ) const
    {
        return HashFunctor< Head >()( head );
    }

    template < typename Head, typename... Tail >
    inline HashCodeType operator()( const Head& head, const Tail&... tail ) const
    {
        return HashCombiner()( HashFunctor< Head >()( head ), HashFunctorVariadic()( tail... ) );
    }
};
} // namespace internal

class Hash : public internal::HashCode
{
public:
    Hash() {}

    template < typename... Args >
    Hash( const Args&... args )
        : internal::HashCode( { internal::HashFunctorVariadic()( args... ) } )
    {
    }

    HashCodeType get() const { return m_data; }
    void         set( HashCodeType hash ) { m_data = hash; }

    inline bool operator==( const Hash& cmp ) const { return m_data == cmp.m_data; }
    inline bool operator!=( const Hash& cmp ) const { return m_data != cmp.m_data; }
    inline bool operator<( const Hash& cmp ) const { return m_data < cmp.m_data; }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_data;
    }

    inline void operator^=( const Hash& code ) { m_data = internal::HashCombiner()( m_data, code.get() ); }

    template < typename... Args >
    inline void operator^=( Args const&... args )
    {
        m_data = internal::HashCombiner()( m_data, internal::HashFunctorVariadic()( args... ) );
    }

    inline void toHexString( std::ostream& os ) const
    {
        os << "0x" << std::setfill( '0' ) << std::setw( sizeof( m_data ) * 2 ) << std::hex << m_data;
    }

    inline std::string toHexString() const
    {
        std::ostringstream os;
        toHexString( os );
        return os.str();
    }

    static HashCodeType fromHexString( const std::string& str )
    {
        if( str.size() > 2 )
        {
            if( str.substr( 0, 2 ) == "0x" )
            {
                HashCodeType       result;
                std::istringstream is( str.substr( 2, str.size() - 2 ) );
                is >> std::hex >> result;
                return result;
            }
        }
        THROW_RTE( "Invalid hex string: " << str );
    }
};

inline std::ostream& operator<<( std::ostream& os, const common::Hash& hash )
{
    return os << hash.get();
}

inline std::istream& operator>>( std::istream& is, common::Hash& hash )
{
    std::size_t sz = 0U;
    is >> sz;
    hash.set( sz );
    return is;
}

} // namespace common

#endif // COMMON_HASH_UTILS_28_OCT_2020
