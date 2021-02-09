
#ifndef STASH_9_FEB_2021
#define STASH_9_FEB_2021

#include "common/hash.hpp"

#include "boost/filesystem/path.hpp"

namespace task
{

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

#endif //STASH_9_FEB_2021