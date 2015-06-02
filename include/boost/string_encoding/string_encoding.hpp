#if !defined(BOOST_STRING_ENCODING_HPP)
#define BOOST_STRING_ENCODING_HPP

#include <iterator>
#include <type_traits>
#include <boost/config.hpp>
#include <boost/assert.hpp>

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