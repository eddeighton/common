
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
        
        const Task::PtrVector& getTasks() const { return m_tasks; }
        
    private:
        Task::PtrVector m_tasks;
    };

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
    class Scheduler
    {
    public:
        using ScheduleOwner = void*;
        
        class ScheduleRun : public std::enable_shared_from_this< ScheduleRun >
        {
        public:
            using Ptr = std::shared_ptr< ScheduleRun >;
            
            ScheduleRun( Scheduler& scheduler, ScheduleOwner pOwner, Schedule::Ptr pSchedule );
            
            ScheduleOwner getOwner() const { return m_pOwner; }
            
            bool wait();
            void cancel();
            
        //private:
            void cancelWithoutStart();
            void runTask( Task::RawPtr pTask );
            void progress();
            
        private:
            Scheduler& m_scheduler;
            ScheduleOwner m_pOwner;
            Schedule::Ptr m_pSchedule;
            Task::RawPtrSet m_pending, m_active, m_finished;
            std::mutex m_mutex;
            std::promise< bool > m_promise;
            std::future< bool > m_future;
            bool m_bCancelled;
        };
        
    private:
        using ScheduleRunMap = std::map< ScheduleOwner, ScheduleRun::Ptr >;
    public:
        static auto getDefaultAliveRate()
        {
            using namespace std::chrono_literals;
            static const auto DEFAULT_ALIVE_RATE = 250ms;
            return DEFAULT_ALIVE_RATE;
        }
        
        Scheduler( TaskProgressFIFO& fifo, 
            std::chrono::milliseconds keepAliveRate, 
            std::optional< unsigned int > maxThreads );
        ~Scheduler();
        
        ScheduleRun::Ptr run( ScheduleOwner pOwner, Schedule::Ptr pSchedule );
        void stop();
        
    //private:
        void OnKeepAlive( const boost::system::error_code& ec );
        void OnRunComplete( ScheduleRun::Ptr pRun );
        
    private:
        TaskProgressFIFO& m_fifo;
        bool m_bStop;
        std::mutex m_mutex;
        boost::asio::io_context m_queue;
        std::chrono::milliseconds m_keepAliveRate;
        boost::asio::steady_timer m_keepAliveTimer;
        std::vector< std::thread > m_threads;
        ScheduleRunMap m_runs;
        ScheduleRunMap m_pending;
    };

}

#endif //TASK_SCHEDULER_08_FEB_2021