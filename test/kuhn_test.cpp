//  string_encoding/test/kuhn_test.cpp  ------------------------------------------------//

//  © Copyright Beman Dawes 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//--------------------------------------------------------------------------------------//
//                                                                                      //
//  Markus Kuhn's "UTF-8 decoder capability and stress test" is arguably the most       //
//  complete and best known set of TUF-8 test cases available. It is a work of art.     //
//                                                                                      //
//  See http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt                      //
//                                                                                      //
//  This program supplies some of those test cases, normally intended for visual        //
//  interpretation, in a form suitable for automated regression testing.                //
//                                                                                      //
//--------------------------------------------------------------------------------------//

#include <iostream>
using std::cout;
using std::endl;
#include <typeinfo>
#include "../include/boost/string_encoding/string_encoding.hpp"
#include <cassert>
#include <string>
#include <iterator>
#include <cwchar>
#define BOOST_LIGHTWEIGHT_TEST_OSTREAM std::cout
#include <boost/core/lightweight_test.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::string_encoding;
using std::string;
using std::wstring;
using std::u16string;
using std::u32string;

namespace
{

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

    string s2_1_1 = "\"\0\"";
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
