
#ifndef TASK_TOOLS_14_OCT_2020
#define TASK_TOOLS_14_OCT_2020

#include "common/hash.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>
#include <set>
#include <memory>
#include <mutex>
#include <ostream>
#include <optional>

namespace task
{
    class TaskInfo
    {
        std::string m_strTaskName, m_strSource, m_strTarget;
        bool m_bCached = false, m_bComplete = false;
        std::vector< std::string > m_msgs;
        struct TaskInfoPimpl;
        std::shared_ptr< TaskInfoPimpl > m_pPimpl;
        mutable std::mutex m_mutex;
    public:
        using Ptr = std::shared_ptr< TaskInfo >;
        using PtrVector = std::vector< Ptr >;
        
        TaskInfo( std::ostream& log );
        
        void update();
        
        const std::string& taskName() const             { std::lock_guard< std::mutex > lock( m_mutex ); return m_strTaskName;         }
        const std::string& source() const               { std::lock_guard< std::mutex > lock( m_mutex ); return m_strSource;           }
        const std::string& target() const               { std::lock_guard< std::mutex > lock( m_mutex ); return m_strTarget;           }
        const bool cached() const                       { std::lock_guard< std::mutex > lock( m_mutex ); return m_bCached;             }
        const bool complete() const                     { std::lock_guard< std::mutex > lock( m_mutex ); return m_bComplete;           }
        const std::vector< std::string >& msgs() const  { std::lock_guard< std::mutex > lock( m_mutex ); return m_msgs;                }
        
        void taskName( const std::string& strTaskName )         { std::lock_guard< std::mutex > lock( m_mutex ); m_strTaskName = strTaskName;   }
        void source( const std::string& strSource )             { std::lock_guard< std::mutex > lock( m_mutex ); m_strSource = strSource;       }
        void source( const boost::filesystem::path& file )      { std::lock_guard< std::mutex > lock( m_mutex ); m_strSource = file.string();   }
        void target( const std::string& strTarget )             { std::lock_guard< std::mutex > lock( m_mutex ); m_strTarget = strTarget;       }
        void target( const boost::filesystem::path& file )      { std::lock_guard< std::mutex > lock( m_mutex ); m_strTarget = file.string();   }
        void cached( bool bCached )                             { std::lock_guard< std::mutex > lock( m_mutex ); m_bCached = bCached;           }
        void complete( bool bComplete )                         { std::lock_guard< std::mutex > lock( m_mutex ); m_bComplete = bComplete;       }
        void msg( const std::string& strMsg )                   { std::lock_guard< std::mutex > lock( m_mutex ); m_msgs.push_back( strMsg );    }
    };
    
    class Task
    {
    public:
        using Ptr = std::shared_ptr< Task >;
        using PtrVector = std::vector< Ptr >;
        using RawPtr = Task*;
        using RawPtrSet = std::set< RawPtr >;
        
        Task( std::ostream& log, const RawPtrSet& dependencies );
        
        virtual ~Task();
        
        virtual bool isReady( const RawPtrSet& finished );
        virtual void run() = 0;
        
        TaskInfo& getTaskInfo() { return m_taskInfo; }
        
        void updateProgress() { m_taskInfo.update(); }
        
    protected:
        TaskInfo m_taskInfo;
        RawPtrSet m_dependencies;
    };
    
    class Scheduler
    {
    public:
        Scheduler( std::ostream& log, const Task::PtrVector& tasks );
        
        void run( std::optional< unsigned int > maxThreads = std::optional< unsigned int >() );
        
    private:
        void thread_run();
    
    private:
        std::ostream& m_log;
        Task::PtrVector m_tasks;
        Task::RawPtrSet m_pending, m_finished;
        std::mutex m_mutex;
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