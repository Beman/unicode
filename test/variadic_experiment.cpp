#include <iostream>
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#include <boost/unicode/detail/utf8_codecvt_facet.hpp>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::unicode;
using boost::unicode::detail::hex_string;
using std::cout;
using std::endl;

namespace
{
  const std::string     u8str(u8"$€𐐷𤭢");
  const std::u16string  u16str(u"$€𐐷𤭢");
  const std::u32string  u32str(U"$€𐐷𤭢");
  const std::wstring    wstr(  L"$€𐐷𤭢");
  const std::string     nstr( u8"$€𐐷𤭢");

  const std::string     ill_u8str
    = {0x24,                                      // $
    char(0xE2),char(0x82),                        // €
    char(0xAC),                                   // ill-formed
    char(0xF0),char(0x90),char(0x90),char(0xB7),  // 𐐷
    char(0xF0),char(0xA4),char(0xAD),char(0xA2),  // 𤭢
    char(0xED),char(0xA0),char(0x80)};            // ill-formed
  const std::u16string  ill_u16str(u"$€𐐷𤭢\xD800");
  const std::u32string  ill_u32str(U"$€𐐷𤭢\xD800");
  const std::wstring    ill_wstr(  L"$€𐐷𤭢\xD800");

  //  User supplied error handlers
  struct err8  { const char* operator()() const     { return "*ill*"; } };
  struct err16 { const char16_t* operator()() const { return u"*ill*"; } };
  struct err32 { const char32_t* operator()() const { return U"*ill*"; } };
  struct errw  { const wchar_t* operator()() const  { return L"*ill*"; } };
  struct err8nul  { const char* operator()() const     { return ""; } };
  struct err16nul { const char16_t* operator()() const { return u""; } };
  struct err32nul { const char32_t* operator()() const { return U""; } };
  struct errwnul  { const wchar_t* operator()() const  { return L""; } };
  struct errnnul  { const char* operator()() const     { return ""; } };

  //  The ancient utf8 codecvt facet used for testing works only for the basic
  //  multilingual plane (BMP) on Windows. Thus for any tests that involve narrow strings,
  //  test characters must be limited to the BMP.

  const std::string     u8bmp(u8"$€Ꭶ❄");
  const std::u16string u16bmp( u"$€Ꭶ❄");
  const std::u32string u32bmp( U"$€Ꭶ❄");
  const std::wstring     wbmp( L"$€Ꭶ❄");
  const std::string      nbmp(u8"$€Ꭶ❄");

  const std::string     ill_u8bmp
    = {0x24,                                      // $
    char(0xE2),char(0x82),                        // €
    char(0xAC),                                   // ill-formed
    char(0xE1),char(0x8E),char(0xA6),             // Ꭶ
    char(0xE2),char(0x9D),char(0x84),             // ❄
    char(0xED),char(0xA0),char(0x80)};            // ill-formed
  const std::u16string  ill_u16bmp(u"$€Ꭶ❄\xD800");
  const std::u32string  ill_u32bmp(U"$€Ꭶ❄\xD800");
  const std::wstring    ill_wbmp(  L"$€Ꭶ❄\xD800");
  const std::string     ill_nbmp
    = {0x24,                                      // $
    char(0xE2),char(0x82),                        // €
    char(0xAC),                                   // ill-formed
    char(0xE1),char(0x8E),char(0xA6),             // Ꭶ
    char(0xE2),char(0x9D),char(0x84),             // ❄
    char(0xED),char(0xA0),char(0x80)};            // ill-formed

  boost::unicode::detail::utf8_codecvt_facet ccvt(0);

  void basic_tests()
  {
    cout << "basic_tests();"
      "\n  all input characters are well-formed" << endl;
    {
      std::string s;
      s = to_string<utf8>(u8str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(u16str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(u32str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(wstr);
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(nbmp, ccvt);
      BOOST_TEST(s == u8bmp);
    }
    {
      std::string s;
      s = to_string<narrow>(u8bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(u16bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(u32bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(wbmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(nbmp, ccvt, ccvt);
      BOOST_TEST(s == nbmp);
    }
    {
      std::u16string s;
      s = to_string<utf16>(u8str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(u16str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(u32str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(wstr);
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(nbmp, ccvt);
      BOOST_TEST(s == u16bmp);
    }
    {
      std::u32string s;
      s = to_string<utf32>(u8str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(u16str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(u32str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(wstr);
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(nbmp, ccvt);
      BOOST_TEST(s == u32bmp);
    }
    {
      std::wstring s;
      s = to_string<wide>(u8str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(u16str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(u32str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(wstr);
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(nbmp, ccvt);
      BOOST_TEST(s == wbmp);
    }
    cout << "  end basic_tests()" << endl;
  }

  void user_supplied_error_handler_tests()
  {
    cout << "user_supplied_error_handler_tests();"
      "\n  all input strings include ill-formed characters" << endl;
    {
      std::string s;
      s = to_string<utf8>(ill_u8str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(ill_u16str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(ill_u32str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(ill_wstr, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = to_string<utf8>(ill_nbmp, ccvt, err8nul());
      BOOST_TEST(s == u8bmp);
    }
    {
      std::string s;
      s = to_string<narrow>(ill_u8bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      cout << hex_string(ill_u8bmp) << endl;
      cout << hex_string(s) << endl;
      cout << hex_string(nbmp) << endl;

      s.clear();
      s = to_string<narrow>(ill_u16bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(ill_u32bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(ill_wbmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = to_string<narrow>(ill_nbmp, ccvt, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
    }
    {
      std::u16string s;
      s = to_string<utf16>(ill_u8str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(ill_u16str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(ill_u32str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(ill_wstr, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = to_string<utf16>(ill_nbmp, ccvt, err16nul());
      BOOST_TEST(s == u16bmp);
    }
    {
      std::u32string s;
      s = to_string<utf32>(ill_u8str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(ill_u16str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(ill_u32str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(ill_wstr, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = to_string<utf32>(ill_nbmp, ccvt, err32nul());
      BOOST_TEST(s == u32bmp);
    }
    {
      std::wstring s;
      s = to_string<wide>(ill_u8str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(ill_u16str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(ill_u32str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(ill_wstr, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = to_string<wide>(ill_nbmp, ccvt, errwnul());
      BOOST_TEST(s == wbmp);
    }
    cout << "  end user_supplied_error_handler_tests()" << endl;
  }
}  // unnamed namespace

int cpp_main(int, char*[])
{
  basic_tests();
  user_supplied_error_handler_tests();

  return ::boost::report_errors();
}
