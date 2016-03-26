//  boost/unicode/codecvt_conversion.hpp  ----------------------------------------------//

//  © Copyright Beman Dawes 2015

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

//  TODO:
//
//  * string inserters and extractors.  See the old string_interop repo.

//--------------------------------------------------------------------------------------//

namespace boost
{
namespace unicode
{

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  Error handler function object operator()() is called with no arguments and either
  //  throws an exception or returns a const pointer to a possibly empty C-style string.
  //  The returned string is UTF encode.

  //  default error handler:  function object returns a UTF encoded C-string of type
  //  charT with a  value of U+FFFD, encoded via ToEncoding.
  template <class charT> struct err_hdlr;
  template <> class err_hdlr<char>
    { public: const char* operator()() const noexcept { return u8"\uFFFD"; } };
  template <> class err_hdlr<char16_t>
    { public: const char16_t* operator()() const noexcept { return u"\uFFFD"; } };
  template <> class err_hdlr<char32_t>
    { public: const char32_t* operator()() const noexcept { return U"\uFFFD"; } };
  template <> class err_hdlr<wchar_t>
    { public: const wchar_t* operator()() const noexcept { return L"\uFFFD"; } };


  //------------------------------------------------------------------------------------//
  //                 codecvt based conversions between char and wchar_t                 // 
  //------------------------------------------------------------------------------------//

  //  codecvt_convert  -----------------------------------------------------------------//

  template <class ToEncoding, class InputIterator, class OutputIterator, class Error>
  inline OutputIterator
    codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh);

  template <class ToEncoding, class InputIterator, class OutputIterator>
  inline OutputIterator
    codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt);

  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class ToCharT, class FromTraits =
    std::char_traits<typename encoded<FromEncoding>::type>,
    class ToTraits = std::char_traits<typename encoded<ToEncoding>::type>,
    class ToAlloc = std::allocator<typename encoded<ToEncoding>::type>,
    class Error>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(boost::basic_string_view<FromCharT,
       std::char_traits<FromCharT>> v,
       Error eh, const ToAlloc& a);

  //  codecvt based conversion convenience functions  ----------------------------------//

  //  codecvt_to_wstring
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    codecvt_to_wstring(boost::string_view v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

  //   codecvt_to_string
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    codecvt_to_string(boost::wstring_view v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//

namespace detail
{
    //  codecvt_convert

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator codecvt_convert(narrow, wide,
      InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh)
    {
      constexpr std::size_t in_size = 32;  // TODO increase this after initial testing
      constexpr std::size_t out_size = 32;  // TODO increase this after initial testing
      char in[in_size];
      const char* in_end = &in[0] + in_size;
      wchar_t out[out_size];
      std::mbstate_t mbstate  = std::mbstate_t();
      wchar_t* out_next;

      while (first != last)
      {
        //  fill the in buffer
        char* in_last = &in[0];
        for (; first != last && in_last != in_end; ++in_last, ++first)
          *in_last = *first;

        //  loop until the entire input buffer is processed by the codecvt facet; 
        //  required because the codecvt facet will not process the entire input sequence
        //  when an error occurs or the in buffer ended with only a portion of a multibyte
        //  character.
        const char* in_first = &in[0];
        for (; in_first != in_last;)
        {
          //  convert in buffer to out buffer
          const char* in_next;
          std::codecvt_base::result cvt_result
            = ccvt.in(mbstate, in, in_last, in_next, out, out + out_size, out_next);

          BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

          if (cvt_result == std::codecvt_base::error)
          {
            BOOST_ASSERT_MSG(false, "error handling not implemented yet");
          }

          //  Note: it is not necessary to further distinguish between an error, partial,
          //  or ok result. The distinctiion is already captured in the mbstate.

          //  copy out buffer to result
          for (const wchar_t* itr = &out[0]; itr != out_next; ++itr, ++result)
            *result = *itr;

          in_first = in_next;
        }

        //  process cvt_result ok, partial, error

      }

      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator codecvt_convert(wide, narrow,
      InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh)
    {
      constexpr std::size_t in_size = 20;  // TODO increase this after initial testing
      constexpr std::size_t out_size = 60;  // TODO increase this after initial testing
      wchar_t in[in_size];
      const wchar_t* in_end = &in[0] + in_size;
      char out[out_size];
      std::mbstate_t mbstate  = std::mbstate_t();
      char* out_next;

      while (first != last)
      {
        //  fill the in buffer
        wchar_t* in_last = &in[0];
        for (; first != last && in_last != in_end; ++in_last, ++first)
          *in_last = *first;

        //  loop until the entire input buffer is processed by the codecvt facet; 
        //  required because the codecvt facet will not process the entire input sequence
        //  when an error occurs or the in buffer ended with only a portion of a multibyte
        //  character.
        const wchar_t* in_first = &in[0];
        for (; in_first != in_last;)
        {
          //  convert in buffer to out buffer
          const wchar_t* in_next;
          std::codecvt_base::result cvt_result
            = ccvt.out(mbstate, in, in_last, in_next, out, out + out_size, out_next);

          BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

          if (cvt_result == std::codecvt_base::error)
          {
            BOOST_ASSERT_MSG(false, "error handling not implemented yet");
          }

          //  Note: it is not necessary to further distinguish between an error, partial,
          //  or ok result. The distinctiion is already captured in the mbstate.

          //  copy out buffer to result
          for (const char* itr = &out[0]; itr != out_next; ++itr, ++result)
            *result = *itr;

          in_first = in_next;
        }

        //  process cvt_result ok, partial, error

      }

      return result;
    }

  } // namespace detail

  //  codecvt_convert  -----------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class Error>
  inline OutputIterator
    codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh)
  {
    return detail::codecvt_convert(FromEncoding(), ToEncoding(), first, last, result,
      ccvt, eh);
  }

  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class FromTraits, class ToTraits,
    class ToAlloc, class Error>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    codecvt_to_basic_string(
      const boost::basic_string_view<typename encoded<FromEncoding>::type, FromTraits>& v,
      const codecvt_type& ccvt, Error eh, const ToAlloc& a)
  {
    std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    codecvt_convert<FromEncoding, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), ccvt, eh);
    return tmp;
  }

  //  codecvt_to_{string|wstring} convenience functions  -------------------------------//

  template <class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    codecvt_to_wstring(boost::string_view v, const codecvt_type& ccvt,
      const ToAlloc& a)
  {
    return codecvt_to_basic_string<narrow, wide>(v, ccvt, a);
  }

  template <class ToTraits,  class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    codecvt_to_string(boost::wstring_view v, const codecvt_type& ccvt,
      const ToAlloc& a)
  {
    return codecvt_to_basic_string<wide, narrow>(v, ccvt, a);
  }

namespace detail
{
  //  helper so static_assert can reference dependent type
  template <class T>
  constexpr bool fail() { return false; }

}  // namespace detail

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_CODECVT_CONVERSION_HPP