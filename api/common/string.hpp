
#ifndef STRING_UTILS_6_AUGUST_2015
#define STRING_UTILS_6_AUGUST_2015

#include <ostream>
#include <string>

namespace common
{

    template< class Iter >
    void delimit( Iter pBegin, Iter pEnd, const std::string& delimiter, std::ostream& os )
    {
        for( Iter p = pBegin, pNext = pBegin; p!=pEnd; ++p )
        {
            ++pNext;
            if( pNext == pEnd )
            {
                os << *p;
            }
            else
            {
                os << *p << delimiter;
            }
        }
    }

    std::string uuid();
    std::string date();
}

#endif //STRING_UTILS_6_AUGUST_2015
