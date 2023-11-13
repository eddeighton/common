//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
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

/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef COMMON_TIME_UTILS_22_OCT_2023
#define COMMON_TIME_UTILS_22_OCT_2023

#include <chrono>
#include <iomanip>
#include <ratio>

namespace common
{

inline auto elapsed( std::chrono::steady_clock::time_point& last )
{
    const auto timeNow = std::chrono::steady_clock::now();
    const auto delta   = std::chrono::duration_cast< std::chrono::steady_clock::duration >( timeNow - last );
    last               = timeNow;
    return delta;
}

template < typename T >
inline std::string printDuration( const T& dur )
{
#ifdef _WIN64
    using I64 = signed long long;
#else
    using I64 = signed long int;
#endif

    std::ostringstream os;
    using DurationType = std::chrono::duration< I64, std::ratio< 1, 1'000'000'000 > >;
    auto c             = std::chrono::duration_cast< DurationType >( dur ).count();
    auto sec           = c / 1'000'000'000;
    auto ms            = ( c % 1'000'000'000 ) / 1'000'000;
    auto us            = ( c % 1'000'000 ) / 1'000;
    os << std::setfill( '0' ) << sec << "." << std::setw( 3 ) << std::setfill( '0' ) << ms << "ms." << std::setw( 3 )
       << std::setfill( '0' ) << us << "us";
    return os.str();
}

template < typename T >
inline std::string printTimeStamp( const T& timeStamp )
{
    std::stringstream ss;
    {
        auto in_time_t = std::chrono::system_clock::to_time_t( timeStamp );
        ss << std::put_time( std::localtime( &in_time_t ), " %Y-%m-%d %X" );
    }
    return ss.str();
}

inline std::string printCurrentTime()
{
    return printTimeStamp( std::chrono::system_clock::now() );
}
} // namespace common

#endif // COMMON_TIME_UTILS_22_OCT_2023
