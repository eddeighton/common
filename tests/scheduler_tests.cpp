
#include "common/task.hpp"
#include "common/scheduler.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <chrono>

namespace
{
    
    std::ostream& operator <<( std::ostream& os, const task::TaskProgress& taskProgress )
    {
        if( taskProgress.m_bCached.has_value() )
        {
            os << taskProgress.m_strTaskName << " cached: " << taskProgress.m_bCached.value();
        }
        else if( taskProgress.m_bComplete.has_value() )
        {
            os << taskProgress.m_strTaskName << " success: " << taskProgress.m_bComplete.value();
        }
        else
        {
            os << taskProgress.m_strTaskName << " started";
        }
        
        if( taskProgress.m_elapsed.has_value() )
        {
            os << " " << taskProgress.m_elapsed.value();
        }
        
        return os;
    }
    
    class TestTask : public task::Task
    {
        std::string m_strName;
    public:
        TestTask( const std::string& strName, const task::Task::RawPtrSet& dependencies )
            :   Task( dependencies ),
                m_strName( strName )
        {
            
        }
        
        virtual void run( task::NotifiedTaskProgress& taskProgress )
        {
            taskProgress.setTaskInfo( m_strName, m_strName, m_strName );
            
            using namespace std::chrono_literals;
            std::this_thread::sleep_for( 1ms );
            
            taskProgress.complete( true );
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
        
        virtual void run( task::NotifiedTaskProgress& taskProgress )
        {
            taskProgress.setTaskInfo( m_strName, m_strName, m_strName );
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
}


TEST( Scheduler, Basic )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks = createGoodSchedule();
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    task::TaskProgressFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, 10ms, ( std::optional< unsigned int >() ) );
    Scheduler::ScheduleRun::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    //std::thread testSharedFuture
    //(
    //    [ pRun ]() 
    //    { 
    //        if( !pRun->wait() )
    //            throw std::runtime_error( "failed" );
    //    } 
    //);
    
    ASSERT_TRUE( pRun->wait() );
    
    //testSharedFuture.join();
    
    //{
    //    while( !fifo.empty() )
    //    {
    //        const task::TaskProgress progress = fifo.pop();
    //        std::cout << progress << std::endl;
    //    }
    //}
}

TEST( Scheduler, BasicFail )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks = createBadSchedule();
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    task::TaskProgressFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, 10ms, ( std::optional< unsigned int >() ) );
    Scheduler::ScheduleRun::Ptr pRun = scheduler.run( nullptr, pSchedule );
    
    ASSERT_THROW( pRun->wait(), std::runtime_error );
    
    {
        while( !fifo.empty() )
        {
            const task::TaskProgress progress = fifo.pop();
            std::cout << progress << std::endl;
        }
    }
    
}


TEST( Scheduler, MultiSchedule )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    task::Task::PtrVector tasks1 = createGoodSchedule();
    task::Task::PtrVector tasks2 = createGoodSchedule();
    task::Task::PtrVector tasks3 = createGoodSchedule();
    
    Schedule::Ptr pSchedule1( new Schedule( tasks1 ) );
    Schedule::Ptr pSchedule2( new Schedule( tasks2 ) );
    Schedule::Ptr pSchedule3( new Schedule( tasks3 ) );
    
    task::TaskProgressFIFO fifo;
    
    using namespace std::chrono_literals;
    Scheduler scheduler( fifo, 10ms, ( std::optional< unsigned int >() ) );
    
    int schedule1, schedule2, schedule3;
    
    Scheduler::ScheduleRun::Ptr pRun1 = scheduler.run( &schedule1, pSchedule1 );
    Scheduler::ScheduleRun::Ptr pRun2 = scheduler.run( &schedule2, pSchedule2 );
    Scheduler::ScheduleRun::Ptr pRun3 = scheduler.run( &schedule3, pSchedule3 );

    ASSERT_TRUE( pRun1->wait() );
    ASSERT_TRUE( pRun2->wait() );
    ASSERT_TRUE( pRun3->wait() );
    
    
}
