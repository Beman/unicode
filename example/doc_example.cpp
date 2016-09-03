//  unicode/example/doc_examples.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/unicode/string_encoding.hpp>
#include <string>
#include <locale>
#include <cvt/big5>  // vendor supplied
#include <cvt/sjis>  // vendor supplied

using namespace boost::unicode;
using namespace std;

string sjisstr() { string s; /*load s*/ return s; }
string big5str() { string s; /*load s*/ return s; }

int main()
{
  string     locstr("abc123...");       // narrow encoding known to std::locale()
  string     u8str(u8"abc123$€𐐷𤭢..."); // UTF-8 encoded
  u16string u16str(u"abc123$€𐐷𤭢...");  // UTF-16 encoded
  u32string u32str(U"abc123$€𐐷𤭢...");  // UTF-32 encoding
  wstring     wstr(L"abc123$€𐐷𤭢...");  // implementation defined wide encoding

  stdext::cvt::codecvt_big5<wchar_t> big5;  // vendor supplied Big-5 facet
  stdext::cvt::codecvt_sjis<wchar_t> sjis;  // vendor supplied Shift-JIS facet

  auto loc = std::locale();
  auto& loc_ccvt(std::use_facet<ccvt_type>(loc));

  u16string s1 = to_string<utf16>(u8str);                  // UTF-16 from UTF-8
  wstring   s2 = to_string<wide>(locstr, loc_ccvt);        // wide from narrow
  u32string s3 = to_string<utf32>(sjisstr(), sjis);        // UTF-32 from Shift-JIS
  string    s4 = to_string<narrow>(u32str, big5);          // Big-5 from UTF-32
  string    s5 = to_string<narrow>(big5str(), big5, sjis); // Shift-JIS from Big-5

  string s6 = to_string<utf8>(u8str);  // replace errors with u8"\uFFFD"
  string s7 = to_string<utf8>(u16str, []() {return "?";});  // replace errors with '?'
  string s8 = to_string<utf8>(wstr, []() {throw "barf"; return "";});  // throw on error
  
  string s9 = to_string<narrow>(u16str, big5); // OK
  string s10 = to_string<utf8>(u16str, big5);  // error: ccvt_type arg not allowed
  string s11 = to_string<narrow>(u16str);      // error: ccvt_type arg required
  string s12 = to_string<narrow>(u16str, big5, big5); // error: >1 ccvt_type arg
  wstring s13 = to_string<wide>(locstr, big5, big5); // error: >1 ccvt_type arg
  string  s14 = to_string<narrow>(locstr);  // error: ccvt_type arg required
}
