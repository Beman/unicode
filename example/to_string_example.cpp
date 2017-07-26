//  unicode/example/doc_examples.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016, 2017

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/unicode/string_encoding.hpp>
#include <string>
#include <locale>
#include <cvt/big5>  // vendor supplied
#include <cvt/sjis>  // vendor supplied
#include <iostream>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/unicode/detail/hex_string.hpp>

#include <type_traits>

using namespace boost::unicode;
using namespace std;

namespace
{
  // simple enable_if example

  template <class T>
  std::enable_if_t<std::is_same<T, int>::value, void>
    f()
  {
    cout << "overload 1\n";
  }

  template <class T>
  std::enable_if_t<std::is_same<T, long>::value, void>
    f()
  {
  cout << "overload 2\n";
  }

  template <class T>
  std::enable_if_t<!std::is_same<T, int>::value && !std::is_same<T, long>::value, void>
    f()
  {
  cout << "overload 3\n";
  }

}


int cpp_main(int, char*[])
{

  f<int>();
  f<long>();
  f<double>();

  // test strings
  string     narrow_str("abc123$?????"); // a narrow encoding known to std::locale()
  string     utf8_str(u8"abc123$€𐐷𤭢");  // UTF-8 encoded
  u16string  utf16_str(u"abc123$€𐐷𤭢");  // UTF-16 encoded
  u32string  utf32_str(U"abc123$€𐐷𤭢");  // UTF-32 encoding
  wstring    wide_str(L"abc123$€𐐷𤭢");   // the implementation defined wide encoding

  // test strings for Big-5
  string     hello_world_big5("\xA7\x41\xA6\x6E\x20\xA5\x40\xAC\xC9"); // 你好 世界
  string     hello_world_utf8(u8"你好 世界");
  u16string  hello_world_utf16(u"你好 世界");
  u32string  hello_world_utf32(U"你好 世界");
  wstring    hello_world_wide(L"你好 世界");


  // codecvt facets
  stdext::cvt::codecvt_big5<wchar_t> big5;  // vendor supplied Big-5 facet
  stdext::cvt::codecvt_sjis<wchar_t> sjis;  // vendor supplied Shift-JIS facet
  auto loc = std::locale();
  auto& loc_ccvt(std::use_facet<codecvt_type>(loc));

  //  test each "from" encoding for each "to" encoding including the default

  //  both encodings known (i.e. UTF-8, UTF-16, UTF-32, wide)

  auto default_from_utf8 = to_string(utf8_str);        // default (i.e. UTF-8) from UTF-8
  BOOST_TEST(default_from_utf8 == utf8_str);              
                                                      
  auto utf8_from_utf8 = to_string<utf8>(utf8_str);     // UTF-8 from UTF-8
  BOOST_TEST(utf8_from_utf8 == utf8_str);                 
                                                      
  auto utf16_from_utf8 = to_string<utf16>(utf8_str);   // UTF-16 from UTF-8
  BOOST_TEST(utf16_from_utf8 == utf16_str);               
                                                      
  auto utf32_from_utf8 = to_string<utf32>(utf8_str);   // UTF-32 from UTF-8
  BOOST_TEST(utf32_from_utf8 == utf32_str);               
                                                      
  auto wide_from_utf8 = to_string<wide>(utf8_str);     // wide from UTF-8
  BOOST_TEST(wide_from_utf8 == wide_str);

  auto default_from_utf16 = to_string(utf16_str);      // default (i.e. UTF-8) from UTF-16
  BOOST_TEST(default_from_utf16 == utf8_str);

  auto utf8_from_utf16 = to_string<utf8>(utf16_str);   // UTF-8 from UTF-16
  BOOST_TEST(utf8_from_utf16 == utf8_str);

  auto utf16_from_utf16 = to_string<utf16>(utf16_str); // UTF-16 from UTF-16
  BOOST_TEST(utf16_from_utf16 == utf16_str);

  auto utf32_from_utf16 = to_string<utf32>(utf16_str); // UTF-32 from UTF-16
  BOOST_TEST(utf32_from_utf16 == utf32_str);

  auto wide_from_utf16 = to_string<wide>(utf16_str);   // wide from UTF-16
  BOOST_TEST(wide_from_utf16 == wide_str);

  auto default_from_utf32 = to_string(utf32_str);      // default (i.e. UTF-8) from UTF-32
  BOOST_TEST(default_from_utf32 == utf8_str);

  auto utf8_from_utf32 = to_string<utf8>(utf32_str);    // UTF-8 from UTF-32
  BOOST_TEST(utf8_from_utf32 == utf8_str);

  auto utf16_from_utf32 = to_string<utf16>(utf32_str);  // UTF-16 from UTF-32
  BOOST_TEST(utf16_from_utf32 == utf16_str);

  auto utf32_from_utf32 = to_string<utf32>(utf32_str);  // UTF-32 from UTF-32
  BOOST_TEST(utf32_from_utf32 == utf32_str);

  auto wide_from_utf32 = to_string<wide>(utf32_str);    // wide from UTF-32
  BOOST_TEST(wide_from_utf32 == wide_str);

  auto default_from_wide = to_string(wide_str);         // default (i.e. UTF-8) from wide
  BOOST_TEST(default_from_wide == utf8_str);

  auto utf8_from_wide = to_string<utf8>(wide_str);      // UTF-8 from wide
  BOOST_TEST(utf8_from_wide == utf8_str);

  auto utf16_from_wide = to_string<utf16>(wide_str);    // UTF-16 from wide
  BOOST_TEST(utf16_from_wide == utf16_str);

  auto utf32_from_wide = to_string<utf32>(wide_str);    // UTF-32 from wide
  BOOST_TEST(utf32_from_wide == utf32_str);

  auto wide_from_wide = to_string<wide>(wide_str);      // wide from wide
  BOOST_TEST(wide_from_wide == wide_str);

  //  to encoding narrow, from encoding known, without errors

  auto hello_from_utf8 = to_string<narrow>(hello_world_utf8, big5);     // narrow from UTF-8
//  cout << '*' << hello_from_utf8 << '*' << endl;
  BOOST_TEST(hello_from_utf8 == hello_world_big5);

  auto hello_from_utf16 = to_string<narrow>(hello_world_utf16, big5);   // hello from UTF-16
//  cout << '*' << hello_from_utf16 << '*' << endl;
  BOOST_TEST(hello_from_utf16 == hello_world_big5);

  auto hello_from_utf32 = to_string<narrow>(hello_world_utf32, big5);   // hello from UTF-32
//  cout << '*' << hello_from_utf32 << '*' << endl;
  BOOST_TEST(hello_from_utf32 == hello_world_big5);

  auto hello_from_wide = to_string<narrow>(hello_world_wide, big5);     // hello from wide
//  cout << '*' << hello_from_wide << '*' << endl;
  BOOST_TEST(hello_from_wide == hello_world_big5);

  //  to encoding narrow, from encoding known, with errors

  auto narrow_from_utf8 = to_string<narrow>(utf8_str, big5);     // narrow from UTF-8
  cout << '*' << narrow_from_utf8 << '*' << endl;
  BOOST_TEST(narrow_from_utf8 == narrow_str);

  auto narrow_from_utf16 = to_string<narrow>(utf16_str, big5);   // narrow from UTF-16
  cout << '*' << narrow_from_utf16 << '*' << endl;
  BOOST_TEST(narrow_from_utf16 == narrow_str);

  auto narrow_from_utf32 = to_string<narrow>(utf32_str, big5);   // narrow from UTF-32
  cout << '*' << narrow_from_utf32 << '*' << endl;
  BOOST_TEST(narrow_from_utf32 == narrow_str);

  auto narrow_from_wide = to_string<narrow>(wide_str, big5);     // narrow from wide
  cout << '*' << narrow_from_wide << '*' << endl;
  BOOST_TEST(narrow_from_wide == narrow_str);

  auto big5_narrow = to_string<narrow>(hello_world_wide, big5); // narrow from wide
  cout << detail::hex_string(big5_narrow) << endl;

  //  to encoding builtin, from encoding narrow
                                                      
  auto utf8_from_big5 = to_string<utf8>(hello_world_big5, big5);     // UTF-8 from Big-5
  BOOST_TEST(utf8_from_big5 == hello_world_utf8);
                                                      
  auto utf16_from_big5 = to_string<utf16>(hello_world_big5, big5);   // UTF-16 from Big-5
  BOOST_TEST(utf16_from_big5 == hello_world_utf16);
                                                      
  auto utf32_from_big5 = to_string<utf32>(hello_world_big5, big5);   // UTF-32 from Big-5
  BOOST_TEST(utf32_from_big5 == hello_world_utf32);
                                                      
  auto wide_from_big5 = to_string<wide>(hello_world_big5, big5);     // wide from Big-5
  BOOST_TEST(wide_from_big5 == hello_world_wide);

  //  both to and from encoding narrow

  auto big5_from_big5 = to_string<narrow>(hello_world_big5, big5, big5); 
  BOOST_TEST(big5_from_big5 == hello_world_big5);

  return ::boost::report_errors();
}
