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
#include "../include/boost/unicode/string_encoding.hpp"
#include <boost/unicode/detail/hex_string.hpp>
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

  void section_2()
  {
    cout << "Section 2, Boundary condition test cases" << endl;

    //string s2_1_4(u8"\"\U00010000\"");
    //u32string U2_1_4({U'"', U'\x10000', U'"'});
    //BOOST_TEST(to_string<utf32>(s2_1_4) == U2_1_4);
    //cout << s2_1_4.size() << " " << hex_string(s2_1_4) << endl;
    //cout << U2_1_4.size() << " " << hex_string(U2_1_4) << endl;

    //  2.1  First possible sequence of a certain length

    BOOST_TEST((to_string<utf32, utf8>(u8"\"\u0000\"") == U"\"\u0000\""));          // 2.1.1  1 byte
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\u0080\"") == U"\"\u0080\""));          // 2.1.2  2 bytes
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\u0800\"") == U"\"\u0800\""));          // 2.1.3  3 bytes
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\U00010000\"") == U"\"\U00010000\""));  // 2.1.4  4 bytes

    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\u0000\"")) == u8"\"\u0000\""));
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\u0080\"")) == u8"\"\u0080\""));
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\u0800\"")) == u8"\"\u0800\""));          
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\U00010000\"")) == u8"\"\U00010000\""));

    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\u0000\"")) == u8"\"\u0000\""));
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\u0080\"")) == u8"\"\u0080\""));
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\u0800\"")) == u8"\"\u0800\""));          
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\U00010000\"")) == u8"\"\U00010000\""));

    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\u0000\"")) == u8"\"\u0000\""));
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\u0080\"")) == u8"\"\u0080\""));
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\u0800\"")) == u8"\"\u0800\""));          
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\U00010000\"")) == u8"\"\U00010000\""));

    //  2.2  Last possible sequence of a certain length

    BOOST_TEST((to_string<utf32, utf8>(u8"\"\u007f\"") == U"\"\u007f\""));          // 2.2.1  1 byte
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\u07ff\"") == U"\"\u07ff\""));          // 2.2.2  2 bytes
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\uffff\"") == U"\"\uffff\""));          // 2.2.3  3 bytes
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\U0010FFFF\"") == U"\"\U0010FFFF\""));  // 2.2.4 4b (modified)

    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\u007f\"")) == u8"\"\u007f\""));        
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\u07ff\"")) == u8"\"\u07ff\""));         
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\uffff\"")) == u8"\"\uffff\""));         
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\"")); 

    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\u007f\"")) == u8"\"\u007f\""));        
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\u07ff\"")) == u8"\"\u07ff\""));         
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\uffff\"")) == u8"\"\uffff\""));         
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\"")); 

    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\u007f\"")) == u8"\"\u007f\""));        
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\u07ff\"")) == u8"\"\u07ff\""));         
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\uffff\"")) == u8"\"\uffff\""));         
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\U0010FFFF\"")) == u8"\"\U0010FFFF\"")); 

    //  2.3  Other boundary conditions

    BOOST_TEST((to_string<utf32, utf8>(u8"\"\uD7FF\"") == U"\"\uD7FF\""));
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\uE000\"") == U"\"\uE000\""));
    BOOST_TEST((to_string<utf32, utf8>(u8"\"\uFFFD\"") == U"\"\uFFFD\""));

    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\uD7FF\"")) == u8"\"\uD7FF\""));
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\uE000\"")) == u8"\"\uE000\""));
    BOOST_TEST((to_string<utf8, utf32>(to_string<utf32, utf8>(u8"\"\uFFFD\"")) == u8"\"\uFFFD\""));

    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\uD7FF\"")) == u8"\"\uD7FF\""));
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\uE000\"")) == u8"\"\uE000\""));
    BOOST_TEST((to_string<utf8, utf16>(to_string<utf16, utf8>(u8"\"\uFFFD\"")) == u8"\"\uFFFD\""));

    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\uD7FF\"")) == u8"\"\uD7FF\""));
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\uE000\"")) == u8"\"\uE000\""));
    BOOST_TEST((to_string<utf8, wide>(to_string<wide, utf8>(u8"\"\uFFFD\"")) == u8"\"\uFFFD\""));

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
