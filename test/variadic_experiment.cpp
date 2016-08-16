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
}

int cpp_main(int, char*[])
{
  //  for now, limit code points to the BMP to ensure test codecvt facet support
  const std::string     u8str(u8"$€Ꭶ❄");
  const std::u16string u16str(u"$€Ꭶ❄");
  const std::u32string u32str(U"$€Ꭶ❄");
  const std::wstring     wstr(L"$€Ꭶ❄");

  boost::unicode::detail::utf8_codecvt_facet ccvt(0);

  // UTF to UTF
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
    s = test::recode<utf8, narrow>(u8str, ccvt);
    BOOST_TEST(s == u8str);
  }
  {
    std::string s;
    s = test::recode<narrow, utf8>(u8str, ccvt);
    BOOST_TEST(s == u8str);
    s.clear();
    s = test::recode<narrow, utf16>(u16str, ccvt);
    BOOST_TEST(s == u8str);
    s.clear();
    s = test::recode<narrow, utf32>(u32str, ccvt);
    BOOST_TEST(s == u8str);
    s.clear();
    s = test::recode<narrow, wide>(wstr, ccvt);
    BOOST_TEST(s == u8str);
    s.clear();
    s = test::recode<narrow, narrow>(u8str, ccvt, ccvt);
    BOOST_TEST(s == u8str);
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
    s = test::recode<utf16, narrow>(u8str, ccvt);
    BOOST_TEST(s == u16str);
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
    s = test::recode<utf32, narrow>(u8str, ccvt);
    BOOST_TEST(s == u32str);
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
    s = test::recode<wide, narrow>(u8str, ccvt);
    BOOST_TEST(s == wstr);
  }

  return ::boost::report_errors();
}
