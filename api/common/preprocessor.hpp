
#ifndef PREPROCESSOR_2_SEPT_2015
#define PREPROCESSOR_2_SEPT_2015

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

#include "common/file.hpp"
#include "common/grammar.hpp"
#include "common/variant_utils.hpp"

namespace Preprocessor
{

    class IncludeDirective : public std::string
    {
    public:
        IncludeDirective(){}
        IncludeDirective( const std::string& str ) : std::string( str ) {}
        IncludeDirective( const char* pszStr ) : std::string( pszStr ) {}
    };

    typedef boost::variant< std::string, IncludeDirective > PreprocessVariant;
    typedef std::vector< PreprocessVariant > PreprocessVariantVector;

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
            return boost::optional< const IncludeDirective& >( targetType );
        }

        template< class TOther >
        boost::optional< const IncludeDirective& > operator()( TOther& ) const
        {
            return boost::optional< const IncludeDirective& >();
        }

    };

    struct NullIncludeFunctor
    {
        std::string operator()( const IncludeDirective& directive ) const
        {
            return std::string();
        }
    };

    template< class T >
    static void preprocess_string( const std::string& str, T& includeFunctor, std::ostream& os )
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

        for( PreprocessVariantVector::const_iterator i = result.begin(),
             iEnd = result.end();
             i!=iEnd; ++i )
        {
            if( boost::optional< const IncludeDirective& > opt =
                    boost::apply_visitor( ( PreprocessorVisitor() ), *i ) )
            {
                os << includeFunctor( opt.get() );
            }
            else
            {
                boost::optional< const std::string& > optString =
                    boost::apply_visitor( boost::TypeAccessor< const std::string >(), *i );
                VERIFY_RTE( optString );
                os << optString.get();
            }
        }
    }

}

#endif //PREPROCESSOR_2_SEPT_2015
