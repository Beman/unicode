//  string_encoding/test/invalid_utf_test  ---------------------------------------------//

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
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>

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
////    BOOST_TEST(to_utf16<utf8>(e1) == u"\xFFFD");
//    cout << "  invalid_utf8_characters done" << endl;
//  }

//void ill_formed_utf32()
//  {
//    cout << "ill_formed_utf32 test" << endl;
//    cout << "  ill_formed_utf32 test done" << endl;
//  }

  //  User supplied error handler

  constexpr char16_t* my_ill_formed = u"***ill-formed***";
  class my_err_hdlr
  {
  public:
    constexpr char16_t* operator()() const noexcept
    { return my_ill_formed; }
  };

void default_arguments()
  {
    cout << "default_arguments test" << endl;
    BOOST_TEST(to_utf16(U"A\x110000Z") == u"A\uFFFDZ");
    BOOST_TEST(to_utf16(U"A\x110000Z", err_hdlr<utf16>()) == u"A\uFFFDZ");
    BOOST_TEST(to_utf16(U"A\x110000Z", err_hdlr<utf16>(), std::allocator<char16_t>())
      == u"A\uFFFDZ");
    BOOST_TEST(to_utf16<my_err_hdlr>(U"A\x110000Z")
      == u"A***ill-formed***Z");
    BOOST_TEST(to_utf16<my_err_hdlr>(U"A\x110000Z", my_err_hdlr())
      == u"A***ill-formed***Z");
    BOOST_TEST(to_utf16<my_err_hdlr>(U"A\x110000Z", my_err_hdlr(), std::allocator<char16_t>())
      == u"A***ill-formed***Z");

    //u16string x = to_utf16<my_err_hdlr>(U"A\x110000Z", my_err_hdlr(), std::allocator<char16_t>());
    //cout << x.size();
    //cout << hex_string(x) << endl;
    cout << "  default_arguments test done" << endl;
  }

void ill_formed_utf32()
  {
    cout << "ill_formed_utf32 test" << endl;
    u32string u32s(U"\x110000");
    u16string u16s = to_utf16(u32s);
    BOOST_TEST(u16s == u"\uFFFD");
    BOOST_TEST(to_utf16(U"\x110000") == u"\uFFFD");
    BOOST_TEST(to_utf16(U"A\x110000") == u"A\uFFFD");
    BOOST_TEST(to_utf16(U"\x110000Z") == u"\uFFFDZ");
    BOOST_TEST(to_utf16(U"A\x110000Z") == u"A\uFFFDZ");
    cout << "  ill_formed_utf32 test done" << endl;
  }

}  // unnamed namespace

int main()
{

  //ill_formed_utf32();
  default_arguments();
  //invalid_utf8_characters();
  return boost::report_errors();
}
