
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
        
        class Run : public std::enable_shared_from_this< Run >
        {
        public:
            using Owner = const void*;
            using Ptr = std::shared_ptr< Run >;
            
            Run( Scheduler& scheduler, Owner pOwner, Schedule::Ptr pSchedule );
            
            Owner getOwner() const { return m_pOwner; }
            bool isCancelled() const;
            
            bool wait();
            void cancel();
            
        //private:
            void finished();
            void cancelWithoutStart();
            void runTask( Task::RawPtr pTask );
            void start();
            void next();
            
        private:
            Scheduler& m_scheduler;
            Owner m_pOwner;
            Schedule::Ptr m_pSchedule;
            Task::RawPtrSet m_pending, m_active, m_finished;
            mutable std::recursive_mutex m_mutex;
            std::promise< bool > m_promise;
            std::future< bool > m_future;
            bool m_bCancelled;
            std::optional< std::exception_ptr > m_pExceptionPtr;
        };
        
    private:
        using ScheduleRunMap = std::map< Run::Owner, Run::Ptr >;
    public:
        static auto getDefaultAliveRate()
        {
            using namespace std::chrono_literals;
            static const auto DEFAULT_ALIVE_RATE = 100ms;
            return DEFAULT_ALIVE_RATE;
        }
        
        Scheduler( StatusFIFO& fifo, 
            std::chrono::milliseconds keepAliveRate, 
            std::optional< unsigned int > maxThreads );
        ~Scheduler();
        
        Run::Ptr run( Run::Owner pOwner, Schedule::Ptr pSchedule );
        void stop();
        
    //private:
        void OnKeepAlive( const boost::system::error_code& ec );
        void OnRunComplete( Run::Ptr pRun );
        
    private:
        StatusFIFO& m_fifo;
        bool m_bStop;
        std::recursive_mutex m_mutex;
        boost::asio::io_context m_queue;
        std::chrono::milliseconds m_keepAliveRate;
        boost::asio::steady_timer m_keepAliveTimer;
        std::vector< std::thread > m_threads;
        ScheduleRunMap m_runs;
        ScheduleRunMap m_pending;
    };
    
    void run( task::Schedule::Ptr pSchedule, std::ostream& os );

}

#endif //TASK_SCHEDULER_08_FEB_2021