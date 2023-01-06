
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


TEST( FileUtils, edsIncludeFolder1 )
{
    const path pFile = "c:/a/b/";
    const path pInclude = "c:/a/b/d.h";
    const path pResult = "d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder2 )
{
    const path pFile = "c:/a/b/";
    const path pInclude = "c:/a/d.h";
    const path pResult = "../d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder3 )
{
    const path pFile = "c:/a/b/";
    const path pInclude = "c:/d.h";
    const path pResult = "../../d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder4 )
{
    const path pFile = "c:/a/b/";
    const path pInclude = "c:/e/d.h";
    const path pResult = "../../e/d.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder5 )
{
    const path pFile = "c:/a/a/";
    const path pInclude = "c:/a/a/a/a/a.h";
    const path pResult = "a/a/a.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder6 )
{
    const path pFile = "/a/a/";
    const path pInclude = "/a/a/a/a/a.h";
    const path pResult = "a/a/a.h";
    ASSERT_EQ( pResult, edsInclude( pFile, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder7 )
{
    const path pFolder = "/a/b/";
    const path pInclude = "/a/b/c/d/e.h";
    const path pResult = "c/d/e.h";
    ASSERT_EQ( pResult, edsInclude( pFolder, pInclude ) );
}

TEST( FileUtils, edsIncludeFolder8 )
{
    const path pFolder = "w:/testworkspace";
    const path pInclude = "w:/testworkspace/reddwarf/testhost/testproject/test.eg";
    const path pResult = "reddwarf/testhost/testproject/test.eg";
    ASSERT_EQ( pResult, edsInclude( pFolder, pInclude ) );
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

TEST( FileUtils, updateFileIfChanged )
{
    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path() / "common_tests";

    boost::filesystem::path tempFile = tempDir / "test";
    boost::filesystem::remove_all( tempDir );
    boost::filesystem::ensureFoldersExist( tempFile );

    const std::string strText = "This is a test\r\n123_\n123123123";

    ASSERT_TRUE( boost::filesystem::updateFileIfChanged( tempFile, strText ) );
    
    {
        std::string str;
        boost::filesystem::loadBinaryFile( tempFile, str );
        ASSERT_EQ( str, strText );
    }
    
    ASSERT_FALSE( boost::filesystem::updateFileIfChanged( tempFile, strText ) );

}
