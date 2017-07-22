//  unicode/example/doc_examples.cpp  --------------------------------------------------//

//  © Copyright Beman Dawes 2016, 2017

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

#include <boost/unicode/string_encoding.hpp>
#include <string>
#include <locale>
#include <cvt/big5>  // vendor supplied
#include <cvt/sjis>  // vendor supplied

using namespace boost::unicode;
using namespace std;

string sjis_str() { string s; /*load s*/ return s; }
string big5_str() { string s; /*load s*/ return s; }

int main()
{
  // source strings
  string     narrow_str("abc123");       // a narrow encoding known to std::locale()
  string     utf8_str(u8"abc123$€𐐷𤭢");  // UTF-8 encoded
  u16string  utf16_str(u"abc123$€𐐷𤭢");  // UTF-16 encoded
  u32string  utf32_str(U"abc123$€𐐷𤭢");  // UTF-32 encoding
  wstring    wide_str(L"abc123$€𐐷𤭢");   // the implementation defined wide encoding


  // codecvt facets
  stdext::cvt::codecvt_big5<wchar_t> big5;  // vendor supplied Big-5 facet
  stdext::cvt::codecvt_sjis<wchar_t> sjis;  // vendor supplied Shift-JIS facet
  auto loc = std::locale();
  auto& loc_ccvt(std::use_facet<codecvt_narrow>(loc));

  //  test each "from" encoding for each "to" encoding including the default

  //  both encodings known (i.e. UTF-8, UTF-16, UTF-32, wide)

  auto default_from_utf8 = to_string(utf8_str);        // default (i.e. UTF-8) from UTF-8
  assert(default_from_utf8 == utf8_str);              
                                                      
  auto utf8_from_utf8 = to_string<utf8>(utf8_str);     // UTF-8 from UTF-8
  assert(utf8_from_utf8 == utf8_str);                 
                                                      
  auto utf16_from_utf8 = to_string<utf16>(utf8_str);   // UTF-16 from UTF-8
  assert(utf16_from_utf8 == utf16_str);               
                                                      
  auto utf32_from_utf8 = to_string<utf32>(utf8_str);   // UTF-32 from UTF-8
  assert(utf32_from_utf8 == utf32_str);               
                                                      
  auto wide_from_utf8 = to_string<wide>(utf8_str);     // wide from UTF-8
  assert(wide_from_utf8 == wide_str);

  auto default_from_utf16 = to_string(utf16_str);      // default (i.e. UTF-8) from UTF-16
  assert(default_from_utf16 == utf8_str);

  auto utf8_from_utf16 = to_string<utf8>(utf16_str);   // UTF-8 from UTF-16
  assert(utf8_from_utf16 == utf8_str);

  auto utf16_from_utf16 = to_string<utf16>(utf16_str); // UTF-16 from UTF-16
  assert(utf16_from_utf16 == utf16_str);

  auto utf32_from_utf16 = to_string<utf32>(utf16_str); // UTF-32 from UTF-16
  assert(utf32_from_utf16 == utf32_str);

  auto wide_from_utf16 = to_string<wide>(utf16_str);   // wide from UTF-16
  assert(wide_from_utf16 == wide_str);

  auto default_from_utf32 = to_string(utf32_str);      // default (i.e. UTF-8) from UTF-32
  assert(default_from_utf32 == utf8_str);

  auto utf8_from_utf32 = to_string<utf8>(utf32_str);    // UTF-8 from UTF-32
  assert(utf8_from_utf32 == utf8_str);

  auto utf16_from_utf32 = to_string<utf16>(utf32_str);  // UTF-16 from UTF-32
  assert(utf16_from_utf32 == utf16_str);

  auto utf32_from_utf32 = to_string<utf32>(utf32_str);  // UTF-32 from UTF-32
  assert(utf32_from_utf32 == utf32_str);

  auto wide_from_utf32 = to_string<wide>(utf32_str);    // wide from UTF-32
  assert(wide_from_utf32 == wide_str);

  auto default_from_wide = to_string(wide_str);         // default (i.e. UTF-8) from wide
  assert(default_from_wide == utf8_str);

  auto utf8_from_wide = to_string<utf8>(wide_str);      // UTF-8 from wide
  assert(utf8_from_wide == utf8_str);

  auto utf16_from_wide = to_string<utf16>(wide_str);    // UTF-16 from wide
  assert(utf16_from_wide == utf16_str);

  auto utf32_from_wide = to_string<utf32>(wide_str);    // UTF-32 from wide
  assert(utf32_from_wide == utf32_str);

  auto wide_from_wide = to_string<wide>(wide_str);      // wide from wide
  assert(wide_from_wide == wide_str);

  //  to encoding known, from encoding narrow

  //  to encoding narrow, from encoding known

  //  both to and from encoding narrow 

  //u16string s1 = to_string<utf16>(u8str);                  // UTF-16 from UTF-8
  //wstring   s2 = to_string<wide>(locstr, loc_ccvt);        // wide from narrow
  //u32string s3 = to_string<utf32>(sjisstr(), sjis);        // UTF-32 from Shift-JIS
  //string    s4 = to_string<narrow>(u32str, big5);          // Big-5 from UTF-32
  //string    s5 = to_string<narrow>(big5str(), big5, sjis); // Shift-JIS from Big-5

  //string s6 = to_string<utf8>(u8str);  // replace errors with u8"\uFFFD"
  //string s7 = to_string(u16str, []() {return "?";});  // replace errors with '?'
  //string s8 = to_string<utf8>(wstr, []() {throw "barf"; return "";});  // throw on error
  //
  //string s9 = to_string<narrow>(u16str, big5); // OK
  ////string s10 = to_string<utf8>(u16str, big5);  // error: ccvt_type arg not allowed
  ////string s11 = to_string<narrow>(u16str);      // error: ccvt_type arg required
  ////string s12 = to_string<narrow>(u16str, big5, big5); // error: >1 ccvt_type arg
  ////wstring s13 = to_string<wide>(locstr, big5, big5); // error: >1 ccvt_type arg
  ////string  s14 = to_string<narrow>(locstr);  // error: ccvt_type arg required
}
