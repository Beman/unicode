//  boost/unicode/stream.hpp  ----------------------------------------------------------//

//  © Copyright Beman Dawes 2011, 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_STREAM_HPP)
# define BOOST_UNICODE_STREAM_HPP

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <ostream>
#include <iterator>

#if !defined(BOOST_UNICODE_ERROR_HPP)
# include <boost/unicode/error.hpp>
#endif

namespace boost
{
namespace unicode
{
namespace detail
{

template <class ToCharT, class ToTraits, class FromCharT, class FromTraits>
  std::basic_ostream<ToCharT, ToTraits>&
    inserter(std::basic_ostream<ToCharT, ToTraits>& os,
             boost::basic_string_view<FromCharT, FromTraits> v)
{
  return os << to_utf_string<ToCharT, FromCharT>(v, ufffd<ToCharT>());
}

} // namespace detail
} // namespace unicode
} // namespace boost


//--------------------------------------------------------------------------------------//
//                                                                                      //
//                         stream inserters and extractors                              //
//                                                                                      //
//     These need to be found by argument dependent lookup so go in namespace std       //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace std
{

//  basic_string_view overload

template <class ToCharT, class ToTraits, class FromCharT, class FromTraits>
inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
  basic_ostream<ToCharT, ToTraits>&>::type
    operator<<(basic_ostream<ToCharT, ToTraits>& os,
               boost::basic_string_view<FromCharT, FromTraits> v)
{
  return boost::unicode::detail::inserter(os, v);
}

//  basic_string overload

template <class ToCharT, class ToTraits, class FromCharT, class FromTraits, class Alloc>
inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
  basic_ostream<ToCharT, ToTraits>&>::type
    operator<<(basic_ostream<ToCharT, ToTraits>& os,
               const basic_string<FromCharT, FromTraits, Alloc>& s)
{
  return boost::unicode::detail::inserter(os,
    boost::basic_string_view<FromCharT, FromTraits>(s));
}

//  Character pointer overloads
//
//  Standard basic_ostream supplies this overload:
//
//    basic_ostream<charT,traits>& operator<<(const void* p);
//
//  That means pointers to types other than charT will select the void* overload rather
//  than a template <class Ostream, class InputIterator> overload.
//
//  As a fix, supply individual overloads for the ostreams and pointers we care about

inline basic_ostream<char>& operator<<(basic_ostream<char>& os, const wchar_t* p)
{
  boost::wstring_view v(p);
  return boost::unicode::detail::inserter(os, v);
}
//
//basic_ostream<char>& operator<<(basic_ostream<char>& os, const char16_t* p)
//{
//  return boost::unicode::detail::inserter(os, p);
//}
//
//basic_ostream<char>& operator<<(basic_ostream<char>& os, const char32_t* p)
//{
//  return boost::unicode::detail::inserter(os, p);
//}

}  // namespace std

#endif  // BOOST_UNICODE_STREAM_HPP
