#if !defined(BOOST_STRING_ENCODING_HPP)
#define BOOST_STRING_ENCODING_HPP

#include <iterator>
#include <string>
#include <boost/utility/string_ref.hpp> 
//#include <type_traits>
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>

//--------------------------------------------------------------------------------------//

//  TODO:
//
//  * string inserters and extractors.  See the old string_interop repo.
//  * Unicode algorithms on utf-8, 16, and 32 strings and string iterators, such as
//    utf_next(), utf_prior(), utf_advance(), utf_size()

//--------------------------------------------------------------------------------------//

namespace boost
{
namespace string_encoding
{

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  encodings

  struct narrow {};  // char
  struct wide {};    // wchar_t
  struct utf8 {};    // char
  struct utf16 {};   // char16_t
  struct utf32 {};   // char32_t


  //  encoding value_type type-trait

  template <class Encoding> struct encoded;
  template <> struct encoded<narrow> { typedef char type; };
  template <> struct encoded<wide>   { typedef wchar_t type; };
  template <> struct encoded<utf8>   { typedef char type; };
  template <> struct encoded<utf16>  { typedef char16_t type; };
  template <> struct encoded<utf32>  { typedef char32_t type; };


  //  Error handling  ------------------------------------------------------------------//

  //  Template template parameters named Error are error handling function object types
  //  taking two template parameters class charT and class OutputIterator.
  //  Requirements : if itr is an object of type OutputIterator and c is an object of type
  //  charT, *itr = c is well formed. if eh is an error handler function object, a call
  //  to eh(itr) may throw an exception, write a sequence of charT to itr, or do nothing.

  template <class charT, class OutputIterator>
  class err_hdlr;   // default error handler

  //  recode helpers -------------------------------------------------------------------//

  namespace detail
  {
    constexpr ::boost::uint16_t high_surrogate_base = 0xD7C0u;
    constexpr ::boost::uint16_t low_surrogate_base = 0xDC00u;
    constexpr ::boost::uint32_t ten_bit_mask = 0x3FFu;

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to utf16" << endl;
      return result;
    }

    //  utf-8 to utf-32 via finite state machine,
    //  inspired by Richard Gillam "Unicode Demystified", page 543

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to utf32" << endl;
      constexpr unsigned char X = -1;  // illegal leading byte
      constexpr unsigned char Y = -2;  // illegal trailing byte
      constexpr unsigned char states[4][32] =
      {
        //00 08 10 18 20 28 30 38 40 48 50 58 60 68 70 78
        // 80 88 90 98 A0 A8 B0 B8 C0 C8 D0 D8 E0 E8 F0 F8
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 2, 2, 3, 4 },
        { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
          0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5 },
        { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
          1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5 },
        { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
          2, 2, 2, 2, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5 }
      };
      constexpr unsigned char masks[4] = {0x7F, 0x1F, 0x0F, 0x07};

      int state = 0;
      unsigned char mask = 0u;
      char32_t u32 = char32_t(0);  // UTF-32 character being built up

      for (; first != last;)
      {
        int c = *first & 0xff;
        state = states[state][c >> 3];

        BOOST_ASSERT_MSG(state >= 0 && state <= 5, "program logic error");

        switch (state)
        {
        case 0:  // last utf-8 character for code point
          *result++ = u32 + (c & 0x7F);
          ++first;
          u32 = char32_t(0);
          mask = 0;
          break;

        case 1:
        case 2:
        case 3:
          if (mask == 0)
            mask = masks[state];
          u32 += c & mask;
          u32 <<= 6;
          mask = static_cast<unsigned char>(0x3F);
          ++first;
          break;

        case 4:
          ++first;
          // fall through
        case 5:
          *result++ = 0xFFFD;
          state = 0;
          mask = 0;
          break;
        }
      }
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(narrow, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  narrow to utf16" << endl;
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(wide, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  wide to utf16" << endl;
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf16 to utf16" << endl;
      return std::copy(first, last, result);
    }

    template <class InputIterator, class OutputIterator/*, class Error*/> inline
    OutputIterator recode(utf32, utf16, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf32 to utf16" << endl;

      for (; first != last;)
      {
        char32_t c = *first++;

        if (c < 0xD800u)
        {
          *result++ = static_cast<char16_t>(c);  // single code unit code point
        }

        else if (c < 0xDC00)
        {
          // c is low surrogate so must be followed by high surrogate
          if (first != last && *first >= 0xDC00 && *first <= 0xDFFF)
          {
            *result++ = static_cast<char16_t>(c);         // low surrogate
            *result++ = static_cast<char16_t>(*first++);  // high surrogate
          }
          else  // low surrogate was not followed by high surrogate
          {
            // treat as U+FFFD
            *result++ = static_cast<char16_t>(0xFFFDu);
          }
        }

        else if (c <= 0x10FFFFu)
        {
          // split into two surrogates:
          *result++ = detail::high_surrogate_base + static_cast<char16_t>(c >> 10);
          *result++ = detail::low_surrogate_base
            + static_cast<char16_t>(c & detail::ten_bit_mask);
        }
        else  // invalid code point
        {
          // treat as U+FFFD
          *result++ = static_cast<char16_t>(0xFFFDu);
        }
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf32, utf8, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf32 to utf8" << endl;
      
      for (; first != last; ++first)
      {
        char32_t c = *first;

        if (c <= 0x007Fu)
          *result++ = static_cast<unsigned char>(c);
        else if (c <= 0x07FFu)
        {
          *result++ = static_cast<unsigned char>(0xC0u + (c >> 6));
          *result++ = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
        }
        else if (c <= 0xFFFFu)
        {
          *result++ = static_cast<unsigned char>(0xE0u + (c >> 12));
          *result++ = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
          *result++ = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
        }
        else if (c <= 0x10FFFFu)
        {
          *result++ = static_cast<unsigned char>(0xF0u + (c >> 18));
          *result++ = static_cast<unsigned char>(0x80u + ((c >> 12) & 0x3Fu));
          *result++ = static_cast<unsigned char>(0x80u + ((c >> 6) & 0x3Fu));
          *result++ = static_cast<unsigned char>(0x80u + (c & 0x3Fu));
        }
        else  // report invalid code point as U+FFFD
        {
          *result++ = 0xEF;
          *result++ = 0xBF;
          *result++ = 0xBD;
        }
      }
      return result;
    }

  } // namespace detail

  //  recode  --------------------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator/*,
    class Error = err_hdlr<encoding<ToEncoding>::value_type, OutputIterator>*/>
  inline OutputIterator
    recode(InputIterator first, InputIterator last, OutputIterator result /*,
      Error eh = Error()*/)
    {
      return detail::recode(FromEncoding(), ToEncoding(), first, last, result /*, eh*/);
    }

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //class OutputIterator, class ErrorHandler = error_handler>
  //  OutputIterator
  //  recode_codecvt(InputIterator first, InputIterator last, OutputIterator result,
  //    const std::codecvt<wchar_t, char>& loc, Error eh = error_handler());

  //  make_recoded_string  -------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class FromTraits =
    std::char_traits<encoded<FromEncoding>::type>,
    class ToTraits = std::char_traits<encoded<ToEncoding>::type>,
    class ToAlloc = std::allocator<encoded<ToEncoding>::type> /*,
    class class Error*/>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_recoded_string(const boost::basic_string_ref<typename encoded<FromEncoding>::type,
      FromTraits>& v,
      /*Error eh = Error<ToCharT, OutputIterator>,*/
      const ToAlloc& a = std::allocator<typename encoded<ToEncoding>::type>())
  {
    std::basic_string<encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    recode<FromEncoding, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp) /*, eh*/);
    return tmp;
  }

  //template <class FromEncoding, class ToEncoding, class FromCharT, class FromTraits =
  //  std::char_traits<FromCharT>, class ToCharT, class  ToTraits = std::char_traits<ToCharT>,
  //class ToAlloc = std::allocator<ToCharT>, class class Error>
  //  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
  //  make_recoded_string_via_codecvt(const boost::basic_string_ref<FromCharT, FromTraits>& v,
  //    Error eh = Error<ToCharT, OutputIterator>, const std::codecvt<wchar_t, char>&);

  //  convenience functions  -----------------------------------------------------------//

  //  narrow and utf8 to utf16
  template <class FromEncoding,
    class ToTraits = std::char_traits<char16_t>, class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16() from char" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf16, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  //  narrow and utf8 to utf32
  template <class FromEncoding,
    class ToTraits = std::char_traits<char32_t>, class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::basic_string_ref<char>& v,
      const ToAlloc& a = std::allocator<char32_t>())
  {
    std::cout << " to_utf32() from char" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf32, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  //  wide to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<wchar_t>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16() from wchar_t" << std::endl;
    return make_recoded_string<wide, utf16>(v, a);
  }

  //  utf16 to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char16_t>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16() from char16_t" << std::endl;
    return make_recoded_string<utf16, utf16>(v, a);
  }

  //  utf32 to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char32_t>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16() from char32_t" << std::endl;
    return make_recoded_string<utf32, utf16>(v, a);
  }



//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//

namespace detail
{
  //  helper so static_assert can reference dependent type
  template <class T>
  constexpr bool fail() { return false; }

  ////  primary template handles the error cases  ----------------------------------------//
  ////
  ////  Initial implementatioon only handles narrow to wide, and wide to narrow, so these
  ////  cases will fail:
  ////    narrow, narrow
  ////    narrow, utf8
  ////    narrow, utf16
  ////    narrow, utf32
  ////    uft8, narrow
  ////    utf16, narrow
  ////    utf32, narrow

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //class OutputIterator, class ErrorHandler = error_handler>
  //  OutputIterator
  //  recode(InputIterator, InputIterator, OutputIterator,
  //    const std::locale&, ErrorHandler)
  //{
  //  static_assert(detail::fail<FromEncoding>(), "Invalid template parameters");
  //}

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //class OutputIterator, class ErrorHandler = error_handler>
  //  OutputIterator
  //  recode(InputIterator, InputIterator, OutputIterator, ErrorHandler)
  //{
  //  static_assert(detail::fail<FromEncoding>(), "Invalid template parameters");
  //}
  //  
  ////  non-converting cases  ------------------------------------------------------------//
  //
  ////  narrow to narrow
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //inline OutputIterator recode(narrow, narrow,
  //  InputIterator first, InputIterator last, OutputIterator result, ErrorHandler)
  //    { return std::copy(first, last, result); }
  ////  wide to wide
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //inline OutputIterator recode(narrow, narrow,
  //  InputIterator first, InputIterator last, OutputIterator result, ErrorHandler)
  //    { return std::copy(first, last, result); }
  ////  utf-8 to utf-8
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //inline OutputIterator recode(narrow, narrow,
  //  InputIterator first, InputIterator last, OutputIterator result, ErrorHandler)
  //    { return std::copy(first, last, result); }
  ////  utf16 to utf16
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //inline OutputIterator recode(narrow, narrow,
  //  InputIterator first, InputIterator last, OutputIterator result, ErrorHandler)
  //    { return std::copy(first, last, result); }
  ////  utf32 to utf32
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //inline OutputIterator recode(narrow, narrow,
  //  InputIterator first, InputIterator last, OutputIterator result, ErrorHandler)
  //    { return std::copy(first, last, result); }

  ////  converting cases------------------------------------------------------------------//

  ////  narrow to wide
  //template <class InputIterator, class OutputIterator, class ErrorHandler>
  //OutputIterator recode(narrow, wide,
  //  InputIterator first, InputIterator last, OutputIterator result,
  //  const std::locale& loc, ErrorHandler eh)
  //{
  //  constexpr std::size_t buf_size = 20;  // TODO increase this after initial testing
  //  char in[buf_size];
  //  const char* in_end = &in[0] + buf_size;
  //  wchar_t out[buf_size];
  //  const std::codecvt<wchar_t,char,mbstate_t>& cvt =
  //    std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(loc);
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
  //    char* in_first = &in[0];
  //    for (; in_first != in_last;)
  //    {
  //      //  convert in buffer to out buffer
  //      const char* in_next;
  //      std::codecvt_base::result cvt_result
  //        = cvt.in(mbstate, in, in_last, in_next, out, out + buf_size, out_next);

  //      BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

  //      if (cvt_result == std::codecvt_base::error)
  //      {
  //        //BOOST_ASSERT_MSG(false, "error handling not implemented yet")
  //        wchar_t placeholder = eh()
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

}  // namespace detail

  //struct error_handler   // default error handler
  //{
  //  void operator()() const { throw "barf"; }
  //};

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //  class OutputIterator, class ErrorHandler>
  //OutputIterator
  //recode(InputIterator first, InputIterator last, OutputIterator result,
  //      const std::locale& loc, ErrorHandler eh)
  //{
  //  return detail::recode(FromEncoding(), ToEncoding(), first, last, result, loc, eh);
  //}

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //  class OutputIterator, class ErrorHandler>
  //OutputIterator
  //recode(InputIterator first, InputIterator last, OutputIterator result,
  //  ErrorHandler eh)
  //{
  //  return recode<FromEncoding, ToEncoding>(first, last, result, std::locale(), eh);
  //}
}  // namespace string_encoding
}  // namespace boost

#endif  // BOOST_STRING_ENCODING_HPP