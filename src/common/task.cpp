
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
Progress::Progress( StatusFIFO& fifo )
    :   m_fifo( fifo )
{
}

bool Progress::isFinished() const
{
    switch( m_status.m_state )
    {
        case Status::ePending   :
        case Status::eStarted   :
            return false;
        case Status::eCached    :
        case Status::eSucceeded :
        case Status::eFailed    :
            return true;
        default:
            THROW_RTE( "Unknown task state" );
    }
}

void Progress::start( const std::string& strTaskName, 
        const std::string& strSource, const std::string& strTarget )
{
    VERIFY_RTE( m_status.m_state == Status::ePending );
    m_status.m_state              = Status::eStarted;
    m_timer.start();
    
    m_status.m_strTaskName        = strTaskName;
    m_status.m_optSourceString    = strSource;
    m_status.m_optTargetString    = strTarget;
    m_status.m_elapsed            = getElapsedTime();
    m_fifo.push( m_status );
}

void Progress::start( const std::string& strTaskName, 
        const boost::filesystem::path& fileSource, const std::string& fileTarget )
{
    VERIFY_RTE( m_status.m_state == Status::ePending );
    m_status.m_state              = Status::eStarted;
    m_timer.start();
    
    m_status.m_strTaskName        = strTaskName;
    m_status.m_optSourcePath      = fileSource;
    m_status.m_optTargetPath      = fileTarget;
    m_status.m_elapsed            = getElapsedTime();
    m_fifo.push( m_status );
}

void Progress::setState( Status::State state )
{
    m_status.m_state = state;
    switch( state )
    {
        case Status::ePending   :
        case Status::eStarted   :
            break;
        case Status::eCached    :
        case Status::eSucceeded :
        case Status::eFailed    :
            m_status.m_elapsed = getElapsedTime();
            m_timer.stop();
            break;
        default:
            THROW_RTE( "Unknown task state" );
    }
    
    m_fifo.push( m_status );
}

void Progress::msg( const std::string& strMsg )              
{
    m_status.m_elapsed = getElapsedTime();
    m_status.m_msgs.push_back( strMsg );
    m_fifo.push( m_status );
}

std::string Progress::getElapsedTime() const
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



