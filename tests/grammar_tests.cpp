
#include "common/grammar.hpp"

#include <gtest/gtest.h>

using namespace boost::spirit_ext;

const std::string strInput1 =
        "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG 0123456789"\
        "the quick brown fox jumped over the lazy dog";
const std::string strExpected1 =
        "THEQUICKBROWNFOXJUMPEDOVERTHELAZYDOG0123456789"\
        "thequickbrownfoxjumpedoverthelazydog";
std::string strInput2    = "//comment\nstuff//comment\n";
std::string strInput3    = "/* This is a multiline\n comment \n and ends here */STRING/* This has "\
                           " some nested /* comment tags */STRING";
                       
TEST( GrammarTests, Skip_1 )
{
    std::string strOutput;
    boost::spirit_ext::strip( strInput1, strOutput );
    ASSERT_EQ( strExpected1, strOutput );
}

TEST( GrammarTests, Skip_2 )
{
    std::string strExpected = "stuff";
    std::string strOutput;
    boost::spirit_ext::strip( strInput2, strOutput );
    ASSERT_EQ( strExpected, strOutput );
}

TEST( GrammarTests, Skip_3 )
{
    std::string strExpected = "STRINGSTRING";
    std::string strOutput;
    boost::spirit_ext::strip( strInput3, strOutput );
    ASSERT_EQ( strExpected, strOutput );
}