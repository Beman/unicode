//  boost/unicode/string_encoding.hpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_STRING_ENCODING_HPP)
#define BOOST_UNICODE_STRING_ENCODING_HPP

#include <iterator>
#include <string>
#include <locale>
#include <array>
#include <boost/utility/string_view_fwd.hpp> 
#include <boost/utility/string_view.hpp> 
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>     // todo: remove me

#if !defined(BOOST_UNICODE_ERROR_HPP)
# include <boost/unicode/error.hpp>
#endif

//--------------------------------------------------------------------------------------//
//  This header deals with both Unicode Transformation Format (UTF) encodings and       //
//  non-UTF encodings of strings and other character sequences. Strings and sequences   //
//  of types char16_t, char32_t, and wchar_t are UTF encoded. Strings and sequences of  //
//  type char are UTF-8 encoded unless there is an associated codecvt argument, in      //
//  which case their encoding is determined by that associated codecvt argument, and    //
//  is commonly not a UTF encoding.                                                     //
//--------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//
// <!-- snippet=utfcvt -->
namespace boost
{
namespace unicode
{
  //     encoding tags
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

  template <class CharT> struct utf_encoding;
  template <> struct utf_encoding<char> { typedef utf8 type; };
  template <> struct utf_encoding<char16_t> { typedef utf16 type; };
  template <> struct utf_encoding<char32_t> { typedef utf32 type; };
    // It remains to be seen if WCHAR_MAX is a sufficient heuristic for determining the
    // encoding of wchar_t. The values below work for Windows and Ubuntu Linux.
# if WCHAR_MAX >= 0x1FFFFFFFu
    template <> struct utf_encoding<wchar_t>  { typedef utf32 type; };
# elif WCHAR_MAX >= 0x1FFFu
    template <> struct utf_encoding<wchar_t>  { typedef utf16 type; };
# else
    template <> struct utf_encoding<wchar_t>  { typedef utf8 type; };
# endif

  //typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

  //  convert_encoding UTF-to-UTF conversion algorithm

  //    converts between UTF-8, UTF-16, UTF-32, and wchar_t encodings;
  //    char encoding must be UTF-8
  //    char16_t encoding must be UTF-16
  //    char32_t encoding must be UTF-32
  //    wchar_t encoding must be UTF-8, UTF-16, or UTF-32
  //    Remarks: Errors in input encoding are detected, even when the input
  //    and output encodings are the same. The eh function object's returned sequence if
  //    any replaces the invalid input encoding in the output, even when the input and
  //    output encodings are the same. This implies that if the eh function object always
  //    returns a valid UTF character sequence, the overall function output sequence is
  //    a valid UTF sequence.
  //    Note: ToCharT cannot be inferred via
  //    std::iterator_traits<OutputIterator>::value_type
  //    because std::iterator_traits<OutputIterator>::value_type is void for
  //    OutputIterators such as back_inserter.
  template <class ToCharT, class InputIterator, class OutputIterator,
    class Error = ufffd<ToCharT>>
  inline OutputIterator
    convert_encoding(InputIterator first, InputIterator last, 
      OutputIterator result, Error eh = Error());

  //  to_utf_string generic UTF-to-UTF string conversion function

  //    Remarks: Performs the conversion by calling convert_encoding.
  //    Note: This implies that errors in input encoding are detected, even when the input
  //    and output encodings are the same. The eh function object's returned sequence if
  //    any replaces the invalid input encoding in the output, even when the input and
  //    output encodings are the same. This implies that if the eh function object always
  //    returns a valid UTF character sequence, the overall function output sequence is
  //    a valid UTF sequence.
  //    Note: See "Probe CharTraits template argument deduction" in test/smoke_test.cpp
  //    for why two overloads are needed for each form rather than providing a default
  //    template parameter for FromTraits. Short answer: With a default FromTraits
  //    parameter, template argument deduction fails.

  template <class ToCharT, class FromCharT,
    //class FromTraits = typename std::char_traits<FromCharT>,
    class Error = ufffd<ToCharT>,
    class ToTraits = std::char_traits<ToCharT>,
    class ToAlloc = std::allocator<ToCharT>>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    to_utf_string(boost::basic_string_view<FromCharT/*, FromTraits*/> v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  codecvt_to_basic_string
  template <class ToCharT, class FromCharT, class Codecvt,
    class FromTraits = std::char_traits<FromCharT>,
    class Error = ufffd<ToCharT>,
    class ToTraits = std::char_traits<ToCharT>,
    class ToAlloc = std::allocator<ToCharT>,
    class View = boost::basic_string_view<FromCharT, FromTraits>>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt,
                            Error eh=Error(), const ToAlloc& a=ToAlloc());
  
  //  UTF-to-UTF string-conversion convenience functions

  //    Remarks: Performs the converstion by calling to_utf_string
  //    Note: This implies that errors in input encoding are detected, even when the input
  //    and output encodings are the same. The eh function object's returned sequence, if
  //    any, replaces the invalid input encoding in the output, even when the input and
  //    output encodings are the same. This implies that if the eh function object always
  //    returns a valid UTF character sequence, the overall function output sequence is
  //    a valid UTF sequence.

  //  to_string from UTF
  template <class ToEncoding, class Error = ufffd<typename encoded<ToEncoding>::type>>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::string_view v, Error eh = Error());
  template <class ToEncoding, class Error = ufffd<typename encoded<ToEncoding>::type>>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::u16string_view v, Error eh = Error());
  template <class ToEncoding, class Error = ufffd<typename encoded<ToEncoding>::type>>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::u32string_view v, Error eh = Error());
  template <class ToEncoding, class Error = ufffd<typename encoded<ToEncoding>::type>>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::wstring_view v, Error eh = Error());

  //  narrow-to-narrow string-conversion convenience function
  template <class Error = ufffd<char>>
    inline std::string codecvt_to_string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& from_ccvt,
      const std::codecvt<wchar_t, char, std::mbstate_t>& to_ccvt, const Error eh=Error());

  //  UTF-to-narrow string-conversion convenience functions
  template <class Error = ufffd<char>>
    inline std::string codecvt_to_string(boost::string_view v,  // v is UTF-8 encoded
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<char>>
    inline std::string codecvt_to_string(boost::u16string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<char>>
    inline std::string codecvt_to_string(boost::u32string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<char>>
    inline std::string codecvt_to_string(boost::wstring_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());

  //  narrow-to-UTF string-conversion convenience functions
  template <class Error = ufffd<wchar_t>>
    inline std::string codecvt_to_u8string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<wchar_t>>
    inline std::u16string codecvt_to_u16string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<wchar_t>>
    inline std::u32string codecvt_to_u32string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());
  template <class Error = ufffd<wchar_t>>
    inline std::wstring codecvt_to_wstring(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh=Error());

}  // namespace unicode
}  // namespace boost
// <!-- end snippet -->

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//

namespace boost
{
namespace unicode
{
/*
UTF-32 (D90) extract:

* "Because surrogate code points are not included in the set of Unicode scalar values,
   UTF-32 code units in the range U+D800..U+DFFF are ill-formed."

* "Any UTF-32 code unit greater than U+10FFFF is ill-formed."

UTF-16 (D91) extract:

* "Because surrogate code points are not Unicode scalar values, isolated UTF-16
   code units in the range U+D800..U+DFFF are ill-formed."

UTD-8 (D92) extract:

* "Any UTF-8 byte sequence that does not match the patterns listed in Table 3-7 is
   ill-formed."

* "... “non-shortest form” byte sequences in UTF-8 ... are ill-formed ..."

* "Because surrogate code points are not Unicode scalar values, any UTF-8 byte
   sequence that would otherwise map to code points D800..DFFF is ill-formed."

Encoding Form Conversion (D93) extract:

* "A conformant encoding form conversion will treat any ill-formed code unit
   sequence as an error condition. ... it will neither interpret nor emit an ill-formed
   code unit sequence. Any implementation of encoding form conversion must take this
   requirement into account, because an encoding form conversion implicitly involves a
   verification that the Unicode strings being converted do, in fact, contain well-formed
   code unit sequences."

*/

//  convert_encoding helpers  ----------------------------------------------------------//

  namespace detail
  {
    //  For any conversion that uses utf32 as an intermediary,
    //  we need a value that can never appear in valid utf32 to pass the error through
    //  to the final output type and there be detected as an error and then processed
    //  by the appropriate error handler for that output type.
    struct u32_err_pass_thru { const char32_t* operator()() const { return U"\x110000"; } };

    //  handy constants
    constexpr char16_t high_surrogate_base = 0xD7C0u;
    constexpr char16_t low_surrogate_base = 0xDC00u;
    constexpr char32_t ten_bit_mask = 0x3FFu;

    //  forward declare functions used to implement u32_outputer
    template <class ToCharT, class OutputIterator, class Error> inline
      OutputIterator char32_t_to_utf16(char32_t u32, OutputIterator result, Error eh);
    template <class ToCharT, class OutputIterator, class Error> inline
      OutputIterator char32_t_to_utf8(char32_t u32, OutputIterator result, Error eh);

    //  char32_t outputers; these helpers take a single char32_t code point, and output as
    //  many code units as needed to represent the code point. OutputT may be wchar_t for
    //  one of the overloads; which one depends on the platform.
    template <class ToCharT, class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf32, char32_t x, OutputIterator result, Error)
    {
      *result++ = static_cast<ToCharT>(x);
      return result;
    }
    template <class ToCharT, class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf16, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_utf16<ToCharT>(x, result, eh);
    }
    template <class ToCharT, class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf8, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_utf8<ToCharT>(x, result, eh);
    }

//--------------------------------------------------------------------------------------//
//  Algorithms for converting UTF-8 and UTF-16 to and from a single UTF-32 encoded      //
//  char32_t; these can then be composed into complete conversion functions without     //
//  having to duplicate a lot of complex code and without the need for intermediate     //
//  strings. For example, in a UTF-8 to UTF-16 conversion.                              //
//--------------------------------------------------------------------------------------//
 
    template <class ToCharT, class OutputIterator, class Error>
    inline
    OutputIterator char32_t_to_utf8(char32_t u32, OutputIterator result, Error eh)
    {
      if (u32 <= 0x007Fu)
        *result++ = static_cast<ToCharT>(u32);
      else if (u32 <= 0x07FFu)
      {
        *result++ = static_cast<ToCharT>(0xC0u + (u32 >> 6));
        *result++ = static_cast<ToCharT>(0x80u + (u32 & 0x3Fu));
      }
      else if (u32 >= 0xD800u && u32 <= 0xDFFFu)  // surrogates are ill-formed
      {
        for (const char* rep = eh(); *rep; ++rep)
          *result++ = *rep;
      }
      else if (u32 <= 0xFFFFu)
      {
        *result++ = static_cast<ToCharT>(0xE0u + (u32 >> 12));
        *result++ = static_cast<ToCharT>(0x80u + ((u32 >> 6) & 0x3Fu));
        *result++ = static_cast<ToCharT>(0x80u + (u32 & 0x3Fu));
      }
      else if (u32 <= 0x10FFFFu)
      {
        *result++ = static_cast<ToCharT>(0xF0u + (u32 >> 18));
        *result++ = static_cast<ToCharT>(0x80u + ((u32 >> 12) & 0x3Fu));
        *result++ = static_cast<ToCharT>(0x80u + ((u32 >> 6) & 0x3Fu));
        *result++ = static_cast<ToCharT>(0x80u + (u32 & 0x3Fu));
      }
      else  // invalid code point
      {
        for (auto rep = eh(); *rep; ++rep)
          *result++ = *rep;
      }
      return result;
    }

    template <class ToCharT, class OutputIterator, class OutError>
    inline
    OutputIterator char32_t_to_utf16(char32_t u32, OutputIterator result,
      OutError out_eh)
    {
      if (u32 < 0xD800u || (u32 >= 0xE000u && u32 <=0xFFFFu))  // valid code point in BMP
      {
        *result++ = static_cast<ToCharT>(u32);  
      }
      else if (u32 >= 0x10000u && u32 <= 0x10FFFFu) // valid code point needing surrogate pair
      {
        *result++ = static_cast<ToCharT>(high_surrogate_base
          + static_cast<ToCharT>(u32 >> 10));
        *result++ = static_cast<ToCharT>(low_surrogate_base
          + static_cast<ToCharT>(u32 & ten_bit_mask));
      }
      else  // invalid code point
      {
        for (auto itr = out_eh(); *itr; ++itr)
          *result++ = *itr;
      }
      return result;
    }

    template <class ToCharT, class OutputIterator, class OutError>
    inline
    OutputIterator char32_t_to_utf32(char32_t u32, OutputIterator result,
      OutError out_eh)
    {
      if (u32 < 0xD800u || (u32 >= 0xE000u && u32 <= 0x10FFFFu))  // valid code point
      {
        *result++ = static_cast<ToCharT>(u32);  
      }
      else  // invalid code point
      {
        for (auto itr = out_eh(); *itr; ++itr)
          *result++ = *itr;
      }
      return result;
    }

    template <class ToCharT, class InputIterator, class OutputIterator,
      class U32Error, class OutError>
    inline
    OutputIterator utf8_to_char32_t(InputIterator first, InputIterator last,
      OutputIterator result, U32Error u32_eh, OutError out_eh)
    {
      typedef typename utf_encoding<ToCharT>::type encoding_tag;

      for (; first != last;)
      {
        char32_t u32 = static_cast<unsigned char>(*first++);

        if (u32 <= 0x7Fu)  // 7-bit ASCII
        {
          //  by definition, 7-bit ASCII is valid UTF-8, so bypass further checking
          result = u32_outputer<ToCharT>(encoding_tag(), u32, result, out_eh);
          continue;
        }

        int continues = 0;
        bool overlong = false;

        if ((u32 & 0xE0u) == 0xC0u)    // 2 byte sequence
        {
          u32 &= 0x1Fu;
          continues = 1;
          if ((u32 & 0xFEu) == 0xC0)   // overlong?
            overlong = true;
        }
        else if ((u32 & 0xF0u) == 0xE0u)  // 3 byte sequence
        {
          u32 &= 0x0Fu;
          continues = 2;
          if (u32 == 0xE0u               
            && first != last
            && (static_cast<unsigned char>(*first) & 0xE0u) == 0x80u)  // overlong?
            overlong = true;
        }
        else if ((u32 & 0xF8u) == 0xF0u)  // 4 byte sequence
        {
          u32 &= 0x07u;
          continues = 3;
          if (u32 == 0xF0u               
            && first != last
            && (static_cast<unsigned char>(*first) & 0xF0u) == 0x80u)  // overlong?
            overlong = true;
        }
        else
          continues = -1;  // flag as error

        //  process the continuation bytes
        //    requirement: increment past continuation bytes even if overlong 
        for (; continues > 0
          && first != last                                          // continuation byte
          && (static_cast<unsigned char>(*first) & 0xC0u) == 0x80u; //   not missing
          --continues)
        {
          u32 <<= 6;
          u32 += static_cast<unsigned char>(*first++) & 0x3Fu;
        }

        if (overlong                             // overlong sequence
          || continues != 0                      // missing continuation
          || u32 > 0x10FFFFu                     // out-of-range
          || (u32 >= 0xD800u && u32 <= 0xDFFFu)  // surrogate (which is ill-formed UTF-32)
          )
        {
          for (auto itr = u32_eh(); *itr; ++itr)
            result = u32_outputer<ToCharT>(encoding_tag(), *itr, result, out_eh);
        }
        else
          result = u32_outputer<ToCharT>(encoding_tag(), u32, result, out_eh);
      }
      return result;
    }

    template <class ToCharT, class InputIterator, class OutputIterator,
      class U32Error, class OutError>
    inline
      OutputIterator utf16_to_char32_t(InputIterator first, InputIterator last,
        OutputIterator result, U32Error u32_eh, OutError out_eh)
    {
      typedef typename utf_encoding<ToCharT>::type encoding_tag;

      for (; first != last;)
      {
        char32_t u32;

        char16_t c = *first++;

        if (c < 0xD800 || c > 0xDFFF)  // not a surrogate
        {
          u32 = c; // BMP
        }
        //  verify we have a valid surrogate pair
        else if (first != last
          && (c & 0xFC00) == 0xD800        // 0xD800 to 0xDBFF aka low surrogate
          && (*first & 0xFC00) == 0xDC00)  // 0xDC00 to 0xDFFF aka high surrogate
        {
          // combine the surrogate pair into a single UTF-32 code point
          u32 = (static_cast<char32_t>(c) << 10) + *first++ - 0x35FDC00;
        }
        else  // invalid code point
        {
          for (auto itr = u32_eh(); *itr; ++itr)
            result = u32_outputer<ToCharT>(encoding_tag(), *itr, result, out_eh);
          continue;  // no need to increment first; that has already been done above
          // cases: c was high surrogate          action: do not increment first again
          //        *first is not high surrogate  action: do not increment first again
          //        first == last                 action: do not increment first again
        }
        result = u32_outputer<ToCharT>(encoding_tag(), u32, result, out_eh);
      }
      return result;
    }

//--------------------------------------------------------------------------------------//
//                      detail::convert_encoding implementation                         //
//        overload resolution performed by tag dispatch on from_tag, to_tag             //
//--------------------------------------------------------------------------------------//

# if WCHAR_MAX >= 0x1FFFFFFFu
#   define BOOST_UNICODE_WIDE_UTF utf32
# elif WCHAR_MAX >= 0x1FFFu
#   define BOOST_UNICODE_WIDE_UTF utf16  
# else
#   define BOOST_UNICODE_WIDE_UTF utf8
# endif

    // from utf8 -----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf8 /*from*/, utf8 /*to*/,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf8_to_char32_t<char>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf8_to_char32_t<char16_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<char32_t>(first, last, result, eh, eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf8, wide,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<wchar_t>(first, last, result, eh, eh);
    }

    // from utf16 ----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf16, utf8,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf16_to_char32_t<char>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf16_to_char32_t<char16_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf16, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<char32_t>(first, last, result, eh, eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf16, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<wchar_t>(first, last, result, eh, eh);
    }

    // from utf32 ----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf32, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf8<char>(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf32, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf16<char16_t>(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf32, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf32<char32_t>(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(utf32, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
#       if WCHAR_MAX >= 0x1FFFFFFFu
        result = char32_t_to_utf32<wchar_t>(*first, result, eh);
#       elif WCHAR_MAX >= 0x1FFFu
        result = char32_t_to_utf16<wchar_t>(*first, result, eh);
#       else
        result = char32_t_to_utf8<wchar_t>(*first, result, eh);
#       endif
      }
      return result;
    }

    // from wide -----------------------------------------------------------------------//
 
    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(wide, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return convert_encoding(BOOST_UNICODE_WIDE_UTF(), utf8(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(wide, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return convert_encoding(BOOST_UNICODE_WIDE_UTF(), utf16(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(wide, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return convert_encoding(BOOST_UNICODE_WIDE_UTF(), utf32(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_encoding(wide, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return convert_encoding(BOOST_UNICODE_WIDE_UTF(), wide(), first, last, result, eh);
    }

  } // namespace detail

  //------------------------------------------------------------------------------------//
  //               Unicode Transformation Format (UTF) conversions                      //
  //                              implementation                                        //
  //------------------------------------------------------------------------------------//

  //  convert_encoding algorithm  -----------------------------------------------------------//

  template <class ToCharT, class InputIterator, class OutputIterator, class Error>
  inline OutputIterator
    convert_encoding(InputIterator first, InputIterator last, OutputIterator result, Error eh)
  {
    // tag dispatch to the specific conversion function
    return detail::convert_encoding(
      typename  // from encoding
       utf_encoding<typename std::iterator_traits<InputIterator>::value_type>::type(),
      typename  // to encoding
       utf_encoding<ToCharT>::type(),
      first, last, result, eh);
  }

  //  to_utf_string  -------------------------------------------------------------------//

  template <class ToCharT, class FromCharT,// class FromTraits,
    class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    to_utf_string(boost::basic_string_view<FromCharT/*, FromTraits*/> v, Error eh,
      const ToAlloc& a)
  {
    std::basic_string<ToCharT, ToTraits, ToAlloc> tmp(a);
    convert_encoding<ToCharT>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  //  to_string convenience functions  -------------------------------------------------//

  //  to_string from UTF
  template <class ToEncoding, class Error>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::string_view v, Error eh)
  { return to_utf_string<typename encoded<ToEncoding>::type, char, Error>(v, eh); }

  template <class ToEncoding, class Error>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::u16string_view v, Error eh)
  { return to_utf_string<typename encoded<ToEncoding>::type, char16_t, Error>(v, eh); }

  template <class ToEncoding, class Error>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::u32string_view v, Error eh)
  { return to_utf_string<typename encoded<ToEncoding>::type, char32_t, Error>(v, eh); }

  template <class ToEncoding, class Error>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      to_string(boost::wstring_view v, Error eh)
  { return to_utf_string<typename encoded<ToEncoding>::type, wchar_t, Error>(v, eh); }

  namespace detail
  {
    //  detail::codecvt_to_basic_string()
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
  }
  
  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
    class Error, class ToTraits, class ToAlloc, class View>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a)
  {
    return detail::codecvt_to_basic_string<ToCharT, FromCharT, Codecvt, FromTraits, Error,
      ToTraits, ToAlloc, View>(v, ccvt, eh, a,
        std::is_same<FromCharT, typename Codecvt::intern_type>());  // tag dispatch
  }

  //  convenience functions  -----------------------------------------------------------//

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

  template <class Error>
  inline std::string codecvt_to_string(boost::string_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& from_ccvt,
    const std::codecvt<wchar_t, char, std::mbstate_t>& to_ccvt, const Error eh)
  {
    return codecvt_to_string(codecvt_to_wstring(v, from_ccvt, eh), to_ccvt);
  }
  template <class Error>
    inline std::string codecvt_to_string(boost::string_view v,  // UTF-8 encoded
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return codecvt_to_string(to_string<wide>(v, eh), ccvt);
  }
  template <class Error>
    inline std::string codecvt_to_string(boost::u16string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return codecvt_to_string(to_string<wide>(v, eh), ccvt);
  }
  template <class Error>
    inline std::string codecvt_to_string(boost::u32string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return codecvt_to_string(to_string<wide>(v, eh), ccvt);
  }
  template <class Error>
    inline std::string codecvt_to_u8string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return to_string<utf8>(codecvt_to_wstring(v, ccvt, eh));
  }
  template <class Error>
    inline std::u16string codecvt_to_u16string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return to_string<utf16>(codecvt_to_wstring(v, ccvt, eh));
  }
  template <class Error>
    inline std::u32string codecvt_to_u32string(boost::string_view v,
      const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, const Error eh)
  {
    return to_string<utf32>(codecvt_to_wstring(v, ccvt, eh));
  }

//--------------------------------------------------------------------------------------//
//                              detail implementation                                   //
//--------------------------------------------------------------------------------------//

namespace detail
{
  //  detail::codecvt_to_basic_string()
  //
  //  Overview for both overloads:
  //
  //    Start with an empty output string.
  //    Until all the input has been processed:
  //      * Convert as many characters as will fit into an output buffer.
  //      * Append the characters in the output buffer into the output string.
  //    Return the output string.

  //  Overload for is_same<FromCharT, Codecvt::intern_type> == true_type
  //  This overload converts from Codecvt::internT (i.e. FromCharT, typically wchar_t)
  //  to Codecvt::externT (ToCharT, typically char) via ccvt.out()

  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
  class Error, class ToTraits, class ToAlloc, class View>
    inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a,
      std::true_type)
  {
    static_assert(std::is_same<FromCharT, typename Codecvt::intern_type>::value,
      "FromCharT and Codecvt::intern_type must be the same type");
    static_assert(std::is_same<ToCharT, typename Codecvt::extern_type>::value,
      "ToCharT and Codecvt::extern_type must be the same type");
    static_assert(!std::is_same<FromCharT, ToCharT>::value,
      "FromCharT and ToCharT must not be the same type");

    typedef std::basic_string<ToCharT, ToTraits, ToAlloc> string_type;
    string_type temp(a);
    std::array<ToCharT, 128> buf;  // TODO: increase size after preliminary testing

    //  for clarity, use the same names for ccvt.out() arguments as the standard library
    std::mbstate_t mbstate  = std::mbstate_t();
    const FromCharT* from = v.data();
    const FromCharT* from_end = from + v.size();
    const FromCharT* from_next;
    std::codecvt_base::result ccvt_result = std::codecvt_base::ok;

    //  loop until the entire input sequence is processed by the codecvt facet 

    while (from != from_end)
    {
      ToCharT* to = buf.data();
      ToCharT* to_end = to + buf.size();
      ToCharT* to_next = to;

      ccvt_result
        = ccvt.out(mbstate, from, from_end, from_next, to, to_end, to_next);

      if (ccvt_result == std::codecvt_base::ok)
      {
        temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
        temp.append(eh());
        from = from_next + 1;  // bypass error
      }
      else  // ccvt_result == std::codecvt_base::partial
      {
        if (to_next == buf.data())
        {
          temp.append(eh());
          from = from_end;
        }
        else
        {
          // eliminate the possibility that buf does not have enough room
          temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
          from = from_next;
        }
      }
    }
    return temp;
  }

  //  Overload for is_same<FromCharT, Codecvt::intern_type> == false_type
  //  This overload converts from Codecvt::externT (i.e. FromCharT, typically char)
  //  to Codecvt::internT (ToCharT, typically wchar_t) via ccvt.in()

  template <class ToCharT, class FromCharT, class Codecvt, class FromTraits,
  class Error, class ToTraits, class ToAlloc, class View>
    inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt, Error eh, const ToAlloc& a,
      std::false_type)
  {
    static_assert(std::is_same<FromCharT, typename Codecvt::extern_type>::value,
      "FromCharT and Codecvt::extern_type must be the same type");
    static_assert(std::is_same<ToCharT, typename Codecvt::intern_type>::value,
      "ToCharT and Codecvt::intern_type must be the same type");
    static_assert(!std::is_same<FromCharT, ToCharT>::value,
      "FromCharT and ToCharT must not be the same type");

    typedef std::basic_string<ToCharT, ToTraits, ToAlloc> string_type;
    string_type temp(a);
    std::array<ToCharT, 128> buf;  // TODO: increase size after preliminary testing

    //  for clarity, use the same names for ccvt.in() arguments as the standard library
    std::mbstate_t mbstate  = std::mbstate_t();
    const FromCharT* from = v.data();
    const FromCharT* from_end = from + v.size();
    const FromCharT* from_next;
    std::codecvt_base::result ccvt_result = std::codecvt_base::ok;

    //  loop until the entire input sequence is processed by the codecvt facet 

    while (from != from_end)
    {
      ToCharT* to = buf.data();
      ToCharT* to_end = to + buf.size();
      ToCharT* to_next = to;

      ccvt_result
        = ccvt.in(mbstate, from, from_end, from_next, to, to_end, to_next);

      if (ccvt_result == std::codecvt_base::ok)
      {
        temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
        temp.append(eh());
        from = from_next + 1;  // bypass error
      }
      else  // ccvt_result == std::codecvt_base::partial
      {
        if (to_next == buf.data())
        {
          temp.append(eh());
          from = from_end;
        }
        else
        {
          // eliminate the possibility that buf does not have enough room
          temp.append(to, static_cast<typename string_type::size_type>(to_next - to));
          from = from_next;
        }
      }
    }
    return temp;
  }

} // namespace detail
}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_STRING_ENCODING_HPP