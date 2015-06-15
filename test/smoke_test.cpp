#include <iostream>
#include <typeinfo>
#include "../include/boost/string_encoding/string_encoding.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <boost/core/lightweight_test.hpp>

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

  void to_utf16_test()
  {
    cout << "to_utf16_test" << endl;
    boost::string_ref csref(cs);
    //to_utf16(csref);
    to_utf16<utf8>(csref);
    to_utf16<utf8>(cs);
    boost::wstring_ref cwref(cw);
    to_utf16(cwref);
    to_utf16(cw);
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
  to_utf16_test();
  to_utf32_test();

  return boost::report_errors();
}
