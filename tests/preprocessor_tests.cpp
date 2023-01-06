
#include "common/preprocessor.hpp"

#include <gtest/gtest.h>

#ifdef BUILD_COMMON_PREPROCESSOR

TEST( PreprocessorTests, test1 )
{
    std::string str = "test";
    std::ostringstream os;

    Preprocessor::preprocess_string( str, Preprocessor::NullIncludeFunctor(), os );

    ASSERT_EQ( "test", os.str() );
}

TEST( PreprocessorTests, test2 )
{
    std::string str = "test\nstuff#include</thing>\nstuff";
    
    std::ostringstream os;

    Preprocessor::preprocess_string( str, Preprocessor::NullIncludeFunctor(), os );

    ASSERT_EQ( "test\nstuff\nstuff", os.str() );
}

struct SimpleIncludeFunctor
{
    std::string operator()( const Preprocessor::IncludeDirective& directive ) const
    {
        return "test";
    }
};

TEST( PreprocessorTests, test3 )
{
    std::string str = "test\nstuff#include</thing>\nstuff";
    
    std::ostringstream os;

    Preprocessor::preprocess_string( str, SimpleIncludeFunctor(), os );

    ASSERT_EQ( "test\nstufftest\nstuff", os.str() );
}

#endif
