//  unicode/test/simple_test.cpp  ------------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <string>
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#include <boost/unicode/detail/utf8_codecvt_facet.hpp>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::unicode;

int cpp_main(int, char*[])
{
  detail::utf8_codecvt_facet ccvt(0);

  const std::string u8str(u8"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::u16string u16str(u"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::u32string u32str(U"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");
  const std::wstring wstr(L"ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ");

  std::string s;
  std::wstring sw;
  std::u16string s16;
  std::u32string s32;

  s32.clear();
  recode<utf8, utf32>(u8str.cbegin(), u8str.cend(), std::back_inserter(s32));
  BOOST_TEST(s32 == u32str);

  s.clear();
  s = to_string<utf8, wide>(wstr);
  BOOST_TEST(s == u8str);

  s.clear();
  recode<wide, utf8>(wstr.cbegin(), wstr.cend(), std::back_inserter(s));
  BOOST_TEST(s == u8str);

  s.clear();
  s = to_string<utf8, wide>(wstr);
  BOOST_TEST(s == u8str);

  sw.clear();
  sw = to_string<wide, utf8>(u8str);
  BOOST_TEST(sw == wstr);

  s16.clear();
  recode<narrow, utf16>(u8str.data(), u8str.data()+u8str.size(),
    std::back_inserter(s16), ccvt);
  BOOST_TEST(s16 == u16str);

  s.clear();
  recode<utf16, narrow>(u16str.data(), u16str.data()+u16str.size(),
    std::back_inserter(s), ccvt);
  BOOST_TEST(s == u8str);


   return ::boost::report_errors();
}

