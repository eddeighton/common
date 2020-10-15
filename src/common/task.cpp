
#include "common/task.hpp"
#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/timer/timer.hpp>

#include <functional>
#include <thread>
#include <chrono>
#include <iomanip>

namespace task
{
    
class TaskInfo::TaskInfoPimpl
{
    const TaskInfo& m_taskInfo;
    boost::timer::cpu_timer timer_internal;
    std::ostream& m_log;
public:
    TaskInfoPimpl( const TaskInfo& taskInfo, std::ostream& log )
        :   m_taskInfo( taskInfo ),
            m_log( log )
    {
    }
    
    void TaskInfoPimpl::update()
    {
        static std::mutex globalMutex;
        std::lock_guard< std::mutex > lock( globalMutex );
        
        static const int taskPadding = 25;
        static const int pathPadding = 110;
        static const int timePadding = 10;
        if( m_taskInfo.cached() )
        {
            timer_internal.stop();
            //m_log << 
            //    std::left << std::setw( taskPadding ) << m_taskInfo.taskName() << " " << 
            //    std::right << std::setw( pathPadding ) << m_taskInfo.source() << " -> " << 
            //    std::left << std::setw( pathPadding ) << m_taskInfo.target() << " " << 
            //    std::left << std::setw( timePadding ) << timer_internal.format( 3, "%w" ) << " CACHED" << std::endl;
        }
        else if( m_taskInfo.complete() )
        {
            timer_internal.stop();
            m_log << 
                std::left << std::setw( taskPadding ) << m_taskInfo.taskName() << " " << 
                std::right << std::setw( pathPadding ) << m_taskInfo.source() << " -> " << 
                std::left << std::setw( pathPadding ) << m_taskInfo.target() << " " << 
                std::left << std::setw( timePadding ) << timer_internal.format( 3, "%w" ) << std::endl;
        }
        else 
        {
            timer_internal.start();
        }
    }
    
};
    
    
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
TaskInfo::TaskInfo( std::ostream& log )
    :   m_pPimpl( std::make_shared< TaskInfoPimpl >( *this, log ) )
{
    
}
    
void TaskInfo::update()
{
    m_pPimpl->update();
}
        
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Task::Task( std::ostream& log, const RawPtrSet& dependencies )
    :   m_taskInfo( log ),
        m_dependencies( dependencies )
{
    
}
    
Task::~Task()
{
    
}
    
bool Task::isReady( const RawPtrSet& finished )
{
    for( RawPtr pTask : m_dependencies )
    {
        if( !finished.count( pTask ) )
            return false;
    }
    return true;
}
    
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Scheduler::Scheduler( std::ostream& log, const Task::PtrVector& tasks )
    :   m_log( log ),
        m_tasks( tasks )
{
    for( Task::Ptr pTask : m_tasks )
    {
        m_pending.insert( pTask.get() );
    }
}

void Scheduler::thread_run()
{
    bool bContinue = true;
    while( bContinue )
    {
        bool bRemainingTasks = false;
        Task::RawPtr pTaskTodo = nullptr;
        {
            std::lock_guard< std::mutex > lock( m_mutex );
            
            if( !m_pending.empty() )
            {
                bRemainingTasks = true;
                //attempt to find ready task
                for( Task::RawPtr pTask : m_pending )
                {
                    if( pTask->isReady( m_finished ) )
                    {
                        m_pending.erase( pTask );
                        pTaskTodo = pTask;
                        break;
                    }
                }
            }
        }
        
        if( bRemainingTasks )
        {
            if( pTaskTodo )
            {
                try
                {
                    pTaskTodo->run();
                }
                catch( std::exception& ex )
                {
                    m_log << "ERROR in task: " << ex.what() << std::endl;
                    throw;
                }
                
                std::lock_guard< std::mutex > lock( m_mutex );
                m_finished.insert( pTaskTodo );
                pTaskTodo->updateProgress();
            }
            else
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for( 0ms );
            }
        }
        else
        {
            bContinue = false;
        }
    }
}

void Scheduler::run( std::optional< unsigned int > maxThreads )
{
    std::vector< std::thread > threads;
    const unsigned int nMaxThreads = 
        maxThreads.has_value() ? maxThreads.value() : std::thread::hardware_concurrency();
    for( int i = 0; i < nMaxThreads; ++i )
    {
        threads.push_back( 
            std::thread( 
                std::bind( &Scheduler::thread_run, std::ref( *this ) ) ) );
    }
    
    for( std::thread& th : threads )
    {
        if( th.joinable() )
            th.join();
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
HashCode hash_combine( HashCode left, HashCode right )
{
    return left ^ ( right << 1 );
}

std::size_t hash_strings( const std::vector< std::string >& strings )
{
    std::size_t szHash = 123456;
    
    for( const std::string& str : strings )
    {
        szHash = hash_combine( szHash, std::hash< std::string >{}( str ) );
    }
    
    return szHash;
}
    
std::size_t hash_file( const boost::filesystem::path& file )
{
    if( boost::filesystem::exists( file ) )
    {
        //error seems to occur if attempt to memory map an empty file
        if( boost::filesystem::is_empty( file ) )
        {
            return boost::filesystem::hash_value( file );
        }
        else
        {
            boost::iostreams::mapped_file_source fileData( file/*, boost::iostreams::mapped_file::readonly*/ );
            const std::string_view dataView( fileData.data(), fileData.size() );
            return std::hash< std::string_view >{}( dataView );
        }
    }
    THROW_RTE( "File does not exist: " << file.string() );
}

struct Stash::Pimpl
{
    const boost::filesystem::path m_stashDirectory;
    
    struct FileHash
    {
        boost::filesystem::path file;
        HashCode code;
        inline bool operator<( const FileHash& hash ) const
        {
            return ( file != hash.file ) ? ( file < hash.file ) :
                   ( code != hash.code ) ? ( code < hash.code ) :
                   false;
        }
    };
    using Manifest = std::map< FileHash, boost::filesystem::path >;
    using HashCodeMap = std::map< boost::filesystem::path, HashCode >;
    
    mutable std::mutex m_mutex;
    Manifest m_manifest;
    HashCodeMap m_hashCodes;
    
    inline static const char* pszManifestFileName = "manifest.txt";
    
    static void load( std::istream& inStream, Manifest& output )
    {
        std::string strLine;
        while( std::getline( inStream, strLine ) )
        {
            using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
            boost::char_separator< char > sep( "," );
            Tokeniser tokens( strLine, sep );
            for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
            {
                FileHash fileHash;
                fileHash.file = *i;
                
                if( ++i == tokens.end() )
                    THROW_RTE( "Error in stash manifest" );
                fileHash.code = boost::lexical_cast< HashCode >( *i );
                
                if( ++i == tokens.end() )
                    THROW_RTE( "Error in stash manifest" );
                
                output.insert( std::make_pair( fileHash, *i ) );
            }
        }
    }

    static void save( const Manifest& input, std::ostream& outStream )
    {
        for( Manifest::const_iterator i = input.begin(),
            iEnd = input.end(); i!=iEnd; ++i )
        {
            outStream << i->first.file.string() << ',' << i->first.code << ',' << i->second.string() << '\n';
        }
    }
    
    static void load( std::istream& inStream, HashCodeMap& output )
    {
        std::string strLine;
        while( std::getline( inStream, strLine ) )
        {
            using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
            boost::char_separator< char > sep( "," );
            Tokeniser tokens( strLine, sep );
            for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
            {
                std::pair< boost::filesystem::path, HashCode > input;
                input.first = *i;
                
                if( ++i == tokens.end() )
                    THROW_RTE( "Error in HashCodeMap file" );
                input.second = boost::lexical_cast< HashCode >( *i );
                
                output.insert( input );
            }
        }
    }

    static void save( const HashCodeMap& input, std::ostream& outStream )
    {
        for( HashCodeMap::const_iterator i = input.begin(),
            iEnd = input.end(); i!=iEnd; ++i )
        {
            outStream << i->first.string() << ',' << i->second << '\n';
        }
    }

    void saveManifest()
    {
        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        boost::filesystem::ensureFoldersExist( manifestFile );
        std::unique_ptr< boost::filesystem::ofstream > pFileStream = 
            boost::filesystem::createNewFileStream( manifestFile );
        save( m_manifest, *pFileStream );
    }
    
    Pimpl( const boost::filesystem::path& stashDirectory )
         :   m_stashDirectory( stashDirectory )
    {
        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        if( boost::filesystem::exists( manifestFile ) )
        {
            std::ifstream inputFileStream( manifestFile.native().c_str(), std::ios::in );
            if( !inputFileStream.good() )
            {
                THROW_RTE( "Failed to open file: " << manifestFile.string() );
            }
            load( inputFileStream, m_manifest );
        }
        else
        {
            m_manifest.clear();
        }
    }
    
    HashCode getHashCode( const boost::filesystem::path& key ) const
    {
        std::lock_guard lock( m_mutex );
        
        HashCodeMap::const_iterator iFind = m_hashCodes.find( key );
        VERIFY_RTE_MSG( iFind != m_hashCodes.end(), "Failed to locate hash code for: " << key.string() );
        return iFind->second;
    }
    
    void setHashCode( const boost::filesystem::path& key, HashCode hashCode )
    {
        std::lock_guard lock( m_mutex );
        
        m_hashCodes.insert( std::make_pair( key, hashCode ) );
    }
    
    void loadHashCodes( const boost::filesystem::path& file )
    {
        if( boost::filesystem::exists( file ) )
        {
            std::ifstream inputFileStream( file.native().c_str(), std::ios::in );
            if( !inputFileStream.good() )
            {
                THROW_RTE( "Failed to open file: " << file.string() );
            }
            load( inputFileStream, m_hashCodes );
        }
        else
        {
            THROW_RTE( "Failed to open file: " << file.string() );
        }
    }
    
    void saveHashCodes( const boost::filesystem::path& file ) const
    {
        std::lock_guard lock( m_mutex );
        
        boost::filesystem::ensureFoldersExist( file );
        std::unique_ptr< boost::filesystem::ofstream > pFileStream = 
            boost::filesystem::createNewFileStream( file );
        save( m_hashCodes, *pFileStream );
    }  
    
    void stash( const boost::filesystem::path& file, const HashCode& code )
    {
        std::lock_guard lock( m_mutex );
        
        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        boost::filesystem::ensureFoldersExist( manifestFile );
        
        std::ostringstream osFileName;
        osFileName << "stash_" << m_manifest.size() << ".st";
        
        const boost::filesystem::path stashFile = m_stashDirectory / osFileName.str();

        if( boost::filesystem::exists( stashFile ) )
        {
            boost::filesystem::remove( stashFile );
        }
        boost::filesystem::copy( file, stashFile );
        
        m_manifest[ FileHash{ file, code } ] = stashFile;
        
        saveManifest();
    }

    bool restore( const boost::filesystem::path& file, const HashCode& code )
    {
        std::lock_guard lock( m_mutex );
        
        Manifest::const_iterator iFind = m_manifest.find( FileHash{ file, code } );
        if( iFind != m_manifest.end() )
        {
            boost::filesystem::path stashFile = iFind->second;
            
            if( boost::filesystem::exists( stashFile ) )
            {
                if( boost::filesystem::exists( file ) )
                {
                    boost::filesystem::remove( file );
                }
                //recheck the hash code??
                boost::filesystem::copy( stashFile, file );
                return true;
            }
        }
        return false;
    }
};

Stash::Stash( const boost::filesystem::path& stashDirectory )
    :   m_pPimpl( std::make_shared< Pimpl >( stashDirectory ) )
{
    
}

HashCode Stash::getHashCode( const boost::filesystem::path& key ) const
{
    return m_pPimpl->getHashCode( key );
}

void Stash::setHashCode( const boost::filesystem::path& key, HashCode hashCode )
{
    m_pPimpl->setHashCode( key, hashCode );
}

void Stash::loadHashCodes( const boost::filesystem::path& file )
{
    m_pPimpl->loadHashCodes( file );
}

void Stash::saveHashCodes( const boost::filesystem::path& file ) const
{
    m_pPimpl->saveHashCodes( file );
}
    
void Stash::stash( const boost::filesystem::path& file, const HashCode& code )
{
    m_pPimpl->stash( file, code );
}

bool Stash::restore( const boost::filesystem::path& file, const HashCode& code )
{
    return m_pPimpl->restore( file, code );
}

}



