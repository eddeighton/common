
#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"
#include "common/terminal.hpp"

#include "boost/current_function.hpp"

#include <functional>
#include <iostream>
#include <stdexcept>

namespace task
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Schedule::Schedule( const Task::PtrVector& tasks )
    : m_tasks( tasks )
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scheduler::Run::Run( Scheduler& scheduler, Owner pOwner, Schedule::Ptr pSchedule )
    : m_scheduler( scheduler )
    , m_pOwner( pOwner )
    , m_pSchedule( pSchedule )
    , m_bStarted( false )
    , m_bCancelled( false )
    , m_bFinished( false )
    , m_bComplete( false )
    , m_future( m_promise.get_future() )
{
    for( Task::Ptr pTask : m_pSchedule->getTasks() )
    {
        m_pending.insert( pTask.get() );
    }
}

bool Scheduler::Run::isCancelled() const
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );
    return m_bCancelled;
}

bool Scheduler::Run::wait()
{
    if( m_future.valid() )
    {
        return m_future.get();
    }
    else
    {
        return false;
    }
}

void Scheduler::Run::complete()
{
    Run::Ptr pThis = shared_from_this();
    {
        std::lock_guard< std::recursive_mutex > lock( m_mutex );
        if( !m_bComplete )
        {
            m_bComplete = true;
            m_scheduler.OnRunComplete( pThis );
        }
    }
}

void Scheduler::Run::cancel()
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );
    if( !m_bCancelled )
    {
        m_pending.clear();
        m_bCancelled = true;

        if( m_bStarted )
        {
            complete();
        }
        else
        {
            finished();
        }
    }
}

void Scheduler::Run::finished()
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );
    if( !m_bFinished )
    {
        m_bFinished = true;
        if( m_pExceptionPtr.has_value() )
        {
            m_promise.set_exception( m_pExceptionPtr.value() );
        }
        else
        {
            m_promise.set_value( !m_bCancelled );
        }
    }
}

void Scheduler::Run::runTask( Task::RawPtr pTask )
{
    Progress progress( m_scheduler.m_fifo, m_pOwner );

    try
    {
        pTask->run( progress );

        if( !progress.isFinished() )
        {
            std::lock_guard< std::recursive_mutex > lock( m_mutex );
            cancel();
            return;
        }

        bool bRemaining = true;
        {
            std::lock_guard< std::recursive_mutex > lock( m_mutex );

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
            m_scheduler.m_queue.post( std::bind( &Scheduler::Run::next, shared_from_this() ) );
        }
        else
        {
            complete();
        }
    }
    catch( std::exception& ex )
    {
        pTask->failed( progress );

        std::lock_guard< std::recursive_mutex > lock( m_mutex );
        m_pExceptionPtr = std::current_exception();
        cancel();
    }
}

void Scheduler::Run::start()
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );
    if( !m_bCancelled )
    {
        if( !m_pending.empty() )
        {
            next();
        }
        else
        {
            complete();
        }
    }
}

void Scheduler::Run::next()
{
    Task::RawPtrSet ready;
    {
        std::lock_guard< std::recursive_mutex > lock( m_mutex );
        if( !m_bCancelled )
        {
            for( Task::RawPtrSet::iterator i = m_pending.begin(), iEnd = m_pending.end(); i != iEnd; ++i )
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

    // schedule tasks
    if( !ready.empty() )
    {
        for( Task::RawPtr pTask : ready )
        {
            m_scheduler.m_queue.post( std::bind( &Scheduler::Run::runTask, shared_from_this(), pTask ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scheduler::Scheduler( StatusFIFO& fifo, std::chrono::milliseconds keepAliveRate,
                      std::optional< unsigned int > maxThreads )
    : m_fifo( fifo )
    , m_bStop( false )
    , m_keepAliveRate( keepAliveRate )
    , m_keepAliveTimer( m_queue, keepAliveRate )
{
    {
        using namespace std::placeholders;
        m_keepAliveTimer.async_wait( std::bind( &Scheduler::OnKeepAlive, this, _1 ) );
    }

    const unsigned int nMaxThreads = maxThreads.has_value() ? maxThreads.value() : std::thread::hardware_concurrency();
    VERIFY_RTE( nMaxThreads > 0U );

    boost::asio::io_context* pQueue = &m_queue;
    for( auto i = 0U; i < nMaxThreads; ++i )
    {
        m_threads.emplace_back( [ pQueue ]() { pQueue->run(); } );
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
        std::lock_guard< std::recursive_mutex > lock( m_mutex );
        bStopped = m_bStop;
    }

    if( !bStopped && ec.value() == boost::system::errc::success )
    {
        m_keepAliveTimer.expires_at( m_keepAliveTimer.expiry() + m_keepAliveRate );
        using namespace std::placeholders;
        m_keepAliveTimer.async_wait( std::bind( &Scheduler::OnKeepAlive, this, _1 ) );
    }
}

void Scheduler::OnRunComplete( Run::Ptr pRun )
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );

    Run::Owner pRunOwner = pRun->getOwner();

    ScheduleRunMap::iterator iFind = m_runs.find( pRunOwner );
    VERIFY_RTE_MSG( iFind != m_runs.end(), "Error in scheduler" );
    VERIFY_RTE_MSG( iFind->second == pRun, "Error in scheduler" );

    m_runs.erase( iFind );
    pRun->finished();

    ScheduleRunMap::iterator iFindPending = m_pending.find( pRunOwner );
    if( iFindPending != m_pending.end() )
    {
        Run::Ptr pPendingRun = iFindPending->second;
        m_pending.erase( iFindPending );

        auto ibResult = m_runs.insert( std::make_pair( pRunOwner, pPendingRun ) );
        VERIFY_RTE( ibResult.second );
        pPendingRun->m_bStarted = true;
        m_queue.post( std::bind( &Scheduler::Run::start, pPendingRun ) );
    }
}

Scheduler::Run::Ptr Scheduler::run( Run::Owner pOwner, Schedule::Ptr pSchedule )
{
    Run::Ptr pScheduleRun( new Run( *this, pOwner, pSchedule ) );

    Run::Ptr pOldRun;
    {
        std::lock_guard< std::recursive_mutex > lock( m_mutex );

        ScheduleRunMap::iterator iFind = m_runs.find( pOwner );
        if( iFind != m_runs.end() )
        {
            // overwrite any existing pending schedule run
            ScheduleRunMap::iterator iFindPending = m_pending.find( pOwner );
            if( iFindPending != m_pending.end() )
            {
                Run::Ptr pPendingRun = iFindPending->second;
                m_pending.erase( iFindPending );
                pPendingRun->cancel();
            }

            m_pending.insert( std::make_pair( pOwner, pScheduleRun ) );

            // careful - DO NOT lock the run here while have outer lock
            pOldRun = iFind->second;
        }
        else
        {
            auto ibResult = m_runs.insert( std::make_pair( pOwner, pScheduleRun ) );
            VERIFY_RTE( ibResult.second );
            pScheduleRun->m_bStarted = true;
            m_queue.post( std::bind( &Scheduler::Run::start, pScheduleRun ) );
        }
    }

    if( pOldRun )
    {
        if( !pOldRun->isCancelled() )
        {
            pOldRun->cancel();
        }
    }

    return pScheduleRun;
}

void Scheduler::stop()
{
    std::lock_guard< std::recursive_mutex > lock( m_mutex );
    m_bStop = true;
}

void run( task::Schedule::Ptr pSchedule, std::ostream& )
{
    task::StatusFIFO fifo;
    int              owner = 0;
    Scheduler        scheduler( fifo, Scheduler::getDefaultAliveRate(), std::thread::hardware_concurrency() - 1 );
    try
    {
        Scheduler::Run::Ptr pRun = scheduler.run( &owner, pSchedule );

        pRun->wait();
        if( pRun->isCancelled() )
        {
            throw std::runtime_error( "Task failed" );
        }
    }
    catch( std::exception& ex )
    {
        throw;
    }
}
} // namespace task