//  string_encoding/test/smoke_test.cpp  -----------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include "../include/boost/string_encoding/string_encoding.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <cwchar>
#include <codecvt>  // for codecvt_utf8
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>
#include <iostream>
#include <typeinfo>

using std::cout;
using std::endl;

using namespace boost::string_encoding;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{

  void utf32_round_trip_test()
  {
    cout << "utf32_round_trip_test" << endl;

    u32string all_valid_utf32;

    for (char32_t c = U'\0'; c < U'\U0000D7FF'; ++c)
      all_valid_utf32 += c;
    for (char32_t c = U'\U0000E000'; c < U'\U0010FFFF'; ++c)
      all_valid_utf32 += c;

    BOOST_TEST(to_utf32(to_utf8(all_valid_utf32)) == all_valid_utf32);


    cout << "  utf32_round_trip_test done" << endl;
  }

}  // unnamed namespace

int main()
{

  utf32_round_trip_test();

  return boost::report_errors();
}
