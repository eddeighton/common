
#include "common/hash.hpp"
#include "common/assert_verify.hpp"

#include "boost/filesystem.hpp"
#include "boost/iostreams/device/mapped_file.hpp"

namespace common
{
    
std::size_t hash_strings( const std::vector< std::string >& strings )
{
    std::size_t szHash = 123456;
    
    for( const std::string& str : strings )
    {
        szHash = hash_combine( szHash, std::hash< std::string >{}( str ) );
    }
    
    return szHash;
}
    
std::size_t hash_file( const boost::filesystem::path& file )
{
    if( boost::filesystem::exists( file ) )
    {
        //error seems to occur if attempt to memory map an empty file
        if( boost::filesystem::is_empty( file ) )
        {
            return boost::filesystem::hash_value( file );
        }
        else
        {
            boost::iostreams::mapped_file_source fileData( file/*, boost::iostreams::mapped_file::readonly*/ );
            const std::string_view dataView( fileData.data(), fileData.size() );
            return std::hash< std::string_view >{}( dataView );
        }
    }
    THROW_RTE( "File does not exist: " << file.string() );
}

}