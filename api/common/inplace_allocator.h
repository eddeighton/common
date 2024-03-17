
#ifndef INPLACE_ALLOCATOR_20_12_2015
#define INPLACE_ALLOCATOR_20_12_2015

#include <cstddef>

#define IPAType size_t

typedef struct _Inplace_Allocator
{
    IPAType* pBuffer;
    IPAType  iSize;
    IPAType  iFirstFreeIndex;
} InplaceAllocator;

inline InplaceAllocator inplace_allocator_initialise( IPAType* pBuffer, IPAType iSize )
{
    InplaceAllocator result = { pBuffer, iSize, 0U };

    IPAType iCounter = 1;
    for( IPAType *p = result.pBuffer, *pEnd = result.pBuffer + result.iSize; p != pEnd; ++p )
    {
        *p = iCounter++;
    }

    return result;
}

inline IPAType* inplace_allocator_new( InplaceAllocator* pAllocator )
{
    IPAType* pResult = pAllocator->pBuffer + pAllocator->iFirstFreeIndex;

    if( pAllocator->iFirstFreeIndex != pAllocator->iSize )
    {
        pAllocator->iFirstFreeIndex = *pResult;
    }

    return pResult;
}

inline bool inplace_allocator_test( InplaceAllocator* pAllocator, IPAType* p )
{
    return ( p >= pAllocator->pBuffer && p < pAllocator->pBuffer + pAllocator->iSize ) ? true : false;
}

inline void inplace_allocator_free( InplaceAllocator* pAllocator, IPAType* pToFree )
{
    const auto iReleasedIndex = static_cast< IPAType >( pToFree - pAllocator->pBuffer );

    // find the previous free element
    if( iReleasedIndex < pAllocator->iFirstFreeIndex )
    {
        // insert the released element as the first free index
        *pToFree                    = pAllocator->iFirstFreeIndex;
        pAllocator->iFirstFreeIndex = iReleasedIndex;
    }
    else
    {
        // find the previous free index
        IPAType* pIter = pAllocator->pBuffer + pAllocator->iFirstFreeIndex;
        while( true )
        {
            IPAType* pNext = pAllocator->pBuffer + *pIter;
            if( pNext > pToFree )
            {
                // found it
                *pToFree = *pIter;
                *pIter   = iReleasedIndex;
                break;
            }
            pIter = pNext;
        }
    }
}

inline size_t inplace_allocator_remaining( InplaceAllocator* pAllocator )
{
    size_t iResult = 0;

    for( IPAType *pIter = pAllocator->pBuffer + pAllocator->iFirstFreeIndex,
                 *pEnd  = pAllocator->pBuffer + pAllocator->iSize;

         pIter != pEnd;

         pIter = pAllocator->pBuffer + *pIter, ++iResult )
        ;

    return iResult;
}

#endif // INPLACE_ALLOCATOR_20_12_2015
