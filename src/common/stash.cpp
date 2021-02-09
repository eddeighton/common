
#include "common/stash.hpp"
#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

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
    
    struct FileHash
    {
        boost::filesystem::path file;
        common::HashCode code;
        inline bool operator<( const FileHash& hash ) const
        {
            return ( file != hash.file ) ? ( file < hash.file ) :
                   ( code != hash.code ) ? ( code < hash.code ) :
                   false;
        }
    };
    using Manifest = std::map< FileHash, boost::filesystem::path >;
    using HashCodeMap = std::map< boost::filesystem::path, common::HashCode >;
    
    mutable std::mutex m_mutex;
    Manifest m_manifest;
    HashCodeMap m_buildHashCodes;
    
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
                fileHash.code = boost::lexical_cast< common::HashCode >( *i );
                
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
                std::pair< boost::filesystem::path, common::HashCode > input;
                input.first = *i;
                
                if( ++i == tokens.end() )
                    THROW_RTE( "Error in HashCodeMap file" );
                input.second = boost::lexical_cast< common::HashCode >( *i );
                
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
    
    common::HashCode getBuildHashCode( const boost::filesystem::path& key ) const
    {
        std::lock_guard lock( m_mutex );
        
        HashCodeMap::const_iterator iFind = m_buildHashCodes.find( key );
        VERIFY_RTE_MSG( iFind != m_buildHashCodes.end(), "Failed to locate hash code for: " << key.string() );
        return iFind->second;
    }
    
    void setBuildHashCode( const boost::filesystem::path& key, common::HashCode hashCode )
    {
        std::lock_guard lock( m_mutex );
        
        m_buildHashCodes.insert( std::make_pair( key, hashCode ) );
    }
    
    void loadBuildHashCodes( const boost::filesystem::path& file )
    {
        if( boost::filesystem::exists( file ) )
        {
            std::ifstream inputFileStream( file.native().c_str(), std::ios::in );
            if( !inputFileStream.good() )
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
        std::lock_guard lock( m_mutex );
        
        boost::filesystem::ensureFoldersExist( file );
        std::unique_ptr< boost::filesystem::ofstream > pFileStream = 
            boost::filesystem::createNewFileStream( file );
        save( m_buildHashCodes, *pFileStream );
    }  
    
    void stash( const boost::filesystem::path& file, const common::HashCode& code )
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

    bool restore( const boost::filesystem::path& file, const common::HashCode& code )
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
                ensureFoldersExist( file );
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

common::HashCode Stash::getBuildHashCode( const boost::filesystem::path& key ) const
{
    return m_pPimpl->getBuildHashCode( key );
}

void Stash::setBuildHashCode( const boost::filesystem::path& key, common::HashCode hashCode )
{
    m_pPimpl->setBuildHashCode( key, hashCode );
}

void Stash::loadBuildHashCodes( const boost::filesystem::path& file )
{
    m_pPimpl->loadBuildHashCodes( file );
}

void Stash::saveBuildHashCodes( const boost::filesystem::path& file ) const
{
    m_pPimpl->saveBuildHashCodes( file );
}
    
void Stash::stash( const boost::filesystem::path& file, const common::HashCode& code )
{
    m_pPimpl->stash( file, code );
}

bool Stash::restore( const boost::filesystem::path& file, const common::HashCode& code )
{
    return m_pPimpl->restore( file, code );
}


}