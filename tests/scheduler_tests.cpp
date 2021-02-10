
#include "common/task.hpp"
#include "common/scheduler.hpp"
#include "common/assert_verify.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <chrono>

namespace
{
    class TestTask : public task::Task
    {
        std::string m_strName;
    public:
        TestTask( const std::string& strName, const task::Task::RawPtrSet& dependencies )
            :   Task( dependencies ),
                m_strName( strName )
        {
            
        }
        
        virtual void run( task::Progress& progress )
        {
            progress.start( m_strName, m_strName, m_strName );
            
            using namespace std::chrono_literals;
            std::this_thread::sleep_for( 1ms );
            
            progress.msg( "hello" );
            
            progress.setState( task::Status::eSucceeded );
        }
    };
    
    class FailTask : public task::Task
    {
        std::string m_strName;
    public:
        FailTask( const std::string& strName, const task::Task::RawPtrSet& dependencies )
            :   Task( dependencies ),
                m_strName( strName )
        {
            
        }
        
        virtual void run( task::Progress& progress )
        {
            progress.start( m_strName, m_strName, m_strName );
            
            using namespace std::chrono_literals;
            std::this_thread::sleep_for( 1ms );
            
            throw std::runtime_error( "fail" );
        }
    };
        
    task::Task::PtrVector createGoodSchedule()
    {
        using namespace task;
        Task::Ptr pTask1( new TestTask( "test1", Task::RawPtrSet{} ) );
        Task::Ptr pTask2( new TestTask( "test2", Task::RawPtrSet{ pTask1.get() } ) );
        Task::Ptr pTask3( new TestTask( "test3", Task::RawPtrSet{ pTask1.get() } ) );
        Task::Ptr pTask4( new TestTask( "test4", Task::RawPtrSet{ pTask3.get()} ) );
        Task::Ptr pTask5( new TestTask( "test5", Task::RawPtrSet{ pTask2.get(), pTask3.get()} ) );
        Task::Ptr pTask6( new TestTask( "test6", Task::RawPtrSet{ pTask4.get() } ) );
        Task::Ptr pTask7( new TestTask( "test7", Task::RawPtrSet{ pTask5.get() } ) );
        Task::Ptr pTask8( new TestTask( "test8", Task::RawPtrSet{} ) );
        
        Task::PtrVector tasks{ pTask1, pTask2, pTask3, pTask4, pTask5, pTask6, pTask7, pTask8 };
        return tasks;
    }


    task::Task::PtrVector createBadSchedule()
    {
        using namespace task;
        Task::Ptr pTask1( new TestTask( "test1", Task::RawPtrSet{} ) );
        Task::Ptr pTask2( new TestTask( "test2", Task::RawPtrSet{ pTask1.get() } ) );
        Task::Ptr pTask3( new FailTask( "test3", Task::RawPtrSet{ pTask2.get() } ) );
        Task::Ptr pTask4( new TestTask( "test4", Task::RawPtrSet{ pTask1.get(), pTask3.get() } ) );
        Task::Ptr pTask5( new TestTask( "test5", Task::RawPtrSet{ } ) );
        
        Task::PtrVector tasks{ pTask1, pTask2, pTask3, pTask4, pTask5 };
        return tasks;
    }
    
    static auto getKeepAliveTime()
    {
        using namespace std::chrono_literals;
        return 2ms;
    }
    
}

TEST( Scheduler, Empty )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks;
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    task::StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    Scheduler::Run::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    ASSERT_TRUE( pRun->wait() );
}

TEST( Scheduler, Basic )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks = createGoodSchedule();
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    task::StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    Scheduler::Run::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    //std::thread testSharedFuture
    //(
    //    [ pRun ]() 
    //    { 
    //        if( !pRun->wait() )
    //            throw std::runtime_error( "failed" );
    //    } 
    //);
    //testSharedFuture.join();
    
    ASSERT_TRUE( pRun->wait() );
    
    
    //{
    //    while( !fifo.empty() )
    //    {
    //        const task::TaskProgress progress = fifo.pop();
    //        std::cout << progress << std::endl;
    //    }
    //}
}

TEST( Scheduler, BasicRun )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks = createGoodSchedule();
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    run( pSchedule, osLog );
}

TEST( Scheduler, BasicFail1 )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::PtrVector tasks = createBadSchedule();
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    Scheduler::Run::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    ASSERT_THROW( pRun->wait(), std::runtime_error );
    
    {
        while( !fifo.empty() )
        {
            std::cout << fifo.pop() << std::endl;
        }
    }
}

TEST( Scheduler, BasicFail2 )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::PtrVector tasks1 = createGoodSchedule();
    Task::PtrVector tasks2 = createBadSchedule();
    Task::PtrVector tasks3 = createBadSchedule();
    
    Task::PtrVector tasks;
    std::copy( tasks1.begin(), tasks1.end(), std::back_inserter( tasks ) );
    std::copy( tasks2.begin(), tasks2.end(), std::back_inserter( tasks ) );
    std::copy( tasks3.begin(), tasks3.end(), std::back_inserter( tasks ) );
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    Scheduler::Run::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    ASSERT_THROW( pRun->wait(), std::runtime_error );
    
    {
        while( !fifo.empty() )
        {
            std::cout << fifo.pop() << std::endl;
        }
    }
}

TEST( Scheduler, MultiSchedule )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::PtrVector tasks1 = createGoodSchedule();
    Task::PtrVector tasks2 = createGoodSchedule();
    Task::PtrVector tasks3 = createGoodSchedule();
    
    Schedule::Ptr pSchedule1( new Schedule( tasks1 ) );
    Schedule::Ptr pSchedule2( new Schedule( tasks2 ) );
    Schedule::Ptr pSchedule3( new Schedule( tasks3 ) );
    
    StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    
    int schedule1, schedule2, schedule3;
    
    Scheduler::Run::Ptr pRun1 = scheduler.run( &schedule1, pSchedule1 );
    Scheduler::Run::Ptr pRun2 = scheduler.run( &schedule2, pSchedule2 );
    Scheduler::Run::Ptr pRun3 = scheduler.run( &schedule3, pSchedule3 );

    ASSERT_TRUE( pRun1->wait() );
    ASSERT_TRUE( pRun2->wait() );
    ASSERT_TRUE( pRun3->wait() );
    
    
}

TEST( Scheduler, ReSchedule )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::PtrVector tasks1 = createGoodSchedule();
    Schedule::Ptr pSchedule1( new Schedule( tasks1 ) );
    
    StatusFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, getKeepAliveTime() );
    
    const int schedule1 = 0;
    
    std::vector< Scheduler::Run::Ptr > runs;
    for( int i = 0; i < 100; ++i )
    {
        Scheduler::Run::Ptr pRun = scheduler.run( &schedule1, pSchedule1 );
        runs.push_back( pRun );
    }
    
    Scheduler::Run::Ptr pFinalRun = scheduler.run( &schedule1, pSchedule1 );
    
    for( Scheduler::Run::Ptr pRun : runs )
    {
        ASSERT_TRUE( !pRun->wait() );
    }

    ASSERT_TRUE( pFinalRun->wait() );
    
    
}
