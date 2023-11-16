
#include "common/processID.hpp"

#include <boost/process.hpp>

#include <string>

namespace common
{

namespace
{
static std::string g_processDesc;
}

ProcessID::ProcessID( int szPID, const std::string& strDesc )
    : m_pid( szPID )
    , m_description( strDesc )
{
}

ProcessID ProcessID::get() { return ProcessID{ boost::this_process::get_id(), g_processDesc }; }

void ProcessID::setDescription( const char* pszDescToCopy ) { g_processDesc = pszDescToCopy; }

std::ostream& operator<<( std::ostream& os, common::ProcessID processID )
{
    return os << "PID: " << processID.getPID() << " " << processID.getDescription();
}

} // namespace common
