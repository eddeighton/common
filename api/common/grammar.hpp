/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef GRAMMAR_UTILS_28_MAY_2015
#define GRAMMAR_UTILS_28_MAY_2015

#include <ostream>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

#include "assert_verify.hpp"

#define PARSER_ROUTINE_DECL( TheType ) \
    void parse_impl_actual( boost::spirit_ext::ParseResult& parseResult, TheType& result, std::ostream& errorStream );

#define PARSER_ROUTINE_IMPL( TheGrammar, TheType )                                                                    \
    void parse_impl_actual( boost::spirit_ext::ParseResult& parseResult, TheType& result, std::ostream& errorStream ) \
    {                                                                                                                 \
        boost::spirit_ext::invoke_parser< TheGrammar< boost::spirit_ext::LinePosIterator >,                           \
                                          boost::spirit_ext::SkipGrammar< boost::spirit_ext::LinePosIterator >,       \
                                          TheType >( parseResult, result, errorStream );                              \
    }

#define PARSER_ROUTINE_IMPL_NO_SKIP( TheGrammar, TheType )                                                            \
    void parse_impl_actual( boost::spirit_ext::ParseResult& parseResult, TheType& result, std::ostream& errorStream ) \
    {                                                                                                                 \
        boost::spirit_ext::invoke_parser< TheGrammar< boost::spirit_ext::LinePosIterator >,                           \
                                          boost::spirit_ext::NoSkipGrammar< boost::spirit_ext::LinePosIterator >,     \
                                          TheType >( parseResult, result, errorStream );                              \
    }

namespace boost::spirit_ext
{
using StdStringIterator = std::string::const_iterator;
using LinePosIterator   = boost::spirit::line_pos_iterator< StdStringIterator >;

struct ParseResult
{
    template < class IteratorType >
    ParseResult( IteratorType _iterStart, IteratorType _iterEnd )
        : iterStart( _iterStart )
        , iterReached( _iterStart )
        , iterEnd( _iterEnd )
    {
    }

    bool            bParseSucceeded{};
    LinePosIterator iterStart, iterReached, iterEnd;

    inline bool fullParse() const { return bParseSucceeded && ( iterEnd.base() == iterReached.base() ); }
    inline bool partialParse() const { return bParseSucceeded && ( iterStart.base() < iterReached.base() ); }
    inline bool success() const { return bParseSucceeded; }

    template < typename IterType >
    inline IterType getIterReached() const
    {
        return IterType( iterReached.base() );
    }
};

template < typename Iterator >
struct error_handler
{
    template < typename, typename, typename, typename >
    struct result
    {
        using type = void;
    };

    std::ostream& errorStream;
    error_handler( std::ostream& _errorStream )
        : errorStream( _errorStream )
    {
    }

    void operator()( Iterator, Iterator, Iterator err_pos, boost::spirit::info const& what ) const
    {
        // Iterator eol = err_pos;
        // int line = calculateLineNumber( first, err_pos );
        int line = boost::spirit::get_line( err_pos );
        if( line != -1 )
            errorStream << '(' << line << ')';
        else
            errorStream << "( unknown )";
        errorStream << " : Error! Expected " << what << std::endl;
    }
};

template < class GrammarType, class SkipGrammarType, typename ResultType >
inline void invoke_parser( ParseResult& parseResult, ResultType& result, std::ostream& errorStream )
{
    GrammarType grammar;
    {
        boost::phoenix::function< error_handler< LinePosIterator > > const eHandler( errorStream );
        boost::spirit::qi::on_error< boost::spirit::qi::fail >(
            grammar.m_main_rule,
            eHandler( boost::spirit::qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3, boost::spirit::qi::_4 ) );
    }

    parseResult.bParseSucceeded = boost::spirit::qi::phrase_parse(
        parseResult.iterReached, parseResult.iterEnd, grammar, SkipGrammarType(), result );
}

template < class GrammarType, typename ResultType >
inline void invoke_parser_noskip( ParseResult& parseResult, ResultType& result, std::ostream& errorStream )
{
    GrammarType grammar;
    {
        boost::phoenix::function< error_handler< LinePosIterator > > const eHandler( errorStream );
        boost::spirit::qi::on_error< boost::spirit::qi::fail >(
            grammar.m_main_rule,
            eHandler( boost::spirit::qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3, boost::spirit::qi::_4 ) );
    }

    parseResult.bParseSucceeded
        = boost::spirit::qi::parse( parseResult.iterReached, parseResult.iterEnd, grammar, result );
}

template < class GrammarType, class SkipGrammarType >
inline void invoke_parser( ParseResult& parseResult, std::ostream& errorStream )
{
    GrammarType grammar;
    {
        boost::phoenix::function< error_handler< LinePosIterator > > const eHandler( errorStream );
        boost::spirit::qi::on_error< boost::spirit::qi::fail >(
            grammar.m_main_rule,
            eHandler( boost::spirit::qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3, boost::spirit::qi::_4 ) );
    }

    parseResult.bParseSucceeded
        = boost::spirit::qi::phrase_parse( parseResult.iterReached, parseResult.iterEnd, grammar, SkipGrammarType() );
}

template < typename ResultType, typename InputIteratorType >
ParseResult
parse( InputIteratorType iterBegin, InputIteratorType iterEnd, ResultType& result, std::ostream& errorStream )
{
    ParseResult parseResult( iterBegin, iterEnd );
    parse_impl_actual( parseResult, result, errorStream );
    return parseResult;
}

template < class T, class IterType >
inline ParseResult parse( IterType iStart, IterType iEnd, T& resultValue )
{
    std::ostringstream osError;
    return parse( iStart, iEnd, resultValue, osError );
}

template < class T, class IterType >
inline T parse( IterType iStart, IterType iEnd, std::ostream& osError )
{
    T                 resultType;
    const ParseResult result = parse( iStart, iEnd, resultType, osError );
    VERIFY_RTE_MSG( result.bParseSucceeded && result.iterReached.base() == iEnd,
                    "Failed to parse string: " << std::string( iStart, iEnd ) );
    return resultType;
}

template < class T, class IterType >
inline T parse( IterType iStart, IterType iEnd )
{
    std::ostringstream osError;
    T                  resultType;
    const ParseResult  result = parse( iStart, iEnd, resultType, osError );
    VERIFY_RTE_MSG( result.bParseSucceeded && result.iterReached.base() == iEnd,
                    "Failed to parse string: " << std::string( iStart, iEnd ) << " : " << osError.str() );
    return resultType;
}

template < class T >
inline ParseResult parse( const std::string& str, T& resultValue, std::ostream& osError )
{
    return parse( str.begin(), str.end(), resultValue, osError );
}

template < class T >
inline ParseResult parse( const std::string& str, T& resultValue )
{
    return parse( str.begin(), str.end(), resultValue );
}

template < class T >
inline T parse( const std::string& str, std::ostream& osError )
{
    return parse< T >( str.begin(), str.end(), osError );
}

template < class T >
inline T parse( const std::string& str )
{
    return parse< T >( str.begin(), str.end() );
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
// Skip Grammars
template < typename Iterator >
class CommentGrammar : public boost::spirit::qi::grammar< Iterator >
{
public:
    CommentGrammar()
        : CommentGrammar::base_type( start )
    {
        start = "//" >> *( boost::spirit::ascii::char_ - '\n' ) >> '\n'    // C++ comment
                | "/*" >> *( boost::spirit::ascii::char_ - "*/" ) >> "*/"; // C comment
    }

private:
    boost::spirit::qi::rule< Iterator > start;
};

template < typename Iterator >
class SkipGrammar : public boost::spirit::qi::grammar< Iterator >
{
public:
    SkipGrammar()
        : SkipGrammar::base_type( start )
    {
        start = +( boost::spirit::ascii::space | comments );
    }

    boost::spirit::qi::rule< Iterator > start;

private:
    CommentGrammar< Iterator > comments;
};

template < typename Iterator >
class NoSkipGrammar : public boost::spirit::qi::grammar< Iterator >
{
public:
    NoSkipGrammar()
        : NoSkipGrammar::base_type( start )
    {
        start = !boost::spirit::ascii::char_;
    }

    // private:
    boost::spirit::qi::rule< Iterator > start;
};

template < typename Iterator >
class AllGrammar : public boost::spirit::qi::grammar< Iterator, SkipGrammar< Iterator >, std::string() >
{
public:
    AllGrammar()
        : AllGrammar::base_type( m_main_rule, "all" )
    {
        m_main_rule = *(
            boost::spirit::ascii::char_[ boost::phoenix::push_back( boost::spirit::_val, boost::spirit::qi::_1 ) ] );
    }

    boost::spirit::qi::rule< Iterator, SkipGrammar< Iterator >, std::string() > m_main_rule;
};

inline void strip( const std::string& strInput, std::string& strOutput )
{
    std::ostringstream osError;
    ParseResult        parseResult( strInput.begin(), strInput.end() );
    invoke_parser< AllGrammar< boost::spirit_ext::LinePosIterator >,
                   SkipGrammar< boost::spirit_ext::LinePosIterator >,
                   std::string >( parseResult, strOutput, osError );
    VERIFY_RTE( parseResult.bParseSucceeded );
}

inline void stripComments( const std::string& strInput, std::string& strOutput )
{
    std::ostringstream osError;
    ParseResult        parseResult( strInput.begin(), strInput.end() );
    invoke_parser< AllGrammar< boost::spirit_ext::LinePosIterator >,
                   CommentGrammar< boost::spirit_ext::LinePosIterator >,
                   std::string >( parseResult, strOutput, osError );
    VERIFY_RTE( parseResult.bParseSucceeded );
}
} // namespace boost::spirit_ext

#endif // GRAMMAR_UTILS_28_MAY_2015
