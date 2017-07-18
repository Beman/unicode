//  boost/unicode/string_encoding.hpp  -------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_STRING_ENCODING_HPP)
#define BOOST_UNICODE_STRING_ENCODING_HPP

#include <iterator>
#include <string>
#include <locale>
#include <array>
#include <type_traits>
#include <cstdint>
#include <boost/config.hpp>
#include <boost/utility/string_view_fwd.hpp> 
#include <boost/utility/string_view.hpp> 
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>     // todo: remove me

// TODO: update this:
//--------------------------------------------------------------------------------------//
//  This header deals with both Unicode Transformation Format (UTF) encodings and       //
//  non-UTF encodings of strings and other character sequences. Strings and sequences   //
//  of types char16_t, char32_t, and wchar_t are UTF encoded. Strings and sequences of  //
//  type char are either UTF or narrow encoded. Narrow encoded strings and sequences    //
//  have an associated codecvt argument that determines the specific narrow encoding,   //
//  such as Big5, Shift-JIS, EBCIDIC, or even UTF-8.                                    //
//--------------------------------------------------------------------------------------//

/*
ISO/IEC 10646:2014

9.1 General
This International Standard provides three encoding forms expressing each UCS scalar
value in a unique sequence of one or more code units. These are named UTF-8, UTF-16,
and UTF-32 respectively.

9.2 UTD-8 extract:

* Because surrogate code points are not UCS scalar values, any UTF-8 sequence that would
  otherwise map to code points D800-DFFF is ill-formed.

* Any UTF-8 sequence that does not match the patterns listed in table 3 is ill-formed.

* As a consequence of the well-formedness conditions specified in table 9.2, the following
  octet values are disallowed in UTF-8: C0-C1, F5-FE.

9.3 UTF-16 extract:

* Because surrogate code points are not UCS scalar values, unpaired surrogate code units
  are ill-formed.

9.4 UTF-32 (UCS-4) extract:

* Because surrogate code points are not UCS scalar values, UTF-32 code units in the range
  0000 D800 - 0000 DFFF are ill-formed.

The Unicode standard says "Any UTF-32 code unit greater than U+10FFFF is ill-formed."
10646:2014 seems to rely on numerous mentions of U+10FFFF as the maximum code point.

6.2 Coding of characters extract:

* Each encoded character within the UCS codespace is represented by an integer between 0
  and 10FFFF identified as a code point.

Unicode Standard - Encoding Form Conversion (D93) extract:

* "A conformant encoding form conversion will treat any ill-formed code unit
   sequence as an error condition. ... it will neither interpret nor emit an ill-formed
   code unit sequence. Any implementation of encoding form conversion must take this
   requirement into account, because an encoding form conversion implicitly involves a
   verification that the Unicode strings being converted do, in fact, contain well-formed
   code unit sequences."
*/

#ifndef BOOST_UNICODE_BUFFER_SIZE
# define BOOST_UNICODE_BUFFER_SIZE 128
#endif

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//
// <!-- snippet=utfcvt -->
namespace boost
{
namespace unicode
{
  //  [uni.encoding] encoding types
  struct narrow {using value_type = char    ; }; // encoding determined by codecvt facet
  struct utf8   {using value_type = char    ; }; // UTF-8 encoded
  struct utf16  {using value_type = char16_t; }; // UTF-16 encoded
  struct utf32  {using value_type = char32_t; }; // UTF-32 encoded
  struct wide   {using value_type = wchar_t ; }; // UTF-8, 16, or 32, platform determined

  //  [uni.is_encoding] is_encoding type-trait
  template <class T> struct is_encoding : public std::false_type {};
  template<> struct is_encoding<narrow> : std::true_type {};
  template<> struct is_encoding<utf8>   : std::true_type {};
  template<> struct is_encoding<utf16>  : std::true_type {};
  template<> struct is_encoding<utf32>  : std::true_type {};
  template<> struct is_encoding<wide>   : std::true_type {};

# ifndef BOOST_NO_CXX14_VARIABLE_TEMPLATES
  template <class T> constexpr bool is_encoding_v = is_encoding<T>::value;
# endif

  //  [uni.is_encoded_character] is_encoded_character type-trait
  template <class T> struct is_encoded_character   : public std::false_type {};
  template<> struct is_encoded_character<char>     : std::true_type {};
  template<> struct is_encoded_character<char16_t> : std::true_type {};
  template<> struct is_encoded_character<char32_t> : std::true_type {};
  template<> struct is_encoded_character<wchar_t>  : std::true_type {};

# ifndef BOOST_NO_CXX14_VARIABLE_TEMPLATES
  template <class T> constexpr bool is_encoded_character_v
    = is_encoded_character<T>::value;
# endif

  // [uni.err] default error handler
  template <class CharT> struct ufffd;
  template <> struct ufffd<char>;
  template <> struct ufffd<char16_t>;
  template <> struct ufffd<char32_t>;
  template <> struct ufffd<wchar_t>;

  //  [uni.recode] encoding conversion algorithm
  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class ... T>
  OutputIterator recode(InputIterator first, InputIterator last, OutputIterator result,  
    const T& ... args);

  ////  [uni.to_string] string encoding conversion
  //template <class ToEncoding = utf8, class ...Pack>
  //  std::basic_string<typename ToEncoding::value_type>
  //    to_string(boost::string_view v, const Pack& ... args);
  //template <class ToEncoding = utf8, class ...Pack>
  //  std::basic_string<typename ToEncoding::value_type>
  //    to_string(boost::u16string_view v, const Pack& ... args);
  //template <class ToEncoding = utf8, class ...Pack>
  //  std::basic_string<typename ToEncoding::value_type>
  //    to_string(boost::u32string_view v, const Pack& ... args);
  //template <class ToEncoding = utf8, class ...Pack>
  //  std::basic_string<typename ToEncoding::value_type>
  //    to_string(boost::wstring_view v, const Pack& ... args);

  //  [uni.utf-query] UTF encoding queries
  template <class ForwardIterator>
    std::pair<ForwardIterator, ForwardIterator>
      first_ill_formed(ForwardIterator first, ForwardIterator last) BOOST_NOEXCEPT;

  bool is_well_formed(boost::string_view v) BOOST_NOEXCEPT;
  bool is_well_formed(boost::u16string_view v) BOOST_NOEXCEPT;
  bool is_well_formed(boost::u32string_view v) BOOST_NOEXCEPT;
  bool is_well_formed(boost::wstring_view v) BOOST_NOEXCEPT;

  //  The R2 experimental signatures

  //  [uni.to_string] string encoding conversion

  //  The built-in encodings (i.e. UTF-8, UTF-16, UTF-32, and wchar_t)

  template <class ToEncoding = utf8,   // enable_if ToEncoding is utf8, utf16, utf32, or wide
    class Error = ufffd<char>> inline
    std::basic_string<typename ToEncoding::value_type>
    to_string(boost::string_view v, Error eh = Error())
  {
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<utf8, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  template <class ToEncoding = utf8,   // enable_if ToEncoding is utf8, utf16, utf32, or wide
    class Error = ufffd<char>> inline
    std::basic_string<typename ToEncoding::value_type>
    to_string(boost::u16string_view v, Error eh = Error())
  {
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<utf16, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  template <class ToEncoding = utf8,   // enable_if ToEncoding is utf8, utf16, utf32, or wide
    class Error = ufffd<char>> inline
    std::basic_string<typename ToEncoding::value_type>
    to_string(boost::u32string_view v, Error eh = Error())
  {
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<utf32, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  template <class ToEncoding = utf8,   // enable_if ToEncoding is utf8, utf16, utf32, or wide
    class Error = ufffd<char>> inline
    std::basic_string<typename ToEncoding::value_type>
    to_string(boost::wstring_view v, Error eh = Error())
  {
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<wide, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp), eh);
    return tmp;
  }

  //template <class ToEncoding,   // enable_if ToEncoding is narrow
  //  class Error = ufffd<char>, class Elem = wchar_t> inline
  //  std::basic_string<typename ToEncoding::value_type>
  //  string(boost::string_view v,
  //    const std::codecvt<Elem, char, std::mbstate_t>& ccvt, Error eh = Error())
  //{
  //  std::basic_string<typename ToEncoding::value_type> tmp;
  //  recode<utf8, ToEncoding>(v.cbegin(), v.cend(), std::back_inserter(tmp),  eh);
  //  return tmp;
  //}

  //template <class ToEncoding = utf8, class ...Pack>
  //std::basic_string<typename ToEncoding::value_type>
  //  to_string(boost::u16string_view v, const Pack& ... args);
  //template <class ToEncoding = utf8, class ...Pack>
  //std::basic_string<typename ToEncoding::value_type>
  //  to_string(boost::u32string_view v, const Pack& ... args);
  //template <class ToEncoding = utf8, class ...Pack>
  //std::basic_string<typename ToEncoding::value_type>
  //  to_string(boost::wstring_view v, const Pack& ... args);


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
  // to_string implementation ----------------------------------------------------------//

  namespace detail
  {
    using wccvt_t = std::codecvt<wchar_t, char, std::mbstate_t>;
    using u32ccvt_t = std::codecvt<char32_t, char, std::mbstate_t>;
    using ul32ccvt_t = std::codecvt<uint_least32_t, char, std::mbstate_t>;

    template <class T = long, class ...Pack>
    constexpr int ccvt_count()
    {
      return (std::is_base_of<wccvt_t, T>::value
        || std::is_base_of<u32ccvt_t, T>::value
        || std::is_base_of<ul32ccvt_t, T>::value)
        ? 1 + ccvt_count<Pack...>()
        : 0;
    }
  }
 
  template <class ToEncoding, class ...Pack> inline
    std::basic_string<typename ToEncoding::value_type>
      to_string(boost::string_view v, const Pack& ... args)
  {
    static_assert(is_encoding<ToEncoding>::value,
      "ToEncoding must be utf8, utf16, utf32, narrow, or wide");
    //static_assert(detail::ccvt_count<Pack...>() >= 0,
    //  "Yuck!");  // fails if expression did not evaluate to a constant
    //std::cout << detail::ccvt_count<Pack...>() << std::endl;

    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() != 0, "A ccvt_type argument is required");
    static_assert((!std::is_same<ToEncoding, narrow>::value
        || detail::ccvt_count<Pack...>() <= 2)
      && (std::is_same<ToEncoding, narrow>::value
        || detail::ccvt_count<Pack...>() <= 1),
      "Too many ccvt_type arguments");

    using FromEncoding = typename std::conditional<
      (detail::ccvt_count<Pack...>() == 1 && !std::is_same<ToEncoding, narrow>::value)
      || detail::ccvt_count<Pack...>() == 2,
      narrow, utf8>::type;

    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<FromEncoding, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), args...);
    return tmp;
  }

  template <class ToEncoding, class ...Pack> inline
    std::basic_string<typename ToEncoding::value_type>
      to_string(boost::u16string_view v, const Pack& ... args)
  {
    static_assert(is_encoding<ToEncoding>::value,
      "ToEncoding must be utf8, utf16, utf32, narrow, or wide");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() != 0, "A ccvt_type argument is required");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() < 2,
          "Multiple ccvt_type arguments are not allowed");
    static_assert(std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() == 0, "A ccvt_type argument is not allowed");
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<utf16, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), args ...);
    return tmp;
  }

  template <class ToEncoding, class ...Pack> inline
    std::basic_string<typename ToEncoding::value_type>
      to_string(boost::u32string_view v, const Pack& ... args)
  {
    static_assert(is_encoding<ToEncoding>::value,
      "ToEncoding must be utf8, utf16, utf32, narrow, or wide");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() != 0, "A ccvt_type argument is required");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() < 2,
          "Multiple ccvt_type arguments are not allowed");
    static_assert(std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() == 0, "A ccvt_type argument is not allowed");
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<utf32, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), args ...);
    return tmp;
  }

  template <class ToEncoding, class ...Pack> inline
    std::basic_string<typename ToEncoding::value_type>
      to_string(boost::wstring_view v, const Pack& ... args)
  {
    static_assert(is_encoding<ToEncoding>::value,
      "ToEncoding must be utf8, utf16, utf32, narrow, or wide");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() != 0, "A ccvt_type argument is required");
    static_assert(!std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() < 2,
          "Multiple ccvt_type arguments are not allowed");
    static_assert(std::is_same<ToEncoding, narrow>::value
      || detail::ccvt_count<Pack...>() == 0, "A ccvt_type argument is not allowed");
    std::basic_string<typename ToEncoding::value_type> tmp;
    recode<wide, ToEncoding>(v.cbegin(), v.cend(),
      std::back_inserter(tmp), args ...);
    return tmp;
  }

  namespace detail
  {
    // forward declare the functions needed to implement recode_utf_to_utf -------------//

    template <class ToCharT, class InputIterator, class OutputIterator,
      class U32Error, class OutError> inline
      OutputIterator utf8_to_char32_t(InputIterator first, InputIterator last,
        OutputIterator result, U32Error u32_eh, OutError out_eh);
    template <class ToCharT, class InputIterator, class OutputIterator,
      class U32Error, class OutError> inline
      OutputIterator utf16_to_char32_t(InputIterator first, InputIterator last,
        OutputIterator result, U32Error u32_eh, OutError out_eh);
    template <class ToCharT, class OutputIterator, class Error> inline
      OutputIterator char32_t_to_utf8(char32_t u32, OutputIterator result, Error eh);
    template <class ToCharT, class OutputIterator, class OutError> inline
      OutputIterator char32_t_to_utf16(char32_t u32, OutputIterator result,
        OutError out_eh);
    template <class ToCharT, class OutputIterator, class OutError> inline
      OutputIterator char32_t_to_utf32(char32_t u32, OutputIterator result,
        OutError out_eh);
    
    //----------------------------------------------------------------------------------//
    //                      recode_utf_to_utf implementation                            //
    //        The appropriate overload is called by recode_dispatch()                   //
    //----------------------------------------------------------------------------------//

    //  For any conversion that uses utf32 as an intermediary,
    //  we need a value that can never appear in valid utf32 to pass the error through
    //  to the final output type and there be detected as an error and then processed
    //  by the appropriate error handler for that output type.
    struct u32_err_pass_thru { const char32_t* operator()() const { return U"\x110000"; } };

# if WCHAR_MAX >= 0x1FFFFFFFu
#   define BOOST_UNICODE_WIDE_UTF utf32
# elif WCHAR_MAX >= 0x1FFFu
#   define BOOST_UNICODE_WIDE_UTF utf16  
# else
#   define BOOST_UNICODE_WIDE_UTF utf8
# endif

    // from utf8 -----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char>> inline
    OutputIterator recode_utf_to_utf(utf8 /*from*/, utf8 /*to*/,
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf8_to_char32_t<char>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char16_t>> inline
    OutputIterator recode_utf_to_utf(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf8_to_char32_t<char16_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char32_t>> inline
    OutputIterator recode_utf_to_utf(utf8, utf32,
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return utf8_to_char32_t<char32_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<wchar_t>> inline
    OutputIterator recode_utf_to_utf(utf8, wide,
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return utf8_to_char32_t<wchar_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    // from utf16 ----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char>> inline
    OutputIterator recode_utf_to_utf(utf16, utf8,
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf16_to_char32_t<char>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char16_t>> inline
    OutputIterator recode_utf_to_utf(utf16, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      // pass input sequence through UTF-32 conversion to ensure the
      // output sequence is valid even if the input sequence isn't valid
      return utf16_to_char32_t<char16_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char32_t>> inline
    OutputIterator recode_utf_to_utf(utf16, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return utf16_to_char32_t<char32_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<wchar_t>> inline
    OutputIterator recode_utf_to_utf(utf16, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return utf16_to_char32_t<wchar_t>(first, last, result, u32_err_pass_thru(), eh);
    }

    // from utf32 ----------------------------------------------------------------------//

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char>> inline
    OutputIterator recode_utf_to_utf(utf32, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf8<char>(static_cast<char32_t>(*first), result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char16_t>> inline
    OutputIterator recode_utf_to_utf(utf32, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf16<char16_t>(static_cast<char32_t>(*first), result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char32_t>> inline
    OutputIterator recode_utf_to_utf(utf32, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      for (; first != last; ++first)
      {
        result = char32_t_to_utf32<char32_t>(static_cast<char32_t>(*first), result, eh);
      }
      return result;
    }

    template <class InputIterator, class OutputIterator,
      class Error = ufffd<wchar_t>> inline
    OutputIterator recode_utf_to_utf(utf32, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      for (; first != last; ++first)
      {
#       if WCHAR_MAX >= 0x1FFFFFFFu
        result = char32_t_to_utf32<wchar_t>(static_cast<char32_t>(*first), result, eh);
#       elif WCHAR_MAX >= 0x1FFFu
        result = char32_t_to_utf16<wchar_t>(static_cast<char32_t>(*first), result, eh);
#       else
        result = char32_t_to_utf8<wchar_t>(static_cast<char32_t>(*first), result, eh);
#       endif
      }
      return result;
    }

    // from wide -----------------------------------------------------------------------//
 
    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char>> inline
    OutputIterator recode_utf_to_utf(wide, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return recode_utf_to_utf(BOOST_UNICODE_WIDE_UTF(), utf8(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char16_t>> inline
    OutputIterator recode_utf_to_utf(wide, utf16, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return recode_utf_to_utf(BOOST_UNICODE_WIDE_UTF(), utf16(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator,
      class Error = ufffd<char32_t>> inline
    OutputIterator recode_utf_to_utf(wide, utf32, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return recode_utf_to_utf(BOOST_UNICODE_WIDE_UTF(), utf32(), first, last, result, eh);
    }
 
    template <class InputIterator, class OutputIterator,
      class Error = ufffd<wchar_t>> inline
    OutputIterator recode_utf_to_utf(wide, wide, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh = Error())
    {
      return recode_utf_to_utf(BOOST_UNICODE_WIDE_UTF(), wide(), first, last, result, eh);
    }

    //----------------------------------------------------------------------------------//
    //                             codecvt implementation                               //
    //----------------------------------------------------------------------------------//
    
    // forward declare functions used to implement recode to/from narrow 
    template <class OutputIterator, class Codecvt, class Error> inline
      OutputIterator codecvt_narrow_to_utf(const char* from, const char* from_end,
        OutputIterator result, const Codecvt& ccvt, Error eh);
    template <class FromCharT, class OutputIterator, class Error> inline
      OutputIterator codecvt_utf_to_narrow(const FromCharT* from,
        const FromCharT* from_end, OutputIterator result,
        const std::codecvt<FromCharT, char, std::mbstate_t>& ccvt, Error eh);

    template <class CharT> struct utf_encoding;
    template<> struct utf_encoding<char>     { using tag = utf8; };
    template<> struct utf_encoding<char16_t> { using tag = utf16; };
    template<> struct utf_encoding<char32_t> { using tag = utf32; };

    //  Microsoft 14.0 codecvt facets don't always work for char32_t so workaround:
    template<> struct utf_encoding<uint_least32_t> { using tag = utf32; };

    // It remains to be seen if WCHAR_MAX is a sufficient heuristic for determining the
    // encoding of wchar_t. The values below work for Windows and Ubuntu Linux.
# if WCHAR_MAX >= 0x1FFFFFFFu
    template<> struct utf_encoding<wchar_t>  { using tag = utf32; };
# elif WCHAR_MAX >= 0x1FFFu
    template<> struct utf_encoding<wchar_t>  { using tag = utf16; };
# else
    template<> struct utf_encoding<wchar_t>  { using tag = utf8; };
# endif

    template <class CharT> struct encoding;
    template<> struct encoding<char32_t> { using type = utf32; };
    template<> struct encoding<uint_least32_t> { using type = utf32; };
    template<> struct encoding<wchar_t>  { using type = wide; };

    template <class T>
    constexpr bool is_ccvt()
    {
      return std::is_base_of<wccvt_t, T>::value
        || std::is_base_of<u32ccvt_t, T>::value
        || std::is_base_of<ul32ccvt_t, T>::value;
    }

    //  For any conversion that uses a wide intermediary,
    //  we need a string that can never appear in valid UTF to pass the error through
    //  to the final output type and there be detected as an error and then processed
    //  by the appropriate error handler for that output type.
# if WCHAR_MAX >= 0x1FFFFFFFu
    struct wide_err_pass_thru {const wchar_t* operator()() const {return L"\x110000";}};
# elif WCHAR_MAX >= 0x1FFFu
    struct wide_err_pass_thru {const wchar_t* operator()() const {return L"\xDC00\xFFFF";}};
# else
    struct wide_err_pass_thru {const wchar_t* operator()() const {return L"\xED\B0\80";}};
# endif

    // TODO: Both functions should have a second overload to deal with case of the utf
    // already being wide, so no need to converting to an intermediary wstring.

    // recode_utf_to_narrow
    template <class InputIterator, class OutputIterator, class Codecvt,
      class Error = ufffd<char>> inline
    OutputIterator recode_utf_to_narrow(InputIterator first, InputIterator last,
      OutputIterator result, const Codecvt& ccvt, Error eh = Error())
    {
      static_assert(is_ccvt<Codecvt>(),
        "fourth argument must be type std::codecvt<wchar_t, char, std::mbstate_t>"
        " or type std::codecvt<char32_t, char, std::mbstate_t>");
      using intermediate_type = typename Codecvt::intern_type;
      std::basic_string<intermediate_type> tmp;
      recode<typename 
        utf_encoding<typename std::iterator_traits<InputIterator>::value_type>::tag,
        typename utf_encoding<intermediate_type>::tag>
        (first, last, std::back_inserter(tmp), wide_err_pass_thru());
      return codecvt_utf_to_narrow(tmp.data(), tmp.data()+tmp.size(), result,
        ccvt, eh);
    }

    // recode_narrow_to_utf
    template <class ToEncoding, class InputIterator, class OutputIterator,
      class Codecvt,
      class Error = ufffd<typename ToEncoding::value_type>> inline
    OutputIterator recode_narrow_to_utf(InputIterator first, InputIterator last,
      OutputIterator result, const Codecvt& ccvt, Error eh = Error())
    {
      static_assert(is_ccvt<Codecvt>(),
        "fourth argument must be type std::codecvt<wchar_t, char, std::mbstate_t>"
        " or type std::codecvt<char32_t, char, std::mbstate_t>");
      using intermediate_type = typename Codecvt::intern_type;
      std::basic_string<intermediate_type> tmp;
      codecvt_narrow_to_utf(first, last, std::back_inserter(tmp), ccvt,
        wide_err_pass_thru());
      return recode<typename encoding<intermediate_type>::type,
        ToEncoding>(tmp.cbegin(), tmp.cend(), result, eh);
    }

    // recode_narrow_to_narrow
    template <class InputIterator, class OutputIterator,
      class FromCodecvt, class ToCodecvt, class Error = ufffd<char>> inline
    OutputIterator recode_narrow_to_narrow(InputIterator first, InputIterator last,
      OutputIterator result, const FromCodecvt& from_ccvt, const ToCodecvt& to_ccvt,
      Error eh = Error())
    {
      static_assert(is_ccvt<FromCodecvt>(),
        "fourth argument must be type std::codecvt<wchar_t, char, std::mbstate_t>"
        " or type std::codecvt<char32_t, char, std::mbstate_t>");
      static_assert(std::is_same<typename FromCodecvt::intern_type, typename
        ToCodecvt::intern_type>::value,
        "fourth and fifth arguments must have same intern_type");
      using intermediate_type = typename FromCodecvt::intern_type;
      std::basic_string<intermediate_type> tmp;
      codecvt_narrow_to_utf(first, last, std::back_inserter(tmp), from_ccvt,
        wide_err_pass_thru());
      return codecvt_utf_to_narrow(tmp.data(), tmp.data()+tmp.size(), result,
        to_ccvt, eh);
    }

    //  recode_dispatch implementation -------------------------------------------------//

    //  Four recode_dispatch overloads are used to implement recode for
    //  utf-to-utf, utf-to-narrow, narrow-to-utf and narrow-to-narrow conversions,
    //  respectively.

    struct utf_tag {};
    struct narrow_tag {};

    template <class FromEncoding, class ToEncoding,
      class InputIterator, class OutputIterator, class ... T> inline
    OutputIterator recode_dispatch(utf_tag, utf_tag, InputIterator first,
        InputIterator last, OutputIterator result, const T& ... args)
    {
      static_assert(sizeof...(args) <= 1, "too many arguments");
      return recode_utf_to_utf(FromEncoding(), ToEncoding(),
        first, last, result, args ...);
    }

    template <class FromEncoding, class ToEncoding,
      class InputIterator, class OutputIterator, class ... T> inline
    OutputIterator recode_dispatch(narrow_tag, utf_tag, InputIterator first,
        InputIterator last, OutputIterator result, const T& ... args)
    {
      static_assert(sizeof...(args) <= 2, "too many arguments");
      static_assert(sizeof...(args) > 0, "ccvt argument required");
      return recode_narrow_to_utf<ToEncoding>(first, last, result, args ...);
    }

    template <class FromEncoding, class ToEncoding,
      class InputIterator, class OutputIterator, class ... T> inline
    OutputIterator recode_dispatch(utf_tag, narrow_tag, InputIterator first,
        InputIterator last, OutputIterator result, const T& ... args)
    {
      static_assert(sizeof...(args) <= 2, "too many arguments");
      static_assert(sizeof...(args) > 0, "codecvt argument required");
      return recode_utf_to_narrow(first, last, result, args ...);
    }

    template <class FromEncoding, class ToEncoding,
      class InputIterator, class OutputIterator, class ... T> inline
    OutputIterator recode_dispatch(narrow_tag, narrow_tag, InputIterator first,
        InputIterator last, OutputIterator result, const T& ... args)
    {
      static_assert(sizeof...(args) <= 3, "too many arguments");
      static_assert(sizeof...(args) > 1, "two codecvt arguments required");
      return recode_narrow_to_narrow(first, last, result, args ...);
    }

    template <class Encoding> struct dispatch;
    template<> struct dispatch<narrow> { using tag = narrow_tag; };
    template<> struct dispatch<utf8>   { using tag = utf_tag; };
    template<> struct dispatch<utf16>  { using tag = utf_tag; };
    template<> struct dispatch<utf32>  { using tag = utf_tag; };
    template<> struct dispatch<wide>   { using tag = utf_tag; };

  }  // namespace detail

  //------------------------------ recode definition -----------------------------------//

  template <class FromEncoding, class ToEncoding,
    class InputIterator, class OutputIterator, class ... T> inline
  OutputIterator recode(InputIterator first, InputIterator last, OutputIterator result,  
    const T& ... args)
  {
    static_assert(is_encoding<FromEncoding>::value,
      "FromEncoding must be utf8, utf16, utf32, narrow, or wide");
    static_assert(is_encoding<ToEncoding>::value,
      "ToEncoding must be utf8, utf16, utf32, narrow, or wide");
    return detail::recode_dispatch<FromEncoding, ToEncoding>(
      typename detail::dispatch<FromEncoding>::tag(),
      typename detail::dispatch<ToEncoding>::tag(),
      first, last, result, args ...);
  }

  namespace detail
  {
    //  utf-to-utf conversion helpers  -------------------------------------------------//

    //  handy constants
    constexpr char16_t high_surrogate_base = 0xD7C0u;
    constexpr char16_t low_surrogate_base = 0xDC00u;
    constexpr char32_t ten_bit_mask = 0x3FFu;

    //  char32_t outputers; these helpers take a single char32_t code point, and output as
    //  many code units as needed to represent the code point. OutputT may be wchar_t for
    //  one of the overloads; which one depends on the platform.
    template <class ToCharT, class OutputIterator, class Error> inline
    OutputIterator u32_outputer(utf32, char32_t x, OutputIterator result, Error eh)
    {
      return char32_t_to_utf32<ToCharT>(x, result, eh);
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

    //  table3
    //
    //  Based on ISO/IEC 10646:2014 9.2 Table 3, Well-formed UTF-8 Octet sequences
    //
    //  The entries represent the first octet values 0xC2-0xF5
    //  Format bits:  llhh00cc
    //    ll is the offset above 0x80 for the second octet's lowest valid value.
    //    hh is the offset below 0xBF for the second octet's highest valid value.
    //    cc is the number of continuation octets (0x01-0x03).

    constexpr const unsigned char table3[] = {
      // one continuation octet
      1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0xC2 - 0xCF
      1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0xD0 - 0xDF
      // two continuation octets
      0x82u,   // 0xE0  A0-BF
      0x02u,   // 0xE1
      0x02u,   // 0xE2
      0x02u,   // 0xE3
      0x02u,   // 0xE4
      0x02u,   // 0xE5
      0x02u,   // 0xE6
      0x02u,   // 0xE7
      0x02u,   // 0xE8
      0x02u,   // 0xE9
      0x02u,   // 0xEA
      0x02u,   // 0xEB
      0x02u,   // 0xEC
      0x22u,   // 0xED  80-9F    (i.e. surrogates are invalid)
      0x02u,   // 0xEE
      0x02u,   // 0xEF
      // three continuation octets
      0x43u,   // 0xF0  90-BF
      0x03u,   // 0xF1
      0x03u,   // 0xF2
      0x03u,   // 0xF3
      0x03u,   // 0xF4
    };

    template <class ToCharT, class InputIterator, class OutputIterator,
      class U32Error, class OutError>
    inline
    OutputIterator utf8_to_char32_t(InputIterator first, InputIterator last,
      OutputIterator result, U32Error u32_eh, OutError out_eh)
    {
      using encoding_tag = utf_encoding<ToCharT>::tag;

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
      using encoding_tag = utf_encoding<ToCharT>::tag;

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
//                     codecvt based recoding implementation                            //
//--------------------------------------------------------------------------------------//

  //  TODO: Both functions need to protect against buffer overflow caused by excessively
  //  long error handler returned strings. But if the only use is for pass_thru that is
  //  not strictly needed.

  //  TODO: Combine these functions into one. The are identical except for the
  //  ccvt.in/ccvt.out call and the easy-to-parameterize from/to character types.

  //  codecvt_utf_to_narrow
  template <class FromCharT, class OutputIterator, class Error> inline
    //  for clarity, use the same names for ccvt.in() arguments as the standard library
    OutputIterator codecvt_utf_to_narrow(const FromCharT* from, const FromCharT* from_end,
      OutputIterator result, const std::codecvt<FromCharT, char, std::mbstate_t>& ccvt,
      Error eh)
  {
    //static_assert(std::is_same<FromCharT, typename Codecvt::intern_type>::value,
    //  "FromCharT and Codecvt::intern_type must be the same type");
    //static_assert(std::is_same<ToCharT, typename Codecvt::extern_type>::value,
    //  "ToCharT and Codecvt::extern_type must be the same type");
    //static_assert(!std::is_same<FromCharT, ToCharT>::value,
    //  "FromCharT and ToCharT must not be the same type");

    std::array<char, BOOST_UNICODE_BUFFER_SIZE> buf;
    std::mbstate_t mbstate  = std::mbstate_t();
    const FromCharT* from_next;
    std::codecvt_base::result ccvt_result = std::codecvt_base::ok;

    //  loop until the entire input sequence is processed by the codecvt facet 

    while (from != from_end)
    {
      char* to = buf.data();
      char* to_end = to + buf.size();
      char* to_next = to;

      ccvt_result
        = ccvt.out(mbstate, from, from_end, from_next, to, to_end, to_next);

      if (ccvt_result == std::codecvt_base::ok)
      {
        for (; to != to_next; ++to)
          *result++ = *to;
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        for (; to != to_next; ++to)
          *result++ = *to;
        for (auto it = eh(); *it != '\0'; ++it)
          *result++ = *it;
        from = from_next + 1;  // bypass error
      }
      else  // ccvt_result == std::codecvt_base::partial
      {
        if (to_next == buf.data())
        {
          for (auto it = eh(); *it != '\0'; ++it)
            *result++ = *it;
          from = from_end;
        }
        else
        {
          // eliminate the possibility that buf does not have enough room
          for (; to != to_next; ++to)
            *result++ = *to;
          from = from_next;
        }
      }
    }
    return result;
  }

  //  codecvt_narrow_to_utf
  template <class OutputIterator, class Codecvt, class Error> inline
    //  for clarity, use the same names for ccvt.in() arguments as the standard library
  OutputIterator codecvt_narrow_to_utf(const char* from, const char* from_end,
    OutputIterator result, const Codecvt& ccvt, Error eh)
  {
    using utf_type = typename Codecvt::intern_type;

    std::array<utf_type, BOOST_UNICODE_BUFFER_SIZE> buf;
    std::mbstate_t mbstate  = std::mbstate_t();
    const char* from_next;
    std::codecvt_base::result ccvt_result = std::codecvt_base::ok;

    //  loop until the entire input sequence is processed by the codecvt facet 

    while (from != from_end)
    {
      utf_type* to = buf.data();
      utf_type* to_end = to + buf.size();
      utf_type* to_next = to;

      ccvt_result
        = ccvt.in(mbstate, from, from_end, from_next, to, to_end, to_next);

      if (ccvt_result == std::codecvt_base::ok)
      {
        for (; to != to_next; ++to)
          *result++ = *to;
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        for (; to != to_next; ++to)
          *result++ = *to;
        for (auto it = eh(); *it != '\0'; ++it)
          *result++ = *it;
        from = from_next + 1;  // bypass error
      }
      else  // ccvt_result == std::codecvt_base::partial
      {
        if (to_next == buf.data())
        {
          for (auto it = eh(); *it != '\0'; ++it)
            *result++ = *it;
          from = from_end;
        }
        else
        {
          // eliminate the possibility that buf does not have enough room
          for (; to != to_next; ++to)
            *result++ = *to;
          from = from_next;
        }
      }
    }
    return result;
  }
  template <class T> struct is_known_encoding : public std::false_type {};
  template<> struct is_known_encoding<utf8>   : std::true_type {};
  template<> struct is_known_encoding<utf16>  : std::true_type {};
  template<> struct is_known_encoding<utf32>  : std::true_type {};
  template<> struct is_known_encoding<wide>   : std::true_type {};

  //  TODO: variable templates only available with -std=c++14  
  //template <class T> constexpr bool is_known_encoding_v = is_known_encoding<T>::value;

  template <class ForwardIterator>
  std::pair<ForwardIterator, ForwardIterator>
    first_ill_formed(ForwardIterator first, ForwardIterator last, utf32) BOOST_NOEXCEPT
  {
    for (; first != last; ++first)
    {
      auto c = static_cast<char32_t>(*first);
      if (c > 0x10FFFFu || (c >= 0xD800u && c < 0xE000u))
        return std::make_pair(first, first+1);
    }
    return std::make_pair(last, last);
  }

  template <class ForwardIterator>
  std::pair<ForwardIterator, ForwardIterator>
    first_ill_formed(ForwardIterator first, ForwardIterator last, utf16) BOOST_NOEXCEPT
  {
    for (; first != last; ++first)
    {
      auto c = static_cast<char16_t>(*first);
      if (c >= 0xD800u && c < 0xE000u)  // surrogates must always be paired
      {
        auto first_code_unit = first;
        if (++first == last)
          return std::make_pair(first_code_unit, first);
        else if ((c = static_cast<char16_t>(*first)) < 0xD800u || c > 0xDFFFu)
          return std::make_pair(first_code_unit, ++first);
      }
    }
    return std::make_pair(last, last);
  }

  template <class ForwardIterator>
  std::pair<ForwardIterator, ForwardIterator>
    first_ill_formed(ForwardIterator first, ForwardIterator last, utf8) BOOST_NOEXCEPT
  {
    ForwardIterator first_code_unit;

    for (; first != last;) // each code point
    {
      //  Loop invariants:
      //    first points to the next unprocessed code_unit
      //    first_code_unit points to the first code unit for the code point
      //    octet contains the value of the current code unit

      first_code_unit = first;
      bool error = false;
      unsigned octet = static_cast<unsigned char>(*first++);
      
      if (octet <= 0x7Fu)
        continue;  // 7-bit ASCII so nothing further to do

      //  The sequence 'a', 0xE0, 'b' must treat 0xE0 as having a missing continuation
      //  octet (i.e. error range [1, 2)) rather than treating 'b' as an invalid
      //  continuation octet (i.e. error range [1, 3)).
      //
      //  In terms of code, this means that paths through "else if" block below must
      //  increment first for each valid octet, but not for an invalid continuation octet. 

      else if (octet >= 0xC2u && octet <= 0xF4u)  // first octet is valid
      {
        // unpack the table entries
        unsigned continuations = table3[octet - 0xC2u];
        unsigned lowest = 0x80u + ((continuations & 0xC0) >> 2);
        unsigned highest = 0xBFu - (continuations & 0x30u);
        continuations &= 0x03u;

        //lowest_trace = lowest;      // debugging aid
        //highest_trace = highest;

        // validate the continuation octets
        for (;;)  // for each continuation octet
        {
          if (first == last
            || (octet = static_cast<unsigned char>(*first)) < lowest
            || octet > highest)  // octet  is invalid
          {
            error = true;
            break;
          }
          ++first;
          if (--continuations == 0)
            break;
          // third and fourth octets, if present, require value in range 0x80u-0xBFu 
          lowest = 0x80u;
          highest = 0xBFu;
        }  // each continuation octet

      }  // first octet is valid and any continuation octets have been processed
      else  // first octet is invalid (and first has already been incremented)
        error = true;

      if (error)
      {
        //  First is pointing to last, the first octet after an invalid first octet,
        //  or to the first invalid continuation octet. Bypass octets not last and not a
        //  valid initial octet so that these invalid octets are included in the error
        //  range.

        for (;  //  bypass octets not last and not a valid initial octet
             first != last  // not last
               && (((octet = static_cast<unsigned char>(*first)) >= 0x80u
                 && octet <= 0xC1u)
                 || (octet >= 0xF5u && octet <= 0xFEu)); // not valid initial octet
             ++first) {}
        return std::make_pair(first_code_unit, first);
      }
    } // each code point

    BOOST_ASSERT(first == last);
    return std::make_pair(last, last);  // success
  }

} // namespace detail

  template <> struct ufffd<char>
  {
    constexpr const char* operator()() const noexcept { return u8"\uFFFD"; }
  };

  template <> struct ufffd<char16_t>
  { 
    constexpr const char16_t* operator()() const noexcept { return u"\uFFFD"; }
  };

  template <> struct ufffd<char32_t>
  { 
    constexpr const char32_t* operator()() const noexcept { return U"\uFFFD"; }
  };

  template <> struct ufffd<wchar_t>
  { 
    constexpr const wchar_t* operator()() const noexcept { return L"\uFFFD"; }
  };

  template <class ForwardIterator> inline
  std::pair<ForwardIterator, ForwardIterator>
    first_ill_formed(ForwardIterator first, ForwardIterator last) BOOST_NOEXCEPT
  {
    static_assert(is_encoded_character
        <typename std::iterator_traits<ForwardIterator>::value_type>::value,
      "ForwardIterator value_type must be char, char16_t, char32_t, or wchar_t");
    return detail::first_ill_formed(first, last,
      typename detail::utf_encoding
        <typename std::iterator_traits<ForwardIterator>::value_type>::tag());
  }

  inline bool is_well_formed(boost::string_view v) BOOST_NOEXCEPT
  {
    return first_ill_formed(v.cbegin(), v.cend()).first == v.end();
  }
  inline bool is_well_formed(boost::u16string_view v) BOOST_NOEXCEPT
  {
    return first_ill_formed(v.cbegin(), v.cend()).first == v.end();
  }
  inline bool is_well_formed(boost::u32string_view v) BOOST_NOEXCEPT
  {
    return first_ill_formed(v.cbegin(), v.cend()).first == v.end();
  }
  inline bool is_well_formed(boost::wstring_view v) BOOST_NOEXCEPT
  {
    return first_ill_formed(v.cbegin(), v.cend()).first == v.end();
  }
}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_STRING_ENCODING_HPP
