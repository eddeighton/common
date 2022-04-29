
#ifndef STASH_9_FEB_2021
#define STASH_9_FEB_2021

#include "common/hash.hpp"

#include "boost/filesystem/path.hpp"

#include <memory>

namespace task
{
    class DeterminantHash;
    class FileHash : public common::Hash
    {
        // prevent accidental conversion from DeterminantHash to FileHash
        FileHash( const DeterminantHash& fileHash );

    public:
        FileHash() {}
        explicit FileHash( const boost::filesystem::path& file )
            : Hash( file )
        {
        }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_data;
        }
    };

    class DeterminantHash : public common::Hash
    {
    public:
        DeterminantHash() {}
        DeterminantHash( const FileHash& fileHash ) { set( fileHash.get() ); }
        template < typename... Args >
        DeterminantHash( Args const&... args )
            : Hash( args... )
        {
        }

        inline void operator^=( const Hash& code ) { m_data = common::internal::HashCombiner()( m_data, code.get() ); }
        inline void operator^=( const FileHash& fileHash ) { m_data = common::internal::HashCombiner()( m_data, fileHash.get() ); }
        template < typename... Args >
        inline void operator^=( Args const&... args )
        {
            m_data = common::internal::HashCombiner()( m_data, common::internal::HashFunctorVariadic()( args... ) );
        }

        template < class Archive >
        inline void serialize( Archive& archive, const unsigned int version )
        {
            archive& m_data;
        }
    };

    class Stash
    {
    public:
        Stash( const boost::filesystem::path& stashDirectory );

        FileHash getBuildHashCode( const boost::filesystem::path& filePath ) const;
        void     setBuildHashCode( const boost::filesystem::path& filePath, FileHash hashCode );
        void     loadBuildHashCodes( const boost::filesystem::path& file );
        void     saveBuildHashCodes( const boost::filesystem::path& file ) const;

        void stash( const boost::filesystem::path& file, DeterminantHash code );
        bool restore( const boost::filesystem::path& file, DeterminantHash code );

    private:
        struct Pimpl;
        std::shared_ptr< Pimpl > m_pPimpl;
    };
} // namespace task

#endif // STASH_9_FEB_2021
