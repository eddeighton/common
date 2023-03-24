
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

std::ostream& operator<<( std::ostream& os, const Status::Subject& subject )
{
    if( std::holds_alternative< std::string >( subject ) )
    {
        os << std::get< std::string >( subject );
    }
    else if( std::holds_alternative< boost::filesystem::path >( subject ) )
    {
        os << std::get< boost::filesystem::path >( subject ).generic_string();
    }
    else
    {
        THROW_RTE( "Unknown subject type" );
    }
    
    return os;
}

std::ostream& operator<<( std::ostream& os, const Status& status )
{
    switch( status.m_state )
    {                                                                                             
        case Status::ePending   : os << common::COLOUR_WHITE_BEGIN  << "PENDING   " ;      break;
        case Status::eStarted   : os << common::COLOUR_WHITE_BEGIN  << "STARTED   " ;      break;
        case Status::eCached    : os << common::COLOUR_BLUE_BEGIN   << "CACHED    " ;      break;
        case Status::eSucceeded : os << common::COLOUR_GREEN_BEGIN  << "SUCCEEDED " ;      break;
        case Status::eFailed    : os << common::COLOUR_RED_BEGIN    << "FAILED    " ;      break;
        default:
            THROW_RTE( "Unknown task state" );
    }
    
    os << status.m_strTaskName << " ";
    
    if( status.m_source.has_value() && status.m_target.has_value() )
    {
        os << status.m_source.value() << " -> " << status.m_target.value();
    }
    else if( status.m_source.has_value() )
    {
        os << status.m_source.value() << " -> " << "";
    }
    else if( status.m_target.has_value() )
    {
        os << "" << " -> " << status.m_target.value();
    }
    
    if( status.m_elapsed.has_value() )
    {
        os << " : " << status.m_elapsed.value();
    }
    
    os << common::COLOUR_END;
    
    for( const std::string& strMsg : status.m_msgs )
    {
        os << "\n" << common::COLOUR_YELLOW_BEGIN << strMsg << common::COLOUR_END;
    }
    
    return os;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Progress::Progress( StatusFIFO& fifo, Status::Owner owner )
    :   m_fifo( fifo ),
        m_status( owner )
{
}

bool Progress::isFinished() const
{
    switch( m_status.m_state )
    {
        case Status::ePending   :
        case Status::eStarted   :
        case Status::eFailed    :
            return false;
        case Status::eCached    :
        case Status::eSucceeded :
            return true;
        default:
            THROW_RTE( "Unknown task state" );
    }
}

void Progress::start( const std::string& strTaskName, Status::Subject source, Status::Subject target )
{
    VERIFY_RTE( m_status.m_state == Status::ePending );
    m_status.m_state              = Status::eStarted;
    m_timer.start();
    
    m_status.m_strTaskName  = strTaskName;
    m_status.m_source       = source;
    m_status.m_target       = target;
    m_status.m_elapsed      = getElapsedTime();
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
            // m_fifo.push( m_status );
            break;
        default:
            THROW_RTE( "Unknown task state" );
    }
}

void Progress::cached()
{
    setState( Status::eCached );
}
void Progress::succeeded()
{
    setState( Status::eSucceeded );
}
void Progress::failed()
{
    setState( Status::eFailed );
}

void Progress::msg( const std::string& strMsg )              
{
    m_status.m_elapsed = getElapsedTime();
    m_status.m_msgs.push_back( strMsg );
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
    
void Task::failed( Progress& taskProgress )
{
    taskProgress.failed();
}

}



