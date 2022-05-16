
#ifndef TASK_TOOLS_14_OCT_2020
#define TASK_TOOLS_14_OCT_2020

#include "common/hash.hpp"

#include "boost/filesystem/path.hpp"
#include "boost/timer/timer.hpp"

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>
#include <variant>

namespace task
{    

    class Status
    {
    public:
        using Owner = const void*;
        
        enum State
        {
            ePending,
            eStarted,
            eCached,
            eSucceeded,
            eFailed
        };
        
        Status( Owner owner )
            :   m_state( ePending ),
                m_owner( owner )
        {}
        
        State m_state;
        Owner m_owner;
        
        using Subject = std::variant< std::string, boost::filesystem::path >;
        
        std::string m_strTaskName;
        std::optional< Subject > m_source, m_target;
        
        std::vector< std::string > m_msgs;
        std::optional< std::string > m_elapsed;
    };
    
    std::ostream& operator<<( std::ostream& os, const Status::Subject& subject );
    std::ostream& operator<<( std::ostream& os, const Status& status );
    
    class StatusFIFO
    {
    public:
        bool empty() const
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            return m_fifo.empty();
        }
    
        void push( const Status& status )
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            m_fifo.push_back( status );
        }
        
        Status pop()
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            Status front = m_fifo.front();
            m_fifo.pop_front();
            return front;
        }
        
    private:
        mutable std::mutex m_mutex;
        std::deque< Status > m_fifo;
    };
    
    class Progress
    {
    public:
        Progress( StatusFIFO& fifo, Status::Owner owner );
        
        const Status& getStatus() const { return m_status; }
        bool isFinished() const;
        
        void start( const std::string& strTaskName, Status::Subject source, Status::Subject target );
        
        void cached();
        void succeeded();
        void failed();
        void setState( Status::State state );
        
        void msg( const std::string& strMsg );
    private:
        std::string getElapsedTime() const;
        
    private:
        StatusFIFO& m_fifo;
        boost::timer::cpu_timer m_timer;
        Status m_status;
    };
    
    class Task
    {
    public:
        using Ptr = std::shared_ptr< Task >;
        using PtrVector = std::vector< Ptr >;
        using RawPtr = Task*;
        using RawPtrSet = std::set< RawPtr >;
        
        Task( const RawPtrSet& dependencies );
        virtual ~Task();
        
        virtual bool isReady( const RawPtrSet& finished );
        virtual void run( Progress& taskProgress ) = 0;
        virtual void failed( Progress& taskProgress );
        
    protected:
        RawPtrSet m_dependencies;
    };
    
    
    
}

#endif //TASK_TOOLS_14_OCT_2020