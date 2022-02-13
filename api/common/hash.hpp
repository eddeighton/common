
#ifndef COMMON_HASH_UTILS_28_OCT_2020
#define COMMON_HASH_UTILS_28_OCT_2020

#include "boost/filesystem/path.hpp"

#include <string>
#include <vector>

namespace common
{

    using HashCode = std::size_t;
    
    inline HashCode hash_combine( HashCode left, HashCode right )
    {
        return left ^ ( right + 0x9e3779b9 + ( left << 6 ) + ( left >> 2 ) );
    }
    
    inline HashCode hash_combine( HashCode& seed ) { return seed; }
    
    inline HashCode hash_combine( std::initializer_list< HashCode > hashCodes ) 
    {
        HashCode hash = 0U;
        for( HashCode h : hashCodes )
        {
            hash = hash_combine( hash, h );
        }
        return hash;
    }

    
    HashCode hash_strings( const std::vector< std::string >& strings );
    HashCode hash_file( const boost::filesystem::path& file );
    
}


#endif //COMMON_HASH_UTILS_28_OCT_2020
