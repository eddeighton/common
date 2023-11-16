
#ifndef PROCESSID_8_12_2016
#define PROCESSID_8_12_2016

#include <cstddef>
#include <ostream>

namespace common
{
class ProcessID
{
    ProcessID( int szPID, const std::string& strDesc );

public:
    static ProcessID get();
    static void      setDescription( const char* pszDescToCopy );

    int         getPID() const { return m_pid; }
    const char* getDescription() const { return m_description.c_str(); }

private:
    int                m_pid = 0U;
    const std::string& m_description;
};

std::ostream& operator<<( std::ostream& os, common::ProcessID processID );

} // namespace common


#endif // PROCESSID_8_12_2016

