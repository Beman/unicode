#include <iostream>
#include <typeinfo>
#include "../include/boost/string_encoding/string_encoding.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <cwchar>
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::string_encoding;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;
using std::cout;
using std::endl;

namespace
{
  const string cs("foo bar bah");
  const wstring cw(L"foo bar bah");
  const string cu8(u8"foo bar bah");
  const u16string cu16(u"foo bar bah");
  const u32string cu32(U"foo bar bah");

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
    underlying<T>::type big_x
      = boost::endian::native_to_big(static_cast<underlying<T>::type>(x));

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
    recode<utf8, utf16 /*, u16string::const_iterator, std::back_inserter<u16string>*/>(
      cu8.cbegin(), cu8.cend(), std::back_inserter(ru16));
    BOOST_TEST(ru16 == cu16);
    cout << "  recode_test done" << endl;
  }

  void make_recoded_string_test()
  {
    cout << "make_recoded_string_test" << endl;
    boost::string_ref csref(cs);
    u16string ru16 = make_recoded_string<utf8, utf16>(csref);
    BOOST_TEST(ru16 == cu16);
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
    boost::string_ref csref(cs);
    //to_utf16(csref);
    to_utf16<utf8>(csref);
    to_utf16<utf8>(cs);
    //boost::wstring_ref cwref(cw);
    //to_utf16(cwref);
    //to_utf16(cw);

    const u32string u32s(U"$€𐐷𤭢");
    const u16string u16s(u"$€𐐷𤭢");
    const string u8s(u8"$€𐐷𤭢");
    cout << "  u32s.size() " << u32s.size() << endl;
    cout << "  u16s.size() " << u16s.size() << endl;
    u16string u16r = to_utf16(u32s);
    cout << "  u16r.size() " << u16r.size() << endl;
    BOOST_TEST_EQ(u16r.size(), 6u);
    BOOST_TEST(u16r == u16s);

    u16r.clear();
    u16r = to_utf16<utf8>(u8s);
    BOOST_TEST_EQ(u16r.size(), u16s.size());
    BOOST_TEST(u16r == u16s);

    cout << "  to_utf16_test done" << endl;
  }

  void to_utf32_test()
  {
    cout << "to_utf32_test" << endl;
    boost::string_ref csref(cs);
    //to_utf32(csref);
    to_utf32<utf8>(csref);
    BOOST_TEST(to_utf32<utf8>(cs) == cu32);

    const char32_t kosme[6] = {0x3BA, 0x1F79, 0x3C, 0x3BC, 0x3B5, 0x0};
//    u32string kosme;

    string u8s(u8"$¢€𐍈");
    BOOST_TEST_EQ(u8s.size(), 10u);
    u32string u32r = to_utf32<utf8>(u8s);
    BOOST_TEST_EQ(u32r.size(), 4u);
    u32string u32s = {0x24, 0xA2, 0x20AC, 0x10348};
    BOOST_TEST(u32r == u32s);

    u32string u32s2(U"$¢€𐍈");
    cout << "****" << u32s2.size() << endl;

    u32string u32s3(U"$€𐐷𤭢");
    BOOST_TEST_EQ(u32s3.size(), 4u);
    u16string u16s3(u"$€𐐷𤭢");
    BOOST_TEST_EQ(u16s3.size(), 6u);
    u32string u32sr3 = to_utf32(u16s3);
    BOOST_TEST_EQ(u32sr3.size(), 4u);
    BOOST_TEST(u32sr3 == u32s3);

    cout << "u16s3 :" << hex_string(u16s3) << endl;
    cout << "u32sr3:" << hex_string(u32sr3) << endl;
    cout << "u32s3 :" << hex_string(u32s3) << endl;

    //boost::wstring_ref cwref(cw);
    //to_utf32(cwref);
    //to_utf32(cw);
    cout << "  to_utf32_test done" << endl;
  }

}  // unnamed namespace

int main()
{
  //std::string s("foo bar bah");
  //std::string sresult;
  //recode<narrow, narrow>(s.cbegin(), s.cend(), std::back_inserter(sresult));
  //assert(s == sresult);
  //std::cout << sresult << std::endl;

  //std::wstring wresult;
  //recode<narrow, wide>(s.cbegin(), s.cend(), std::back_inserter(wresult));
  //std::wstring w(L"foo bar bah");
  //assert(w == wresult);

  recode_test();
  make_recoded_string_test();
  to_utf8_test();
  to_utf16_test();
  to_utf32_test();

  return boost::report_errors();
}
