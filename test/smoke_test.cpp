#include <iostream>
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


  return boost::report_errors();
}
