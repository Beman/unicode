#include <iostream>
#include <boost/unicode/recoder.hpp>
#include <iterator>
#include <system_error>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/unicode/detail/hex_string.hpp>

using boost::unicode::detail::hex_string;
using std::string;
using std::u16string;
using std::u32string;
using std::wstring;
using std::cout;
using std::endl;

namespace
{
  
  boost::unicode::recoder<char, char32_t> rcdr_8_32("UTF-8", "UTF-32");

  template <class Error = boost::unicode::ufffd<char32_t>>
  u32string to_u32string(boost::string_view v, Error eh = Error())
  {
    u32string tmp;
    rcdr_8_32.recode(v.data(), v.data()+v.size(), std::back_inserter(tmp), eh);
    return tmp;
  };

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

  //  User supplied error handlers
  struct err8  { const char* operator()() const     { return "*ill*"; } };
  struct err16 { const char16_t* operator()() const { return u"*ill*"; } };
  struct err32 { const char32_t* operator()() const { return U"*ill*"; } };
  struct errw  { const wchar_t* operator()() const  { return L"*ill*"; } };
  struct err8nul  { const char* operator()() const     { return ""; } };
  struct err16nul { const char16_t* operator()() const { return u""; } };
  struct err32nul { const char32_t* operator()() const { return U""; } };
  struct errwnul  { const wchar_t* operator()() const  { return L""; } };

  void ill_formed_utf8_source()
  {
    int n = 0;
    cout << "ill_formed_utf8_source test" << endl;

    // test well-formed cases first to ensure these still work
    BOOST_TEST((to_u32string("") == U""));
    cout << "  test " << ++n << " complete" << endl;
    BOOST_TEST((to_u32string(u8str) == u32str));
    cout << "  test " << ++n << " complete" << endl;
    
    // ditto with user supplied error handler
    BOOST_TEST((to_u32string(u8str, err32()) == u32str));
    cout << "  test " << ++n << " complete" << endl;

    BOOST_TEST((to_u32string("\xed\xa0\x80") == U"\uFFFD"));
    cout << hex_string(to_u32string("\xed\xa0\x80")) << endl;
    cout << "  test " << ++n << " complete" << endl;
    BOOST_TEST((to_u32string("\xed\xa0\x80", err32()) == U"*ill*"));
    cout << "  test " << ++n << " complete" << endl;
    BOOST_TEST((to_u32string("\xed\xa0\x80OK", err32()) == U"*ill*OK"));
    cout << "  test " << ++n << " complete" << endl;
    BOOST_TEST((to_u32string("\xed\xa0\x80", err32nul()) == U""));
    cout << "  test " << ++n << " complete" << endl;

    BOOST_TEST((to_u32string(ill_u8str, err32nul()) == u32str));
    //cout << hex_string(ill_u8str) << endl;
    //cout << hex_string(to_string<utf32, err32nul>(ill_u8str)) << endl;
    cout << "  test " << ++n << " complete" << endl;
    cout << "  ill_formed_utf8_source test done" << endl;
  }
}

int main()
{
  cout << "BOOST_UNICODE_BUFFER_SIZE=" << BOOST_UNICODE_BUFFER_SIZE << endl;
  std::u32string tmp;
  std::string test(u8"abc");
  std::u32string expected(U"abc");

  rcdr_8_32.recode(test.c_str(), test.c_str()+test.size(), std::back_inserter(tmp));

  BOOST_TEST(tmp == expected);
  std::cout << "result:" << hex_string(tmp) << std::endl;
  std::cout << "expect:" << hex_string(expected) << std::endl;

  tmp.clear();
  tmp = to_u32string(test);
  BOOST_TEST(tmp == expected);
  std::cout << "result:" << hex_string(tmp) << std::endl;
  std::cout << "expect:" << hex_string(expected) << std::endl;

  tmp.clear();
  tmp = to_u32string(u8str);
  BOOST_TEST(tmp == u32str);
  std::cout << "result:" << hex_string(tmp) << std::endl;
  std::cout << "expect:" << hex_string(u32str) << std::endl;

  ill_formed_utf8_source();

  return boost::report_errors();
}
