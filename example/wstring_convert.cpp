//  unicode/example/wstring_convert.cpp  -----------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <string>
#include <codecvt>
#include <cvt/sjis>  // vendor supplied
#include <cvt/big5>  // vendor supplied
#include <boost/unicode/string_encoding.hpp>

int main()
{
  // example 1 - convert UTF-8 to UTF-16

  std::string read();                      // read UTF-8 string from database 1
  void write(const std::u16string& data);  // write UTF-16 string to database 2

  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvtr;

  write(cvtr.from_bytes(read()));
  
  // problem 2 - convert Big-5 to Shift-JIS

  std::string read();                      // read Big-5 string from database 1
  void write(const std::string& data);     // write Shift-JIS string to database 2

  //std::wstring_convert<stdext::cvt::codecvt_sjis<char32_t>> sjis;
  //std::wstring_convert<stdext::cvt::codecvt_big5<char32_t>> big5;
  std::wstring_convert<stdext::cvt::codecvt_sjis<wchar_t>> sjis;
  std::wstring_convert<stdext::cvt::codecvt_big5<wchar_t>> big5;

  write(sjis.to_bytes(big5.from_bytes(read())));

}
