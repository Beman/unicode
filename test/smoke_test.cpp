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

  string s("foo bar bah");
  wstring w(L"foo bar bah");
  u16string u16(u"foo bar bah");
  u32string u32(U"foo bar bah");

  BOOST_TEST(to_string(s) == s);
  BOOST_TEST(to_string(w) == s);
  BOOST_TEST(to_string(u16) == s);
  BOOST_TEST(to_string() == s);


  return boost::report_errors();
}
