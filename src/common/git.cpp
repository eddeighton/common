
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

#include "common/git.hpp"
#include "common/process.hpp"
#include "common/string.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/filesystem.hpp>

#include <sstream>

namespace git
{

bool isGitRepo( const boost::filesystem::path& gitDirectory )
{
    std::ostringstream os;
    os << "git -C " << gitDirectory.string() << " rev-parse";

    std::string strOutput, strError;
    const auto  returnValue = common::runProcess( os.str(), strOutput, strError );

    if( returnValue == EXIT_SUCCESS && strError.empty() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector< std::string > getFileGitHashes( const boost::filesystem::path& gitDirectory,
                                             const boost::filesystem::path& filePath )
{
    using namespace std::string_literals;
    static const std::string cmdArgs = R"(--pretty=tformat:"%H" )";

    const auto currentPath = boost::filesystem::current_path();

    VERIFY_RTE( boost::filesystem::exists( gitDirectory ) );
    VERIFY_RTE( boost::filesystem::exists( filePath ) );

    const auto relPath = boost::filesystem::relative( filePath, gitDirectory );

    std::vector< std::string > result;

    boost::filesystem::current_path( gitDirectory );
    try
    {
        VERIFY_RTE( boost::filesystem::exists( relPath ) );

        std::ostringstream os;
        os << "git log " << cmdArgs << relPath.string();

        std::string strOutput, strError;

        const auto returnValue = common::runProcess( os.str(), strOutput, strError );

        VERIFY_RTE_MSG(
            ( returnValue == EXIT_SUCCESS ) && strError.empty(), "Error performing git log command: " << strError );

        // command generates quotes for each line
        result = common::simpleTokenise( strOutput, "\"\n" );
    }
    catch( std::exception& ex )
    {
        boost::filesystem::current_path( currentPath );
        THROW_RTE( ex.what() );
    }
    boost::filesystem::current_path( currentPath );

    return result;
}

std::string getGitFile( const boost::filesystem::path& gitDirectory,
                        const boost::filesystem::path& filePath,
                        const std::string&             strGitHash )
{
    VERIFY_RTE( boost::filesystem::exists( gitDirectory ) );
    VERIFY_RTE( boost::filesystem::exists( filePath ) );

    const auto currentPath = boost::filesystem::current_path();
    const auto relPath     = boost::filesystem::relative( filePath, gitDirectory );

    std::string strContents;

    boost::filesystem::current_path( gitDirectory );
    try
    {
        VERIFY_RTE( boost::filesystem::exists( relPath ) );

        std::ostringstream os;
        os << "git show " << strGitHash << ":" << relPath.string();

        std::string strOutput, strError;

        const auto returnValue = common::runProcess( os.str(), strOutput, strError );

        VERIFY_RTE_MSG(
            ( returnValue == EXIT_SUCCESS ) && strError.empty(), "Error performing git log command: " << strError );

        // command generates quotes for each line
        strContents = strOutput;
    }
    catch( std::exception& ex )
    {
        boost::filesystem::current_path( currentPath );
        THROW_RTE( ex.what() );
    }
    boost::filesystem::current_path( currentPath );

    return strContents;
}

void commit( const boost::filesystem::path& gitDirectory, const std::string& strMsg )
{
    VERIFY_RTE( boost::filesystem::exists( gitDirectory ) );

    const auto currentPath = boost::filesystem::current_path();
    boost::filesystem::current_path( gitDirectory );
    try
    {
        std::ostringstream os;
        os << "git commit . -m \"" << strMsg << "\"";

        std::string strOutput, strError;

        const auto returnValue = common::runProcess( os.str(), strOutput, strError );

        VERIFY_RTE_MSG(
            ( returnValue == EXIT_SUCCESS ) && strError.empty(), "Error performing git commit command: " << strError );
    }
    catch( std::exception& ex )
    {
        boost::filesystem::current_path( currentPath );
        THROW_RTE( ex.what() );
    }
    boost::filesystem::current_path( currentPath );
}

} // namespace git