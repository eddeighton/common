
#ifndef TASK_TOOLS_14_OCT_2020
#define TASK_TOOLS_14_OCT_2020

#include "common/hash.hpp"

#include <boost/filesystem/path.hpp>
#include "boost/timer/timer.hpp"

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>
#include <deque>

namespace task
{    
    class TaskProgress
    {
    public:
        std::string m_strTaskName;
        std::optional< std::string > m_optSourceString;
        std::optional< boost::filesystem::path > m_optSourcePath;
        std::optional< std::string > m_optTargetString;
        std::optional< boost::filesystem::path > m_optTargetPath;
        
        std::optional< bool > m_bCached;
        std::optional< bool > m_bComplete;
        std::vector< std::string > m_msgs;
        
    };
    
    class TaskProgressFIFO
    {
    public:
    
        bool empty() const
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            return m_fifo.empty();
        }
    
        void push( const TaskProgress& taskProgress )
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            m_fifo.push_back( taskProgress );
        }
        
        TaskProgress pop()
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            TaskProgress front = m_fifo.front();
            m_fifo.pop_front();
            return front;
        }
        
    
    private:
        mutable std::mutex m_mutex;
        std::deque< TaskProgress > m_fifo;
    };
    
    class NotifiedTaskProgress
    {
    public:
        NotifiedTaskProgress( TaskProgressFIFO& fifo );
        
        void setTaskInfo( const std::string& strTaskName, const std::string& strSource, const std::string& strTarget );
        void setTaskInfo( const std::string& strTaskName, const boost::filesystem::path& fileSource, const std::string& fileTarget );
        
        void cached( bool bCached );
        void complete( bool bComplete );
        
        void msg( const std::string& strMsg );
    private:
        
    private:
        TaskProgressFIFO& m_fifo;
        boost::timer::cpu_timer m_timer;
        TaskProgress m_progress;
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
        virtual void run( NotifiedTaskProgress& taskProgress ) = 0;
        
    protected:
        RawPtrSet m_dependencies;
    };
    
    
    class Stash
    {
    public:
        Stash( const boost::filesystem::path& stashDirectory );
        
        common::HashCode getBuildHashCode( const boost::filesystem::path& key ) const;
        void setBuildHashCode( const boost::filesystem::path& key, common::HashCode hashCode );
        void loadBuildHashCodes( const boost::filesystem::path& file );
        void saveBuildHashCodes( const boost::filesystem::path& file ) const;
        
        void stash( const boost::filesystem::path& file, const common::HashCode& code );
        bool restore( const boost::filesystem::path& file, const common::HashCode& code );
    private:
        struct Pimpl;
        std::shared_ptr< Pimpl > m_pPimpl;
    };
    
}

#endif //TASK_TOOLS_14_OCT_2020