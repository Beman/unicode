#include "../include/boost/string_encoding/string_encoding.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <iterator>

using namespace boost::string_encoding;

int main()
{
  std::string s("foo bar bah");
  std::string sresult;
  recode<narrow, narrow>(s.cbegin(), s.cend(), std::back_inserter(sresult));
  assert(s == sresult);
  std::cout << sresult << std::endl;

  return 0;
}
