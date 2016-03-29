//  boost/unicode/codecvt_conversion.hpp  ----------------------------------------------//

//  � Copyright Beman Dawes 2015, 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_CODECVT_CONVERSION_HPP)
#define BOOST_UNICODE_CODECVT_CONVERSION_HPP

#include <iterator>
#include <string>
#include <locale>
#include <boost/utility/string_view.hpp> 
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>

//--------------------------------------------------------------------------------------//
//  This interface is tailored to std::codecvt, particularly that codecvt::in and 
//  codecvt::out traffic in raw pointer ranges rather than iterators, and thus require
//  that both from and to ranges be contiguous. This create an impedance mismatch between
//  these codecvt functions and the  the STL "InputIterator first, InputIterator last,
//  OutputIterator result" model for arguments. Resolving that mismatch would require
//  an implementation to copy between both to and from arguments and internal buffers,
//  with unacceptable efficiency implications.
//
//  Q. Why not use std::wstring_convert?
//  A. Its error handling does not meet the recommendation of Unicode experts,
//     particularly the recommendation that the default for invalid characters be
//     replacement by U+FFFD to avoid security risks that have been used as attack
//     vectors.
//--------------------------------------------------------------------------------------//

/*

The initial development plan is to concentrate on char <---> wchar_t conversion via
the standard library's traditional codecvt<char16_t,char,mbstate_t> specialization.

If the interface can easily be extended to handle other codecvt specializations, fine.
But if that does not work out, codecvt<char16_t,char,mbstate_t> is sufficiently useful
that the attempt to support other specializations will be abandoned.

*/


namespace boost
{
namespace unicode
{
//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class ToCharT, class FromCharT, class Codecvt,
     class FromTraits = std::char_traits<FromCharT>,
     class Error = err_hdlr<ToCharT>,
     class ToTraits = std::char_traits<ToCharT>,
     class ToAlloc = std::allocator<ToCharT>,
     class View = boost::basic_string_view<FromCharT, FromTraits>>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt,
      Error eh = Error(), const ToAlloc& a = ToAlloc());

  //  codecvt_to_{string|wstring}  -----------------------------------------------------//

  template <class Error = err_hdlr<char>>
  inline std::string  codecvt_to_string(boost::wstring_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, 
    const Error eh = Error());

  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  codecvt_to_wstring(boost::string_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, 
    const Error eh = Error());

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//

namespace detail
{
  //  codecvt_to_basic_string ()
  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
  class Error, class ToTraits, class ToAlloc, class View>
    inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a,
      std::true_type);

  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
  class Error, class ToTraits, class ToAlloc, class View>
    inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a,
      std::false_type);

  //  //  codecvt_convert

  //template <class InputIterator, class OutputIterator, class Codecvt, class Error>
  //OutputIterator codecvt_convert(narrow, wide,
  //  InputIterator first, InputIterator last, OutputIterator result,
  //  const Codecvt& ccvt, Error eh)
  //{
  //  constexpr std::size_t in_size = 32;  // TODO increase this after initial testing
  //  constexpr std::size_t out_size = 32;  // TODO increase this after initial testing
  //  char in[in_size];
  //  const char* in_end = &in[0] + in_size;
  //  wchar_t out[out_size];
  //  std::mbstate_t mbstate  = std::mbstate_t();
  //  wchar_t* out_next;

  //  while (first != last)
  //  {
  //    //  fill the in buffer
  //    char* in_last = &in[0];
  //    for (; first != last && in_last != in_end; ++in_last, ++first)
  //      *in_last = *first;

  //    //  loop until the entire input buffer is processed by the codecvt facet; 
  //    //  required because the codecvt facet will not process the entire input sequence
  //    //  when an error occurs or the in buffer ended with only a portion of a multibyte
  //    //  character.
  //    const char* in_first = &in[0];
  //    for (; in_first != in_last;)
  //    {
  //      //  convert in buffer to out buffer
  //      const char* in_next;
  //      std::codecvt_base::result cvt_result
  //        = ccvt.in(mbstate, in, in_last, in_next, out, out + out_size, out_next);

  //      BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

  //      if (cvt_result == std::codecvt_base::error)
  //      {
  //        BOOST_ASSERT_MSG(false, "error handling not implemented yet");
  //      }

  //      //  Note: it is not necessary to further distinguish between an error, partial,
  //      //  or ok result. The distinctiion is already captured in the mbstate.

  //      //  copy out buffer to result
  //      for (const wchar_t* itr = &out[0]; itr != out_next; ++itr, ++result)
  //        *result = *itr;

  //      in_first = in_next;
  //    }

  //    //  process cvt_result ok, partial, error

  //  }

  //  return result;
  //}

    // template <class InputIterator, class OutputIterator, class Error> inline
    // OutputIterator codecvt_convert(wide, narrow,
    //  InputIterator first, InputIterator last, OutputIterator result,
    //  const Codecvt& ccvt, Error eh)
    //{
    //  constexpr std::size_t in_size = 20;  // TODO increase this after initial testing
    //  constexpr std::size_t out_size = 60;  // TODO increase this after initial testing
    //  wchar_t in[in_size];
    //  const wchar_t* in_end = &in[0] + in_size;
    //  char out[out_size];
    //  std::mbstate_t mbstate  = std::mbstate_t();
    //  char* out_next;

    //  while (first != last)
    //  {
    //    //  fill the in buffer
    //    wchar_t* in_last = &in[0];
    //    for (; first != last && in_last != in_end; ++in_last, ++first)
    //      *in_last = *first;

    //    //  loop until the entire input buffer is processed by the codecvt facet; 
    //    //  required because the codecvt facet will not process the entire input sequence
    //    //  when an error occurs or the in buffer ended with only a portion of a multibyte
    //    //  character.
    //    const wchar_t* in_first = &in[0];
    //    for (; in_first != in_last;)
    //    {
    //      //  convert in buffer to out buffer
    //      const wchar_t* in_next;
    //      std::codecvt_base::result cvt_result
    //        = ccvt.out(mbstate, in, in_last, in_next, out, out + out_size, out_next);

    //      BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

    //      if (cvt_result == std::codecvt_base::error)
    //      {
    //        BOOST_ASSERT_MSG(false, "error handling not implemented yet");
    //      }

    //      //  Note: it is not necessary to further distinguish between an error, partial,
    //      //  or ok result. The distinctiion is already captured in the mbstate.

    //      //  copy out buffer to result
    //      for (const char* itr = &out[0]; itr != out_next; ++itr, ++result)
    //        *result = *itr;

    //      in_first = in_next;
    //    }

    //    //  process cvt_result ok, partial, error

    //  }

    //  return result;
    //}

  } // namespace detail

  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
    class Error, class ToTraits, class ToAlloc, class View>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a)
  {
    return detail::codecvt_to_basic_string<ToCharT, FromCharT, Codecvt, FromTraits, Error,
      ToTraits, ToAlloc, View>(v, ccvt, eh, a,
        std::is_same<ToCharT, typename Codecvt::intern_type>());  // tag dispatch
  }

  //  codecvt_to_{string|wstring}  -----------------------------------------------------//

  template <class Error>
  inline std::string  codecvt_to_string(boost::wstring_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return codecvt_to_basic_string<char, wchar_t,
      std::codecvt<wchar_t, char, std::mbstate_t>>(v, ccvt, eh);
  }

  template <class Error>
  inline std::wstring  codecvt_to_wstring(boost::string_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return codecvt_to_basic_string<wchar_t, char,
      std::codecvt<wchar_t, char, std::mbstate_t>>(v, ccvt, eh);
  }


//namespace detail
//{
//  //  helper so static_assert can reference dependent type
//  template <class T>
//  constexpr bool fail() { return false; }
//
//}  // namespace detail

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_CODECVT_CONVERSION_HPP