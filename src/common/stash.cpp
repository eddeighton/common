
#include "common/stash.hpp"
#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/hash.hpp"

#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"
#include <shared_mutex>
#include <mutex>

#include <map>
#include <istream>
#include <ostream>

namespace task
{

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct Stash::Pimpl
{
    const boost::filesystem::path m_stashDirectory;

    struct FileDeterminant
    {
        boost::filesystem::path file;
        DeterminantHash         determinant;
        inline bool             operator<( const FileDeterminant& hash ) const
        {
            return ( file != hash.file )                 ? ( file < hash.file )
                   : ( determinant != hash.determinant ) ? ( determinant < hash.determinant )
                                                         : false;
        }
    };
    struct StashItem
    {
        boost::filesystem::path filePath;
        std::time_t             fileTime;
    };
    using Manifest    = std::map< FileDeterminant, StashItem >;
    using HashCodeMap = std::map< boost::filesystem::path, FileHash >;

    mutable std::shared_mutex m_manifestMutex, m_buildCodesMutex;
    using WriteLock = std::unique_lock< std::shared_mutex >;
    using ReadLock  = std::shared_lock< std::shared_mutex >;

    Manifest    m_manifest;
    HashCodeMap m_buildHashCodes;

    inline static const char* pszManifestFileName = "stash_manifest.txt";

    static void load( std::istream& inStream, Manifest& output )
    {
        std::string strLine;
        while ( std::getline( inStream, strLine ) )
        {
            using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
            boost::char_separator< char > sep( "," );
            Tokeniser                     tokens( strLine, sep );
            for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
            {
                FileDeterminant fileHash;
                fileHash.file = *i;

                if ( ++i == tokens.end() )
                    THROW_RTE( "Error in stash manifest" );
                fileHash.determinant = boost::lexical_cast< common::Hash >( *i );

                if ( ++i == tokens.end() )
                    THROW_RTE( "Error in stash manifest" );

                const boost::filesystem::path filePath = *i;

                if ( ++i == tokens.end() )
                    THROW_RTE( "Error in stash manifest" );

                const std::time_t fileTime = boost::lexical_cast< std::time_t >( *i );

                output.insert( std::make_pair( fileHash, StashItem{ filePath, fileTime } ) );
            }
        }
    }

    static void save( const Manifest& input, std::ostream& outStream )
    {
        for ( Manifest::const_iterator i = input.begin(), iEnd = input.end(); i != iEnd; ++i )
        {
            outStream << i->first.file.string() << ',' << i->first.determinant << ',' << i->second.filePath.string()
                      << ',' << i->second.fileTime << '\n';
        }
    }

    static void load( std::istream& inStream, HashCodeMap& output )
    {
        std::string strLine;
        while ( std::getline( inStream, strLine ) )
        {
            using Tokeniser = boost::tokenizer< boost::char_separator< char > >;
            boost::char_separator< char > sep( "," );
            Tokeniser                     tokens( strLine, sep );
            for ( Tokeniser::iterator i = tokens.begin(); i != tokens.end(); ++i )
            {
                std::pair< boost::filesystem::path, FileHash > input;
                input.first = *i;

                if ( ++i == tokens.end() )
                    THROW_RTE( "Error in HashCodeMap file" );
                input.second = boost::lexical_cast< FileHash >( *i );

                output.insert( input );
            }
        }
    }

    static void save( const HashCodeMap& input, std::ostream& outStream )
    {
        for ( HashCodeMap::const_iterator i = input.begin(), iEnd = input.end(); i != iEnd; ++i )
        {
            outStream << i->first.string() << ',' << i->second << '\n';
        }
    }

    void saveManifest()
    {
        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        boost::filesystem::ensureFoldersExist( manifestFile );
        std::unique_ptr< boost::filesystem::ofstream > pFileStream
            = boost::filesystem::createNewFileStream( manifestFile );
        save( m_manifest, *pFileStream );
    }

    Pimpl( const boost::filesystem::path& stashDirectory )
        : m_stashDirectory( stashDirectory )
    {
        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        if ( boost::filesystem::exists( manifestFile ) )
        {
            std::ifstream inputFileStream( manifestFile.native().c_str(), std::ios::in );
            if ( !inputFileStream.good() )
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

    FileHash getBuildHashCode( const boost::filesystem::path& key ) const
    {
        ReadLock lock( m_buildCodesMutex );

        HashCodeMap::const_iterator iFind = m_buildHashCodes.find( key );
        VERIFY_RTE_MSG( iFind != m_buildHashCodes.end(), "Failed to locate hash code for: " << key.string() );
        return iFind->second;
    }

    void setBuildHashCode( const boost::filesystem::path& key, FileHash hashCode )
    {
        WriteLock lock( m_buildCodesMutex );

        m_buildHashCodes.insert( std::make_pair( key, hashCode ) );
    }

    void resetBuildHashCodes()
    {
        WriteLock lock( m_buildCodesMutex );
        m_buildHashCodes.clear();
    }

    void loadBuildHashCodes( const boost::filesystem::path& file )
    {
        if ( boost::filesystem::exists( file ) )
        {
            std::ifstream inputFileStream( file.native().c_str(), std::ios::in );
            if ( !inputFileStream.good() )
            {
                THROW_RTE( "Failed to open file: " << file.string() );
            }
            load( inputFileStream, m_buildHashCodes );
        }
        else
        {
            THROW_RTE( "Failed to open file: " << file.string() );
        }
    }

    void saveBuildHashCodes( const boost::filesystem::path& file ) const
    {
        ReadLock lock( m_buildCodesMutex );

        boost::filesystem::ensureFoldersExist( file );
        std::unique_ptr< boost::filesystem::ofstream > pFileStream = boost::filesystem::createNewFileStream( file );
        save( m_buildHashCodes, *pFileStream );
    }

    void stash( const boost::filesystem::path& file, DeterminantHash determinant )
    {
        WriteLock lock( m_manifestMutex );

        const boost::filesystem::path manifestFile = m_stashDirectory / pszManifestFileName;
        boost::filesystem::ensureFoldersExist( manifestFile );

        // determine a new stash file
        boost::filesystem::path stashFile;
        for ( std::size_t szFileID = m_manifest.size();; ++szFileID )
        {
            std::ostringstream osFileName;
            osFileName << "stash_" << szFileID << ".st";
            const boost::filesystem::path tryFile = m_stashDirectory / osFileName.str();
            if ( !boost::filesystem::exists( tryFile ) )
            {
                stashFile = tryFile;
                break;
            }
        }

        boost::filesystem::copy( file, stashFile );

        m_manifest[ FileDeterminant{ file, determinant } ]
            = StashItem{ stashFile, boost::filesystem::last_write_time( file ) };

        saveManifest();
    }

    bool restore( const boost::filesystem::path& file, DeterminantHash determinant )
    {
        ReadLock lock( m_manifestMutex );

        Manifest::const_iterator iFind = m_manifest.find( FileDeterminant{ file, determinant } );
        if ( iFind != m_manifest.end() )
        {
            const StashItem& stashItem = iFind->second;

            if ( boost::filesystem::exists( stashItem.filePath ) )
            {
                if ( boost::filesystem::exists( file ) )
                {
                    // attempt to re-use existing file
                    if ( common::Hash( file ) == common::Hash( stashItem.filePath ) )
                    {
                        if ( last_write_time( file ) != stashItem.fileTime )
                            last_write_time( file, stashItem.fileTime );
                        return true;
                    }
                    boost::filesystem::remove( file );
                }
                ensureFoldersExist( file );

                boost::filesystem::copy( stashItem.filePath, file );
                last_write_time( file, stashItem.fileTime );
                return true;
            }
        }
        return false;
    }
};

Stash::Stash( const boost::filesystem::path& stashDirectory )
    : m_pPimpl( std::make_shared< Pimpl >( stashDirectory ) )
{
}

FileHash Stash::getBuildHashCode( const boost::filesystem::path& key ) const
{
    return m_pPimpl->getBuildHashCode( key );
}

void Stash::setBuildHashCode( const boost::filesystem::path& key, FileHash hashCode )
{
    m_pPimpl->setBuildHashCode( key, hashCode );
}

void Stash::resetBuildHashCodes() { m_pPimpl->resetBuildHashCodes(); }

void Stash::loadBuildHashCodes( const boost::filesystem::path& file ) { m_pPimpl->loadBuildHashCodes( file ); }

void Stash::saveBuildHashCodes( const boost::filesystem::path& file ) const { m_pPimpl->saveBuildHashCodes( file ); }

void Stash::stash( const boost::filesystem::path& file, const DeterminantHash determinant )
{
    m_pPimpl->stash( file, determinant );
}

bool Stash::restore( const boost::filesystem::path& file, const DeterminantHash determinant )
{
    return m_pPimpl->restore( file, determinant );
}

} // namespace task
