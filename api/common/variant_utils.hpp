/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef VARIANT_UTILS_26_12_2013
#define VARIANT_UTILS_26_12_2013

#include <boost/variant.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>

namespace boost
{

struct VariantStrictEqualityVisitor : boost::static_visitor< bool >
{
    template< class T1 >
    bool operator()( const T1& left, const T1& right ) const
    {
        return left == right;
    }
    template< class T1, class T2 >
    bool operator()( const T1&, const T2& ) const
    {
        return false;
    }
};

template< class T >
struct TypeAccessor : boost::static_visitor< boost::optional< T& > >
{
    boost::optional< T& > operator()( T& targetType ) const
    {
        return boost::optional< T& >( targetType );
    }
    template< class TOther >
    boost::optional< T& > operator()( TOther& ) const
    {
        return boost::optional< T& >();
    }
};

template< class T, class TVariantType >
inline boost::optional< T& > get_from_variant( const TVariantType& var )
{
    return boost::apply_visitor( boost::TypeAccessor< T >(), var );
}

template< class T >
struct TypeDispatcher : boost::static_visitor< void >
{
    template< class Functor >
    TypeDispatcher( Functor& _functor )
        : functor( _functor )
    {}

    void operator()( const T& targetType ) const
    {
        functor( targetType );
    }
    template< class TOther >
    void operator()( const TOther& fileRef ) const
    {
        //ignor
    }
private:
    boost::function< void( const T& ) > functor;
};

}

#endif //VARIANT_UTILS_26_12_2013