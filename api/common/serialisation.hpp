
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2023_October_21_serialisation
#define GUARD_2023_October_21_serialisation

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/utility.hpp>

#include <boost/filesystem/path.hpp>

#include <boost/dynamic_bitset.hpp>
#include <boost/dynamic_bitset/serialization.hpp>

#include "serialisation_variant.hpp"

#include <optional>
#include <variant>

namespace boost::serialization
{

template < typename T >
struct IsXMLArchive
{
    static constexpr auto value = std::is_base_of< boost::archive::xml_iarchive, T >::value
                                  || std::is_base_of< boost::archive::xml_oarchive, T >::value;
};

template < typename T >
concept IsXMLArchiveConcept = IsXMLArchive< T >::value;


template < class Archive >
inline void serialize( Archive& ar, boost::filesystem::path& p, const unsigned int version )
{
    std::string s;
    if constexpr( Archive::is_saving::value )
        s = p.generic_string();
    ar& boost::serialization::make_nvp( "filepath", s );
    if constexpr( Archive::is_loading::value )
        p = s;
}

template < class Archive >
inline void serialize( Archive& ar, std::optional< boost::filesystem::path >& p, const unsigned int version )
{
    std::string s;
    if constexpr( Archive::is_saving::value )
    {
        if( p.has_value() )
            s = p.value().generic_string();
        else
            s = "";
    }
    ar& boost::serialization::make_nvp( "string", s );
    if constexpr( Archive::is_loading::value )
    {
        if( s.empty() )
            p = std::optional< boost::filesystem::path >();
        else
            p = s;
    }
}

template < class Archive, class T >
inline void serialize( Archive& ar, std::optional< T >& optionalValue, const unsigned int version )
{
    if constexpr( Archive::is_saving::value )
    {
        if( optionalValue.has_value() )
        {
            bool bTrueIsTrue = true;
            ar&  bTrueIsTrue;
            ar&  optionalValue.value();
        }
        else
        {
            bool bFalseIsFalse = false;
            ar&  bFalseIsFalse;
        }
    }

    if constexpr( Archive::is_loading::value )
    {
        bool bHasValue = false;
        ar&  bHasValue;
        if( bHasValue )
        {
            // MUST ensure the ObjectInfo passed to archive can have address captured
            optionalValue.emplace( T{} );
            ar& optionalValue.value();
        }
    }
}

} // namespace boost::serialization


#endif //GUARD_2023_October_21_serialisation
