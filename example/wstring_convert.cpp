//  unicode/example/wstring_convert.cpp  -----------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <string>
#include <locale>
#include <codecvt>
#include <cvt/sjis>  // vendor supplied

int main()
{
  std::string sjis_str;
  std::string u8_str;

  std::wstring_convert<stdext::cvt::codecvt_sjis<wchar_t>> sjis;
  std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8;

  u8_str = utf8.to_bytes(sjis.from_bytes(sjis_str));

}
