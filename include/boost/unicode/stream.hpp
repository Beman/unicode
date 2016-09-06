//  boost/unicode/stream.hpp  ----------------------------------------------------------//

//  © Copyright Beman Dawes 2011, 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_STREAM_HPP)
# define BOOST_UNICODE_STREAM_HPP

#include <boost/unicode/string_encoding.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <ostream>
#include <iterator>

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                         stream inserters and extractors                              //
//                                                                                      //
//     These need to be found by argument dependent lookup so go in namespace std       //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// <!-- snippet=synopsis -->
namespace std
{

  //  basic_string_view overload
  template <class ToCharT, class ToTraits, class FromCharT, class FromTraits>
  inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
    basic_ostream<ToCharT, ToTraits>&>::type
      operator<<(basic_ostream<ToCharT, ToTraits>& os,
                 boost::basic_string_view<FromCharT, FromTraits> v);

  //  basic_string overload
  template <class ToCharT, class ToTraits, class FromCharT, class FromTraits,
            class Alloc>
  inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
    basic_ostream<ToCharT, ToTraits>&>::type
      operator<<(basic_ostream<ToCharT, ToTraits>& os,
                 const basic_string<FromCharT, FromTraits, Alloc>& s);

  //  pointer overload
  //    works despite the standard library's operator<<(const void* p) overload
  template <class ToCharT, class ToTraits, class FromCharT>
  inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
    basic_ostream<ToCharT, ToTraits>&>::type
      operator<<(basic_ostream<ToCharT, ToTraits>& os, const FromCharT* p);

}  // namespace std
// <!-- end snippet -->

//--------------------------------------------------------------------------------------//
//                               implementation                                         //
//--------------------------------------------------------------------------------------//

namespace boost
{
namespace unicode
{
namespace detail
{
    template <class CharT> struct default_encoding;
    template <> struct default_encoding<char>     { typedef utf8 tag; };
    template <> struct default_encoding<char16_t> { typedef utf16 tag; };
    template <> struct default_encoding<char32_t> { typedef utf32 tag; };
    template <> struct default_encoding<wchar_t>  { typedef wide  tag; };

template <class ToCharT, class ToTraits, class FromCharT, class FromTraits>
  inline std::basic_ostream<ToCharT, ToTraits>&
    inserter(std::basic_ostream<ToCharT, ToTraits>& os,
             boost::basic_string_view<FromCharT, FromTraits> v)
{
  return os << to_string<typename default_encoding<ToCharT>::tag>(v, ufffd<ToCharT>());
}

} // namespace detail
} // namespace unicode
} // namespace boost

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

//  pointer overload
//    works despite std basic_ostream<charT,traits>& operator<<(const void* p) overload

template <class ToCharT, class ToTraits, class FromCharT>
inline typename boost::enable_if_c<!boost::is_same<ToCharT, FromCharT>::value,
  basic_ostream<ToCharT, ToTraits>&>::type
    operator<<(basic_ostream<ToCharT, ToTraits>& os,
               const FromCharT* p)
{
  return boost::unicode::detail::inserter(os,
    boost::basic_string_view<FromCharT>(p));
}

}  // namespace std

#endif  // BOOST_UNICODE_STREAM_HPP
