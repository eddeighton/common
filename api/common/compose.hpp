#ifndef COMPOSE_11_02_2013
#define COMPOSE_11_02_2013

#include <algorithm>
#include <functional>
#include <utility>

namespace generics
{
#ifdef ALLWAYS
#undef ALLWAYS
#endif
#ifdef all
#undef all
#endif
#ifdef PASSTHROUGH
#undef PASSTHROUGH
#endif
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
class ALLWAYS
{
public:
    template< class T >
    bool operator()( const T& ) const
    {
        return true;
    }
};
static ALLWAYS all() { return ALLWAYS(); }

class PASSTHROUGH
{
public:
    template< class T >
    const T& operator()( const T& v ) const
    {
        return v;
    }
    template< class T >
    T& operator()( T& v ) const
    {
        return v;
    }
};
static PASSTHROUGH passthrough() { return PASSTHROUGH(); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class IterType >
struct DeRef
{
    typedef typename IterType::value_type ValueType;
    ValueType operator()( const IterType& iter ) const
    {
        return *iter;
    }
};

template< class IterType >
DeRef< IterType > deref() { return DeRef< IterType >(); }

template< class IterType, class Functor >
struct DeRefFunctor
{
    Functor& functor;
    DeRefFunctor( Functor& _functor ) : functor( _functor ) {}
    typedef typename Functor::result_type ReturnType;
    ReturnType operator()( const IterType& iter ) const
    {
        return functor( *iter );
    }
};

template< class IterType, class Functor >
DeRefFunctor< IterType, Functor > derefFunctor( Functor& functor ) { return DeRefFunctor< IterType, Functor >( functor ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class IterType >
struct First
{
    typedef typename IterType::value_type ValueType;
    typedef typename ValueType::first_type ReturnType;
    ReturnType operator()( const IterType& iter ) const
    {
        return iter->first;
    }
};

template< class IterType >
First< IterType > first() { return First< IterType >(); }

template< class IterType, class Functor >
struct FirstFunctor
{
    Functor& functor;
    FirstFunctor( Functor& _functor ) : functor( _functor ) {}
    typedef typename Functor::result_type ReturnType;
    ReturnType operator()( const IterType& iter ) const
    {
        return functor( iter->first );
    }
};

template< class IterType, class Functor >
FirstFunctor< IterType, Functor > firstFunctor( Functor& functor ) { return FirstFunctor< IterType, Functor >( functor ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class IterType >
struct Second
{
    typedef typename IterType::value_type ValueType;
    typedef typename ValueType::second_type ReturnType;
    ReturnType operator()( const IterType& iter ) const
    {
        return iter->second;
    }
};
template< class IterType >
Second< IterType > second() { return Second< IterType >(); }

template< class IterType, class Functor >
struct SecondFunctor
{
    Functor& functor;
    SecondFunctor( Functor& _functor ) : functor( _functor ) {}
    typedef typename Functor::result_type ReturnType;
    ReturnType operator()( const IterType& iter ) const
    {
        return functor( iter->second );
    }
};

template< class IterType, class Functor >
SecondFunctor< IterType, Functor > secondFunctor( Functor& functor ) { return SecondFunctor< IterType, Functor >( functor ); }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void push_back( CONT<T>& data, const T& elem )
{
    data.push_back( elem );
}

template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
void push_back( CONT<T>& data, const T& elem )
{
    data.insert( elem );
}

template<typename T1, typename T2,
        template<typename Key, typename Type, typename Traits = std::less< Key >,
            typename Allocator = std::allocator< std::pair< const Key, Type > > > class CONT >
void push_back( CONT<T1,T2>& data, const std::pair< const T1, T2 >& elem )
{
    data.insert( elem );
}

template< class TContainer, class TFunctor >
class Collect
{
public:
    Collect( TContainer& container, const TFunctor& _functor )
        :   m_container( container ),
            functor( _functor )
    {
    }
    template< class T >
    void operator()( T value ) const
    {
        push_back( m_container, functor( value ) );
    }
    template< class T >
    void operator()( T value )
    {
        push_back( m_container, functor( value ) );
    }
private:
    TContainer& m_container;
    TFunctor functor;
};
template< class TContainer, class TFunctor >
Collect< TContainer, TFunctor > collect( TContainer& container, const TFunctor& functor )
{
    return Collect< TContainer, TFunctor >( container, functor );
}

template< class TContainer >
Collect< TContainer, PASSTHROUGH > collect( TContainer& container )
{
    return Collect< TContainer, PASSTHROUGH >( container, passthrough() );
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class TContainer, class TFunctor >
class CollectIf
{
public:
    CollectIf( TContainer& container, const TFunctor& _functor )
        :   m_container( container ),
            functor( _functor )
    {
    }
    template< class T >
    void operator()( T value ) const
    {
        if( functor( value ) )
            push_back( m_container, value );
    }
    template< class T >
    void operator()( T value )
    {
        if( functor( value ) )
            push_back( m_container, value );
    }
private:
    TContainer& m_container;
    TFunctor functor;
};
template< class TContainer, class TFunctor >
CollectIf< TContainer, TFunctor > collectIf( TContainer& container, const TFunctor& functor ) { return CollectIf< TContainer, TFunctor >( container, functor ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class TContainer, class TFunctor, class TConversion >
class CollectIfConvert
{
public:
    CollectIfConvert( TContainer& container, const TFunctor& _functor, const TConversion& _conversion )
        :   m_container( container ),
            functor( _functor ),
            conversion( _conversion )
    {
    }
    template< class T >
    void operator()( T value ) const
    {
        if( functor( value ) )
            push_back( m_container, conversion( value ) );
    }
    template< class T >
    void operator()( T value )
    {
        if( functor( value ) )
            push_back( m_container, conversion( value )  );
    }
private:
    TContainer& m_container;
    TFunctor functor;
    TConversion conversion;
};
template< class TContainer, class TFunctor, class TConversion >
CollectIfConvert< TContainer, TFunctor, TConversion > collectIfConvert( TContainer& container, const TFunctor& functor, const TConversion& conversion )
{
    return CollectIfConvert< TContainer, TFunctor, TConversion >( container, functor, conversion );
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class T1, class T2 >
class AND
{
public:
    T1 f1;
    T2 f2;
    AND( const T1& _f1, const T2& _f2 ) : f1( _f1 ), f2( _f2 ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        return f1( p ) && f2( p );
    }
    template< class T >
    bool operator()( T& p )
    {
        return f1( p ) && f2( p );
    }
};

template< class T1, class T2 >
static AND< T1, T2 > _and( T1 t1, T2 t2 ) { return AND< T1, T2 >( t1, t2 ); }

template< class T1, class T2, class T3 >
static AND< T1, AND< T2, T3 > > _and( T1 t1, T2 t2, T3 t3 ) { return AND< T1, AND< T2, T3 > >( t1, AND< T2, T3 >( t2, t3 ) ); }

template< class T1, class T2, class T3, class T4 >
static AND< T1, AND< T2, AND< T3, T4 > > > _and( T1 t1, T2 t2, T3 t3, T4 t4 ) { return AND< T1, AND< T2, AND< T3, T4 > > >( t1, AND< T2, AND< T3, T4 > >( t2, AND< T3, T4 >( t3, t4 ) ) ); }

template< class T1, class T2, class T3, class T4, class T5 >
static AND< T1, AND< T2, AND< T3, AND< T4, T5 > > > > _and( T1 t1, T2 t2, T3 t3, T4 t4, T5 t5 ) { return AND< T1, AND< T2, AND< T3, AND< T4, T5 > > > >( t1, AND< T2, AND< T3, AND< T4, T5 > > >( t2, AND< T3, AND< T4, T5 > >( t3, AND< T4, T5 >( t4, t5 ) ) ) ); }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class T1, class T2 >
class OR
{
public:
    typedef bool result_type;
    T1 f1;
    T2 f2;
    OR( const T1& _f1, const T2& _f2 ) : f1( _f1 ), f2( _f2 ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        return f1( p ) || f2( p );
    }
    template< class T >
    bool operator()( T& p )
    {
        return f1( p ) || f2( p );
    }
};
template< class T1, class T2 >
static OR< T1, T2 > _or( T1 t1, T2 t2 ) { return OR< T1, T2 >( t1, t2 ); }

template< class T1, class T2, class T3 >
static OR< T1, OR< T2, T3 > > _or( T1 t1, T2 t2, T3 t3 ) { return OR< T1, OR< T2, T3 > >( t1, OR< T2, T3 >( t2, t3 ) ); }

template< class T1, class T2, class T3, class T4 >
static OR< T1, OR< T2, OR< T3, T4 > > > _or( T1 t1, T2 t2, T3 t3, T4 t4 ) { return OR< T1, OR< T2, OR< T3, T4 > > >( t1, OR< T2, OR< T3, T4 > >( t2, OR< T3, T4 >( t3, t4 ) ) ); }

template< class T1, class T2, class T3, class T4, class T5 >
static OR< T1, OR< T2, OR< T3, OR< T4, T5 > > > > _or( T1 t1, T2 t2, T3 t3, T4 t4, T5 t5 ) { return OR< T1, OR< T2, OR< T3, OR< T4, T5 > > > >( t1, OR< T2, OR< T3, OR< T4, T5 > > >( t2, OR< T3, OR< T4, T5 > >( t3, OR< T4, T5 >( t4, t5 ) ) ) ); }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class T1, class T2 >
class XOR
{
public:
    T1 f1;
    T2 f2;
    XOR( const T1& _f1, const T2& _f2 ) : f1( _f1 ), f2( _f2 ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        bool b1 = f1( p ), b2 = f2( p );
        return ( b1 && !b2 ) || ( b2 && !b1 );
    }
    template< class T >
    bool operator()( T& p )
    {
        bool b1 = f1( p ), b2 = f2( p );
        return ( b1 && !b2 ) || ( b2 && !b1 );
    }
};
template< class T1, class T2 >
static XOR< T1, T2 > _xor( T1 t1, T2 t2 ) { return XOR< T1, T2 >( t1, t2 ); }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class T1, class T2 >
class NAND
{
public:
    T1 f1;
    T2 f2;
    NAND( const T1& _f1, const T2& _f2 ) : f1( _f1 ), f2( _f2 ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        return !f1( p ) && !f2( p );
    }
    template< class T >
    bool operator()( T& p )
    {
        return !f1( p ) && !f2( p );
    }
};
template< class T1, class T2 >
static NAND< T1, T2 > _nand( T1 t1, T2 t2 ) { return NAND< T1, T2 >( t1, t2 ); }
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class T1 >
class NOT
{
public:
    T1 f1;
    NOT( const T1& _f1 ) : f1( _f1 ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        return !f1( p );
    }
    template< class T >
    bool operator()( T& p )
    {
        return !f1( p );
    }
};
template< class T1 >
static NOT< T1 > _not( T1 t1 ) { return NOT< T1 >( t1 ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class TValue >
class EQUAL
{
public:
    const TValue& value;
    EQUAL( const TValue& _value ) : value( _value ) {}
    template< class T >
    bool operator()( const T& p ) const
    {
        return value == p;
    }
    template< class T >
    bool operator()( T& p )
    {
        return value == p;
    }
};

template< class TValue >
static EQUAL< TValue > equal( const TValue& value ) { return EQUAL< TValue >( value ); }

template< class TLeft, class TRight >
class EQUAL2
{
    TLeft& leftPredicate;
    TRight& rightPredicate;
public:
    EQUAL2( TLeft& _left, TRight& _right )
        :   leftPredicate( _left ),
            rightPredicate( _right )
    {}
    template< class T1, class T2 >
    bool operator()( const T1& left, const T2& right ) const
    {
        return leftPredicate( left ) == rightPredicate( right );
    }
    template< class T1, class T2 >
    bool operator()( T1& left, T2& right )
    {
        return leftPredicate( left ) == rightPredicate( right );
    }
};

template< class TLeft, class TRight >
static EQUAL2< TLeft, TRight > equal2( TLeft& _left, TRight& _right ) { return EQUAL2< TLeft, TRight >( _left, _right ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
template< class TLeft, class TRight >
class _LESSTHAN
{
    const TLeft& leftPredicate;
    const TRight& rightPredicate;
public:
    _LESSTHAN( const TLeft& _left, const TRight& _right )
        :   leftPredicate( _left ),
            rightPredicate( _right )
    {}
    template< class T1, class T2 >
    bool operator()( const T1& left, const T2& right ) const
    {
        return leftPredicate( left ) < rightPredicate( right );
    }
    template< class T1, class T2 >
    bool operator()( T1& left, T2& right )
    {
        return leftPredicate( left ) < rightPredicate( right );
    }
    template< class T1, class T2 >
    bool opposite( const T1& left, const T2& right ) const
    {
        return rightPredicate( right ) < leftPredicate( left );
    }
    template< class T1, class T2 >
    bool opposite( T1& left, T2& right )
    {
        return rightPredicate( right ) < leftPredicate( left );
    }
};

template< class TLeft, class TRight >
static _LESSTHAN< TLeft, TRight > lessthan( const TLeft& _left, const TRight& _right ) { return _LESSTHAN< TLeft, TRight >( _left, _right ); }

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
bool contains( const CONT<T>& data, const T& elem )
{
    return std::find( data.begin(), data.end(), elem ) != data.end();
}

template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
bool contains( const CONT<T>& data, const T& elem )
{
    return data.find( elem ) != data.end();
}

template< class TContainer >
class WITHIN
{
public:
    //typedef typename TContainer::value_type VALUE;

    const TContainer& container;
    WITHIN( const TContainer& _container ) : container( _container ) {}

    template< class T >
    bool operator()( const T& p ) const
    {
        return contains( container, p );
    }

    template< class T >
    bool operator()( T& p )
    {
        return contains( container, p );
    }
};

template< class TContainer >
static WITHIN< TContainer > within( const TContainer& value ) { return WITHIN< TContainer >( value ); }


}


#endif
