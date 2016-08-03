//  unicode/test/round_trip_test.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include "../include/boost/unicode/string_encoding.hpp"
#include <string>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <iostream>
#include <typeinfo>

using std::cout;
using std::endl;

using namespace boost::unicode;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{

  u32string all_valid_utf32;

  void init()
  {
    //  U+D800 to U+DFFF are reserved for surrogates, and so are not included in the
    //  all_valid_utf32 test data as they must not appear in well-formed UTF-32
    //  sequences. 
    for (char32_t c = U'\0'; c <= U'\uD7FF'; ++c)
      all_valid_utf32 += c;
    for (char32_t c = U'\uE000'; c <= U'\U0010FFFF'; ++c)
      all_valid_utf32 += c;
  }

  void test()
  {
    cout << "UTF-32 to UTF-8 round trip test" << endl;
    cout << "  " << all_valid_utf32.size() << " code points will be tested" << endl;
    BOOST_TEST(to_u32string(to_u8string(all_valid_utf32)) == all_valid_utf32);
    cout << "  UTF-32 to UTF-8 round trip test complete" << endl;
 
    cout << "UTF-32 to UTF-16 round trip test" << endl;
    cout << "  " << all_valid_utf32.size() << " code points will be tested" << endl;
    BOOST_TEST(to_u32string(to_u16string(all_valid_utf32)) == all_valid_utf32);
    cout << "  UTF-32 to UTF-16 round trip test complete" << endl;
  }

}  // unnamed namespace

int main()
{
  init();
  test();

  return boost::report_errors();
}
