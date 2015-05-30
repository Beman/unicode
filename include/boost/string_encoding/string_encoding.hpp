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
  template <class InputIterator, class OutputIterator, class ErrorHandler>
  OutputIterator recode(narrow, narrow,
    InputIterator first, InputIterator last, OutputIterator result,
    const std::locale& loc, ErrorHandler)
  {
    static_assert(std::is_same<std::iterator_traits<InputIterator>::value_type, char>::value,
      "narrow encoding requires InputIterator value_type be char");
    //static_assert(std::is_same<std::iterator_traits<OutputIterator>::value_type, char>::value,
    //  "narrow encoding requires OutputIterator value_type be char");

    for (; first != last; ++first, ++result)
      *result = *first;
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