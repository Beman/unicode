#if !defined(BOOST_STRING_ENCODING_HPP)
#define BOOST_STRING_ENCODING_HPP

#include <iterator>
#include <string>
#include <boost/utility/string_ref.hpp> 
//#include <type_traits>
#include <boost/config.hpp>
#include <boost/assert.hpp>

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
    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(utf8, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  utf8 to 16" << endl;
      return result;
    }

    template <class InputIterator, class OutputIterator /*, class Error*/> inline
    OutputIterator recode(wide, utf16,
      InputIterator first, InputIterator last, OutputIterator result /*, Error eh*/)
    {
      cout << "  wide to 16" << endl;
      return result;
    }

    template <class InputIterator, class OutputIterator, class Error> inline
    OutputIterator recode(utf16, utf8, 
      InputIterator first, InputIterator last, OutputIterator result, Error eh)
    {
      cout << "  utf16 to 8" << endl;
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

  ////  to_*string family  ---------------------------------------------------------------//

  //template <class charT, class traits, template<class> class Error = error_handler<char>,
  //class ToTraits = std::char_traits<charT>, class ToAlloc = std::allocator < charT >>
  //  inline std::basic_string<char, ToTraits, ToAlloc>  
  //  to_string(const boost::basic_string_ref<charT, traits>& sv,
  //    Error eh = Error())
  //  {
  //    cout << "to_string\n"; 
  //  }

  //  convenience functions  -----------------------------------------------------------//

  //template <class ToTraits = std::char_traits<char16_t>,
  //  class ToAlloc = std::allocator<char16_t>,
  //  class FromTraits = std::char_traits<char>>
  //  inline
  //  std::basic_string<char16_t, ToTraits, ToAlloc>
  //    utf8_to_utf16(boost::basic_string_ref<char, FromTraits>& v,
  //    const ToAlloc& a = ToAlloc())
  //  {
  //    return make_recoded_string<utf8, utf16, char, FromTraits, char16_t, ToTraits,
  //      ToAlloc>(v, a);
  //  }

  //template <
  //  class ToTraits = std::char_traits<char16_t>,
  //  class ToAlloc = std::allocator<char16_t>,
  //  class FromTraits = std::char_traits<char>>
  //  inline
  //  std::basic_string<char16_t, ToTraits, ToAlloc>
  //    to_utf16(boost::basic_string_ref<char, FromTraits>& v,
  //      const ToAlloc& a = ToAlloc())
  //  {
  //    return make_recoded_string<utf8, utf16, char, FromTraits, char16_t, ToTraits,
  //      ToAlloc>(v, a);
  //  }

  //template <class T> struct auto_encoding;
  //template<> struct auto_encoding<wchar_t> { typedef wide type; };
  //template<> struct auto_encoding<char16_t> { typedef utf16 type; };
  //template<> struct auto_encoding<char32_t> { typedef utf32 type; };
 
  //template <class FromEncoding = auto_encoding<FromCharT>::type, class FromCharT>
  //inline std::u16string to_utf16(const boost::basic_string_ref<FromCharT>& v)
  //{
  //  std::cout << " to_utf16()" << std::endl;
  //  std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
  //  std::cout << "   FromCharT:    " << typeid(FromCharT).name() << std::endl;;
  //  return make_recoded_string<FromEncoding, utf16>(v);
  //}

  template <class FromEncoding,
    class ToTraits = std::char_traits<char16_t>, class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<char>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16()" << std::endl;
    std::cout << "   FromEncoding: " <<typeid(FromEncoding).name() << std::endl;;
    return make_recoded_string<FromEncoding, utf16, std::char_traits<char>,
      ToTraits, ToAlloc>(v, a);
  }

  template <class ToTraits = std::char_traits<char16_t>,
    class ToAlloc = std::allocator<char16_t>>
  inline std::basic_string<char16_t, ToTraits, ToAlloc>
    to_utf16(const boost::basic_string_ref<wchar_t>& v,
      const ToAlloc& a = std::allocator<char16_t>())
  {
    std::cout << " to_utf16()" << std::endl;
    return make_recoded_string<wide, utf16>(v, a);
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