
#include "common/file.hpp"

#include <gtest/gtest.h>
#include <fstream>

using namespace boost::filesystem;

TEST( FileUtils, edsCannonicalise1 )
{
    const path p = "c:/a/b/c.def";
    ASSERT_EQ( p, edsCannonicalise( p ) );
}

TEST( FileUtils, edsCannonicalise2 )
{
    const path p = "c:/a/d/../b/../../c.def";
    const path p2 = "c:/c.def";
    ASSERT_EQ( p2, edsCannonicalise( p ) );
}

TEST( FileUtils, edsCannonicalise3 )
{
    const path p = "c:/a/d/./././../b/././../../c.def";
    const path p2 = "c:/c.def";
    ASSERT_EQ( p2, edsCannonicalise( p ) );
}

TEST( FileUtils, edsCannonicalise4 )
{
    const path p = "a/b/../../../c";
    ASSERT_THROW( edsCannonicalise( p ), std::runtime_error );
}

TEST( FileUtils, edsCannonicalise5 )
{
    const path p = "c:/a/d/./././../b/././../../../c.def";
    ASSERT_THROW( edsCannonicalise( p ), std::runtime_error );
}

TEST( FileUtils, edsInclude1 )
{
    const path pFile = "c:/a/b/c.h";
    const path pInclude = "c:/a/b/d.h";
    const path pResult = "d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsInclude2 )
{
    const path pFile = "c:/a/b/c.h";
    const path pInclude = "c:/a/d.h";
    const path pResult = "../d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsInclude3 )
{
    const path pFile = "c:/a/b/c.h";
    const path pInclude = "c:/d.h";
    const path pResult = "../../d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsInclude4 )
{
    const path pFile = "c:/a/b/c.h";
    const path pInclude = "c:/e/d.h";
    const path pResult = "../../e/d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsInclude5 )
{
    const path pFile = "c:/a/a/a.h";
    const path pInclude = "c:/a/a/a/a/a.h";
    const path pResult = "a/a/a.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsInclude6 )
{
    const path pFile = "/a/a/a.h";
    const path pInclude = "/a/a/a/a/a.h";
    const path pResult = "a/a/a.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, loadAsciiFile1 )
{
    //hmmm?
    const std::string strTempFileName = "temp123987123987123987.txt";
    const std::string strData =
            "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG 0123456789\n"\
            "the quick brown fox jumped over the lazy dog\n"\
            "!\"�%^&*()_+-={}[]:@~;'#<>?,./";
    {
        std::ofstream f( strTempFileName.c_str() );
        ASSERT_TRUE( f.good() );
        f << strData;
    }
    std::string str;
    loadAsciiFile( strTempFileName, str, false );
    ASSERT_STREQ( strData.c_str(), str.c_str() );

    remove( strTempFileName );
}