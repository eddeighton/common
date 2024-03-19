
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

#include "common/process.hpp"

#include <boost/process.hpp>

#include <boost/asio/io_service.hpp>

#include <string>
#include <future>
#include <sstream>
#include <map>

namespace common
{
    
int runProcess( const std::string& strCmd, std::string& strOutput, std::string& strError )
{
    namespace bp = boost::process;

    std::future< std::string > output, error;

    boost::asio::io_service ios;

    bp::child c( strCmd, bp::std_in.close(), bp::std_out > output, bp::std_err > error, ios );

    ios.run();

    strOutput = output.get();
    strError  = error.get();

#ifdef _WIN32
    if( !strError.empty() )
    {
        return EXIT_FAILURE; // c.exit_code();
    }
    else
    {
        return EXIT_SUCCESS;
    }
#else
    if( !strError.empty() )
    {
        return EXIT_FAILURE; // c.exit_code();
    }
    else
    {
        return EXIT_SUCCESS;
    }
#endif
}

std::string Command::str() const
{
    std::ostringstream os;
    for( const auto& [ key, value ] : m_environmentVars )
    {
        os << key << "=" << value << " ";
    }
    os << m_strCmd;
    return os.str();
}

int runCmd( const Command& cmd, std::string& strOutput, std::string& strError )
{
    namespace bp = boost::process;

    std::future< std::string > output, error;

    boost::asio::io_service ios;

    if( cmd.m_environmentVars.empty() )
    {
        bp::child c( cmd.m_strCmd, bp::std_in.close(), bp::std_out > output, bp::std_err > error, ios );
        ios.run();
    }
    else
    {
        bp::environment env_ = boost::this_process::environment();
        {
            for( const auto& [ key, value ] : cmd.m_environmentVars )
            {
                env_[ key ] = value;
            }
        }

        bp::child c( cmd.m_strCmd, env_, bp::std_in.close(), bp::std_out > output, bp::std_err > error, ios );
        ios.run();
    }

    strOutput = output.get();
    strError  = error.get();

#ifdef _WIN32
    if( !strError.empty() )
    {
        return EXIT_FAILURE; // c.exit_code();
    }
    else
    {
        return EXIT_SUCCESS;
    }
#else
    if( !strError.empty() )
    {
        return EXIT_FAILURE; // c.exit_code();
    }
    else
    {
        return EXIT_SUCCESS;
    }
#endif
}
}