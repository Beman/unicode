//  boost/unicode/error.hpp  -----------------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if !defined(BOOST_UNICODE_ERROR_HPP)
#define BOOST_UNICODE_ERROR_HPP

//--------------------------------------------------------------------------------------//

namespace boost
{
namespace unicode
{

//--------------------------------------------------------------------------------------//
//                                     Synopsis                                         //
//--------------------------------------------------------------------------------------//

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

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  implementation                                      //
//--------------------------------------------------------------------------------------//


}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_ERROR_HPP