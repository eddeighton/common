

#include "common/escape.hpp"

#include <gtest/gtest.h>

TEST( Escape, basic )
{
    ASSERT_EQ( Common::escapeString( "This is a basic test" ), std::string("\"This is a basic test\""));
    ASSERT_EQ( Common::escapeString( "\t \n\r" ), std::string("\"\\t\ \\n\\r\""));
}
