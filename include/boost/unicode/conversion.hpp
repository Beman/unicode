//  boost/unicode/conversion.hpp  ------------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_HPP)
#define BOOST_UNICODE_HPP

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

  //   encodings
  struct narrow {};  // char
  struct wide {};    // wchar_t; synonym for utf8/16/32 depending on platform
  struct utf8 {};    // char
  struct utf16 {};   // char16_t
  struct utf32 {};   // char32_t

  //  utf_encoding type-trait
  template <class CharT> struct utf_encoding;
  template <> struct utf_encoding<char> { typedef utf8 type; };
  template <> struct utf_encoding<char16_t> { typedef utf16 type; };
  template <> struct utf_encoding<char32_t> { typedef utf32 type; };
  // It remains to be seen if WCHAR_MAX is a sufficient heuristic for determining the
  // encoding of wchar_t.
# if WCHAR_MAX >= 0xFFFFFFFFu
  template <> struct utf_encoding<wchar_t> { typedef utf32 type; };
# elif WCHAR_MAX >= 0xFFFFu
  template <> struct utf_encoding<wchar_t> { typedef utf16 type; };
# else
  template <> struct utf_encoding<wchar_t> { typedef char type; };
# endif

  //  encoding_traits type-trait
  template <class Encoding> struct encoding_traits;
  template <> struct encoding_traits<narrow> { typedef char value_type; };
  template <> struct encoding_traits<wide>   { typedef wchar_t value_type; };
  template <> struct encoding_traits<utf8>   { typedef char value_type; };
  template <> struct encoding_traits<utf16>  { typedef char16_t value_type; };
  template <> struct encoding_traits<utf32>  { typedef char32_t value_type; };

  typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

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
  //               Unicode Transformation Format (UTF) conversions                      // 
  //------------------------------------------------------------------------------------//

  //  convert_utf UTF conversion algorithms  -------------------------------------------//

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
  //    Note: ToEncoding cannot be inferred via
  //    utf_encoding<typename std::iterator_traits<OutputIterator>::value_type>::type
  //    because std::iterator_traits<OutputIterator>::value_type is void for some
  //    OutputIterators such as back_inserter.
  template <class ToEncoding, class InputIterator, class OutputIterator, class Error>
  inline OutputIterator
    convert_utf(InputIterator first, InputIterator last, 
      OutputIterator result, Error eh);

  template <class ToEncoding, class InputIterator, class OutputIterator>
  inline OutputIterator
    convert_utf(InputIterator first, InputIterator last, OutputIterator result);

  //  to_utf_string generic UTF string conversion functions  ---------------------------//

  //    Remarks: Performs the conversion by calling convert_utf.
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

  //  three argument forms
  template <class ToCharT, class FromCharT, class FromTraits,  // explicit FromTraits
     class Error = err_hdlr<ToCharT>,
     class ToTraits = std::char_traits<ToCharT>,
     class ToAlloc = std::allocator<ToCharT>>
   inline std::basic_string<ToCharT, ToTraits, ToAlloc>
     to_utf_string(boost::basic_string_view<FromCharT, FromTraits> v,
       Error eh, const ToAlloc& a);

  template <class ToCharT, class FromCharT,  // default FromTraits
     class Error = err_hdlr<ToCharT>,
     class ToTraits = std::char_traits<ToCharT>,
     class ToAlloc = std::allocator<ToCharT>>
   inline std::basic_string<ToCharT, ToTraits, ToAlloc>
     to_utf_string(boost::basic_string_view<FromCharT> v,
       Error eh, const ToAlloc& a)
     {
       return to_utf_string<ToCharT, FromCharT, typename std::char_traits<FromCharT>,
         Error, ToTraits, ToAlloc>(v, eh, a);
     }
  
  //  two argument forms
  template <class ToCharT, class FromCharT, class FromTraits,  // explicit FromTraits
     class Error>
   inline std::basic_string<ToCharT>
     to_utf_string(boost::basic_string_view<FromCharT, FromTraits> v, Error eh)
     {
       return to_utf_string<ToCharT, FromCharT, FromTraits>(
         v, eh, std::allocator<ToCharT>());
     }
  template <class ToCharT, class FromCharT,  // default FromTraits
     class Error>
   inline std::basic_string<ToCharT>
     to_utf_string(boost::basic_string_view<FromCharT> v, Error eh)
     {
       return to_utf_string<ToCharT, FromCharT>(v, eh, std::allocator<ToCharT>());
     }
  
  //  one argument forms
  template <class ToCharT, class FromCharT, class FromTraits>
   inline std::basic_string<ToCharT>
     to_utf_string(boost::basic_string_view<FromCharT, FromTraits> v)
     {
       return to_utf_string<ToCharT, FromCharT, FromTraits>(v,
         err_hdlr<ToCharT>(), std::allocator<ToCharT>());
     }
  template <class ToCharT, class FromCharT>  // default FromTraits
   inline std::basic_string<ToCharT>
     to_utf_string(boost::basic_string_view<FromCharT> v)
     {
       return to_utf_string<ToCharT, FromCharT>(v,
         err_hdlr<ToCharT>(), std::allocator<ToCharT>());
     }
  
  //  to_*string convenience functions for UTF string conversions  ---------------------//

  //    Remarks: Performs the converstion by calling to_utf_string
  //    Note: This implies that errors in input encoding are detected, even when the input
  //    and output encodings are the same. The eh function object's returned sequence if
  //    any replaces the invalid input encoding in the output, even when the input and
  //    output encodings are the same. This implies that if the eh function object always
  //    returns a valid UTF character sequence, the overall function output sequence is
  //    a valid UTF sequence.

  template <class Error = err_hdlr<char>>
  inline std::string  to_u8string(boost::string_view v, Error eh = Error());
  template <class Error = err_hdlr<char>>
  inline std::string  to_u8string(boost::u16string_view v, Error eh = Error());
  template <class Error = err_hdlr<char>>
  inline std::string  to_u8string(boost::u32string_view v, Error eh = Error());
  template <class Error = err_hdlr<char>>
  inline std::string  to_u8string(boost::wstring_view v, Error eh = Error());

  template <class Error = err_hdlr<char16_t>>
  inline std::u16string  to_u16string(boost::string_view v, Error eh = Error());
  template <class Error = err_hdlr<char16_t>>
  inline std::u16string  to_u16string(boost::u16string_view v, Error eh = Error());
  template <class Error = err_hdlr<char16_t>>
  inline std::u16string  to_u16string(boost::u32string_view v, Error eh = Error());
  template <class Error = err_hdlr<char16_t>>
  inline std::u16string  to_u16string(boost::wstring_view v, Error eh = Error());

  template <class Error = err_hdlr<char32_t>>
  inline std::u32string  to_u32string(boost::string_view v, Error eh = Error());
  template <class Error = err_hdlr<char32_t>>
  inline std::u32string  to_u32string(boost::u16string_view v, Error eh = Error());
  template <class Error = err_hdlr<char32_t>>
  inline std::u32string  to_u32string(boost::u32string_view v, Error eh = Error());
  template <class Error = err_hdlr<char32_t>>
  inline std::u32string  to_u32string(boost::wstring_view v, Error eh = Error());

  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  to_wstring(boost::string_view v, Error eh = Error());
  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  to_wstring(boost::u16string_view v, Error eh = Error());
  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  to_wstring(boost::u32string_view v, Error eh = Error());
  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  to_wstring(boost::wstring_view v, Error eh = Error());

  //------------------------------------------------------------------------------------//
  //                 codecvt based conversions between char and wchar_t                 // 
  //------------------------------------------------------------------------------------//

  ////  codecvt_convert  -----------------------------------------------------------------//

  //template <class ToEncoding, class InputIterator, class OutputIterator, class Error>
  //inline OutputIterator
  //  codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
  //    const codecvt_type& ccvt, Error eh);

  //template <class ToEncoding, class InputIterator, class OutputIterator>
  //inline OutputIterator
  //  codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
  //    const codecvt_type& ccvt);

  ////  codecvt_to_generic_string  -------------------------------------------------------//

  //template <class ToCharT, class FromTraits =
  //  std::char_traits<typename encoded<FromEncoding>::type>,
  //  class ToTraits = std::char_traits<typename encoded<ToEncoding>::type>,
  //  class ToAlloc = std::allocator<typename encoded<ToEncoding>::type>,
  //  class Error>
  //inline std::basic_string<ToCharT, ToTraits, ToAlloc>
  //  codecvt_to_generic_string(boost::basic_string_view<FromCharT,
  //     std::char_traits<FromCharT>> v,
  //     Error eh, const ToAlloc& a);

  ////  codecvt based conversion convenience functions  ----------------------------------//

  ////  codecvt_to_wstring
  //template <class ToTraits = std::char_traits<wchar_t>,
  //  class ToAlloc = std::allocator<wchar_t >>
  //inline std::basic_string<wchar_t, ToTraits, ToAlloc>
  //  codecvt_to_wstring(boost::string_view v,
  //    const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

  ////   codecvt_to_string
  //template <class ToTraits = std::char_traits<char>,
  //  class ToAlloc = std::allocator<char>>
  //inline std::basic_string<char, ToTraits, ToAlloc>
  //  codecvt_to_string(boost::wstring_view v,
  //    const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//

  //  default error handlers
  //
  //  The Unicode standard, Conformance chapter, requires conversion functions detect
  //  ill-formed code points and treat them as errors. It defines what constitutes 
  //  ill-formed encoding in excruciating detail. It further mandates reporting errors via
  //  a single code point replacement character and strongly recomments U+FFFD.
  //  It provides rationale for these requirements that boils down to any other approach
  //  as a default, including throwing exceptions, can be and has been used as a security
  //  attack vector.

  //template <> class err_hdlr<narrow>
  //  { public: const char* operator()() const noexcept { return "?"; } };
  //template <> class err_hdlr<wchar_t>
  //  { public: const wchar_t* operator()() const noexcept { return L"\uFFFD"; } };

/*
Unicode Standard Conformance (as of version 7.0)

References: For example, (3.4 D10) refers to the Unicode Standard, Core Specification,
chapter 3, section 4, sub-section D10. See http://www.unicode.org/versions/latest/

Definitions:

* Code point (3.4 D10): "Code point: Any value in the Unicode codespace."

  Informally, a code point can be thought of as a Unicode character.

  (Appendix A - Notational Conventions):
  "In running text, an individual Unicode code point is expressed as U+n, where n is four
   to six hexadecimal digits, using the digits 0–9 and uppercase letters A–F (for 10
   through 15, respectively). Leading zeros are omitted, unless the code point would have
   fewer than four hexadecimal digits—for example, U+0001, U+0012, U+0123, U+1234,
   U+12345, U+102345.

   [e.g] U+0416 is the Unicode code point for the character named CYRILLIC CAPITAL
   LETTER ZHE."

* Code unit (3.9 D77): "The minimal bit combination that can represent a unit of encoded text
  for processing or interchange. Code units are particular units of computer storage. ...
  The Unicode Standard uses 8-bit code units in the UTF-8 encoding form, 16-bit
  code units in the UTF-16 encoding form, and 32-bit code units in the UTF-32
  encoding form."

  In C++: one char, wchar_t, char16_t, or char32_t character holds one code unit.
  One to four code units (type char) are required to hold a UTF-8 encodee code point.
  One or two code units (type char16_t) are required to hold a UTF-16 encoded code point.
  One code unit (type char32_t) is required to hold a UTF-32 code point.

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

//  convert_utf helpers ---------------------------------------------------------------------//

  namespace detail
  {
    //  For the utf8<-->utf16 encoding conversion, which uses utf32 as an intermediary,
    //  we need a value that can never appear in valid utf32 to pass the error through
    //  to the final output type and there be detected as an error and then processed
    //  by the appropriate error handler for the output type.
    struct err_pass_thru { const char32_t* operator()() const { return U"\x110000"; } };

    //  handy constants
    constexpr char16_t high_surrogate_base = 0xD7C0u;
    constexpr char16_t low_surrogate_base = 0xDC00u;
    constexpr char32_t ten_bit_mask = 0x3FFu;

//--------------------------------------------------------------------------------------//
//  Algorithms for converting UTF-8 and UTF-16 to and from a single UTF-32 encoded      //
//  char32_t; these can then be composed into complete conversion functions without     //
//  having to duplicate a lot of complex code and without the need for intermediate     //
//  strings, such as in a UTF-8 to UTF-16 conversion.                                   //
//--------------------------------------------------------------------------------------//
 
    template <class OutputIterator, class Error>
    inline
    OutputIterator char32_t_to_u8string(char32_t u32, OutputIterator result, Error eh)
    {
      if (u32 <= 0x007Fu)
        *result++ = static_cast<unsigned char>(u32);
      else if (u32 <= 0x07FFu)
      {
        *result++ = static_cast<unsigned char>(0xC0u + (u32 >> 6));
        *result++ = static_cast<unsigned char>(0x80u + (u32 & 0x3Fu));
      }
      else if (u32 >= 0xD800u && u32 <= 0xDFFFu)  // surrogates are ill-formed
      {
        for (const char* rep = eh(); *rep; ++rep)
          *result++ = *rep;
      }
      else if (u32 <= 0xFFFFu)
      {
        *result++ = static_cast<unsigned char>(0xE0u + (u32 >> 12));
        *result++ = static_cast<unsigned char>(0x80u + ((u32 >> 6) & 0x3Fu));
        *result++ = static_cast<unsigned char>(0x80u + (u32 & 0x3Fu));
      }
      else if (u32 <= 0x10FFFFu)
      {
        *result++ = static_cast<unsigned char>(0xF0u + (u32 >> 18));
        *result++ = static_cast<unsigned char>(0x80u + ((u32 >> 12) & 0x3Fu));
        *result++ = static_cast<unsigned char>(0x80u + ((u32 >> 6) & 0x3Fu));
        *result++ = static_cast<unsigned char>(0x80u + (u32 & 0x3Fu));
      }
      else  // invalid code point
      {
        for (const char* rep = eh(); *rep; ++rep)
          *result++ = *rep;
      }
      return result;
    }

    template <class OutputIterator, class OutError>
    inline
    OutputIterator char32_t_to_u16string(char32_t u32, OutputIterator result, OutError out_eh)
    {
      if (u32 < 0xD800u || (u32 >= 0xE000u && u32 <=0xFFFFu))  // valid code point in BMP
      {
        *result++ = static_cast<char16_t>(u32);  
      }
      else if (u32 >= 0x10000u && u32 <= 0x10FFFFu) // valid code point needing surrogate pair
      {
        *result++ = static_cast<char16_t>(high_surrogate_base
          + static_cast<char16_t>(u32 >> 10));
        *result++ = static_cast<char16_t>(low_surrogate_base
          + static_cast<char16_t>(u32 & ten_bit_mask));
      }
      else  // invalid code point
      {
        for (auto itr = out_eh(); *itr; ++itr)
          *result++ = *itr;
      }
      return result;
    }

    //  char32_t outputers
    template <class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf32, char32_t x, OutputIterator result, Error)
    {
      *result++ = x;
      return result;
    }
    template <class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf16, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_u16string(x, result, eh);
    }
    template <class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf8, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_u8string(x, result, eh);
    }

    template <class OutEncoding, class InputIterator, class OutputIterator,
      class U32Error, class OutError>
    inline
    OutputIterator utf8_to_char32_t(InputIterator first, InputIterator last,
      OutputIterator result, U32Error u32_eh, OutError out_eh)
    {

      for (; first != last;)
      {
        char32_t u32 = static_cast<unsigned char>(*first++);

        if (u32 <= 0x7Fu)  // 7-bit ASCII
        {
          //  by definition, 7-bit ASCII is valid UTF-8, so bypass further checking
          result = u32_outputer(OutEncoding(), u32, result, out_eh);
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
          for (const char32_t* itr = u32_eh(); *itr; ++itr)
            result = u32_outputer(OutEncoding(), *itr, result, out_eh);
        }
        else
          result = u32_outputer(OutEncoding(), u32, result, out_eh);
      }
      return result;
    }

    template <class OutEncoding, class InputIterator, class OutputIterator,
      class U32Error, class OutError>
    inline
      OutputIterator utf16_to_char32_t(InputIterator first, InputIterator last,
        OutputIterator result, U32Error u32_eh, OutError out_eh)
    {
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
          for (const char32_t* itr = u32_eh(); *itr; ++itr)
            result = u32_outputer(OutEncoding(), *itr, result, out_eh);
          continue;  // no need to increment first; that has already been done above
          // cases: c was high surrogate          action: do not increment first again
          //        *first is not high surrogate  action: do not increment first again
          //        first == last                 action: do not increment first again
        }
        result = u32_outputer(OutEncoding(), u32, result, out_eh);
      }
      return result;
    }

//--------------------------------------------------------------------------------------//
//                      detail::convert_utf implementation                              //
//        overload resolution performed by tag dispatch on from_tag, to_tag             //
//--------------------------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf8 /*from_tag*/, utf8 /*to_tag*/,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf8_to_char32_t<utf8>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf16_to_char32_t<utf16>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf32, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      // TODO: Validate output. Pass through UTF-16?
      return std::copy(first, last, result);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<utf32>(first, last, result, eh, eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf16, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<utf32>(first, last, result, eh , eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf32, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_u16string(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf32, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_u8string(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<utf16>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator convert_utf(utf16, utf8,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<utf8>(first, last, result, detail::err_pass_thru(), eh);
    }

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

  //  convert_utf  ---------------------------------------------------------------------//

  template <class ToEncoding, class InputIterator, class OutputIterator, class Error>
  inline OutputIterator
    convert_utf(InputIterator first, InputIterator last, OutputIterator result, Error eh)
  {
    // tag dispatch to the specific conversion function
    return detail::convert_utf(
      typename
        utf_encoding<typename std::iterator_traits<InputIterator>::value_type>::type(),
      ToEncoding(), first, last, result, eh);
  }

  template <class ToEncoding, class InputIterator, class OutputIterator>
  inline OutputIterator
    convert_utf(InputIterator first, InputIterator last, OutputIterator result)
  {
    // tag dispatch to the specific conversion function
    return detail::convert_utf(
      typename
        utf_encoding<typename std::iterator_traits<InputIterator>::value_type>::type(),
      ToEncoding(), first, last, result,
      err_hdlr<typename encoding_traits<ToEncoding>::value_type>());
  }

  //  to_utf_string  -------------------------------------------------------------------//

  template <class ToCharT, class FromCharT, class FromTraits, class Error,
    class ToTraits,  class ToAlloc>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    to_utf_string(boost::basic_string_view<FromCharT, FromTraits> v,
      Error eh, const ToAlloc& a)
  {
    std::basic_string<ToCharT, ToTraits, ToAlloc> tmp(a);
    convert_utf<typename utf_encoding<ToCharT>::type>
      (v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  //------------------------------------------------------------------------------------//
  //  Unicode Transformation Format (UTF) based convenience function implementations    // 
  //------------------------------------------------------------------------------------//

  template <class Error>
  inline std::string  to_u8string(boost::string_view v, Error eh)
    { return to_utf_string<char, char, Error>(v, eh); }
  template <class Error>
  inline std::string  to_u8string(boost::u16string_view v, Error eh)
    { return to_utf_string<char, char16_t, Error>(v, eh); }
  template <class Error>
  inline std::string  to_u8string(boost::u32string_view v, Error eh)
    { return to_utf_string<char, char32_t, Error>(v, eh); }
  template <class Error>
  inline std::string  to_u8string(boost::wstring_view v, Error eh)
    { return to_utf_string<char, wchar_t, Error>(v, eh); }

  template <class Error>
  inline std::u16string  to_u16string(boost::string_view v, Error eh)
    { return to_utf_string<char16_t, char, Error>(v, eh); }
  template <class Error>
  inline std::u16string  to_u16string(boost::u16string_view v, Error eh)
    { return to_utf_string<char16_t, char16_t, Error>(v, eh); }
  template <class Error>
  inline std::u16string  to_u16string(boost::u32string_view v, Error eh)
    { return to_utf_string<char16_t, char32_t, Error>(v, eh); }
  template <class Error>
  inline std::u16string  to_u16string(boost::wstring_view v, Error eh)
    { return to_utf_string<char16_t, wchar_t, Error>(v, eh); }

  template <class Error>
  inline std::u32string  to_u32string(boost::string_view v, Error eh)
    { return to_utf_string<char32_t, char, Error>(v, eh); }
  template <class Error>
  inline std::u32string  to_u32string(boost::u16string_view v, Error eh)
    { return to_utf_string<char32_t, char16_t, Error>(v, eh); }
  template <class Error>
  inline std::u32string  to_u32string(boost::u32string_view v, Error eh)
    { return to_utf_string<char32_t, char32_t, Error>(v, eh); }
  template <class Error>
  inline std::u32string  to_u32string(boost::wstring_view v, Error eh)
    { return to_utf_string<char32_t, wchar_t, Error>(v, eh); }

  template <class Error>
  inline std::wstring  to_wstring(boost::string_view v, Error eh)
    { return to_utf_string<wchar_t, char, Error>(v, eh); }
  template <class Error>
  inline std::wstring  to_wstring(boost::u16string_view v, Error eh)
    { return to_utf_string<wchar_t, char16_t, Error>(v, eh); }
  template <class Error>
  inline std::wstring  to_wstring(boost::u32string_view v, Error eh)
    { return to_utf_string<wchar_t, char32_t, Error>(v, eh); }
  template <class Error>
  inline std::wstring  to_wstring(boost::wstring_view v, Error eh)
    { return to_utf_string<wchar_t, wchar_t, Error>(v, eh); }

  //template <class FromEncoding, class ToEncoding, class InputIterator,
  //  class OutputIterator, class Error>
  //inline OutputIterator
  //  codecvt_convert(InputIterator first, InputIterator last, OutputIterator result,
  //    const codecvt_type& ccvt, Error eh)
  //{
  //  return detail::codecvt_convert(FromEncoding(), ToEncoding(), first, last, result,
  //    ccvt, eh);
  //}

  //template <class FromEncoding, class ToEncoding, class FromTraits, class ToTraits,
  //  class ToAlloc, class Error>
  //inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
  //  codecvt_to_generic_string(
  //    const boost::basic_string_view<typename encoded<FromEncoding>::type, FromTraits>& v,
  //    const codecvt_type& ccvt, Error eh, const ToAlloc& a)
  //{
  //  std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
  //  codecvt_convert<FromEncoding, ToEncoding>(v.cbegin(), v.cend(),
  //    std::back_inserter(tmp), ccvt, eh);
  //  return tmp;
  //}

  //------------------------------------------------------------------------------------// 
  //  codecvt based convenience function implementations                                //
  //------------------------------------------------------------------------------------// 

  ////  narrow to wide
  //template <class ToTraits, class ToAlloc>
  //inline std::basic_string<wchar_t, ToTraits, ToAlloc>
  //  codecvt_to_wstring(boost::string_view v, const codecvt_type& ccvt,
  //    const ToAlloc& a)
  //{
  //  return codecvt_to_generic_string<narrow, wide>(v, ccvt, a);
  //}

  ////  wide to narrow
  //template <class ToTraits,  class ToAlloc>
  //inline std::basic_string<char, ToTraits, ToAlloc>
  //  codecvt_to_string(boost::wstring_view v, const codecvt_type& ccvt,
  //    const ToAlloc& a)
  //{
  //  return codecvt_to_generic_string<wide, narrow>(v, ccvt, a);
  //}
//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//

namespace detail
{
  //  helper so static_assert can reference dependent type
  template <class T>
  constexpr bool fail() { return false; }

}  // namespace detail

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_HPP