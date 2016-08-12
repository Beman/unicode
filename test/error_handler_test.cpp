//  unicode/test/error_handler_test  ---------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using std::cout;
using std::endl;
#include <typeinfo>
#include "../include/boost/unicode/string_encoding.hpp"
#include "../include/boost/unicode/detail/hex_string.hpp"
#include <cassert>
#include <string>
#include <iterator>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>

using namespace boost::unicode;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{
  const string     u8str(u8"$€𐐷𤭢");
  const u16string  u16str(u"$€𐐷𤭢");
  const u32string  u32str(U"$€𐐷𤭢");
  const wstring    wstr(  L"$€𐐷𤭢");

  const string     ill_u8str
    = {0x24,char(0xE2),char(0x82),char(0xAC),char(0xF0),char(0x90),char(0x90),char(0xB7),
    char(0xF0),char(0xA4),char(0xAD),char(0xA2),char(0xED),char(0xA0),char(0x80)};
  const u16string  ill_u16str(u"$€𐐷𤭢\xD800");
  const u32string  ill_u32str(U"$€𐐷𤭢\xD800");
  const wstring    ill_wstr(  L"$€𐐷𤭢\xD800");

  // TODO: what was being tested here and why is it commented out?
//void invalid_utf8_characters()
//  {
//    cout << "invalid_utf8_characters" << endl;
//    const string e0 { 'a', 'b', 'c' }; // correct initializer list
//    const string e1 (u8"$" /*"\u00C0" + '\xC0', u8"€", '\xC1', u8"𐐷", '\xC0', u8"𤭢"*/);
//    for (auto x : e1)
//    {
//      cout << std::hex << unsigned(x);
//    }
//    cout << endl;
////    cout << std::hex << e1 << endl;
////    BOOST_TEST(to_string<utf16, utf8>(e1) == u"\xFFFD");
//    cout << "  invalid_utf8_characters done" << endl;
//  }

  //  User supplied error handlers
  struct err8  { const char* operator()() const     { return "*ill*"; } };
  struct err16 { const char16_t* operator()() const { return u"*ill*"; } };
  struct err32 { const char32_t* operator()() const { return U"*ill*"; } };
  struct errw  { const wchar_t* operator()() const  { return L"*ill*"; } };
  struct err8nul  { const char* operator()() const     { return ""; } };
  struct err16nul { const char16_t* operator()() const { return u""; } };
  struct err32nul { const char32_t* operator()() const { return U""; } };
  struct errwnul  { const wchar_t* operator()() const  { return L""; } };

  void default_arguments()
  {
    cout << "default_arguments test" << endl;
    BOOST_TEST(to_string<utf16>(U"A\x110000Z") == u"A\uFFFDZ");
    BOOST_TEST(to_string<utf16>(U"A\x110000Z", ufffd<char16_t>()) == u"A\uFFFDZ");
    BOOST_TEST((to_string<utf16, err16>(U"A\x110000Z")
      == u"A*ill*Z"));
    BOOST_TEST((to_string<utf16, err16>(U"A\x110000Z", err16())
      == u"A*ill*Z"));
    cout << "  default_arguments test done" << endl;
  }

  void ill_formed_utf32_source()
  {
    cout << "ill_formed_utf32_source test" << endl;
    // test well-formed cases first to ensure these still work
    BOOST_TEST(to_string<wide>(u32str) == wstr);
    BOOST_TEST(to_string<utf8>(u32str) == u8str);
    BOOST_TEST(to_string<utf16>(u32str) == u16str);
    BOOST_TEST(to_string<utf32>(u32str) == u32str);
    // ditto with user supplied error handler
    BOOST_TEST((to_string<wide, errw>(u32str) == wstr));
    BOOST_TEST((to_string<utf8, err8>(u32str) == u8str));
    BOOST_TEST((to_string<utf16, err16>(u32str) == u16str));
    BOOST_TEST((to_string<utf32, err32>(u32str) == u32str));

    BOOST_TEST(to_string<utf16>(U"\x110000") == u"\uFFFD");
    BOOST_TEST(to_string<utf16>(U"A\x110000") == u"A\uFFFD");
    BOOST_TEST(to_string<utf16>(U"\x110000Z") == u"\uFFFDZ");
    BOOST_TEST(to_string<utf16>(U"A\x110000Z") == u"A\uFFFDZ");

    BOOST_TEST((to_string<wide, errwnul>(ill_u32str) == wstr));
    BOOST_TEST((to_string<utf8, err8nul>(ill_u32str) == u8str));

    BOOST_TEST((to_string<utf16, err16nul>(ill_u32str) == u16str));

    BOOST_TEST((to_string<utf32, err32nul>(ill_u32str) == u32str));
    cout << "  ill_formed_utf32_source test done" << endl;
  }

  void ill_formed_utf16_source()
  {
    cout << "ill_formed_utf16_source test" << endl;
    // test well-formed cases first to ensure these still work
    BOOST_TEST(to_string<wide>(u16str) == wstr);
    BOOST_TEST(to_string<utf8>(u16str) == u8str);
    BOOST_TEST(to_string<utf16>(u16str) == u16str);
    BOOST_TEST(to_string<utf32>(u16str) == u32str);
    // ditto with user supplied error handler
    BOOST_TEST((to_string<wide, errw>(u16str) == wstr));
    BOOST_TEST((to_string<utf8, err8>(u16str) == u8str));
    BOOST_TEST((to_string<utf16, err16>(u16str) == u16str));
    BOOST_TEST((to_string<utf32, err32>(u16str) == u32str));

    BOOST_TEST(to_string<utf16>("\xed\xa0\x80") == u"\uFFFD");
    BOOST_TEST((to_string<utf16, err16>("\xed\xa0\x80") == u"*ill*"));
    BOOST_TEST((to_string<utf16, err16nul>("\xed\xa0\x80") == u""));

    BOOST_TEST((to_string<wide, errwnul>(ill_u16str) == wstr));
    BOOST_TEST((to_string<utf8, err8nul>(ill_u16str) == u8str));
    BOOST_TEST((to_string<utf16, err16nul>(ill_u16str) == u16str));
    BOOST_TEST((to_string<utf32, err32nul>(ill_u16str) == u32str));
    cout << "  ill_formed_utf16_source test done" << endl;
  }

  void ill_formed_utf8_source()
  {
    cout << "ill_formed_utf8_source test" << endl;
    // test well-formed cases first to ensure these still work
    BOOST_TEST(to_string<wide>(u8str) == wstr);
    BOOST_TEST(to_string<utf8>(u8str) == u8str);
    BOOST_TEST(to_string<utf16>(u8str) == u16str);
    BOOST_TEST(to_string<utf32>(u8str) == u32str);
    // ditto with user supplied error handler
    BOOST_TEST((to_string<wide, errw>(u8str) == wstr));
    BOOST_TEST((to_string<utf8, err8>(u8str) == u8str));
    BOOST_TEST((to_string<utf16, err16>(u8str) == u16str));
    BOOST_TEST((to_string<utf32, err32>(u8str) == u32str));

    BOOST_TEST((to_string<utf32>("\xed\xa0\x80") == U"\uFFFD"));
    BOOST_TEST((to_string<utf32, err32>("\xed\xa0\x80") == U"*ill*"));
    BOOST_TEST((to_string<utf32, err32nul>("\xed\xa0\x80") == U""));

    BOOST_TEST((to_string<utf16>("\xed\xa0\x80") == u"\uFFFD"));
    BOOST_TEST((to_string<utf16, err16>("\xed\xa0\x80") == u"*ill*"));
    BOOST_TEST((to_string<utf16, err16nul>("\xed\xa0\x80") == u""));

    BOOST_TEST((to_string<wide, errwnul>(ill_u8str) == wstr));
    BOOST_TEST((to_string<utf8, err8nul>(ill_u8str) == u8str));
    //cout << hex_string(u8str) << endl;
    //cout << hex_string(to_string<utf8>(ill_u8str)) << endl;
    BOOST_TEST((to_string<utf16, err16nul>(ill_u8str) == u16str));
    BOOST_TEST((to_string<utf32, err32nul>(ill_u8str) == u32str));
    //cout << hex_string(ill_u8str) << endl;
    //cout << hex_string(to_string<utf32, err32nul>(ill_u8str)) << endl;
    cout << "  ill_formed_utf8_source test done" << endl;
  }

  void ill_formed_wide_source()
  {
    cout << "ill_formed_wide_source test" << endl;
    // test well-formed cases first to ensure these still work
    BOOST_TEST((to_string<wide>(wstr) == wstr));
    BOOST_TEST((to_string<utf8>(wstr) == u8str));
    BOOST_TEST((to_string<utf16>(wstr) == u16str));
    BOOST_TEST((to_string<utf32>(wstr) == u32str));
    // ditto with user supplied error handler
    BOOST_TEST((to_string<wide, errw>(wstr) == wstr));
    BOOST_TEST((to_string<utf8, err8>(wstr) == u8str));
    BOOST_TEST((to_string<utf16, err16>(wstr) == u16str));
    BOOST_TEST((to_string<utf32, err32>(wstr) == u32str));

    BOOST_TEST((to_string<wide, errwnul>(ill_wstr) == wstr));
    BOOST_TEST((to_string<utf8, err8nul>(ill_wstr) == u8str));
    BOOST_TEST((to_string<utf16, err16nul>(ill_wstr) == u16str));
    BOOST_TEST((to_string<utf32, err32nul>(ill_wstr) == u32str));
    cout << "  ill_formed_wide_source test done" << endl;
  }

}  // unnamed namespace

int main()
{
  
  default_arguments();
  ill_formed_utf32_source();
  ill_formed_utf16_source();
  ill_formed_utf8_source();
  ill_formed_wide_source();

  //invalid_utf8_characters();
  return boost::report_errors();
}
