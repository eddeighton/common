/*
Copyright Deighton Systems Limited (c) 2011
*/
#ifndef STL_GENERICS_26_01_2013
#define STL_GENERICS_26_01_2013

#include <iosfwd>
#include <functional>
#include <algorithm>
#include <utility>
#include <iterator>

namespace generics
{

template< class TContainer >
struct PrintContainer
{
    TContainer& container;
    PrintContainer( TContainer& _container ) : container( _container ) {}
    void print( std::ostream& os ) const
    {
        std::copy( container.begin(), container.end(),
            std::ostream_iterator< typename TContainer::value_type >( os, "\n" ) );
    }
};

template< class TContainer >
static PrintContainer< TContainer > printContainer( TContainer& container ) { return PrintContainer< TContainer >( container ); }

template< class T >
static std::ostream& operator<<( std::ostream& os, const PrintContainer< T >& printer )
{
    printer.print( os );
    return os;
}

//if container not empty get the front and remove it.  Returns either default T or what was the front
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
T popAndRemoveFront( CONT<T>& data )
{
    const T first = data.empty() ? T() : data.front();
    if( !data.empty() )
        data.pop_front();
    return first;
}

//delete all members of data and clear it
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void deleteAndClear( CONT<T>& data )
{
    for( typename CONT<T>::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i )
        delete *i;
    data.clear();
}

//delete all members of data and clear it
template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
void deleteAndClear( CONT<T>& data )
{
    for( typename CONT<T>::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i )
        delete *i;
    data.clear();
}

//delete all seconds of the data and clear it
template<typename T1, typename T2,
        template<typename Key, typename Type, typename Traits = std::less< Key >,
            typename Allocator = std::allocator< std::pair< const Key, Type > > > class CONT >
void deleteAndClearSecond( CONT< T1, T2 >& data )
{
    for( typename CONT< T1, T2 >::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i )
        delete i->second;
    data.clear();
}

//erase all elements in data matching elem
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void eraseAll( CONT<T>& data, const T& elem )
{
    typename CONT<T>::iterator iNewEnd = std::remove( data.begin(), data.end(), elem );
    data.erase( iNewEnd, data.end() );
}

//erase first element in data matching elem
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void eraseFirst( CONT<T>& data, const T& elem )
{
    typename CONT<T>::iterator iFind = std::find( data.begin(), data.end(), elem );
    if( iFind != data.end() )
        data.erase( iFind );
}


template<typename T, typename Predicate,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void eraseIf( CONT<T>& data, const Predicate& predicate )
{
    typename CONT<T>::iterator iNewEnd = std::remove_if( data.begin(), data.end(), predicate );
    data.erase( iNewEnd, data.end() );
}

template<typename T, typename Predicate,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
void eraseIf( CONT<T>& data, const Predicate& predicate )
{
    for( typename CONT<T>::iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd;  )
    {
        if( predicate( *i ) )
        {
            typename CONT<T>::iterator iNext = i;
            ++iNext;
            data.erase( i );
            i = iNext;
        }
        else
            ++i;
    }
}


//if element exists, find it, erase it and delete it
template<typename T,
        template<typename ELEM,typename ALLOC = std::allocator<ELEM> > class CONT >
void findEraseAndDelete( CONT<T>& data, const T& elem )
{
    typename CONT<T>::iterator iFind = std::find( data.begin(), data.end(), elem );
    if( iFind != data.end() )
    {
        data.erase( iFind );
        delete elem;
    }
}
template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
void findEraseAndDelete( CONT<T>& data, const T& elem )
{
    typename CONT<T>::iterator iFind = data.find( elem );
    if( iFind != data.end() )
    {
        data.erase( iFind );
        delete elem;
    }
}


//find element with key v, delete ->second and remove it from container
template<typename T1, typename T2,
        template<typename Key, typename Type, typename Traits = std::less< Key >,
            typename Allocator = std::allocator< std::pair< const Key, Type > > > class CONT >
void findFirstEraseAndDeleteSecond( CONT< T1, T2 >& data, const T1& v )
{
    typename CONT< T1, T2 >::iterator iFind = data.find( v );
    if( iFind != data.end() )
    {
        delete iFind->second;
        data.erase( iFind );
    }
}

template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
void removeSet( CONT<T>& data, const CONT<T>& remove )
{
    typename CONT<T>::const_iterator j = remove.begin(), jEnd = remove.end();
    for( typename CONT<T>::const_iterator
        i = data.begin(), iEnd = data.end();
        i!=iEnd && j!=jEnd;  )
    {
        if( *i == *j )
        {
            typename CONT<T>::const_iterator iErase = i++;
            data.erase( iErase );
            ++j;
        }
        else if( *i > *j )
            ++j;
        else
            ++i;
    }
}

template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
bool isSubSet( const CONT<T>& data, const CONT<T>& subset )
{
    typename CONT<T>::const_iterator j = subset.begin(), jEnd = subset.end();
    for( typename CONT<T>::const_iterator
        i = data.begin(), iEnd = data.end();
        i!=iEnd && j!=jEnd; ++i )
    {
        if( *i == *j ) ++j;
        else if( *i > *j ) break;
    }
    return j == jEnd ? true : false;
}

//erase first element in data where data->second == v
template<typename T1, typename T2,
        template<typename Key, typename Type, typename Traits = std::less< Key >,
            typename Allocator = std::allocator< std::pair< const Key, Type > > > class CONT >
void eraseAllSecond( CONT< T1, T2 >& data, const T2& v )
{
    for( typename CONT<T1,T2>::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; )
    {
        if( i->second == v )
            i = data.erase( i );
        else
            ++i;
    }
}


template<typename T1, typename T2, typename Functor,
        template<typename T1_, typename T2_, typename Traits = std::less< T1_ >,
            typename Allocator = std::allocator< std::pair< const T1_, T2_ > > > class CONT >
void for_each_second( CONT< T1, T2 >& data, Functor&& functor )
{
    for( typename CONT< T1, T2 >::iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i )
        functor( i->second );
}

template<typename T1, typename T2, typename Functor,
        template<typename T1_, typename T2_, typename Traits = std::less< T1_ >,
            typename Allocator = std::allocator< std::pair< const T1_, T2_ > > > class CONT >
void for_each_second( const CONT< T1, T2 >& data, Functor&& functor )
{
    for( typename CONT< T1, T2 >::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i )
        functor( i->second );
}

template<typename T1, typename T2,
        template<typename T1_, typename T2_, typename Traits = std::less< T1_ >,
            typename Allocator = std::allocator< std::pair< const T1_, T2_ > > > class CONT >
T2 find( const CONT< T1, T2 >& data, const T1& value )
{
    typename CONT< T1, T2 >::const_iterator iFind = data.find( value );
    if( iFind != data.end() )
        return iFind->second;
    else
        return T2();
}

/*
template<typename T1, typename T2, typename Functor,
        template<typename T1, typename T2, typename Traits = std::less< T1 >,
            typename Allocator = std::allocator< std::pair< const T1, T2 > > > class CONT >
void findFirst_functorSecond( CONT< T1, T2 >& data, const T1& value, const Functor& functor )
{
    typename CONT< T1, T2 >::iterator iFind = data.find( value);
    if( iFind != data.end() )
        functor( iFind->second );
}*/
/*
template<typename T,
        template< typename _Kty, typename _Pr = std::less<_Kty>, typename _Alloc = std::allocator<_Kty> > class CONT >
T findFirstIntegerGap( const CONT<T>& data, T start = T() )
{
    for( typename CONT<T>::const_iterator i = data.begin(),
        iEnd = data.end(); i!=iEnd; ++i, ++start )
    {
        if( *i != start )
            break;
    }
    return start;
}
*/
template< class CONT >
unsigned int findFirstPointerGap( const CONT& data, unsigned int uiStart = 0u )
{
    unsigned int ui = uiStart;
    if( uiStart < data.size() )
    {
        for( typename CONT::const_iterator i = data.begin() + uiStart, iEnd = data.end();
            i != iEnd; ++ui, ++i )
        {
            if( !( *i ) )
                break;
        }
    }
    return ui;
}

template< class CONT, class CONT2 >
void findNPointerGaps( const CONT& data, CONT2& gaps, unsigned int uiCount, unsigned int uiStart = 0u )
{
    for( unsigned int ui = 0u; ui < uiCount; ++ui)
    {
        unsigned int uiGap = findFirstPointerGap( data, uiStart );
        gaps.insert( uiGap );
        uiStart = uiGap + 1u;
    }
}

template< class Iter1, class Iter2, class Compare, class Removal, class Addition >
void match( Iter1 i1, Iter1 i1End, Iter2 i2, Iter2 i2End, const Compare& cmp, const Removal& rem, const Addition& add )
{
    while( true )
    {
        if( i1 != i1End )
        {
            if( i2 != i2End )
            {
                if( cmp( i1, i2 ) )
                {
                    rem( i1 );
                    ++i1;
                }
                else if( cmp.opposite( i1, i2 ) )
                {
                    add( i2 );
                    ++i2;
                }
                else
                {
                    ++i1;
                    ++i2;
                }
            }
            else
            {
                rem( i1 );
                ++i1;
            }
        }
        else
        {
            if( i2 != i2End )
            {
                add( i2 );
                ++i2;
            }
            else
            {
                break;
            }
        }
    }
}

template< class Iter1, class Iter2, class Compare, class Update, class Removal, class Addition, class Updated >
void matchGetUpdates( Iter1 i1, Iter1 i1End, Iter2 i2, Iter2 i2End,
                     const Compare& cmp, const Update& shouldUpdate, const Removal& rem, const Addition& add, const Updated& updatesNeeded )
{
    while( true )
    {
        if( i1 != i1End )
        {
            if( i2 != i2End )
            {
                if( cmp( i1, i2 ) )
                {
                    rem( i1 );
                    ++i1;
                }
                else if( cmp.opposite( i1, i2 ) )
                {
                    add( i2 );
                    ++i2;
                }
                else
                {
                    if( shouldUpdate( i1, i2 ) )
                        updatesNeeded( i1 );
                    ++i1;
                    ++i2;
                }
            }
            else
            {
                rem( i1 );
                ++i1;
            }
        }
        else
        {
            if( i2 != i2End )
            {
                add( i2 );
                ++i2;
            }
            else
            {
                break;
            }
        }
    }
}

template< class Iter1, class Iter2, class Compare, class Update, class Removal, class Addition, class Updated >
void matchGetUpdatesIndex( Iter1 i1, Iter1 i1End, Iter2 i2, Iter2 i2End,
                     const Compare& cmp, const Update& shouldUpdate, const Removal& rem, const Addition& add, const Updated& updatesNeeded )
{
    int iAccumulator = 0;
    while( true )
    {
        if( i1 != i1End )
        {
            if( i2 != i2End )
            {
                if( cmp( i1, i2, iAccumulator ) )
                {
                    rem( i1 );
                    --iAccumulator;
                    ++i1;
                }
                else if( cmp.opposite( i1, i2, iAccumulator ) )
                {
                    add( i2 );
                    ++iAccumulator;
                    ++i2;
                }
                else
                {
                    if( shouldUpdate( i1, i2 ) )
                        updatesNeeded( i1 );
                    ++i1;
                    ++i2;
                }
            }
            else
            {
                rem( i1 );
                ++i1;
            }
        }
        else
        {
            if( i2 != i2End )
            {
                add( i2 );
                ++i2;
            }
            else
            {
                break;
            }
        }
    }
}

template< class TIter >
void slide( TIter iBegin, std::size_t iCurrent, std::size_t iTarget )
{
    if( iTarget < iCurrent )
    {
        typename TIter::value_type value = *(iBegin+iCurrent);
        std::copy_backward( iBegin + iTarget, iBegin + iCurrent, iBegin + iCurrent + 1 );
        *(iBegin+iTarget) = value;
    }
    else if( iCurrent < iTarget )
    {
        typename TIter::value_type value = *(iBegin+iCurrent);
        std::copy( iBegin + ( iCurrent + 1 ), iBegin + ( iTarget + 1 ), iBegin + iCurrent );
        *(iBegin+iTarget) = value;
    }
}


};
#endif
