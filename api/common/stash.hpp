
#ifndef STASH_9_FEB_2021
#define STASH_9_FEB_2021

#include "common/hash.hpp"

#include "boost/filesystem/path.hpp"

#include <memory>
#include <map>

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
    inline void serialize( Archive& archive, const unsigned int )
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
    inline void operator^=( const FileHash& fileHash )
    {
        m_data = common::internal::HashCombiner()( m_data, fileHash.get() );
    }
    inline void operator^=( const DeterminantHash& determinantHash )
    {
        m_data = common::internal::HashCombiner()( m_data, determinantHash.get() );
    }
    template < typename... Args >
    inline void operator^=( Args const&... args )
    {
        m_data = common::internal::HashCombiner()( m_data, common::internal::HashFunctorVariadic()( args... ) );
    }

    template < class Archive >
    inline void serialize( Archive& archive, const unsigned int )
    {
        archive& m_data;
    }
};

class BuildHashCodes
{
public:
    using HashCodeMap = std::map< boost::filesystem::path, FileHash >;

    inline const HashCodeMap& get() const { return m_buildHashCodes; }

    inline FileHash get( const boost::filesystem::path& key ) const
    {
        HashCodeMap::const_iterator iFind = m_buildHashCodes.find( key );
        VERIFY_RTE_MSG( iFind != m_buildHashCodes.end(), "Failed to locate hash code for: " << key.string() );
        return iFind->second;
    }

    inline void set( const boost::filesystem::path& key, FileHash hashCode )
    {
        m_buildHashCodes.insert( std::make_pair( key, hashCode ) );
    }

    inline void set( const std::map< boost::filesystem::path, task::FileHash >& buidHashCodes )
    {
        m_buildHashCodes = buidHashCodes;
    }

    inline void reset() { m_buildHashCodes.clear(); }

private:
    HashCodeMap m_buildHashCodes;
};

class Stash
{
public:
    Stash( const boost::filesystem::path& stashDirectory );

    void clear();
    void stash( const boost::filesystem::path& file, DeterminantHash code );
    bool restore( const boost::filesystem::path& file, DeterminantHash code );

private:
    struct Pimpl;
    std::shared_ptr< Pimpl > m_pPimpl;
};
} // namespace task

#endif // STASH_9_FEB_2021
