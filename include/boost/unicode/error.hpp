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

  //  Error function objects are called with no arguments and either throw an
  //  exception or return a const pointer to a possibly empty C-style string.

  //  default error handler: function object returns a C-string of type
  //  ToCharT with a UTF encoded value of U+FFFD.
  template <class CharT> struct err_hdlr;
  template <> struct err_hdlr<char>;
  template <> struct err_hdlr<char16_t>;
  template <> struct err_hdlr<char32_t>;
  template <> struct err_hdlr<wchar_t>;

//---------------------------------  end synopsis  -------------------------------------// 

//--------------------------------------------------------------------------------------//
//                                  Implementation                                      //
//--------------------------------------------------------------------------------------//

  template <> struct err_hdlr<char>
  {
    constexpr char* operator()() const noexcept { return u8"\uFFFD"; }
  };

  template <> struct err_hdlr<char16_t>
  { 
    constexpr char16_t* operator()() const noexcept { return u"\uFFFD"; }
  };

  template <> struct err_hdlr<char32_t>
  { 
    constexpr char32_t* operator()() const noexcept { return U"\uFFFD"; }
  };

  template <> struct err_hdlr<wchar_t>
  { 
    constexpr wchar_t* operator()() const noexcept { return L"\uFFFD"; }
  };
}  // namespace unicode
}  // namespace boost

#endif  // BOOST_UNICODE_ERROR_HPP