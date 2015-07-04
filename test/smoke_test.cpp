//  string_encoding/test/smoke_test.cpp  -----------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using std::cout;
using std::endl;
#include <typeinfo>
#include "../include/boost/string_encoding/string_encoding.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <cwchar>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>

#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX11_HDR_CODECVT)
#  include <codecvt>  // for codecvt_utf8
#endif

using namespace boost::string_encoding;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{
  const string     u8str(u8"$€𐐷𤭢");
  const u16string u16str(u"$€𐐷𤭢");
  const u32string u32str(U"$€𐐷𤭢");
  const wstring     wstr(L"$€𐐷𤭢");

  template <class T> struct underlying;
  template<> struct underlying<char> { typedef unsigned char type; };
  template<> struct underlying<char16_t> { typedef boost::uint_least16_t type; };
  template<> struct underlying<char32_t> { typedef boost::uint_least32_t type; };
#if WCHAR_MAX >= 0xFFFFFFFFu
  template<> struct underlying<wchar_t> { typedef boost::uint_least32_t type; };
#else
  template<> struct underlying<wchar_t> { typedef boost::uint_least16_t type; };
#endif

  template <class T>
  std::string to_hex(T x)
  {
    const char hex[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    std::string tmp;
    typename underlying<T>::type big_x
      = boost::endian::native_to_big(static_cast<typename underlying<T>::type>(x));

    const unsigned char* p = reinterpret_cast<const unsigned char*>(&big_x);
    const unsigned char* e = p + sizeof(T);

    for (; p < e; ++p)
    {
      tmp += hex[*p >> 4];    // high-order nibble
      tmp += hex[*p & 0x0f];  // low-order nibble
    }
    return tmp;
  }

  template <class String>
  std::string hex_string(const String& s)
  {
    std::string tmp;
    for (auto x : s)
    {
      tmp += " 0x";
      tmp += to_hex(x);
    }
    return tmp;
  }

void recode_test()
  {
    cout << "recode_test" << endl;
    u16string ru16;
    recode<utf8, utf16>(u8str.cbegin(), u8str.cend(), std::back_inserter(ru16));
    BOOST_TEST(ru16 == u16str);
    cout << "  recode_test done" << endl;
  }

  void make_recoded_string_test()
  {
    cout << "make_recoded_string_test" << endl;
    u16string ru16 = make_recoded_string<utf8, utf16>(boost::string_ref(u8str));
    BOOST_TEST(ru16 == u16str);
    cout << "  make_recoded_string_test done" << endl;
  }
 
  void to_utf8_test()
  {
    cout << "to_utf8_test" << endl;
    u16string u16s(u"$€𐐷𤭢");
    string u8s(u8"$€𐐷𤭢");
    cout << u8s.size() << endl;

    string u8r = to_utf8(u16s);
    BOOST_TEST_EQ(u8r.size(), u8s.size());
    BOOST_TEST(u8r == u8s);
    cout << "u8r :" << hex_string(u8r) << endl;

    cout << "  to_utf8_test done" << endl;
  }

  void to_utf16_test()
  {
    cout << "to_utf16_test" << endl;
    to_utf16(u8str);

    cout << "  u32s.size() " << u32str.size() << endl;
    cout << "  u16s.size() " << u16str.size() << endl;
    u16string u16r = to_utf16(u32str);
    cout << "  u16r.size() " << u16r.size() << endl;
    BOOST_TEST_EQ(u16r.size(), 6u);
    BOOST_TEST(u16r == u16str);

    u16r.clear();
    u16r = to_utf16(u8str);
    BOOST_TEST_EQ(u16r.size(), u16str.size());
    BOOST_TEST(u16r == u16str);

    cout << "  to_utf16_test done" << endl;
  }

  void to_utf32_test()
  {
    cout << "to_utf32_test" << endl;
    BOOST_TEST(to_utf32(u8str) == u32str);

    string u8s(u8"$¢€𐍈");
    BOOST_TEST_EQ(u8s.size(), 10u);
    u32string u32r = to_utf32(u8s);
    BOOST_TEST_EQ(u32r.size(), 4u);
    u32string u32s = {0x24, 0xA2, 0x20AC, 0x10348};
    BOOST_TEST(u32r == u32s);

    u32string u32sr3 = to_utf32(u16str);
    BOOST_TEST(u32sr3 == u32str);

    cout << "  to_utf32_test done" << endl;
  }

  void all_utf_test()
  {
    cout << "all_utf_test" << endl;

    BOOST_TEST(to_wide(wstr) == wstr);
    BOOST_TEST(to_wide(u8str) == wstr);
    BOOST_TEST(to_wide(u8str, err_hdlr<wide>(), std::allocator<wchar_t>()) == wstr);
    BOOST_TEST(to_wide(u16str) == wstr);
    BOOST_TEST(to_wide(u32str) == wstr);
    BOOST_TEST(to_wide(u32str, err_hdlr<wide>(), std::allocator<wchar_t>()) == wstr);

    BOOST_TEST(to_utf8(wstr) == u8str);
    BOOST_TEST(to_utf8(u8str) == u8str);
    BOOST_TEST(to_utf8(u16str) == u8str);
    BOOST_TEST(to_utf8(u32str) == u8str);

    BOOST_TEST(to_utf16(wstr) == u16str);
    BOOST_TEST(to_utf16(u8str) == u16str);
    BOOST_TEST(to_utf16(u16str) == u16str);
    BOOST_TEST(to_utf16(u32str) == u16str);

    BOOST_TEST(to_utf32(wstr) == u32str);
    BOOST_TEST(to_utf32(u8str) == u32str);
    BOOST_TEST(to_utf32(u16str) == u32str);
    BOOST_TEST(to_utf32(u32str) == u32str);

    cout << "  all_utf_test done" << endl;
  }


  void all_codecvt_test()
  {
#if !defined(BOOST_NO_CXX11_HDR_CODECVT)
    cout << "all_codecvt_test" << endl;

    //  for now, limit code points to the BMP to ensure test codecvt facet support

    const string     u8s(u8"$€Ꭶ❄");
    const wstring      ws(L"$€Ꭶ❄");

    std::codecvt_utf8<wchar_t> ccvt_utf8; 

    cout << u8s.size() << endl;
    cout << hex_string(u8s) << endl;
    wstring w = narrow_to_wide(u8s, ccvt_utf8);
    cout << w.size() << endl;
    cout << hex_string(w) << endl;
    BOOST_TEST(w == ws);

    BOOST_TEST(narrow_to_wide(u8s, ccvt_utf8) == ws);
    BOOST_TEST(narrow_to_wide(u8s, ccvt_utf8, std::allocator<wchar_t>()) == ws);

    BOOST_TEST(wide_to_narrow(ws, ccvt_utf8) == u8s);
    BOOST_TEST(wide_to_narrow(ws, ccvt_utf8, std::allocator<char>()) == u8s);

    cout << "  all_codecvt_test done" << endl;
#endif
  }

}  // unnamed namespace

int main()
{
  cout << "wstr   :" << hex_string(wstr) << endl;
  cout << "u8str  :" << hex_string(u8str) << endl;
  cout << "u16str :" << hex_string(u16str) << endl;
  cout << "u32str :" << hex_string(u32str) << endl;

  recode_test();
  make_recoded_string_test();
  to_utf8_test();
  to_utf16_test();
  to_utf32_test();
  all_utf_test();
  all_codecvt_test();

  return boost::report_errors();
}
