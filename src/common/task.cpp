
#include "common/task.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include "boost/timer/timer.hpp"

#include <functional>
#include <thread>
#include <chrono>
#include <iomanip>

namespace task
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
NotifiedTaskProgress::NotifiedTaskProgress( TaskProgressFIFO& fifo )
    :   m_fifo( fifo )
{
    m_timer.start();
}

void NotifiedTaskProgress::setTaskInfo( const std::string& strTaskName, 
        const std::string& strSource, const std::string& strTarget )
{
    m_progress.m_strTaskName        = strTaskName;
    m_progress.m_optSourceString    = strSource;
    m_progress.m_optTargetString    = strTarget;
    m_progress.m_elapsed            = getElapsedTime();
    m_fifo.push( m_progress );
}

void NotifiedTaskProgress::setTaskInfo( const std::string& strTaskName, 
        const boost::filesystem::path& fileSource, const std::string& fileTarget )
{
    m_progress.m_strTaskName        = strTaskName;
    m_progress.m_optSourcePath      = fileSource;
    m_progress.m_optTargetPath      = fileTarget;
    m_progress.m_elapsed            = getElapsedTime();
    m_fifo.push( m_progress );
}

void NotifiedTaskProgress::cached( bool bCached )                        
{
    m_progress.m_bCached = bCached;
    m_progress.m_elapsed            = getElapsedTime();
    if( bCached )
    {
        m_timer.stop();
    }
    m_fifo.push( m_progress );
}
void NotifiedTaskProgress::complete( bool bComplete )                    
{
    m_progress.m_bComplete = bComplete;
    m_progress.m_elapsed            = getElapsedTime();
    m_timer.stop();
    m_fifo.push( m_progress );
}

void NotifiedTaskProgress::msg( const std::string& strMsg )              
{
    m_progress.m_elapsed            = getElapsedTime();
    m_progress.m_msgs.push_back( strMsg );
    m_fifo.push( m_progress );
}

std::string NotifiedTaskProgress::getElapsedTime() const
{
    return m_timer.format( 3, "%w" );
}
        
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Task::Task( const RawPtrSet& dependencies )
    :   m_dependencies( dependencies )
{
    
}
    
Task::~Task()
{
    
}
    
bool Task::isReady( const RawPtrSet& finished )
{
    for( RawPtr pTask : m_dependencies )
    {
        if( !finished.count( pTask ) )
            return false;
    }
    return true;
}
    

}



