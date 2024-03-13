
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

#ifndef GUARD_2024_March_11_git
#define GUARD_2024_March_11_git

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace git
{

bool isGitRepo( const boost::filesystem::path& gitDirectory );

std::vector< std::string > getFileGitHashes( const boost::filesystem::path& gitDirectory,
                                             const boost::filesystem::path& filePath );

std::string getGitFile( const boost::filesystem::path& gitDirectory,
                        const boost::filesystem::path& filePath,
                        const std::string&             strGitHash );

void commit( const boost::filesystem::path& gitDirectory, const std::string& strMsg );

} // namespace git

#endif // GUARD_2024_March_11_git
