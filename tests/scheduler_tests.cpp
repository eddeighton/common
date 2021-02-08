
#include "common/task.hpp"
#include "common/scheduler.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <chrono>

class TestTask : public task::Task
{
    const char* m_pszStr;
public:
    TestTask( const char* pszStr, std::ostream& log, const task::Task::RawPtrSet& dependencies )
        :   Task( log, dependencies ),
            m_pszStr( pszStr )
    {
        
    }
    
    virtual void run()
    {
        //std::cout << "TestTask::run: " << m_pszStr << std::endl;
    }
};


TEST( Scheduler, Basic )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::Ptr pTask1( new TestTask( "test1", osLog, Task::RawPtrSet{} ) );
    Task::Ptr pTask2( new TestTask( "test2", osLog, Task::RawPtrSet{ pTask1.get() } ) );
    Task::Ptr pTask3( new TestTask( "test3", osLog, Task::RawPtrSet{} ) );
    
    Task::PtrVector tasks{ pTask1, pTask2, pTask3 };
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    
    Scheduler scheduler( ( std::optional< unsigned int >() ) );
    scheduler.run( nullptr, pSchedule );
    
    //{
    //    using namespace std::chrono_literals;
    //    std::this_thread::sleep_for( 1000ms );
    //}
    scheduler.stop();
    
}


TEST( Scheduler, Basic2 )
{
    using namespace task;
    
    std::ostringstream osLog;
    
    Task::Ptr pTask1( new TestTask( "test1", osLog, Task::RawPtrSet{} ) );
    Task::Ptr pTask2( new TestTask( "test2", osLog, Task::RawPtrSet{ pTask1.get() } ) );
    Task::Ptr pTask3( new TestTask( "test3", osLog, Task::RawPtrSet{ pTask1.get() } ) );
    Task::Ptr pTask4( new TestTask( "test3", osLog, Task::RawPtrSet{ pTask3.get()} ) );
    Task::Ptr pTask5( new TestTask( "test3", osLog, Task::RawPtrSet{ pTask2.get(), pTask3.get()} ) );
    Task::Ptr pTask6( new TestTask( "test3", osLog, Task::RawPtrSet{} ) );
    Task::Ptr pTask7( new TestTask( "test3", osLog, Task::RawPtrSet{ pTask5.get() } ) );
    Task::Ptr pTask8( new TestTask( "test3", osLog, Task::RawPtrSet{} ) );
    
    Task::PtrVector tasks{ pTask1, pTask2, pTask3 };
    
    Schedule::Ptr pSchedule( new Schedule( tasks ) );
    
    
    Scheduler scheduler( ( std::optional< unsigned int >() ) );
    scheduler.run( nullptr, pSchedule );
    
    //{
    //    using namespace std::chrono_literals;
    //    std::this_thread::sleep_for( 1000ms );
    //}
    scheduler.stop();
    
}
