
#ifndef TASK_SCHEDULER_08_FEB_2021
#define TASK_SCHEDULER_08_FEB_2021

#include "task.hpp"

#include "boost/asio.hpp"

#include <memory>
#include <optional>
#include <vector>
#include <thread>
#include <atomic>

namespace task
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    class Schedule
    {
    public:
        using Ptr = std::shared_ptr< Schedule >;

        Schedule( const Task::PtrVector& tasks );
        ~Schedule();
        
        const Task::PtrVector& getTasks() const { return m_tasks; }
        
    private:
        Task::PtrVector m_tasks;
    };

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    class Scheduler
    {
        class ScheduleRun
        {
        public:
            using Ptr = std::shared_ptr< ScheduleRun >;
            
            ScheduleRun( Scheduler& scheduler, Schedule::Ptr pSchedule );
            
            void taskCompleted( Task::RawPtr pTask );
            void progress();
        private:
            Scheduler& m_scheduler;
            Schedule::Ptr m_pSchedule;
            Task::RawPtrSet m_pending, m_finished;
            std::mutex m_mutex;
        };
        
    public:
        using ScheduleOwner = void*;
        
    private:
        using ScheduleRunMap = std::map< ScheduleOwner, ScheduleRun::Ptr >;
    public:
        Scheduler( std::optional< unsigned int > maxThreads );
        ~Scheduler();
        
        void run( ScheduleOwner pOwner, Schedule::Ptr pSchedule );
        void stop();
        
    //private: use of functional prevents visibility control
        void OnKeepAlive( const boost::system::error_code& ec );
        
    private:
        std::atomic< bool > m_stop;
        boost::asio::io_context m_queue;
        boost::asio::steady_timer m_keepAliveTimer;
        std::vector< std::thread > m_threads;
        ScheduleRunMap m_runs;
    };

}

#endif //TASK_SCHEDULER_08_FEB_2021