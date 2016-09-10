//  unicode/test/first_ill_formed_test.cpp  --------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
using std::cout;
using std::endl;
#include <boost/unicode/string_encoding.hpp>
#include <boost/unicode/detail/hex_string.hpp>
#include <boost/endian/conversion.hpp>
#include <string>
#include <cstring>
#include <iterator>
#include <boost/core/lightweight_test.hpp>
#include <boost/detail/lightweight_main.hpp>

using namespace boost::unicode;
using boost::unicode::detail::hex_string;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{
  void utf8_test()
  {
    cout << "start utf8_test" << endl;
/*
  ISO/IEC 10646:2014 9.4 "As a consequence of the well-formedness conditions specified
  in table 9.2, the following octet values are disallowed in UTF-8: C0-C1, F5-FE."

  The idea of this test is to add additional logic to first_ill_formed() to detect those
  values, then test all possible values of one to four octets with and without the new
  logic enabled. If the tests find a different number of ill-formed sequences, then the
  new logic is necessary. Otherwise, the disallowed octet values are already disallow by
  the existing logic.
*/
    union
    {
      uint32_t int_;
      char c_[4];
    } tmp;


    uint32_t ill_formed_count = 0;

    for (uint32_t value = 1u; value <= 0x10FFFFu; ++value)
    {
      string str;
      tmp.int_ = boost::endian::native_to_big(value);
      if (tmp.c_[0] != '\0')
        str.append(&tmp.c_[0], 4);
      else if (tmp.c_[1] != '\0')
        str.append(&tmp.c_[1], 3);
      else if (tmp.c_[2] != '\0')
        str.append(&tmp.c_[2], 2);
      else
        str.append(&tmp.c_[3], 1);
      if (!boost::unicode::is_well_formed<utf8>(str))
        ++ill_formed_count;
    }

    cout << "    ill-formed count is " << ill_formed_count << endl;

    cout << "  end utf8_test" << endl;

  }

}  // unnamed namespace

int cpp_main(int, char*[])
{
  utf8_test();

  return boost::report_errors();
}
