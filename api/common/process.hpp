//  Copyright (c) Deighton Systems Limited. 2023. All Rights Reserved.
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

#pragma warning( push )
#pragma warning( disable : 4996 ) // iterator thing
#pragma warning( disable : 4244 ) // conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <boost/asio/io_service.hpp>

#include <string>
#include <future>
#include <sstream>

namespace common
{
    inline int runProcess( const std::string& strCmd, std::string& strOutput, std::string& strError )
    {
        namespace bp = boost::process;
        
        std::future< std::string > output, error;
        
        boost::asio::io_service ios;

        bp::child c( strCmd,
                bp::std_in.close(),
                bp::std_out > output,
                bp::std_err > error,
                ios );
        
        ios.run();

        strOutput   = output.get();
        strError    = error.get();
        
#ifdef _WIN32
        if( !strError.empty() )
        {
            return -1; //c.exit_code();
        }
        else
        {
            return 0;
        }
#else
        if( !strError.empty() )
        {
            return -1; //c.exit_code();
        }
        else
        {
            return 0;
        }
#endif
    }
}
