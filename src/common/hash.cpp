
#include "common/hash.hpp"
#include "common/assert_verify.hpp"

#include "boost/filesystem.hpp"
#include "boost/iostreams/device/mapped_file.hpp"
#include <boost/functional/hash.hpp>

namespace common
{
namespace internal
{
HashCodeType hash_file( const boost::filesystem::path& file )
{
    if ( boost::filesystem::exists( file ) )
    {
        // error seems to occur if attempt to memory map an empty file
        if ( boost::filesystem::is_empty( file ) )
        {
            return boost::filesystem::hash_value( file );
        }
        else
        {
            try
            {
                boost::iostreams::mapped_file_source fileData( file );
                const std::string_view               dataView( fileData.data(), fileData.size() );
                return std::hash< std::string_view >{}( dataView );
            }
            catch ( std::ios_base::failure& ex )
            {
                THROW_RTE( "File error hashing: " << file.string() << " exception: " << ex.what() );
            }
            catch ( std::exception& ex )
            {
                THROW_RTE( "File error hashing: " << file.string() << " exception: " << ex.what() );
            }
            catch( ... )
            {
                THROW_RTE( "File error hashing: " << file.string() << " exception: unknown" );
            }
        }
    }
    THROW_RTE( "File does not exist: " << file.string() );
}
} // namespace internal
} // namespace common
