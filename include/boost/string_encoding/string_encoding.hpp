#if !defined(BOOST_STRING_ENCODING_HPP)
#define BOOST_STRING_ENCODING_HPP

#include <iterator>
#include <type_traits>

namespace boost
{
namespace string_encoding
{

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  encodings

  struct narrow {};
  struct wide {};
  struct utf8 {};
  struct utf16 {};
  struct utf32 {};

  template <class Encoding>
  struct encoding{ typedef void value_type; };
  template <> struct encoding<narrow> { typedef char value_type; };
  template <> struct encoding<wide> { typedef wchar_t value_type; };
  template <> struct encoding<utf8> { typedef char value_type; };
  template <> struct encoding<utf16> { typedef char16_t value_type; };
  template <> struct encoding<utf32> { typedef char32_t value_type; };

  struct error_handler;   // default error handler

  template <class FromEncoding, class ToEncoding, class InputIterator,
  class OutputIterator, class ErrorHandler = error_handler>
    OutputIterator
    recode(InputIterator first, InputIterator last, OutputIterator result,
      const std::locale& loc, ErrorHandler eh = error_handler());

  template <class FromEncoding, class ToEncoding, class InputIterator,
  class OutputIterator, class ErrorHandler = error_handler>
    OutputIterator
    recode(InputIterator first, InputIterator last, OutputIterator result,
      ErrorHandler eh = error_handler());

//--------------------------------------------------------------------------------------//
//                                 Implementation                                       //
//--------------------------------------------------------------------------------------//

namespace detail
{

  //  narrow to narrow
  template <class InputIterator, class OutputIterator, class ErrorHandler>
  OutputIterator recode(narrow, narrow,
    InputIterator first, InputIterator last, OutputIterator result,
    const std::locale& loc, ErrorHandler)
  {
    for (; first != last; ++first, ++result)
      *result = *first;
    return result;
  }

  //  narrow to wide
  template <class InputIterator, class OutputIterator, class ErrorHandler>
  OutputIterator recode(narrow, wide,
    InputIterator first, InputIterator last, OutputIterator result,
    const std::locale& loc, ErrorHandler eh)
  {
    constexpr std::size_t buf_size = 20;  // TODO increase this after initial testing
    char in[buf_size];
    const char* in_end = &in[0] + buf_size;
    wchar_t out[buf_size];
    const std::codecvt<wchar_t,char,mbstate_t>& cvt =
      std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t> >(loc);
    std::mbstate_t state  = std::mbstate_t();
    wchar_t* out_next;

    while (first != last)
    {
      //  fill the in buffer
      char* in_last = &in[0];
      for (; first != last && in_last != in_end; ++in_last, ++first)
        *in_last = *first;

      //  convert in buffer to out buffer
      const char* in_next;
      std::codecvt_base::result cvt_result
        = cvt.in(state, in, in_last, in_next, out, out + buf_size, out_next);

      // need to deal with cvt_result  ok, partial, error, noconv

      //  copy the out buffer to result
      for (const wchar_t* itr = &out[0]; itr != out_next; ++itr, ++result)
        *result = *itr;
    }

    return result;
  }

}  // namespace detail

  struct error_handler   // default error handler
  {
    void operator()() const { throw "barf"; }
  };

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class ErrorHandler>
  OutputIterator
  recode(InputIterator first, InputIterator last, OutputIterator result,
        const std::locale& loc, ErrorHandler eh)
  {
    return detail::recode(FromEncoding(), ToEncoding(), first, last, result, loc, eh);
  }

  template <class FromEncoding, class ToEncoding, class InputIterator,
    class OutputIterator, class ErrorHandler>
  OutputIterator
  recode(InputIterator first, InputIterator last, OutputIterator result,
    ErrorHandler eh)
  {
    return recode<FromEncoding, ToEncoding>(first, last, result, std::locale(), eh);
  }
}  // namespace string_encoding
}  // namespace boost

#endif  // BOOST_STRING_ENCODING_HPP