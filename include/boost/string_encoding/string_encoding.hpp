//  boost/string_encoding/string_encoding.hpp  -----------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

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

  //     encoding       value-type

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

  typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;


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
    //  This implementation assumes that wide (i.e. wchar_t) character encoding is always
    //  UTF-16 or UTF-32. As a result, only conversions between the three UTF encodings
    //  need to be supported. The type-trait actual<t>::encoding folds wide encoding into
    //  utf16 or utf32 encoding depending upon the platform.
  
    template <class T> struct actual { typedef T encoding; };
# if WCHAR_MAX >= 0xFFFFFFFFu
    template<> struct encoding<wide> { typedef narrow encoding; };
# else
    template<> struct actual<wide> { typedef utf16 encoding; };
# endif

//--------------------------------------------------------------------------------------//
//                              recode implementation                                   //
//                         overloads selected by tag dispatch                           //
//--------------------------------------------------------------------------------------//
    
    constexpr ::boost::uint16_t high_surrogate_base = 0xD7C0u;
    constexpr ::boost::uint16_t low_surrogate_base = 0xDC00u;
    constexpr ::boost::uint32_t ten_bit_mask = 0x3FFu;

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf8,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to utf8" << endl;
      return std::copy(first, last, result);
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf16 to utf16" << endl;
      return std::copy(first, last, result);
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf32, utf32, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf32 to utf32" << endl;
      return std::copy(first, last, result);
    }

    //  utf-8 to utf-32 via finite state machine,
    //  inspired by Richard Gillam "Unicode Demystified", page 543

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to utf32" << endl;
      constexpr unsigned char states[4][32] =
      {
        // state table

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

        //   valid state values
        //
        //   0 = last, and possibly only, utf-8 character for the current code point;
        //   1, 2, 3 = more utf-8 input characters to come
        //   4 = illegal leading byte.
        //   5 = illegal trailing byte.

        BOOST_ASSERT_MSG(state >= 0 && state <= 5, "program logic error");

        switch (state)
        {
        case 0:  // last utf-8 input character for code point
          *result++ = u32 + (c & 0x7F);
          ++first;
          u32 = char32_t(0);
          mask = 0;
          break;

        case 1:  // more utf-8 input characters to come
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
          *result++ = 0xFFFD;  // report error
          state = 0;
          mask = 0;
          break;
        }
      }
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf16, utf32, 
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf16 to utf32" << endl;

      for (; first != last;)
      {
        char16_t c = *first++;

        if (c < 0xD800 || c > 0xDFFF)  // not a surrogate
          *result++ = c; // BMP
        //  verify we have a valid surrogate pair
        else if (first != last
                 && (c & 0xFC00) == 0xD800        // 0xD800 to 0xDBFF aka low surrogate
                 && (*first & 0xFC00) == 0xDC00)  // 0xDC00 to 0xDFFF aka high surrogate
        {
          // combine the surrogate pair into a single UTF-32 code point
          *result++ = (static_cast<char32_t>(c) << 10) + *first++ - 0x35FDC00;
        }
        else
        {
          *result++ = 0xFFFD;   // report error
          // note that first has already been incremented
          // cases: c was high surrogate          action: do not increment first again
          //        *first is not high surrogate  action: do not increment first again
          //        first == last                 action: do not increment first again
        }
      }
      return result;
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

    template <class InputIterator, class OutputIterator/*, class Error*/> inline
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
          *result++ = 0xEFu;
          *result++ = 0xBFu;
          *result++ = 0xBDu;
        }
      }
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to utf16" << endl;
      std::u32string tmp;
      detail::recode(utf8(), utf32(), first, last, std::back_inserter(tmp));
      return detail::recode(utf32(), utf16(), tmp.cbegin(), tmp.cend(), result);
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf16, utf8,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf16 to utf8" << endl;
      std::u32string tmp;
      detail::recode(utf16(), utf32(), first, last, std::back_inserter(tmp));
      return detail::recode(utf32(), utf8(), tmp.cbegin(), tmp.cend(), result);
    }

    //  recode_codecvt

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode_codecvt(narrow, wide,
      InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt /*, Error eh*/)
    {
      cout << "  narrow to wide" << endl;
      constexpr std::size_t buf_size = 20;  // TODO increase this after initial testing
      char in[buf_size];
      const char* in_end = &in[0] + buf_size;
      wchar_t out[buf_size];
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
        char* in_first = &in[0];
        for (; in_first != in_last;)
        {
          //  convert in buffer to out buffer
          const char* in_next;
          std::codecvt_base::result cvt_result
            = cvt.in(mbstate, in, in_last, in_next, out, out + buf_size, out_next);

          BOOST_ASSERT(cvt_result != std::codecvt_base::noconv);

          if (cvt_result == std::codecvt_base::error)
          {
            //BOOST_ASSERT_MSG(false, "error handling not implemented yet")
            wchar_t placeholder = eh()
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

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode_codecvt(wide, narrow,
      InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt /*, Error eh*/)
    {
      cout << "  wide to narrow" << endl;
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
    return detail::recode(typename detail::actual<FromEncoding>::encoding(),
      typename detail::actual<ToEncoding>::encoding(), first, last, result /*, eh*/);
  }

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class ErrorHandler = error_handler>
    inline OutputIterator
    recode_codecvt(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt /*, Error eh = error_handler()*/)
  {
    return detail::recode_codecvt(FromEncoding(), ToEncoding(), first, last, result,
      ccvt /*, eh*/);
  }

  //  make_recoded_string  -------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class FromTraits =
    std::char_traits<typename encoded<FromEncoding>::type>,
    class ToTraits = std::char_traits<typename encoded<ToEncoding>::type>,
    class ToAlloc = std::allocator<typename encoded<ToEncoding>::type> /*,
    class class Error*/>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_recoded_string(const boost::basic_string_ref<typename encoded<FromEncoding>::type,
      FromTraits>& v, const ToAlloc& a = ToAlloc())
  {
    std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    recode<FromEncoding, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp));
    return tmp;
  }

  template <class FromEncoding, class ToEncoding, class FromCharT, class FromTraits =
    std::char_traits<FromCharT>, class ToCharT, class ToTraits = std::char_traits<ToCharT>,
    class ToAlloc = std::allocator<ToCharT>/*, class Error*/>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    make_codecvted_string(const boost::basic_string_ref<FromCharT, FromTraits>& v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc())
  {
    std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    recode<FromEncoding, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), ccvt);
    return tmp;
  }

  //------------------------------------------------------------------------------------//
  //                                                                                    //
  //  convenience functions                                                             //
  //                                                                                    //
  //------------------------------------------------------------------------------------//

  //------------------------------------------------------------------------------------// 
  //  codecvt based conversions                                                         //
  //------------------------------------------------------------------------------------// 

  //  narrow to wide
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t>>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::basic_string_ref<char>& v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc())
  {
    std::cout << " narrow to_wide()" << std::endl;
    return make_codecvted_string<narrow, wide, ToTraits, ToAlloc>(v, ccvt, a);
  }

  //  wide to narrow
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_narrow(const boost::basic_string_ref<wchar_t>& v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc())
  {
    std::cout << " wide to_narrow()" << std::endl;
    return make_recoded_string<wide, narrow>(v, a);
  }

  //------------------------------------------------------------------------------------//
  //  Unicode Transformation Format (UTF) based conversions                             // 
  //------------------------------------------------------------------------------------//

  //  to_wide  -------------------------------------------------------------------------//

  //  narrow and utf8 to wide
  template <class FromEncoding,
    class ToTraits = std::char_traits<wchar_t>, class ToAlloc = std::allocator<wchar_t>>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::basic_string_ref<char>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char to_wide()" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, wide, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  //  wide to wide
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t>>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::basic_string_ref<wchar_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " wide to_wide()" << std::endl;
    return make_recoded_string<wide, wide>(v, a);
  }

  //  utf16 to wide
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t>>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::basic_string_ref<char16_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char16_t to_wide()" << std::endl;
    return make_recoded_string<utf16, wide>(v, a);
  }

  //  utf32 to wide
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t>>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::basic_string_ref<char32_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char32_t to_wide()" << std::endl;
    return make_recoded_string<utf32, wide>(v, a);
  }

  //  to_utf8  -------------------------------------------------------------------------//

  //  narrow and utf8 to utf8
  template <class FromEncoding,
    class ToTraits = std::char_traits<char>, class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::basic_string_ref<char>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char to_utf8()" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf8, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  //  wide to utf8
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::basic_string_ref<wchar_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " wchar_t to_utf8()" << std::endl;
    return make_recoded_string<wide, utf8>(v, a);
  }

  //  utf16 to utf8
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::basic_string_ref<char16_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char16_t to_utf8()" << std::endl;
    return make_recoded_string<utf16, utf8>(v, a);
  }

  //  utf32 to utf8
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::basic_string_ref<char32_t>& v,
      const ToAlloc& a = ToAlloc())
  {
    std::cout << " char32_t to_utf8()" << std::endl;
    return make_recoded_string<utf32, utf8>(v, a);
  }

  //  to_utf16  ------------------------------------------------------------------------//

  //  narrow and utf8 to utf16
  template <class FromEncoding,
    class ToTraits = std::char_traits<char16_t>, class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char to_utf16()" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf16, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }
 
  //  wide to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<wchar_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " wchar_t to_utf16()" << std::endl;
    return make_recoded_string<wide, utf16>(v, a);
  }

  //  utf16 to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char16_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char16_t to_utf16()" << std::endl;
    return make_recoded_string<utf16, utf16>(v, a);
  }

  //  utf32 to utf16
  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char32_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char32_t to_utf16()" << std::endl;
    return make_recoded_string<utf32, utf16>(v, a);
  }

  //  to_utf32  ------------------------------------------------------------------------//

  //  narrow and utf8 to utf32
  template <class FromEncoding,
    class ToTraits = std::char_traits<char32_t>, class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::basic_string_ref<char>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char to_utf32()" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf32, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  //  wide to utf32
  template <class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::basic_string_ref<wchar_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " wchar_t to_utf32()" << std::endl;
    return make_recoded_string<wide, utf32>(v, a);
  }

  //  utf16 to utf32
  template <class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::basic_string_ref<char16_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char16_t to_utf32()" << std::endl;
    return make_recoded_string<utf16, utf32>(v, a);
  }

  //  utf32 to utf32
  template <class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::basic_string_ref<char32_t>& v, const ToAlloc& a = ToAlloc())
  {
    std::cout << " char32_t to_utf32()" << std::endl;
    return make_recoded_string<utf32, utf32>(v, a);
  }

//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//

namespace detail
{
  //  helper so static_assert can reference dependent type
  template <class T>
  constexpr bool fail() { return false; }

}  // namespace detail

}  // namespace string_encoding
}  // namespace boost

#endif  // BOOST_STRING_ENCODING_HPP