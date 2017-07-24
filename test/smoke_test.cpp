//  unicode/test/smoke_test.cpp  -------------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#if defined(_MSC_VER)
// MSVC -Wall is overaggresive about reporting these as warnings 
#  pragma warning(disable:4710) // function not inlined
#  pragma warning(disable:4514) // unreferenced inline function has been removed
#endif


#include <iostream>
using std::cout;
using std::endl;
#include <typeinfo>
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/stream.hpp>
#include <boost/unicode/detail/utf8_codecvt_facet.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#include <cassert>
#include <string>
#include <sstream>
#include <iterator>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>

using namespace boost::unicode;
using namespace boost::unicode::detail;
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

  const string     ill_u8str
    = {0x24,char(0xE2),char(0x82),char(0xAC),char(0xF0),char(0x90),char(0x90),char(0xB7),
    char(0xF0),char(0xA4),char(0xAD),char(0xA2),char(0xED),char(0xA0),char(0x80)};
  const u16string  ill_u16str(u"$€𐐷𤭢\xD800");
  const u32string  ill_u32str(U"$€𐐷𤭢\xD800");
  const wstring    ill_wstr(  L"$€𐐷𤭢\xD800");

  template <class T>
  void check_inserter(T x, const string& expected)
  {
    //cout << "      test...\n";
    std::stringstream ss;
    std::string result;
    ss << x;
    ss << '\n';
    ss >> result;
    //cout << "result:" << hex_string(result) << endl;
    //cout << "expect:" << hex_string(expected) << endl;

    BOOST_TEST_EQ(result, expected);
  }

  void inserter_test()
  {
    cout << "inserter_test" << endl;

    check_inserter(u8"$€𐐷𤭢", u8str);
    check_inserter(boost::string_view(u8str), u8str);
    check_inserter(u8str, u8str);

    check_inserter(u"$€𐐷𤭢", u8str);
    check_inserter(boost::u16string_view(u16str), u8str);
    check_inserter(u16str, u8str);

    check_inserter(U"$€𐐷𤭢", u8str);
    check_inserter(boost::u32string_view(u32str), u8str);
    check_inserter(u32str, u8str);

    check_inserter(L"$€𐐷𤭢", u8str);
    check_inserter(boost::wstring_view(wstr), u8str);
    check_inserter(wstr, u8str);

    cout << "  inserter_test done" << endl;
  }

  void type_traits_test()
  {
    cout << "type_traits_test" << endl;
    BOOST_TEST(!is_encoded_character<int>::value);
    BOOST_TEST(is_encoded_character<char>::value);
    BOOST_TEST(is_encoded_character<char16_t>::value);
    BOOST_TEST(is_encoded_character<char32_t>::value);
    BOOST_TEST(is_encoded_character<wchar_t>::value);

    BOOST_TEST(!is_encoding<char>::value);
    BOOST_TEST(is_encoding<narrow>::value);
    BOOST_TEST(is_encoding<wide>::value);
    BOOST_TEST(is_encoding<utf8>::value);
    BOOST_TEST(is_encoding<utf16>::value);
    BOOST_TEST(is_encoding<utf32>::value);
    cout << "  type_traits_test done" << endl;
  }

  void first_ill_formed_test()
  {
    cout << "first_ill_formed_test" << endl;

    BOOST_TEST(first_ill_formed(u32str.cbegin(), u32str.end()).first == u32str.end());
    BOOST_TEST(first_ill_formed(u16str.cbegin(), u16str.end()).first == u16str.end());
    BOOST_TEST(first_ill_formed(u8str.cbegin(), u8str.end()).first == u8str.end());
    BOOST_TEST(first_ill_formed(wstr.cbegin(), wstr.end()).first == wstr.end());

    // TODO: Need to probe for correct result iterator being returned for each of the
    // errors that are supposed to be being detected
    BOOST_TEST(first_ill_formed(ill_u32str.cbegin(), ill_u32str.end()).first
      != ill_u32str.end());
    BOOST_TEST(first_ill_formed(ill_u16str.cbegin(), ill_u16str.end()).first 
      != ill_u16str.end());
    BOOST_TEST(first_ill_formed(ill_u8str.cbegin(), ill_u8str.end()).first 
      != ill_u8str.end());
    BOOST_TEST(first_ill_formed(ill_wstr.cbegin(), ill_wstr.end()).first 
      != ill_wstr.end());

    cout << "  first_ill_formed_test done" << endl;
  }

  void is_well_formed_test()
  {
    cout << "is_well_formed_test" << endl;

    BOOST_TEST(is_well_formed(u8str));
    BOOST_TEST(is_well_formed(u16str));
    BOOST_TEST(is_well_formed(u32str));
    BOOST_TEST(is_well_formed(wstr));

    BOOST_TEST(!is_well_formed(ill_u8str));
    BOOST_TEST(!is_well_formed(ill_u16str));
    BOOST_TEST(!is_well_formed(ill_u32str));
    BOOST_TEST(!is_well_formed(ill_wstr));

    cout << "  is_well_formed_test done" << endl;
  }

  void codecvt_short_test()
  {
    cout << "codecvt_short_test" << endl;
    boost::unicode::detail::utf8_codecvt_facet ccvt(0);
    std::mbstate_t state = std::mbstate_t();
    cout << "  u8str.size() is " <<  u8str.size() << endl;
    cout << "  utf8_codecvt_facet length(): "
         << ccvt.length(state, u8str.data(), u8str.data()+u8str.size(), 100) << endl;
    cout << "  utf8_codecvt_facet max_length(): " << ccvt.max_length() << endl;

    const string    str(u8"$€0123456789你好abcdefghijklmnopqrstyvwxyz");
    const wstring   wstr(L"$€0123456789你好abcdefghijklmnopqrstyvwxyz");

    string tmp = to_string<narrow>(wstr, ccvt);
    BOOST_TEST(tmp == str);
    //cout << hex_string(tmp) << endl;
    cout << hex_string(str) << endl;

    wstring wstr_tmp = to_string<wide>(str, ccvt);
    BOOST_TEST(wstr_tmp == wstr);
    //cout << hex_string(wstr_tmp) << endl;
    //cout << hex_string(wstr) << endl;

    string FFEE(u8"\uFFEE");  // U+FFEE HALFWIDTH WHITE CIRCLE

    cout << "utf8:" << hex_string(FFEE) << endl;
    cout << "     " << hex_string(to_string<narrow>(FFEE, ccvt)) << endl;
    cout << "     " << hex_string(to_string<narrow>(boost::string_view(FFEE.data(), 3),
      ccvt)) << endl;
    cout << "     " << hex_string(to_string<narrow>(boost::string_view(FFEE.data(), 2),
      ccvt)) << endl;
    cout << "     " << hex_string(to_string<narrow>(boost::string_view(FFEE.data(), 1),
      ccvt)) << endl;
    
    string    asian(u8"$€你好");
    cout << "asian:" << hex_string(asian) << " | "
      << hex_string(to_string<utf8>(asian, ccvt)) << endl;
    asian.erase(1, 1);
    cout << "asian:" << hex_string(asian) << " | "
      << hex_string(to_string<utf8>(asian, ccvt)) << endl;

    cout << "  codecvt_short_test done" << endl;
  }

  void convert_encoding_test()
  {
    cout << "convert_encoding_test" << endl;
    u16string ru16;
    recode<utf8, utf16>(u8str.cbegin(), u8str.cend(), std::back_inserter(ru16));
    BOOST_TEST(ru16 == u16str);
    cout << "  convert_encoding_test done" << endl;
  }

  void to_utf_string_test()
  {
    cout << "to_utf_string_test" << endl;


    // two argument tests
    u16string ru16 = to_string<utf16>(boost::string_view(u8str), ufffd<char16_t>());
    BOOST_TEST(ru16 == u16str);
    ru16 = to_string<utf16>(
      boost::string_view(u8str), ufffd<char16_t>());
    BOOST_TEST(ru16 == u16str);
    ru16 = to_string<utf16>(u8str, ufffd<char16_t>());
    BOOST_TEST(ru16 == u16str);
    ru16 = to_string<utf16>(
      u8str, ufffd<char16_t>());
    BOOST_TEST(ru16 == u16str);

    // one argument tests
    ru16 = to_string<utf16>(boost::string_view(u8str));
    BOOST_TEST(ru16 == u16str);
    //ru16 = to_string<char16_t, char, std::char_traits<char>>(boost::string_view(u8str));
    //BOOST_TEST(ru16 == u16str);
    ru16 = to_string<utf16>(u8str);
    BOOST_TEST(ru16 == u16str);
    //ru16 = to_string<char16_t, char, std::char_traits<char>>(u8str);
    //BOOST_TEST(ru16 == u16str);
    cout << "  to_utf_string_test done" << endl;
  }
 
  void to_u8string_test()
  {
    cout << "to_u8string_test" << endl;
    u16string u16s(u"$€𐐷𤭢");
    string u8s(u8"$€𐐷𤭢");
    cout << u8s.size() << endl;

    string u8r = to_string<utf8>(u16s);
    BOOST_TEST_EQ(u8r.size(), u8s.size());
    BOOST_TEST(u8r == u8s);
    cout << "u8r :" << hex_string(u8r) << endl;

    cout << "  to_u8string_test done" << endl;
  }

  void to_u16string_test()
  {
    cout << "to_u16string_test" << endl;
    to_string<utf16>(u8str);

    cout << "  u32s.size() " << u32str.size() << endl;
    cout << "  u16s.size() " << u16str.size() << endl;
    u16string u16r = to_string<utf16>(u32str);
    cout << "  u16r.size() " << u16r.size() << endl;
    BOOST_TEST_EQ(u16r.size(), 6u);
    BOOST_TEST(u16r == u16str);

    u16r.clear();
    u16r = to_string<utf16>(u8str);
    BOOST_TEST_EQ(u16r.size(), u16str.size());
    BOOST_TEST(u16r == u16str);

    cout << "  to_u16string_test done" << endl;
  }

  void to_u32string_test()
  {
    cout << "to_u32string_test" << endl;
    BOOST_TEST((to_string<utf32>(u8str) == u32str));

    string u8s(u8"$¢€𐍈");
    BOOST_TEST_EQ(u8s.size(), 10u);
    u32string u32r = to_string<utf32>(u8s);
    BOOST_TEST_EQ(u32r.size(), 4u);
    u32string u32s = {0x24, 0xA2, 0x20AC, 0x10348};
    BOOST_TEST(u32r == u32s);

    u32string u32sr3 = to_string<utf32>(u16str);
    BOOST_TEST(u32sr3 == u32str);

    cout << "  to_u32string_test done" << endl;
  }

  void all_utf_test()
  {
    cout << "all_utf_test" << endl;

    BOOST_TEST((to_string<wide>(wstr) == wstr));
    BOOST_TEST((to_string<wide>(u8str) == wstr));
    BOOST_TEST((to_string<wide>(u8str, ufffd<wchar_t>()) == wstr));
    BOOST_TEST((to_string<wide>(u16str) == wstr));
    BOOST_TEST((to_string<wide>(u32str) == wstr));
    BOOST_TEST((to_string<wide>(u32str, ufffd<wchar_t>()) == wstr));

    BOOST_TEST((to_string<utf8>(wstr) == u8str));
    BOOST_TEST((to_string<utf8>(u8str) == u8str));
    BOOST_TEST((to_string<utf8>(u16str) == u8str));
    BOOST_TEST((to_string<utf8>(u32str) == u8str));

    BOOST_TEST((to_string<utf16>(wstr) == u16str));
    BOOST_TEST((to_string<utf16>(u8str) == u16str));
    BOOST_TEST((to_string<utf16>(u16str) == u16str));
    BOOST_TEST((to_string<utf16>(u32str) == u16str));

    BOOST_TEST((to_string<utf32>(wstr) == u32str));
    BOOST_TEST((to_string<utf32>(u8str) == u32str));
    BOOST_TEST((to_string<utf32>(u16str) == u32str));
    BOOST_TEST((to_string<utf32>(u32str) == u32str));

    cout << "  all_utf_test done" << endl;
  }

  //void all_codecvt_test()
  //{
  //  cout << "all_codecvt_test" << endl;

  //  //  for now, limit code points to the BMP to ensure test codecvt facet support

  //  const string     u8s(u8"$€Ꭶ❄");
  //  const wstring      ws(L"$€Ꭶ❄");

  //  boost::unicode::detail::utf8_codecvt_facet ccvt(0);

  //  cout << u8s.size() << endl;
  //  cout << hex_string(u8s) << endl;
  //  wstring w = to_string<wide>(u8s, ccvt);
  //  BOOST_TEST(w == ws);
  //  cout << ws.size() << endl;
  //  cout << hex_string(ws) << endl;
  //  cout << w.size() << endl;
  //  cout << hex_string(w) << endl;

  //  BOOST_TEST((to_string<wide>(u8s, ccvt) == ws));
  //  cout << hex_string(to_string<wide>(u8s, ccvt)) << endl;
  //  cout << hex_string(ws) << endl;
  //  BOOST_TEST((to_string<wide>(u8s, ccvt, ufffd<wchar_t>()) == ws));

  //  BOOST_TEST((to_string<narrow>(ws, ccvt) == u8s));
  //  BOOST_TEST((to_string<narrow>(ws, ccvt, ufffd<char>()) == u8s));

  //  cout << "  all_codecvt_test done" << endl;
  //}

  //  Probe CharTraits template argument deduction

  template <class CharT, class CharTraits>
  void f(boost::basic_string_view<CharT, CharTraits> v)
  {
    std::cout << "overload 1:" << v << std::endl;
  }

  template <class CharT>
  void f(boost::basic_string_view<CharT> v)
  {
    std::cout << "overload 2:" << v << std::endl;
  }

  template <class CharT, class CharTraits = typename std::char_traits<CharT>>
  void g(boost::basic_string_view<CharT, CharTraits> v)
  {
    std::cout << "g():" << v << std::endl;
  }

}  // unnamed namespace

int main()
{
  //  Probe CharTraits template argument deduction
  std::string std_string("std::string");
  f(boost::string_view("string_view"));                     // works
  f<char>(std_string);                                      // works
  f<char>("C-string");                                      // works
  f<char, std::char_traits<char>>(std_string);              // works
  f<char, std::char_traits<char>>("C-string");              // works
  g(boost::string_view("string_view"));                     // works
  //g<char>(std_string);                        // CharTraits template arg deduction fails
  //g<char>("C-string");                        // CharTraits template arg deduction fails
  g<char, std::char_traits<char>>(std_string);              // works
  g<char, std::char_traits<char>>("C-string");              // works

  cout << "WCHAR_MAX: " << std::hex << WCHAR_MAX << std::dec << endl;

  cout << "wstr   :" << hex_string(wstr) << endl;
  cout << "u8str  :" << hex_string(u8str) << endl;
  cout << "u16str :" << hex_string(u16str) << endl;
  cout << "u32str :" << hex_string(u32str) << endl;


  // TODO: Add test to verify InputIterators do not have to be contiguous and do not
  // have to meet any forward, bidirectional, or random access requirements.

  type_traits_test();
  inserter_test();
  codecvt_short_test();
  convert_encoding_test();
  to_utf_string_test();
  to_u8string_test();
  to_u16string_test();
  to_u32string_test();
  all_utf_test();
  //all_codecvt_test();
  first_ill_formed_test();
  is_well_formed_test();

  return boost::report_errors();
}
