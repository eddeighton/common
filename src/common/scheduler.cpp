
#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"

#include "boost/current_function.hpp"

#include <functional>
#include <iostream>

#define PRINTEXCEPTION_AND_ABORT( code )                                                  \
	DO_STUFF_AND_REQUIRE_SEMI_COLON(                                                      \
		try                                                                               \
		{                                                                                 \
			code                                                                          \
		}                                                                                 \
		catch( std::exception& ex )                                                       \
		{                                                                                 \
			std::cout << BOOST_CURRENT_FUNCTION " exception: " << ex.what() << std::endl; \
			std::abort();                                                                 \
		}                                                                                 \
		catch( ... )                                                                      \
		{                                                                                 \
			std::cout << BOOST_CURRENT_FUNCTION " Unknown exception" << std::endl;        \
		}                                                                                 \
	)

using namespace std::chrono_literals;

namespace 
{
	static const auto KEEP_ALIVE_RATE = 250ms;
}
    
namespace task
{

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    Schedule::Schedule( const Task::PtrVector& tasks )
        :   m_tasks( tasks )
    {
    }
    
    Schedule::~Schedule()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    
    Scheduler::ScheduleRun::ScheduleRun( Scheduler& scheduler, Schedule::Ptr pSchedule )
        :   m_scheduler( scheduler ),
            m_pSchedule( pSchedule )
    {
        for( Task::Ptr pTask : m_pSchedule->getTasks() )
        {
            m_pending.insert( pTask.get() );
        }
    }
    
    void Scheduler::ScheduleRun::taskCompleted( Task::RawPtr pTask )
    {
        //std::cout << "ScheduleRun::taskCompleted()" << std::endl;
        
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            m_finished.insert( pTask );
        }
        
		m_scheduler.m_queue.post( 
            std::bind( &Scheduler::ScheduleRun::progress, this ) );
    }
    
    void Scheduler::ScheduleRun::taskFailed( Task::RawPtr pTask )
    {
        //std::cout << "ScheduleRun::taskCompleted()" << std::endl;
        
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            m_pending.clear();
        }
    }
    
    
    void Scheduler::ScheduleRun::progress()
    {
        //std::cout << "ScheduleRun::run()" << std::endl;
        
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
            //std::cout << "ScheduleRun::run() incomplete: " << ready.size() << std::endl;
            
            TaskProgressFIFO* pFIFO = &m_scheduler.m_fifo;
            Scheduler::ScheduleRun* pRun = this;
            
            {
                for( Task::RawPtr pTask : ready )
                {
                    m_scheduler.m_queue.post
                    ( 
                        [ pFIFO, pRun, pTask ]()
                        {
                            NotifiedTaskProgress progress( *pFIFO );
                            
                            try
                            {
                                pTask->run( progress );
                                
                                pRun->m_scheduler.m_queue.post( 
                                    std::bind( &Scheduler::ScheduleRun::taskCompleted, pRun, pTask ) );
                            }
                            catch( std::exception& ex )
                            {
                                
                                pRun->m_scheduler.m_queue.post( 
                                    std::bind( &Scheduler::ScheduleRun::taskFailed, pRun, pTask ) );
                            }
                        }
                    );
                }
            }
        }
        else
        {
            //std::cout << "ScheduleRun::run() complete" << std::endl;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    Scheduler::Scheduler( TaskProgressFIFO& fifo, std::optional< unsigned int > maxThreads )
		:	m_fifo( fifo ),
            m_stop( false ),
			m_keepAliveTimer( m_queue, KEEP_ALIVE_RATE )
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
		if( !m_stop && ec.value() == boost::system::errc::success )
		{
			m_keepAliveTimer.expires_at( m_keepAliveTimer.expiry() + KEEP_ALIVE_RATE );
			using namespace std::placeholders;
			m_keepAliveTimer.async_wait( std::bind( &Scheduler::OnKeepAlive, this, _1 ) );
		}
        else
        {
            //std::cout << "ScheduleRun::OnKeepAlive() shutdown" << std::endl;
        }
	}
    
    void Scheduler::run( ScheduleOwner pOwner, Schedule::Ptr pSchedule )
    {
        ScheduleRun::Ptr pScheduleRun( new ScheduleRun( *this, pSchedule ) );
        
        m_runs.insert( std::make_pair( pOwner, pScheduleRun ) );
        
		m_queue.post( std::bind( &Scheduler::ScheduleRun::progress, pScheduleRun ) );
    }
    
    void Scheduler::stop()
    {
        m_stop = true;
    }
    
}