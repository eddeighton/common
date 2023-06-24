
#ifndef BOOST_SERIALIZATION_EXTEND_8_12_2016
#define BOOST_SERIALIZATION_EXTEND_8_12_2016

#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

namespace
{
template < unsigned int N >
struct SerializeTupleUtil
{
    template < typename Archive, typename... Args >
    static inline void serialize( Archive& ar, std::tuple< Args... >& t )
    {
        SerializeTupleUtil< N - 1 >::serialize( ar, t );
        ar& std::get< N - 1 >( t );
    }
};

template <>
struct SerializeTupleUtil< 1 >
{
    template < typename Archive, typename... Args >
    static inline void serialize( Archive& ar, std::tuple< Args... >& t )
    {
        ar& std::get< 0 >( t );
    }
};

} // namespace

namespace boost::serialization
{
template < typename Archive, typename... Args >
Archive& serialize( Archive& ar, std::tuple< Args... >& t, const unsigned int version )
{
    SerializeTupleUtil< sizeof...( Args ) >::serialize( ar, t );
    return ar;
}
} // namespace boost::serialization

#endif // BOOST_SERIALIZATION_EXTEND_8_12_2016
