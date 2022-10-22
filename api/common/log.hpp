

#ifndef LOG_30_11_2016
#define LOG_30_11_2016

#include <fstream>

#include <boost/current_function.hpp>

#include "requireSemicolon.hpp"

//common::getBackTrace( _os );\

#define LOG( file, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ofstream _os( file, std::ios::app ); \
        _os << "FILE " << __FILE__ << ":" << __LINE__ << " FUNCTION:" << BOOST_CURRENT_FUNCTION << "MSG:" << msg << "\n"; \
        )


#endif //LOG_30_11_2016
