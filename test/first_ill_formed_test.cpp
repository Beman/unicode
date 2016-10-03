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
//#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
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

/*
  ISO/IEC 10646:2014 9.4 says "Table 3 lists all the ranges (inclusive) of the octet
  sequences that are well-formed in UTF-8. Any UTF-8 sequence that does not match the
  patterns listed in table 3 is ill-formed." and also says "As a consequence of the
  well-formedness conditions specified in table 9.2 [sic], the following octet values
  are disallowed in UTF-8: C0-C1, F5-FE."
  
  pedantic_first_ill_formed() is pedantic implmentation of those rules.
*/

  inline
  std::pair<const char*, const char*>
    pedantic_first_ill_formed(const char* first, const char* last) BOOST_NOEXCEPT
  {
    const char* first_code_unit;
    for (; first != last;) // each code point
    {
      first_code_unit = first;
      unsigned octet = static_cast<unsigned char>(*first++);

      if (octet <= 0x7Fu)
        continue;
      else if (octet >= 0xC2u && octet <= 0xDFu)  // two octets
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0x80u && octet <= 0xBFu)  // octet two
          continue;
      }
      else if (octet == 0xE0u)  // three octets case one
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0xA0u && octet <= 0xBFu)  // octet two
        {
          if (first == last)
            break;
          octet = static_cast<unsigned char>(*first++);
          if (octet >= 0x80u && octet <= 0xBFu)  // octet three
            continue;
        }
      }
      else if ((octet >= 0xE1u && octet <= 0xECu)
        || (octet >= 0xEEu && octet <= 0xEFu))  // three octets case two
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0x80u && octet <= 0xBFu)  // octet two
        {
          if (first == last)
            break;
          octet = static_cast<unsigned char>(*first++);
          if (octet >= 0x80u && octet <= 0xBFu)  // octet three
            continue;
        }
      }
      else if (octet == 0xEDu)  // three octets case three
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0x80u && octet <= 0x9Fu)  // octet two
        {
          if (first == last)
            break;
          octet = static_cast<unsigned char>(*first++);
          if (octet >= 0x80u && octet <= 0xBFu)  // octet three
            continue;
        }
      }
      else if (octet == 0xF0u)  // four octets case one
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0x90u && octet <= 0xBFu)  // octet two
        {
          if (first == last)
            break;
          octet = static_cast<unsigned char>(*first++);
          if (octet >= 0x80u && octet <= 0xBFu)  // octet three
          {
            if (first == last)
              break;
            octet = static_cast<unsigned char>(*first++);
            if (octet >= 0x80u && octet <= 0xBFu)  // octet four
              continue;
          }
        }
      }
      else if (octet >= 0xF1u && octet <= 0xF4u)  // four octets case two
      {
        if (first == last)
          break;
        octet = static_cast<unsigned char>(*first++);
        if (octet >= 0x80u && octet <= 0xBFu)  // octet two
        {
          if (first == last)
            break;
          octet = static_cast<unsigned char>(*first++);
          if (octet >= 0x80u && octet <= 0xBFu)  // octet three
          {
            if (first == last)
              break;
            octet = static_cast<unsigned char>(*first++);
            if (octet >= 0x80u && octet <= 0xBFu)  // octet four
              continue;
          }
        }
      }
      return std::make_pair(first_code_unit, first);   // failure
    }
    return std::make_pair(last, last);                 // success
  }

  void utf8_test()
  {
    cout << "start utf8_test" << endl;

    for (uint32_t i = 0;;)
    {
      auto pedantic = pedantic_first_ill_formed(reinterpret_cast<const char *>(&i),
        reinterpret_cast<const char *>(&i) + 4);
      auto normal = boost::unicode::first_ill_formed(reinterpret_cast<const char *>(&i),
          reinterpret_cast<const char *>(&i)+4);
      //BOOST_TEST(pedantic == normal);
      if (pedantic != normal)
      {
        ++boost::detail::test_errors();
        cout << "    Error:  first     second   for "
             << hex_string(string(reinterpret_cast<const char *>(&i),
        reinterpret_cast<const char *>(&i) + 4)) << " at "
          << reinterpret_cast<void*>(&i) <<
          "\n pedantic:  " << (void*)pedantic.first << "  "
            << (void*)pedantic.second << '\n' <<
          "   normal:  " << (void*)normal.first << "  "
            << (void*)normal.second << '\n';
      }

      if (boost::detail::test_errors() >= 10)
      {
        cout << "  maximum errors exceeded, test cancelled" << endl;
        break;
      }
      ++i;
    };



    cout << "  end utf8_test" << endl;

  }

}  // unnamed namespace

int cpp_main(int, char*[])
{
  utf8_test();

  return boost::report_errors();
}
