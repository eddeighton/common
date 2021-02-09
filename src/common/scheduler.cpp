
#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"

#include "boost/current_function.hpp"

#include <functional>
#include <iostream>

namespace task
{

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    Schedule::Schedule( const Task::PtrVector& tasks )
        :   m_tasks( tasks )
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    Scheduler::ScheduleRun::ScheduleRun( Scheduler& scheduler, ScheduleOwner pOwner, Schedule::Ptr pSchedule )
        :   m_scheduler( scheduler ),
            m_pOwner( pOwner ),
            m_pSchedule( pSchedule ),
            m_bCancelled( false ),
            m_future( m_promise.get_future() )
    {
        for( Task::Ptr pTask : m_pSchedule->getTasks() )
        {
            m_pending.insert( pTask.get() );
        }
    }
    
    bool Scheduler::ScheduleRun::wait()
    {
        if( m_future.valid() )
        {
            return m_future.get();
            //std::shared_future< bool > sharedFuture = m_future.share();
            //return sharedFuture.get();
        }
        else
        {
            return false;
        }
    }
    
    void Scheduler::ScheduleRun::cancel()
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_pending.clear();
        m_bCancelled = true;
        
        if( m_pending.empty() && m_active.empty() )
        {
            m_scheduler.OnRunComplete( shared_from_this() );
        }
    }
    
    void Scheduler::ScheduleRun::finished()
    {
        if( m_pExceptionPtr.has_value() )
        {
            m_promise.set_exception( m_pExceptionPtr.value() );
        }
        else
        {
            m_promise.set_value( !m_bCancelled );
        }
    }
    
    void Scheduler::ScheduleRun::cancelWithoutStart()
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_pending.clear();
        m_bCancelled = true;
        finished();
    }

    void Scheduler::ScheduleRun::runTask( Task::RawPtr pTask )
    {
        NotifiedTaskProgress progress( m_scheduler.m_fifo );
                            
        try
        {
            pTask->run( progress );
            
            bool bRemaining = true;
            bool bCancelled = false;
            {
                std::lock_guard< std::mutex > lock( m_mutex );
                
                bCancelled = m_bCancelled;
                Task::RawPtrSet::iterator iFind = m_active.find( pTask );
                VERIFY_RTE_MSG( iFind != m_active.end(), "Failed to find task in active set" );
                m_active.erase( iFind );
                m_finished.insert( pTask );
                
                if( m_pending.empty() && m_active.empty() )
                {
                    bRemaining = false;
                }
            }
            
            if( bRemaining )
            {
                m_scheduler.m_queue.post( 
                    std::bind( &Scheduler::ScheduleRun::progress, this ) );
            }
            else
            {
                m_scheduler.OnRunComplete( shared_from_this() );
            }
        }
        catch( std::exception& ex )
        {
            progress.complete( false );
            
            std::lock_guard< std::mutex > lock( m_mutex );
            
            Task::RawPtrSet::iterator iFind = m_active.find( pTask );
            VERIFY_RTE_MSG( iFind != m_active.end(), "Failed to find task in active set" );
            m_active.erase( iFind );
            
            m_pending.clear();
            m_bCancelled = true;
            m_pExceptionPtr = std::current_exception();
            
            if( m_pending.empty() && m_active.empty() )
            {
                m_scheduler.OnRunComplete( shared_from_this() );
            }
        }
    }
    
    void Scheduler::ScheduleRun::progress()
    {
        Task::RawPtrSet ready;
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            {
                for( Task::RawPtrSet::iterator 
                    i = m_pending.begin(),
                    iEnd = m_pending.end(); i!=iEnd; ++i )
                {
                    Task::RawPtr pTask = *i;
                    if( pTask->isReady( m_finished ) )
                    {
                        ready.insert( pTask );
                        m_active.insert( pTask );
                    }
                }
                for( Task::RawPtr pTask : ready )
                {
                    m_pending.erase( pTask );
                }
            }
        }
        
        //schedule tasks
        if( !ready.empty() )
        {
            for( Task::RawPtr pTask : ready )
            {
                m_scheduler.m_queue.post(
                    std::bind( &Scheduler::ScheduleRun::runTask, this, pTask ) );
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    Scheduler::Scheduler( TaskProgressFIFO& fifo, 
                    std::chrono::milliseconds keepAliveRate, 
                    std::optional< unsigned int > maxThreads )
		:	m_fifo( fifo ),
            m_bStop( false ),
			m_keepAliveRate( keepAliveRate ),
            m_keepAliveTimer( m_queue, keepAliveRate )
    {
		{
			using namespace std::placeholders;
			m_keepAliveTimer.async_wait( 
                std::bind( &Scheduler::OnKeepAlive, this, _1 ) );
		}
        
        const unsigned int nMaxThreads = 
            maxThreads.has_value() ? 
                maxThreads.value() : 
                std::thread::hardware_concurrency();
        VERIFY_RTE( nMaxThreads > 0U );
            
		boost::asio::io_context* pQueue = &m_queue;
        for( int i = 0; i < nMaxThreads; ++i )
        {
            m_threads.push_back( 
                std::thread( [ pQueue ](){ pQueue->run();} ) );
        }
    }
    
    Scheduler::~Scheduler()
    {
        stop();
        for( std::thread& thread : m_threads )
        {
            thread.join();
        }
    }
    
	void Scheduler::OnKeepAlive( const boost::system::error_code& ec )
	{
        bool bStopped = false;
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            bStopped = m_bStop;
        }
        
		if( !bStopped && ec.value() == boost::system::errc::success )
		{
			m_keepAliveTimer.expires_at( m_keepAliveTimer.expiry() + m_keepAliveRate );
			using namespace std::placeholders;
			m_keepAliveTimer.async_wait( std::bind( &Scheduler::OnKeepAlive, this, _1 ) );
		}
	}
    
    void Scheduler::OnRunComplete( ScheduleRun::Ptr pRun )
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        
        ScheduleOwner pRunOwnder = pRun->getOwner();
        
        ScheduleRunMap::iterator iFind = m_runs.find( pRunOwnder );
        if( iFind != m_runs.end() )
        {
            VERIFY_RTE( iFind->second == pRun );
            m_runs.erase( iFind );
            pRun->finished();
            
            ScheduleRunMap::iterator iFindPending = m_pending.find( pRunOwnder );
            if( iFindPending != m_pending.end() )
            {
                ScheduleRun::Ptr pPendingRun = iFindPending->second;
                m_pending.erase( iFindPending );
                m_runs.insert( std::make_pair( pRunOwnder, pPendingRun ) );
                m_queue.post( std::bind( &Scheduler::ScheduleRun::progress, pPendingRun ) );
            }
        }
        else
        {
            THROW_RTE( "Error in schedule management" );
        }
    }
    
    Scheduler::ScheduleRun::Ptr Scheduler::run( ScheduleOwner pOwner, Schedule::Ptr pSchedule )
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        
        ScheduleRunMap::iterator iFind = m_runs.find( pOwner );
        if( iFind != m_runs.end() )
        {
            ScheduleRun::Ptr pExistingRun = iFind->second;
            ScheduleRun::Ptr pNewScheduleRun( new ScheduleRun( *this, pOwner, pSchedule ) );
            
            //overwrite any existing pending schedule run
            ScheduleRunMap::iterator iFindPending = m_pending.find( pOwner );
            if( iFindPending != m_pending.end() )
            {
                ScheduleRun::Ptr pPendingRun = iFindPending->second;
                m_pending.erase( iFindPending );
                pPendingRun->cancelWithoutStart();
            }
            
            m_pending[ pOwner ] = pNewScheduleRun;
            
            pExistingRun->cancel(); //will call OnRunComplete one active tasks finished
            
            return pNewScheduleRun;
        }
        else
        {
            ScheduleRun::Ptr pScheduleRun( new ScheduleRun( *this, pOwner, pSchedule ) );
            
            m_runs.insert( std::make_pair( pOwner, pScheduleRun ) );
            
            m_queue.post( std::bind( &Scheduler::ScheduleRun::progress, pScheduleRun ) );
            
            return pScheduleRun;
        }
    }
    
    void Scheduler::stop()
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_bStop = true;
    }
    
}