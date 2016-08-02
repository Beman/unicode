//  unicode/example/utf8-to-big5.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <boost/unicode/utf_conversion.hpp>      // needed for example 1
#include <boost/unicode/codecvt_conversion.hpp>  // needed for example 1
#include <locale>                                // needed for example 2
#include <codecvt>                               // needed for example 2
#include <boost/unicode/detail/hex_string.hpp>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cvt/big5>

int cpp_main(int, char*[])
{
  const std::string u8str(u8"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::string big5str("\u00A2\u00B9"  // big-5 encoded romanⅠ-Ⅹ  
                            "\u00A2\u00BA"
                            "\u00A2\u00BB"
                            "\u00A2\u00BC"
                            "\u00A2\u00BD"
                            "\u00A2\u00BE"
                            "\u00A2\u00BF"
                            "\u00A2\u00C0"
                            "\u00A2\u00C1"
                            "\u00A2\u00C2");
  std::wstring wstr;
  std::string str;

  // Example 1 (converts UTF-8 string to Big-5 string via wstring)
  //   requires wstring encoding be UTF-8, UTF-16, or  UTF-32
  stdext::cvt::codecvt_big5<wchar_t> big5ccvt;
  wstr = boost::unicode::to_wstring(u8str);
  str = boost::unicode::codecvt_to_string(wstr, big5ccvt);
  BOOST_TEST_EQ(str, big5str);
  //std::cout << boost::unicode::detail::hex_string(str) << '\n';
  //std::cout << boost::unicode::detail::hex_string(big5str) << '\n';

  // Example 2 (converts UTF-8 string to Big-5 string via wstring)
  //   assumes wstring encoding is UCS2/UCS4
  std::wstring_convert<std::codecvt_utf8<wchar_t>> u8_wide;
  std::wstring_convert<stdext::cvt::codecvt_big5<wchar_t>> big5_wide;
  wstr = u8_wide.from_bytes(u8str);
  str = big5_wide.to_bytes(wstr);
  BOOST_TEST_EQ(str, big5str);

  return ::boost::report_errors();
}

