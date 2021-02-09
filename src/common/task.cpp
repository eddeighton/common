
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
    
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/*
Scheduler::Scheduler( std::ostream& log, const Task::PtrVector& tasks )
    :   m_log( log ),
        m_tasks( tasks )
{
    for( Task::Ptr pTask : m_tasks )
    {
        m_pending.insert( pTask.get() );
    }
}

void Scheduler::thread_run()
{
    bool bContinue = true;
    while( bContinue )
    {
        bool bRemainingTasks = false;
        Task::RawPtr pTaskTodo = nullptr;
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            
            if( !m_pending.empty() )
            {
                bRemainingTasks = true;
                //attempt to find ready task
                for( Task::RawPtr pTask : m_pending )
                {
                    if( pTask->isReady( m_finished ) )
                    {
                        m_pending.erase( pTask );
                        pTaskTodo = pTask;
                        break;
                    }
                }
            }
        }
        
        if( bRemainingTasks )
        {
            if( pTaskTodo )
            {
                try
                {
                    pTaskTodo->run();
                }
                catch( std::exception& ex )
                {
                    m_log << common::RED_BEGIN <<
                            "ERROR in task: " << pTaskTodo->getTaskInfo().taskName() << "\n" <<
                            pTaskTodo->getTaskInfo().source() << " -> " << pTaskTodo->getTaskInfo().target() << "\n" <<
                            ex.what() << common::RED_END << "\n" << std::endl;
                    throw;
                }
                
                std::lock_guard< std::mutex > lock( m_mutex );
                m_finished.insert( pTaskTodo );
                pTaskTodo->updateProgress();
            }
            else
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for( 0ms );
            }
        }
        else
        {
            bContinue = false;
        }
    }
}

void Scheduler::run( std::optional< unsigned int > maxThreads )
{
    std::vector< std::thread > threads;
    const unsigned int nMaxThreads = 
        maxThreads.has_value() ? maxThreads.value() : std::thread::hardware_concurrency();
    for( int i = 0; i < nMaxThreads; ++i )
    {
        threads.push_back( 
            std::thread( 
                std::bind( &Scheduler::thread_run, std::ref( *this ) ) ) );
    }
    
    for( std::thread& th : threads )
    {
        if( th.joinable() )
            th.join();
    }
}*/

}



