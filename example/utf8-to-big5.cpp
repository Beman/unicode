//  unicode/example/utf8-to-big5.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <boost/unicode/string_encoding.hpp>     // needed for example 1
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
  const std::u16string u16str(u"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::u32string u32str(U"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::wstring uwstr(L"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
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
  std::string str;
  std::wstring wstr;
  std::string str8;
  std::u16string str16;
  std::u32string str32;

  // Example 1 (converts UTF-8 string to Big-5 string via wstring)
  //   requires wstring encoding be UTF-8, UTF-16, or  UTF-32
  stdext::cvt::codecvt_big5<wchar_t> big5ccvt;
  wstr = boost::unicode::to_string<boost::unicode::wide>(u8str);
  str = boost::unicode::codecvt_to_string(wstr, big5ccvt);
  BOOST_TEST_EQ(str, big5str);
  //std::cout << boost::unicode::detail::hex_string(str) << '\n';
  //std::cout << boost::unicode::detail::hex_string(big5str) << '\n';

  // Example 2 (converts UTF-8 string to Big-5 string via wstring)
  //   assumes wstring encoding is UCS2/UCS4
  std::wstring_convert<std::codecvt_utf8<wchar_t>> u8_wide;
  std::wstring_convert<stdext::cvt::codecvt_big5<wchar_t>> big5_wide;
  wstr.clear();
  str.clear();
  wstr = u8_wide.from_bytes(u8str);
  str = big5_wide.to_bytes(wstr);
  BOOST_TEST_EQ(str, big5str);

  // Convert UTF-8 string directly to Big-5 string)
  str.clear();
  str = boost::unicode::codecvt_to_string(u8str, big5ccvt);
  BOOST_TEST_EQ(str, big5str);

  // Convert UTF-16 string directly to Big-5 string)
  str.clear();
  str = boost::unicode::codecvt_to_string(u16str, big5ccvt);
  BOOST_TEST_EQ(str, big5str);

  // Convert UTF-32 string directly to Big-5 string)
  str.clear();
  str = boost::unicode::codecvt_to_string(u32str, big5ccvt);
  BOOST_TEST_EQ(str, big5str);

  // Convert wide string directly to Big-5 string)
  str.clear();
  str = boost::unicode::codecvt_to_string(uwstr, big5ccvt);
  BOOST_TEST_EQ(str, big5str);

  // Convert Big-5 string directly to UTF-8 string
  str8 = boost::unicode::codecvt_to_u8string(big5str, big5ccvt);
  BOOST_TEST_EQ(str8, u8str);

  // Convert Big-5 string directly to UTF-16 string
  str16 = boost::unicode::codecvt_to_u16string(big5str, big5ccvt);
  BOOST_TEST(str16 == u16str);

  // Convert Big-5 string directly to UTF-32 string
  str32 = boost::unicode::codecvt_to_u32string(big5str, big5ccvt);
  BOOST_TEST(str32 == u32str);

  // Convert Big-5 string directly to wide string
  wstr.clear();
  wstr = boost::unicode::codecvt_to_wstring(big5str, big5ccvt);
  BOOST_TEST(wstr == uwstr);

  return ::boost::report_errors();
}

