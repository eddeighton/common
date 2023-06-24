
#ifndef STRING_UTILS_6_AUGUST_2015
#define STRING_UTILS_6_AUGUST_2015

#include <ostream>
#include <string>
#include <vector>

namespace common
{

    template< class Iter >
    inline void delimit( Iter pBegin, Iter pEnd, const std::string& delimiter, std::ostream& os )
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

    std::vector< std::string > simpleTokenise( const std::string& str, const char* pszDelimiters );
}

#endif //STRING_UTILS_6_AUGUST_2015
