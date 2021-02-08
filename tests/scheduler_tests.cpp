
#include "common/task.hpp"
#include "common/scheduler.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <chrono>

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
        taskProgress.complete( true );
    }
};

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
    
    return os;
}


TEST( Scheduler, Basic )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::Ptr pTask1( new TestTask( "test1", Task::RawPtrSet{} ) );
    Task::Ptr pTask2( new TestTask( "test2", Task::RawPtrSet{ pTask1.get() } ) );
    Task::Ptr pTask3( new TestTask( "test3", Task::RawPtrSet{ pTask1.get() } ) );
    Task::Ptr pTask4( new TestTask( "test4", Task::RawPtrSet{ pTask3.get()} ) );
    Task::Ptr pTask5( new TestTask( "test5", Task::RawPtrSet{ pTask2.get(), pTask3.get()} ) );
    Task::Ptr pTask6( new TestTask( "test6", Task::RawPtrSet{} ) );
    Task::Ptr pTask7( new TestTask( "test7", Task::RawPtrSet{ pTask5.get() } ) );
    Task::Ptr pTask8( new TestTask( "test8", Task::RawPtrSet{} ) );
    
    Task::PtrVector tasks{ pTask1, pTask2, pTask3, pTask4, pTask5, pTask6, pTask7, pTask8 };
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    task::TaskProgressFIFO fifo;
    
    Scheduler scheduler( fifo, ( std::optional< unsigned int >() ) );
    scheduler.run( nullptr, pSchedule );
    
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for( 1ms );
        while( !fifo.empty() )
        {
            const task::TaskProgress progress = fifo.pop();
            std::cout << progress << std::endl;
            std::this_thread::sleep_for( 1ms );
        }
    }
    
    scheduler.stop();
    
}
