//  unicode/test/kuhn_test.cpp  --------------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  Markus Kuhn's "UTF-8 decoder capability and stress test" is arguably the most       //
//  complete and best known set of UTF-8 test cases available. It is a work of art.     //
//                                                                                      //
//  See http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt                      //
//                                                                                      //
//  This program supplies some of those test cases, normally intended for visual        //
//  interpretation, in a form suitable for automated regression testing.                //
//                                                                                      //
//  Note: Some of the test values have been modified due to changes in the standard,    //
//  such as 5 and 6 byte UTF-8 sequences no longer being valid.                         //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <iostream>
using std::cout;
using std::endl;
#include <typeinfo>
#include "../include/boost/unicode/conversion.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <cwchar>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::unicode;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{

  template <class T> struct underlying;
  template<> struct underlying<char> { typedef unsigned char type; };
  template<> struct underlying<char16_t> { typedef boost::uint_least16_t type; };
  template<> struct underlying<char32_t> { typedef boost::uint_least32_t type; };
#if WCHAR_MAX >= 0xFFFFFFFFu
  template<> struct underlying<wchar_t> { typedef boost::uint_least32_t type; };
#else
  template<> struct underlying<wchar_t> { typedef boost::uint_least16_t type; };
#endif

  template <class T>
  std::string to_hex(T x)
  {
    const char hex[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    std::string tmp;
    typename underlying<T>::type big_x
      = boost::endian::native_to_big(static_cast<typename underlying<T>::type>(x));

    const unsigned char* p = reinterpret_cast<const unsigned char*>(&big_x);
    const unsigned char* e = p + sizeof(T);

    for (; p < e; ++p)
    {
      tmp += hex[*p >> 4];    // high-order nibble
      tmp += hex[*p & 0x0f];  // low-order nibble
    }
    return tmp;
  }

  template <class String>
  std::string hex_string(const String& s)
  {
    std::string tmp;
    for (auto x : s)
    {
      tmp += " 0x";
      tmp += to_hex(x);
    }
    return tmp;
  }

  void section_2()
  {
    cout << "Section 2, Boundary condition test cases" << endl;

    //string s2_1_4(u8"\"\U00010000\"");
    //u32string U2_1_4({U'"', U'\x10000', U'"'});
    //BOOST_TEST(to_utf32(s2_1_4) == U2_1_4);
    //cout << s2_1_4.size() << " " << hex_string(s2_1_4) << endl;
    //cout << U2_1_4.size() << " " << hex_string(U2_1_4) << endl;

    //  2.1  First possible sequence of a certain length

    BOOST_TEST(to_utf32(u8"\"\u0000\"") == U"\"\u0000\"");          // 2.1.1  1 byte
    BOOST_TEST(to_utf32(u8"\"\u0080\"") == U"\"\u0080\"");          // 2.1.2  2 bytes
    BOOST_TEST(to_utf32(u8"\"\u0800\"") == U"\"\u0800\"");          // 2.1.3  3 bytes
    BOOST_TEST(to_utf32(u8"\"\U00010000\"") == U"\"\U00010000\"");  // 2.1.4  4 bytes

    BOOST_TEST(to_utf8(to_utf32(u8"\"\u0000\"")) == u8"\"\u0000\"");
    BOOST_TEST(to_utf8(to_utf32(u8"\"\u0080\"")) == u8"\"\u0080\"");
    BOOST_TEST(to_utf8(to_utf32(u8"\"\u0800\"")) == u8"\"\u0800\"");          
    BOOST_TEST(to_utf8(to_utf32(u8"\"\U00010000\"")) == u8"\"\U00010000\"");

    BOOST_TEST(to_utf8(to_utf16(u8"\"\u0000\"")) == u8"\"\u0000\"");
    BOOST_TEST(to_utf8(to_utf16(u8"\"\u0080\"")) == u8"\"\u0080\"");
    BOOST_TEST(to_utf8(to_utf16(u8"\"\u0800\"")) == u8"\"\u0800\"");          
    BOOST_TEST(to_utf8(to_utf16(u8"\"\U00010000\"")) == u8"\"\U00010000\"");

    BOOST_TEST(to_utf8(to_wide(u8"\"\u0000\"")) == u8"\"\u0000\"");
    BOOST_TEST(to_utf8(to_wide(u8"\"\u0080\"")) == u8"\"\u0080\"");
    BOOST_TEST(to_utf8(to_wide(u8"\"\u0800\"")) == u8"\"\u0800\"");          
    BOOST_TEST(to_utf8(to_wide(u8"\"\U00010000\"")) == u8"\"\U00010000\"");

    //  2.2  Last possible sequence of a certain length

    BOOST_TEST(to_utf32(u8"\"\u007f\"") == U"\"\u007f\"");          // 2.2.1  1 byte
    BOOST_TEST(to_utf32(u8"\"\u07ff\"") == U"\"\u07ff\"");          // 2.2.2  2 bytes
    BOOST_TEST(to_utf32(u8"\"\uffff\"") == U"\"\uffff\"");          // 2.2.3  3 bytes
    BOOST_TEST(to_utf32(u8"\"\U0010FFFF\"") == U"\"\U0010FFFF\"");  // 2.2.4 4b (modified)

    BOOST_TEST(to_utf8(to_utf32(u8"\"\u007f\"")) == u8"\"\u007f\"");        
    BOOST_TEST(to_utf8(to_utf32(u8"\"\u07ff\"")) == u8"\"\u07ff\"");         
    BOOST_TEST(to_utf8(to_utf32(u8"\"\uffff\"")) == u8"\"\uffff\"");         
    BOOST_TEST(to_utf8(to_utf32(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\""); 

    BOOST_TEST(to_utf8(to_utf16(u8"\"\u007f\"")) == u8"\"\u007f\"");        
    BOOST_TEST(to_utf8(to_utf16(u8"\"\u07ff\"")) == u8"\"\u07ff\"");         
    BOOST_TEST(to_utf8(to_utf16(u8"\"\uffff\"")) == u8"\"\uffff\"");         
    BOOST_TEST(to_utf8(to_utf16(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\""); 

    BOOST_TEST(to_utf8(to_wide(u8"\"\u007f\"")) == u8"\"\u007f\"");        
    BOOST_TEST(to_utf8(to_wide(u8"\"\u07ff\"")) == u8"\"\u07ff\"");         
    BOOST_TEST(to_utf8(to_wide(u8"\"\uffff\"")) == u8"\"\uffff\"");         
    BOOST_TEST(to_utf8(to_wide(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\""); 

    //  2.3  Other boundary conditions

    BOOST_TEST(to_utf32(u8"\"\uD7FF\"") == U"\"\uD7FF\"");
    BOOST_TEST(to_utf32(u8"\"\uE000\"") == U"\"\uE000\"");
    BOOST_TEST(to_utf32(u8"\"\uFFFD\"") == U"\"\uFFFD\"");

    BOOST_TEST(to_utf8(to_utf32(u8"\"\uD7FF\"")) == u8"\"\uD7FF\"");
    BOOST_TEST(to_utf8(to_utf32(u8"\"\uE000\"")) == u8"\"\uE000\"");
    BOOST_TEST(to_utf8(to_utf32(u8"\"\uFFFD\"")) == u8"\"\uFFFD\"");

    BOOST_TEST(to_utf8(to_utf16(u8"\"\uD7FF\"")) == u8"\"\uD7FF\"");
    BOOST_TEST(to_utf8(to_utf16(u8"\"\uE000\"")) == u8"\"\uE000\"");
    BOOST_TEST(to_utf8(to_utf16(u8"\"\uFFFD\"")) == u8"\"\uFFFD\"");

    BOOST_TEST(to_utf8(to_wide(u8"\"\uD7FF\"")) == u8"\"\uD7FF\"");
    BOOST_TEST(to_utf8(to_wide(u8"\"\uE000\"")) == u8"\"\uE000\"");
    BOOST_TEST(to_utf8(to_wide(u8"\"\uFFFD\"")) == u8"\"\uFFFD\"");

    cout << "  Boundary condition test cases done" << endl;
  }

}  // unnamed namespace

//--------------------------------------------------------------------------------------//
//                                      main                                            //
//--------------------------------------------------------------------------------------//

int main()
{

  section_2();

  return boost::report_errors();
}
