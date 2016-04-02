//  boost/unicode/codecvt_conversion.hpp  ----------------------------------------------//

//  © Copyright Beman Dawes 2015, 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_CODECVT_CONVERSION_HPP)
#define BOOST_UNICODE_CODECVT_CONVERSION_HPP

#include <iterator>
#include <string>
#include <cstring>  // for strlen
#include <locale>
#include <array>
#include <boost/utility/string_view.hpp> 
#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <boost/cstdint.hpp>

#if !defined(BOOST_UNICODE_ERROR_HPP)
# include <boost/unicode/error.hpp>
#endif

//--------------------------------------------------------------------------------------//
//  This interface is tailored to std::codecvt, particularly that codecvt::in and 
//  codecvt::out traffic in raw pointer ranges rather than iterators, and thus require
//  that both from and to ranges be contiguous. This create an impedance mismatch between
//  these codecvt functions and the  the STL "InputIterator first, InputIterator last,
//  OutputIterator result" model for arguments. Resolving that mismatch would require
//  an implementation to copy between both to and from arguments and internal buffers,
//  with unacceptable efficiency implications.
//
//  Q. Why not use std::wstring_convert?
//  A. Its error handling does not meet the recommendation of Unicode experts,
//     particularly the recommendation that the default for invalid characters be
//     replacement by U+FFFD to avoid security risks that have been used as attack
//     vectors.
//--------------------------------------------------------------------------------------//

/*

The initial development plan is to concentrate on char <---> wchar_t conversion via
the standard library's traditional codecvt<char16_t,char,mbstate_t> specialization.

If the interface can easily be extended to handle other codecvt specializations, fine.
But if that does not work out, codecvt<char16_t,char,mbstate_t> is sufficiently useful
that the attempt to support other specializations will be abandoned.

*/

//  TODO: Add a third detail::codecvt_to_basic_string overload to handle the case
//  where ToCharT and FromCharT are the same type so no conversion is needed. To be
//  decided: Should such an overload detect errors (and call the error handler)?


namespace boost
{
namespace unicode
{
//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

  //  codecvt_to_basic_string  ---------------------------------------------------------//

  template <class ToCharT, class FromCharT, class Codecvt,
     class FromTraits = std::char_traits<FromCharT>,
     class Error = err_hdlr<ToCharT>,
     class ToTraits = std::char_traits<ToCharT>,
     class ToAlloc = std::allocator<ToCharT>,
     class View = boost::basic_string_view<FromCharT, FromTraits>>
  inline std::basic_string<ToCharT, ToTraits, ToAlloc>
    codecvt_to_basic_string(View v, const Codecvt& ccvt,
      Error eh = Error(), const ToAlloc& a = ToAlloc());

  //  codecvt_to_{string|wstring}  -----------------------------------------------------//

  template <class Error = err_hdlr<char>>
  inline std::string  codecvt_to_string(boost::wstring_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, 
    const Error eh = Error());

  template <class Error = err_hdlr<wchar_t>>
  inline std::wstring  codecvt_to_wstring(boost::string_view v,
    const std::codecvt<wchar_t, char, std::mbstate_t>& ccvt, 
    const Error eh = Error());

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//

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

  //  codecvt_to_{string|wstring}  -----------------------------------------------------//

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

    std::basic_string<ToCharT, ToTraits, ToAlloc> temp;
    std::array<ToCharT, 8> buf;  // TODO: increase size after preliminary testing

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
        temp.append(to, to_next - to);
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        temp.append(to, to_next - to);
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
          temp.append(to, to_next - to);
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

    std::basic_string<ToCharT, ToTraits, ToAlloc> temp;
    std::array<ToCharT, 8> buf;  // TODO: increase size after preliminary testing

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
        temp.append(to, to_next - to);
        from = from_next;
      }
      else if (ccvt_result == std::codecvt_base::error)
      {
        temp.append(to, to_next - to);
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
          temp.append(to, to_next - to);
          from = from_next;
        }
      }
    }
    return temp;
  }

} // namespace detail


//namespace detail
//{
//  //  helper so static_assert can reference dependent type
//  template <class T>
//  constexpr bool fail() { return false; }
//
//}  // namespace detail

}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_CODECVT_CONVERSION_HPP