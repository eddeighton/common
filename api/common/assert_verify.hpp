
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

#ifndef EDSASSERT_20_12_2012
#define EDSASSERT_20_12_2012

#include "processID.hpp"
#include "backtrace.hpp"
#include "terminal.hpp"

#include "requireSemicolon.hpp"
#include "unreachable.hpp"

#include <boost/current_function.hpp>

#include <stdexcept>
#include <sstream>
#include <exception>

#ifdef DEBUG

#ifndef _CRT_ASSERT
#define _CRT_ASSERT 0

#endif

#ifndef _CRT_ERROR
#define _CRT_ERROR 0

#endif

// #define DEBUG_BREAK() DO_STUFF_AND_REQUIRE_SEMI_COLON( raise( SIGTRAP ); )

#define DEBUG_BREAK() DO_NOTHING_BUT_REQUIRE_SEMI_COLON

#define THROW_BACKTRACE( exceptionType, msg )                                                                   \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( DEBUG_BREAK(); std::ostringstream _os2; common::getBackTrace( _os2 );      \
                                     _os2 << common::COLOUR_RED_BEGIN << "PROCESS " << common::ProcessID::get() \
                                          << " FILE " << __FILE__ << ":" << __LINE__ << " MSG:" << msg          \
                                          << common::COLOUR_END;                                                \
                                     throw exceptionType( _os2.str() ); UNREACHABLE; )

#define THROW( exceptionType, msg )                                                                             \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( DEBUG_BREAK(); std::ostringstream _os2;                                    \
                                     _os2 << common::COLOUR_RED_BEGIN << "PROCESS " << common::ProcessID::get() \
                                          << " FILE " << __FILE__ << ":" << __LINE__ << " MSG:" << msg          \
                                          << common::COLOUR_END;                                                \
                                     throw exceptionType( _os2.str() ); UNREACHABLE; )

#define ASSERT_MSG( expr, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expr ) ) { THROW( std::runtime_error, #expr << " " << msg ); } )

#define ASSERT( expr ) ASSERT_MSG( expr, "" )

#define TERMINATE_IF_NOT( expression, msg )                  \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expression ) ) { \
        DEBUG_BREAK();                                       \
        std::terminate();                                    \
    } )

#else // DEBUG

#define ASSERT_MSG( expr, msg ) DO_NOTHING_BUT_REQUIRE_SEMI_COLON
#define ASSERT( expr ) DO_NOTHING_BUT_REQUIRE_SEMI_COLON

#define THROW_BACKTRACE( exceptionType, msg )                                                                  \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os; common::getBackTrace( _os );                      \
                                     _os << common::COLOUR_RED_BEGIN << "FILE " << __FILE__ << ":" << __LINE__ \
                                         << " FUNCTION:" << BOOST_CURRENT_FUNCTION << "\nMSG:" << msg          \
                                         << common::COLOUR_END;                                                \
                                     throw exceptionType( _os.str() ); UNREACHABLE; )

#define THROW( exceptionType, msg )                                                                           \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( std::ostringstream _os2; _os2 << msg; throw exceptionType( _os2.str() ); \
                                     UNREACHABLE; )

#define TERMINATE_IF_NOT( expression, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expression ) ) { std::terminate(); UNREACHABLE; } )

#endif //_DEBUG

#define THROW_RTE( msg ) THROW( std::runtime_error, msg )
#define THROW_TODO THROW( std::runtime_error, "TODO - this is not done yet!" )

#define VERIFY( expression, exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( if( !( expression ) ) { THROW( exceptionType, msg ); } )

#define VERIFY_RTE_MSG( expression, msg ) VERIFY( expression, std::runtime_error, msg )
#define VERIFY_RTE( expression ) VERIFY( expression, std::runtime_error, "" )

#endif // EDSASSERT_20_12_2012
