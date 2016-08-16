#include <iostream>
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#include <boost/unicode/detail/utf8_codecvt_facet.hpp>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/detail/lightweight_main.hpp>
#include <boost/core/lightweight_test.hpp>

using boost::unicode::encoded;
using boost::unicode::narrow;
using boost::unicode::utf8;
using boost::unicode::utf16;
using boost::unicode::utf32;
using boost::unicode::wide;
using boost::unicode::detail::hex_string;
using std::cout;
using std::endl;

namespace test
{
  namespace detail
  {
    struct utf_tag {};
    struct narrow_tag {};

    template <class Encoding> struct folded;
    template<> struct folded<narrow> { typedef narrow_tag tag; };
    template<> struct folded<utf8>   { typedef utf_tag tag; };
    template<> struct folded<utf16>  { typedef utf_tag tag; };
    template<> struct folded<utf32>  { typedef utf_tag tag; };
    template<> struct folded<wide>   { typedef utf_tag tag; };

  template <class ToEncoding, class FromEncoding, class ... T>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      recode_dispatch(utf_tag, utf_tag,
        boost::basic_string_view<typename encoded<FromEncoding>::type> v,
        const T& ... args)
    {
      static_assert(sizeof...(args) <= 1, "too many arguments");
      return boost::unicode::recode<ToEncoding>(v, args ...);
    }

  template <class ToEncoding, class FromEncoding, class ... T>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      recode_dispatch(narrow_tag, utf_tag,
        boost::basic_string_view<typename encoded<FromEncoding>::type> v,
        const T& ... args)
    {
      static_assert(sizeof...(args) <= 2, "too many arguments");
      return boost::unicode::recode_to_narrow(v, args ...);
    }

  template <class ToEncoding, class FromEncoding, class ... T>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      recode_dispatch(utf_tag, narrow_tag, 
        boost::basic_string_view<typename encoded<FromEncoding>::type> v,
        const T& ... args)
    {
      static_assert(sizeof...(args) <= 2, "too many arguments");
      return boost::unicode::recode_from_narrow<ToEncoding>(v, args ...);
    }

  template <class ToEncoding, class FromEncoding, class ... T>
    inline std::basic_string<typename encoded<ToEncoding>::type>
      recode_dispatch(narrow_tag, narrow_tag,
        boost::basic_string_view<typename encoded<FromEncoding>::type> v,
        const T& ... args)
    {
      static_assert(sizeof...(args) <= 3, "too many arguments");
      return boost::unicode::recode_narrow_to_narrow(v, args ...);
    }

  }  // detail

  template <class ToEncoding, class FromEncoding, class ... T>
  inline std::basic_string<typename encoded<ToEncoding>::type>
    recode(boost::basic_string_view<typename encoded<FromEncoding>::type> v,
      const T& ... args)
  {
    return detail::recode_dispatch<ToEncoding, FromEncoding>(
      detail::folded<ToEncoding>::tag(), detail::folded<FromEncoding>::tag(),
      v, args ...);
  }
}  // namespace test

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
      s = test::recode<utf8, utf8>(u8str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, utf16>(u16str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, utf32>(u32str);
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, wide>(wstr);
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, narrow>(nbmp, ccvt);
      BOOST_TEST(s == u8bmp);
    }
    {
      std::string s;
      s = test::recode<narrow, utf8>(u8bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, utf16>(u16bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, utf32>(u32bmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, wide>(wbmp, ccvt);
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, narrow>(nbmp, ccvt, ccvt);
      BOOST_TEST(s == nbmp);
    }
    {
      std::u16string s;
      s = test::recode<utf16, utf8>(u8str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, utf16>(u16str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, utf32>(u32str);
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, wide>(wstr);
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, narrow>(nbmp, ccvt);
      BOOST_TEST(s == u16bmp);
    }
    {
      std::u32string s;
      s = test::recode<utf32, utf8>(u8str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, utf16>(u16str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, utf32>(u32str);
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, wide>(wstr);
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, narrow>(nbmp, ccvt);
      BOOST_TEST(s == u32bmp);
    }
    {
      std::wstring s;
      s = test::recode<wide, utf8>(u8str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, utf16>(u16str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, utf32>(u32str);
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, wide>(wstr);
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, narrow>(nbmp, ccvt);
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
      s = test::recode<utf8, utf8>(ill_u8str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, utf16>(ill_u16str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, utf32>(ill_u32str, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, wide>(ill_wstr, err8nul());
      BOOST_TEST(s == u8str);
      s.clear();
      s = test::recode<utf8, narrow>(ill_nbmp, ccvt, err8nul());
      BOOST_TEST(s == u8bmp);
    }
    {
      std::string s;
      s = test::recode<narrow, utf8>(ill_u8bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      cout << hex_string(ill_u8bmp) << endl;
      cout << hex_string(s) << endl;
      cout << hex_string(nbmp) << endl;

      s.clear();
      s = test::recode<narrow, utf16>(ill_u16bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, utf32>(ill_u32bmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, wide>(ill_wbmp, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
      s.clear();
      s = test::recode<narrow, narrow>(ill_nbmp, ccvt, ccvt, errnnul());
      BOOST_TEST(s == nbmp);
    }
    {
      std::u16string s;
      s = test::recode<utf16, utf8>(ill_u8str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, utf16>(ill_u16str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, utf32>(ill_u32str, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, wide>(ill_wstr, err16nul());
      BOOST_TEST(s == u16str);
      s.clear();
      s = test::recode<utf16, narrow>(ill_nbmp, ccvt, err16nul());
      BOOST_TEST(s == u16bmp);
    }
    {
      std::u32string s;
      s = test::recode<utf32, utf8>(ill_u8str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, utf16>(ill_u16str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, utf32>(ill_u32str, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, wide>(ill_wstr, err32nul());
      BOOST_TEST(s == u32str);
      s.clear();
      s = test::recode<utf32, narrow>(ill_nbmp, ccvt, err32nul());
      BOOST_TEST(s == u32bmp);
    }
    {
      std::wstring s;
      s = test::recode<wide, utf8>(ill_u8str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, utf16>(ill_u16str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, utf32>(ill_u32str, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, wide>(ill_wstr, errwnul());
      BOOST_TEST(s == wstr);
      s.clear();
      s = test::recode<wide, narrow>(ill_nbmp, ccvt, errwnul());
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
