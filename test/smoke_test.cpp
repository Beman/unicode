#include "../include/boost/string_encoding/string_encoding.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <iterator>

using namespace boost::string_encoding;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

int main()
{
  std::string s("foo bar bah");
  std::string sresult;
  recode<narrow, narrow>(s.cbegin(), s.cend(), std::back_inserter(sresult));
  assert(s == sresult);
  std::cout << sresult << std::endl;

  std::wstring wresult;
  recode<narrow, wide>(s.cbegin(), s.cend(), std::back_inserter(wresult));
  std::wstring w(L"foo bar bah");
  assert(w == wresult);

  return 0;
}
