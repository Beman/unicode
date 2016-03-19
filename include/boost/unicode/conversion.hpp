//  boost/unicode/conversion.hpp  ------------------------------------------------------//

//  � Copyright Beman Dawes 2015

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

  //     encodings
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

  //  Error handler function object operator()() is called with no arguments and either
  //  throws an exception or returns a const pointer to a possibly empty C-style string of
  //  type encoded<ToEncoding>::type. The returned string is required to be encoded as
  //  specified by ToEncoding.

  //  default error handler:
  //    function object returns C-string of U+FFFD, encoded via ToEncoding.
  template <class ToEncoding> struct err_hdlr;

  //  recode
  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class Error = err_hdlr<ToEncoding>>
  inline OutputIterator
  recode(InputIterator first, InputIterator last, OutputIterator result, 
    Error eh = Error());

  //  recode_codecvt
  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class Error = err_hdlr<ToEncoding>>
  inline OutputIterator
    recode_codecvt(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh = Error());

  //  make_recoded_string 
  template <class FromEncoding, class ToEncoding, class FromTraits =
    std::char_traits<typename encoded<FromEncoding>::type>,
    class ToTraits = std::char_traits<typename encoded<ToEncoding>::type>,
    class ToAlloc = std::allocator<typename encoded<ToEncoding>::type>,
    class Error = err_hdlr<ToEncoding>>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_recoded_string(
      const boost::basic_string_view<typename encoded<FromEncoding>::type, FromTraits>& v,
      Error eh = Error(), const ToAlloc& a = ToAlloc());

    //  make_codecvted_string
  template <class FromEncoding, class ToEncoding, class FromTraits =
    std::char_traits<typename encoded<FromEncoding>::type>,
    class ToTraits = std::char_traits<typename encoded<ToEncoding>::type>,
    class ToAlloc = std::allocator<typename encoded<ToEncoding>::type>,
    class Error>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_codecvted_string(
      const boost::basic_string_view<typename encoded<FromEncoding>::type, FromTraits>& v,
      const codecvt_type& ccvt, Error eh, const ToAlloc& a = ToAlloc());

  //  codecvt facet based conversion convenience functions 

  //  narrow_to_wide
  template <class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    narrow_to_wide(const boost::string_view& v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

  //   wide_to_narrow
  template <class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    wide_to_narrow(const boost::wstring_view& v,
      const codecvt_type& ccvt, const ToAlloc& a = ToAlloc());

  //  Unicode Transformation Format (UTF) based conversion convenience functions 

  //  to_wide (from wide encoding)
  template <class Error = err_hdlr<wide>, class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::wstring_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_wide (from utf8 encoding)
  template <class Error = err_hdlr<wide>, class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::string_view& v, Error eh = Error(), const ToAlloc& a = ToAlloc());

  //  to_wide (from utf16 encoding)
  template <class Error = err_hdlr<wide>, class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::u16string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_wide (from utf32 encoding)
  template <class Error = err_hdlr<wide>, class ToTraits = std::char_traits<wchar_t>,
    class ToAlloc = std::allocator<wchar_t >>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::u32string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf8 (from wide encoding)
  template <class Error = err_hdlr<utf8>, class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char >>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::wstring_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf8 (from utf8 encoding)
  template <class Error = err_hdlr<utf8>, class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char>>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::string_view& v, Error eh = Error(), const ToAlloc& a = ToAlloc());

  //  to_utf8 (from utf16 encoding)
  template <class Error = err_hdlr<utf8>, class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char >>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::u16string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf8 (from utf32 encoding)
  template <class Error = err_hdlr<utf8>, class ToTraits = std::char_traits<char>,
    class ToAlloc = std::allocator<char >>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::u32string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf16 (from wide encoding)
  template <class Error = err_hdlr<utf16>, class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t >>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::wstring_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf16 (from utf8 encoding)
  template <class Error = err_hdlr<utf16>, class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());
 
  //  to_utf16 (from utf16 encoding)
  template <class Error = err_hdlr<utf16>, class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t >>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::u16string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf16 (from utf32 encoding)
  template <class Error = err_hdlr<utf16>,
    class ToTraits = std::char_traits<char16_t>, class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::u32string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf32 (from wide encoding)
  template <class Error = err_hdlr<utf32>, class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::wstring_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf32 (from utf8 encoding)
  template <class Error = err_hdlr<utf32>, class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf32 (from utf16 encoding)
  template <class Error = err_hdlr<utf32>, class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::u16string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

  //  to_utf32 (from utf32 encoding)
  template <class Error = err_hdlr<utf32>, class ToTraits = std::char_traits<char32_t>,
    class ToAlloc = std::allocator<char32_t>>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::u32string_view& v, Error eh = Error(),
      const ToAlloc& a = ToAlloc());

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

  template <> class err_hdlr<narrow>
    { public: const char* operator()() const noexcept { return "?"; } };
  template <> class err_hdlr<wide>
    { public: const wchar_t* operator()() const noexcept { return L"\uFFFD"; } };
  template <> class err_hdlr<utf8>
    { public: const char* operator()() const noexcept { return u8"\uFFFD"; } };
  template <> class err_hdlr<utf16>
    { public: const char16_t* operator()() const noexcept { return u"\uFFFD"; } };
  template <> class err_hdlr<utf32>
    { public: const char32_t* operator()() const noexcept { return U"\uFFFD"; } };

/*
Unicode Standard Conformance (as of version 7.0)

References: For example, (3.4 D10) refers to the Unicode Standard, Core Specification,
chapter 3, section 4, sub-section D10. See http://www.unicode.org/versions/latest/

Definitions:

* Code point (3.4 D10): "Code point: Any value in the Unicode codespace."

  Informally, a code point can be thought of as a Unicode character.

  (Appendix A - Notational Conventions):
  "In running text, an individual Unicode code point is expressed as U+n, where n is four
   to six hexadecimal digits, using the digits 0�9 and uppercase letters A�F (for 10
   through 15, respectively). Leading zeros are omitted, unless the code point would have
   fewer than four hexadecimal digits�for example, U+0001, U+0012, U+0123, U+1234,
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

* "... �non-shortest form� byte sequences in UTF-8 ... are ill-formed ..."

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

//  recode helpers ---------------------------------------------------------------------//

  namespace detail
  {
    //  This implementation assumes that wide (i.e. wchar_t) character encoding is always
    //  UTF-16 or UTF-32. As a result, only conversions between the three UTF encodings
    //  need to be supported. The type-trait actual<t>::encoding folds wide encoding into
    //  utf16 or utf32 encoding depending upon the platform.
  
    template <class T> struct actual { typedef T encoding; };
# if WCHAR_MAX >= 0xFFFFFFFFu
    template<> struct actual<wide> { typedef narrow encoding; };
# else
    template<> struct actual<wide> { typedef utf16 encoding; };
# endif

    //  for the utf8<-->utf16 encoding conversion, which use utf32 as an intermediary,
    //  we need a value that can never appear in valid utf32 to pass the error through
    //  to the final output type and will then be detected as an error and then processed
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
    OutputIterator char32_t_to_utf8(char32_t u32, OutputIterator result, Error eh)
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
    OutputIterator char32_t_to_utf16(char32_t u32, OutputIterator result, OutError out_eh)
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
      return char32_t_to_utf16(x, result, eh);
    }
    template <class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf8, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_utf8(x, result, eh);
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
//                              recode implementation                                   //
//                         overloads selected by tag dispatch                           //
//--------------------------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf8, utf8,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<utf8>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<utf16>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf32, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return std::copy(first, last, result);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<utf32>(first, last, result, eh, eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf16, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<utf32>(first, last, result, eh , eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf32, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf16(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf32, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf8(*first, result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf8_to_char32_t<utf16>(first, last, result, detail::err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf16, utf8,
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      return utf16_to_char32_t<utf8>(first, last, result, detail::err_pass_thru(), eh);
    }

    //  recode_codecvt

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode_codecvt(narrow, wide,
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
    OutputIterator recode_codecvt(wide, narrow,
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

  //  recode  --------------------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class Error>
  inline OutputIterator
    recode(InputIterator first, InputIterator last, OutputIterator result, Error eh)
  {
    return detail::recode(typename detail::actual<FromEncoding>::encoding(),
      typename detail::actual<ToEncoding>::encoding(), first, last, result, eh);
  }

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class Error>
  inline OutputIterator
    recode_codecvt(InputIterator first, InputIterator last, OutputIterator result,
      const codecvt_type& ccvt, Error eh)
  {
    return detail::recode_codecvt(FromEncoding(), ToEncoding(), first, last, result,
      ccvt, eh);
  }

  //  make_recoded_string  -------------------------------------------------------------//

  template <class FromEncoding, class ToEncoding, class FromTraits, class ToTraits,
    class ToAlloc, class Error>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_recoded_string(
      const boost::basic_string_view<typename encoded<FromEncoding>::type,
      FromTraits>& v, Error eh, const ToAlloc& a)
  {
    std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    recode<FromEncoding, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  template <class FromEncoding, class ToEncoding, class FromTraits, class ToTraits,
    class ToAlloc, class Error>
  inline std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc>
    make_codecvted_string(
      const boost::basic_string_view<typename encoded<FromEncoding>::type, FromTraits>& v,
      const codecvt_type& ccvt, Error eh, const ToAlloc& a)
  {
    std::basic_string<typename encoded<ToEncoding>::type, ToTraits, ToAlloc> tmp(a);
    recode_codecvt<FromEncoding, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), ccvt, eh);
    return tmp;
  }

  //------------------------------------------------------------------------------------// 
  //  codecvt based convenience function implementations                                //
  //------------------------------------------------------------------------------------// 

  //  narrow to wide
  template <class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    narrow_to_wide(const boost::string_view& v, const codecvt_type& ccvt,
      const ToAlloc& a)
  {
    return make_codecvted_string<narrow, wide>(v, ccvt, a);
  }

  //  wide to narrow
  template <class ToTraits,  class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    wide_to_narrow(const boost::wstring_view& v, const codecvt_type& ccvt,
      const ToAlloc& a)
  {
    return make_codecvted_string<wide, narrow>(v, ccvt, a);
  }

  //------------------------------------------------------------------------------------//
  //  Unicode Transformation Format (UTF) based convenience function implementations    // 
  //------------------------------------------------------------------------------------//

  //  to_wide  -------------------------------------------------------------------------//

  //  utf8 to wide
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf8, wide, std::char_traits<char>,
      ToTraits, ToAlloc>(v, eh, a);
  }

  //  wide to wide
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::wstring_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<wide, wide>(v, eh, a);
  }

  //  utf16 to wide
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::u16string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf16, wide>(v, eh, a);
  }

  //  utf32 to wide
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<wchar_t, ToTraits, ToAlloc>
    to_wide(const boost::u32string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf32, wide>(v, eh, a);
  }

  //  to_utf8  -------------------------------------------------------------------------//

  //  utf8 to utf8
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf8, utf8, std::char_traits<char>,
      ToTraits, ToAlloc>(v, eh, a);
  }

  //  wide to utf8
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::wstring_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<wide, utf8>(v, eh, a);
  }

  //  utf16 to utf8
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::u16string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf16, utf8>(v, eh, a);
  }

  //  utf32 to utf8
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char, ToTraits, ToAlloc>
    to_utf8(const boost::u32string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf32, utf8>(v, eh, a);
  }

  //  to_utf16  ------------------------------------------------------------------------//

  // utf8 to utf16
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf8, utf16, std::char_traits<char>,
      ToTraits, ToAlloc>(v, eh, a);
  }
 
  //  wide to utf16
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::wstring_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<wide, utf16>(v, eh, a);
  }

  //  utf16 to utf16
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::u16string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf16, utf16>(v, eh, a);
  }

  //  utf32 to utf16
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::u32string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf32, utf16>(v, eh, a);
  }

  //  to_utf32  ------------------------------------------------------------------------//

  //  utf8 to utf32
  template <class Error, class ToTraits, class ToAlloc>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf8, utf32, std::char_traits<char>,
      ToTraits, ToAlloc>(v, eh, a);
  }

  //  wide to utf32
  template <class Error, class ToTraits,
    class ToAlloc>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::wstring_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<wide, utf32>(v, eh, a);
  }

  //  utf16 to utf32
  template <class Error, class ToTraits,
    class ToAlloc>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::u16string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf16, utf32>(v, eh, a);
  }

  //  utf32 to utf32
  template <class Error, class ToTraits,
    class ToAlloc>
  inline std::basic_string<char32_t, ToTraits, ToAlloc>
    to_utf32(const boost::u32string_view& v, Error eh, const ToAlloc& a)
  {
    return make_recoded_string<utf32, utf32>(v, eh, a);
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

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_HPP