
#include "common/string.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/bind/bind.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <algorithm>
#include <cctype>

namespace
{
static const char DELIM = '_';

int eds_isspace( int ch ) { return std::isspace( ch ); }

int eds_isalnum( int ch ) { return std::isalnum( ch ); }

std::string style_strip_whitespace( const std::string& str )
{
    std::string sResult = str;
    sResult.erase( std::remove_if( sResult.begin(), sResult.end(), &eds_isspace ), sResult.end() );
    return sResult;
}

std::string style_replace_non_alpha_numeric( const std::string& str, char r )
{
    std::string strResult;
    using namespace boost::placeholders;
    std::replace_copy_if(
        str.begin(), str.end(), std::back_inserter( strResult ), !boost::bind( &eds_isalnum, _1 ), r );
    return strResult;
}

std::string style_uuid()
{
    boost::uuids::random_generator gen;
    std::string                    strUUID = boost::to_upper_copy( boost::uuids::to_string( gen() ) );
    return style_replace_non_alpha_numeric( strUUID, DELIM );
}

std::string style_date_nice()
{
    return boost::posix_time::to_simple_string( boost::posix_time::second_clock::universal_time() );
}

} // namespace

namespace common
{

std::string uuid() { return style_uuid(); }

std::string date() { return style_replace_non_alpha_numeric( boost::to_upper_copy( style_date_nice() ), DELIM ); }

} // namespace common