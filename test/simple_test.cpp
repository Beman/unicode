//  unicode/test/simple_test.cpp  ------------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::unicode;

int cpp_main(int, char*[])
{
  const std::string u8str(u8"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::u16string u16str(u"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::u32string u32str(U"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::wstring wstr(L"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");

  std::string s;
  std::wstring sw;
  std::u16string s16;
  std::u32string s32;


  recode_utf<char>
    (wstr.cbegin(), wstr.cend(), std::back_inserter(s));
  BOOST_TEST(s == u8str);

  s.clear();
  s = recode_utf_string<char, wchar_t>(wstr);
  BOOST_TEST(s == u8str);

  s.clear();
  s = recode<utf8, wide>(wstr);
  BOOST_TEST(s == u8str);

  recode_utf<wchar_t>
    (u8str.cbegin(), u8str.cend(), std::back_inserter(sw));
  BOOST_TEST(sw == wstr);

  sw.clear();
  sw = recode_utf_string<wchar_t, char>(u8str);
  BOOST_TEST(sw == wstr);

  sw.clear();
  sw = recode<wide, utf8>(u8str);
  BOOST_TEST(sw == wstr);

  return ::boost::report_errors();
}

