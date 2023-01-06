
#ifndef PREPROCESSOR_2_SEPT_2015
#define PREPROCESSOR_2_SEPT_2015

#ifdef _WIN32
// NOTE - windows fix required
#else

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

#include "common/grammar.hpp"
#include "common/variant_utils.hpp"

#ifdef BUILD_COMMON_PREPROCESSOR

namespace Preprocessor
{

    class IncludeDirective : public std::string
    {
    public:
        IncludeDirective() = default;
        IncludeDirective( const std::string& str ) : std::string( str ) {}
        IncludeDirective( const char* pszStr ) : std::string( pszStr ) {}
    };

    using PreprocessVariant = boost::variant<std::string, IncludeDirective>;
    using PreprocessVariantVector = std::vector<PreprocessVariant>;

    template< typename Iterator >
    class ProprocessorGrammar : public boost::spirit::qi::grammar< Iterator, PreprocessVariantVector() >
    {
    public:
        ProprocessorGrammar(  )
        :   ProprocessorGrammar::base_type( m_main_rule, "proprocessor" )
        {
            
            m_include_directive_rule = boost::spirit::qi::lit( "#include<" ) >> *( boost::spirit::ascii::char_ - '>')
				[ boost::phoenix::push_back( boost::spirit::_val, boost::spirit::qi::_1 ) ] >> boost::spirit::qi::lit( ">" );

            m_string_rule =  +( !boost::spirit::qi::lit( "#include<" ) >> boost::spirit::ascii::char_ )
				[ boost::phoenix::push_back( boost::spirit::_val, boost::spirit::qi::_1 ) ];

            m_proprocess_variant_rule =
				m_include_directive_rule[ boost::spirit::_val = boost::spirit::qi::_1 ] |
				m_string_rule[ boost::spirit::_val = boost::spirit::qi::_1 ];

            m_main_rule = *m_proprocess_variant_rule
				[ boost::phoenix::push_back( boost::spirit::_val, boost::spirit::qi::_1 ) ];
        }
        boost::spirit::qi::rule< Iterator, IncludeDirective() >         m_include_directive_rule;
        boost::spirit::qi::rule< Iterator, std::string() >              m_string_rule;
        boost::spirit::qi::rule< Iterator, PreprocessVariant() >        m_proprocess_variant_rule;
        boost::spirit::qi::rule< Iterator, PreprocessVariantVector() >  m_main_rule;

    };

    class PreprocessorVisitor : public boost::static_visitor< boost::optional< const IncludeDirective& > >
    {
    public:
        boost::optional< const IncludeDirective& > operator()( const IncludeDirective& targetType ) const
        {
            return { targetType };
        }

        template< class TOther >
        boost::optional< const IncludeDirective& > operator()( TOther& ) const
        {
            return {};
        }

    };

    struct NullIncludeFunctor
    {
        std::string operator()( const IncludeDirective& directive ) const
        {
            return {};
        }
    };

    template< class T >
    static void preprocess_string( const std::string& str, const T& includeFunctor, std::ostream& os )
    {
        PreprocessVariantVector result;

        std::string::const_iterator
            iBegin = str.begin(),
            iEnd = str.end();

        ProprocessorGrammar< std::string::const_iterator > grammar;

        const bool r = phrase_parse(
            iBegin,
            iEnd,
            grammar,
            boost::spirit_ext::NoSkipGrammar< std::string::const_iterator >(),
            result );

        VERIFY_RTE( r );

        for(auto & i : result)
        {
            if( boost::optional< const IncludeDirective& > opt =
                    boost::apply_visitor( ( PreprocessorVisitor() ), i ) )
            {
                os << includeFunctor( opt.get() );
            }
            else
            {
                boost::optional< const std::string& > optString =
                    boost::apply_visitor( boost::TypeAccessor< const std::string >(), i );
                VERIFY_RTE( optString );
                os << optString.get();
            }
        }
    }

}
#endif

#endif //PREPROCESSOR_2_SEPT_2015
